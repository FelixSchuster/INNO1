# python3 -m pip install --upgrade pip
# pip install --extra-index-url=https://pypi.celonis.cloud/ pycelonis --upgrade
# pip install scikit-learn

from pycelonis import *
from pycelonis.pql import *
import pandas as pd
import numpy as np
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_absolute_error, mean_squared_error
from sklearn.tree import DecisionTreeRegressor
from sklearn.model_selection import LeaveOneOut

course_id = "5C"
data_pool_name = "INNO-3"
data_model_name = "INNO-3-Kurs-5C"
rating_table_name = "ratings"

print("[*] Connecting to the Celonis API ...")
url = "https://academic-if21b054-technikum-wien-at.eu-2.celonis.cloud/"
api_token = "<api-token>"
celonis = get_celonis(base_url=url, api_token=api_token, key_type="APP_KEY", permissions=False)

print("[*] Downloading necessary data ...")
data_pool = celonis.data_integration.get_data_pools().find(data_pool_name)
data_model = data_pool.get_data_models().find(data_model_name)
rating_table = data_model.get_tables().find(rating_table_name)

# convert the rating table to a pandas dataframe
rating_df = DataFrame(
    {
    "course_id": f'"{rating_table_name}"."course_id"',
    "Nr": f'"{rating_table_name}"."Nr"',
    "Test: Upload 1 (Punkte)": f'"{rating_table_name}"."Aufgabe:Â Upload 1 (Punkte)"',
    "Test: Upload 2 (Punkte)": f'"{rating_table_name}"."Aufgabe:Â Upload 2 (Punkte)"',
    "Test: Upload 3 (Punkte)": f'"{rating_table_name}"."Aufgabe:Â Upload 3 (Punkte)"',
    "Test: Upload 4 (Punkte)": f'"{rating_table_name}"."Aufgabe:Â Upload 4 (Punkte)"',
    "Test: Upload 5 (Punkte)": f'"{rating_table_name}"."Aufgabe:Â Upload 5 (Punkte)"',
    "Test: Exam 2022/23 (Punkte)": f'"{rating_table_name}"."Test:Â Exam 2022/23 (Punkte)"',
    "In-lab exercises (Punkte)": f'"{rating_table_name}"."In-lab exercises (Punkte)"',
    "Kurs gesamt (Punkte)": f'"{rating_table_name}"."Kurs gesamt (Punkte)"',
    "Name": f'"{rating_table_name}"."Name"'
    },
    data_model=data_model
).to_pandas()

# filter the rating dataframe by the specified course id
rating_df = rating_df[rating_df["course_id"] == course_id]

# use Aufgabe 1-5 and in-lab exercises to train the model
columns = [string for string in rating_df.columns if "Aufgabe" in string or "exercises" in string]

# convert the values to float
x = rating_df[columns].astype(float)
y_exam_points = rating_df["Test: Exam 2022/23 (Punkte)"].astype(float)
y_total_points = rating_df["Kurs gesamt (Punkte)"].astype(float)

# normalize the training data
min_max_scaler = MinMaxScaler()
min_max_scaler.fit(x)
x = min_max_scaler.transform(x)
x = pd.DataFrame(x)

y = y_total_points
y_test = y_exam_points

print("[*] Training the decision tree machine learning model ...")
errors = []
iterations = 100
for i in range(iterations):
    print(f"[*] Iteration: {i+1} of {iterations}", end="\r")

    collector = []

    # use leave one out because the dataset is small
    loo = LeaveOneOut()

    # split the data used to train the model into training and testing data sets
    for train_index, test_index in loo.split(x):
        x_train, x_test = x.iloc[train_index], x.iloc[test_index]
        y_train, y_test = y.iloc[train_index], y.iloc[test_index]

        # fit decision tree to the training data
        tree_regressor = DecisionTreeRegressor().fit(x_train, y_train)

        # make predictions on the test set and store the result
        y_hat = tree_regressor.predict(x_test)
        collector.append(y_hat[0])

    # round the prediction
    y_pred = np.round(np.array(collector), 3)

    # append errors to the errors list
    errors.append({"index": i,
                  "mean_absolute_error": mean_absolute_error(y, y_pred),
                  "mean_squared_error": mean_squared_error(y, y_pred, squared=False),
                  "predicted_outcome": np.where(y_pred < 50, 0, 1),
                  "predicted_points": y_pred})

# save predicted outcome and predicted points
error_df = pd.DataFrame(errors).round(3)
predicted_outcome = error_df["predicted_outcome"].iloc[error_df["mean_absolute_error"].idxmax()]
predicted_points = error_df["predicted_points"].iloc[error_df["mean_absolute_error"].idxmax()]

# add predicted outcome and predicted points to the initial dataframe
result_df = rating_df.sort_values("Nr")
result_df["Vorraussichtlicher Kursabschluss"] = predicted_outcome
result_df["Vorraussichtliche Gesamtpunkte"] = predicted_points
result_df = result_df.round(2)

# remove final exam and final points from the dataframe to simulate a course in progress
result_df.drop(columns=["Test: Exam 2022/23 (Punkte)", "Kurs gesamt (Punkte)"])

print("[*] Uploading results ...")

# create a table called "DecisionTrees" in the current datapool
data_pool.create_table(df=result_df, table_name="DecisionTrees", drop_if_exists=True, force=True)

# if there is already a table called "DecisionTrees" in the datamodel, delete it
try:
    data_model.get_tables().find("DecisionTrees").delete()
except:
    print("[*] Table does not exist yet, nothing to delete")

# add the new "DecisionTrees" table to the datamodel
data_model.add_table(name="DecisionTrees", alias="DecisionTrees")

print("[*] Reloading the datamodel ...")

# reload the datamodel
data_model.reload()

print("[+] Done!")