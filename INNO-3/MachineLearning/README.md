# MachineLearning Tools
This part of the project includes the machine learning tools written in Python.\
Since the decision trees machine learning model delivered the most accurate results, the toolkit only includes the decision trees machine learning model.\
Take a look at the INNO-2 project for the nearest neighbour and linear regression implementations.

The machine learning model is trained based on the data existing in the Celonis datapools.\
Results will be uploaded to the Celonis API automatically.

## Requirements
```sh
python3 -m pip install --upgrade pip
pip install --extra-index-url=https://pypi.celonis.cloud/ pycelonis --upgrade
pip install scikit-learn
```
