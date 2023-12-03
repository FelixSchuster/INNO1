import sys
import csv
import codecs

FILE_EXTENSION = "_celonis.csv"
FILTERED_FILE_EXTENSION = "_filtered_celonis.csv"


def decode_html_entities(string):
    # Decodes common HTML/XHTML entities
    # Returns a string containing decoded entities

    html_entities = {
        "&quot;": "\"", "&apos;": "'", "&amp;": "&",
        "&gt;": ">", "&lt;": "<", "&frasl;": "/",
        "-,": "0,"
    }

    result = ""
    i = 0

    while i < len(string):
        substring_found = False
        for key, value in html_entities.items():
            if string[i:i+len(key)] == key:
                result += value
                i += len(key)
                substring_found = True
                break
        if not substring_found:
            result += string[i]
            i += 1

    return result

def split_string(string, delimiter):
    # Splits a string into substrings given the delimiter that separates them
    # Returns a list of substrings
    # Supports strings containing the given delimiter marked by quotes as well
    # Example: "substring1 <delimiter> substring2"

    result = []
    string_stream = codecs.open(string, "r", "utf-8")
    sub_string = ""
    sub_string_in_quotes = ""
    sub_string_in_quotes_detected = False
    push_to_result = True

    for char in string:
        if (len(sub_string) > 0 and sub_string[0] == '"') or sub_string_in_quotes_detected:
            if not sub_string_in_quotes_detected:
                sub_string = sub_string[1:]
            sub_string_in_quotes_detected = True
            sub_string_in_quotes += sub_string

            if sub_string[-1] == '"':
                sub_string_in_quotes = sub_string_in_quotes[:-1]

                result.append(sub_string_in_quotes)
                sub_string_in_quotes_detected = False
                sub_string_in_quotes = ""
                push_to_result = False

        if push_to_result and not sub_string_in_quotes_detected and sub_string_in_quotes == "":
            result.append(sub_string)

        if not push_to_result:
            push_to_result = True

    return result

def get_out_file_name(in_file_name):
    # Converts in_file_name to out_file_name
    # Example: <filename>.csv will return <filename>_celonis.csv

    out_file_name = ""
    out_file_name_vector = in_file_name.split('.')

    if len(out_file_name_vector) == 1:
        return in_file_name + FILE_EXTENSION

    for i in range(len(out_file_name_vector) - 1):
        out_file_name += out_file_name_vector[i]

    return out_file_name + FILE_EXTENSION

def get_filtered_out_file_name(in_file_name):
    # Converts in_file_name to out_file_name
    # Example: <filename>.csv will return <filename>_filtered_celonis.csv

    out_file_name = ""
    out_file_name_vector = in_file_name.split('.')

    if len(out_file_name_vector) == 1:
        return in_file_name + FILTERED_FILE_EXTENSION

    for i in range(len(out_file_name_vector) - 1):
        out_file_name += out_file_name_vector[i]

    return out_file_name + FILTERED_FILE_EXTENSION

def main(argv):
    if len(argv) != 2:
        print("Usage:", argv[0], "<logfile>")
        return -1

    in_file_name = argv[1]

    try:
        with open(in_file_name, 'r', encoding='utf-8') as infile, open(get_out_file_name(in_file_name), 'w', encoding='utf-8') as outfile, open(get_filtered_out_file_name(in_file_name), 'w', encoding='utf-8') as filtered_outfile:
            print("[*] Parsing", in_file_name)
            print("[*] Writing to file:", get_out_file_name(in_file_name))
            line = infile.readline()
            indices_are_set = False
            is_activities_file = True
            zeit_index = vollstaendiger_name_index = ereigniskontext_index = ereignisname_index = komponente_index = float('inf')

            while line:
                line = decode_html_entities(line)
                export_line = True
                string_vector = split_string(line, ',')

                if is_activities_file and not indices_are_set:
                    for i, value in enumerate(string_vector):
                        if value == "Zeit":
                            zeit_index = i
                        if value == "Vollständiger Name":
                            vollstaendiger_name_index = i
                        if value == "Ereigniskontext":
                            ereigniskontext_index = i
                        if value == "Komponente":
                            komponente_index = i
                        if value == "Ereignisname":
                            ereignisname_index = i

                    if zeit_index != float('inf') and vollstaendiger_name_index != float('inf') and ereigniskontext_index != float('inf') and komponente_index != float('inf') and ereignisname_index != float('inf'):
                        indices_are_set = True
                        print("[*] Writing to file:", get_filtered_out_file_name(in_file_name))

                    else:
                        is_activities_file = False
                        print("[-] Unknown pattern. ", argv[0], " will try to parse HTML/XHTML characters anyway.")

                if is_activities_file and indices_are_set:
                    if export_line and len(string_vector) >= vollstaendiger_name_index and string_vector[vollstaendiger_name_index] == "User Admin":
                        export_line = False
                    if export_line and len(string_vector) >= ereigniskontext_index and "(Kopie)" in string_vector[ereigniskontext_index]:
                        export_line = False
                    if export_line and len(string_vector) >= komponente_index and string_vector[komponente_index] in ["Forum", "System", "Gruppenwahl", "Zoom Meeting", "Verzeichnis"]:
                        export_line = False
                    if export_line and len(string_vector) >= ereignisname_index and "angezeigt" in string_vector[ereignisname_index]:
                        export_line = False
                    if export_line and len(string_vector) >= ereignisname_index and "heruntergeladen" in string_vector[ereignisname_index]:
                        export_line = False

                    if export_line:
                        filtered_outfile.write(f"{string_vector[zeit_index]},{string_vector[vollstaendiger_name_index]},{string_vector[ereigniskontext_index]} - {string_vector[ereignisname_index]}\n")

                if is_activities_file:
                    outfile.write(f"{string_vector[zeit_index]},{string_vector[vollstaendiger_name_index]},{string_vector[ereigniskontext_index]} - {string_vector[ereignisname_index]}\n")
                else:
                    outfile.write(line)

                line = infile.readline()

            print("[+] Done")

    except IOError:
        print("[!] Can not open file:", in_file_name)
        return -1

if __name__ == "__main__":
    sys.exit(main(sys.argv))
