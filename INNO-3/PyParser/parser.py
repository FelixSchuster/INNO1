import sys
import pandas as pd


def decode_html_entities(column):
    html_entities = {
        "&quot": "\"", "&apos": "'", "&amp": "&",
        "&gt": ">", "&lt": "<", "&frasl": "/",
        "-": "0"
    }
    for entity, char in html_entities.items():
        column = column.str.replace(entity, char)
    return column


# Function to get the output file name
def get_out_file_name(in_file_name):
    return in_file_name.rsplit('.', 1)[0] + "_celonis.csv"


# Function to get the filtered output file name
def get_filtered_out_file_name(in_file_name):
    return in_file_name.rsplit('.', 1)[0] + "_filtered_celonis.csv"


def parseFile(df, input_file_name):
    # Decode HTML entities in all columns
    columns_to_decode = ['Vollständiger Name', 'Ereigniskontext', 'Komponente', 'Ereignisname']
    df[columns_to_decode] = df[columns_to_decode].apply(decode_html_entities)

    # Identify the columns by their names
    # df.columns = df.iloc[0]
    # df = df[1:]
    print(df.columns)
    # Filter the DataFrame based on specified conditions
    filtered_df = df[
        (df["Vollständiger Name"] != "User Admin") &
        (~df["Ereigniskontext"].str.contains("(Kopie)")) &
        (df["Komponente"] != "Forum") &
        (df["Komponente"] != "System") &
        (df["Komponente"] != "Gruppenwahl") &
        (df["Komponente"] != "Zoom Meeting") &
        (df["Komponente"] != "Verzeichnis") &
        (~df["Ereignisname"].str.contains("angezeigt")) &
        (~df["Ereignisname"].str.contains("heruntergeladen"))
        ]

    df.to_csv(get_out_file_name(input_file_name), index=False)
    filtered_df.to_csv(get_filtered_out_file_name(input_file_name), index=False)

    print("[+] Done")


def processFile(filename):
    print(filename)
    df = None
    try:
        df = pd.read_csv(filename,  encoding='utf-8')
        print(f"The file can be opened with pandas.read_csv().")
        parseFile(df, filename)
    except pd.errors.EmptyDataError:
        print(f"The file is empty.")
    except pd.errors.ParserError:
        print(f"The file cannot be opened with pandas.read_csv(). It may have a formatting issue.")
    except Exception as e:
        print(f"An error occurred while opening the file: {str(e)}")


filename = "moodle_log_scripting_anonymized.csv"
"""if(len(sys.argv) > 1):
    filename = sys.argv[1]
    processFile(filename)
else:
    print(f"Usage: {sys.argv[0]} <logfile>")
    processFile(filename)
"""
processFile(filename)