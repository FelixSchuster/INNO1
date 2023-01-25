#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

// A C++ parser for moodle-logfiles containing common HTML/XHTML entities
// Prepares data to be used for celonis process mining
// Felix Schuster | 2022-11-30

const std::string FILE_EXTENSION = "_celonis.csv";
const std::string FILTERED_FILE_EXTENSION = "_filtered_celonis.csv";

std::string getOutFileName(std::string inFileName);
std::string getFilteredOutFileName(std::string inFileName);
std::string decodeHtmlEntities(std::string string);
std::vector<std::string> splitString(std::string string, char delimiter);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <logfile>" << std::endl;
        return -1;
    }

    std::ifstream ifstream = std::ifstream();
    std::ofstream ofstream = std::ofstream();
    std::ofstream filteredOfstream = std::ofstream();

    ifstream.open(argv[1]);

    if (!ifstream.is_open())
    {
        std::cout << "[!] Can not open file: " << argv[1] << std::endl;
        return -1;
    }

    ofstream.open(getOutFileName(argv[1]));

    if (!ofstream.is_open())
    {
        std::cout << "[!] Can not open file: " << getOutFileName(argv[1]) << std::endl;
        ifstream.close();
        return -1;
    }

    std::string line = std::string();
    bool indicesAreSet = false;
    bool isActivitiesFile = true;
    unsigned long long zeitIndex = ULLONG_MAX;
    unsigned long long vollstaendigerNameIndex = ULLONG_MAX;
    unsigned long long ereigniskontextIndex = ULLONG_MAX;
    unsigned long long ereignisnameIndex = ULLONG_MAX;
    unsigned long long komponenteIndex = ULLONG_MAX;

    std::cout << "[*] Parsing " << argv[1] << std::endl;
    std::cout << "[*] Writing to file: " << getOutFileName(argv[1]) << std::endl;

    while (getline(ifstream, line))
    {
        line = decodeHtmlEntities(line);

        bool exportLine = true;
        std::vector<std::string> stringVector = splitString(line, ',');

        if (isActivitiesFile && !indicesAreSet)
        {
            for (int i = 0; i < stringVector.size(); ++i)
            {
                if (stringVector.at(i) == "Zeit")
                {
                    zeitIndex = i;
                }

                if (stringVector.at(i) == "Vollständiger Name")
                {
                    vollstaendigerNameIndex = i;
                }

                if (stringVector.at(i) == "Ereigniskontext")
                {
                    ereigniskontextIndex = i;
                }

                if (stringVector.at(i) == "Komponente")
                {
                    komponenteIndex = i;
                }

                if (stringVector.at(i) == "Ereignisname")
                {
                    ereignisnameIndex = i;
                }
            }

            std::cout << "zeitIndex: " << zeitIndex << std::endl;
            std::cout << "vollstaendigerNameIndex: " << vollstaendigerNameIndex << std::endl;
            std::cout << "ereigniskontextIndex: " << ereigniskontextIndex << std::endl;
            std::cout << "komponenteIndex: " << komponenteIndex << std::endl;
            std::cout << "ereignisnameIndex: " << ereignisnameIndex << std::endl;

            if (zeitIndex != ULLONG_MAX && vollstaendigerNameIndex != ULLONG_MAX && ereigniskontextIndex != ULLONG_MAX && komponenteIndex != ULLONG_MAX && ereignisnameIndex != ULLONG_MAX)
            {
                indicesAreSet = true;

                filteredOfstream.open(getFilteredOutFileName(argv[1]));

                if (!filteredOfstream.is_open())
                {
                    std::cout << "[!] Can not open file: " << getFilteredOutFileName(argv[1]) << std::endl;
                    ifstream.close();
                    return -1;
                }

                std::cout << "[*] Writing to file: " << getFilteredOutFileName(argv[1]) << std::endl;
            }

            else
            {
                isActivitiesFile = false;

                std::cout << "[-] Unknown pattern. " << argv[0] << " will try to parse HTML/XHTML characters anyway." << std::endl;
            }
        }

        if (isActivitiesFile && indicesAreSet)
        {
            if (exportLine && stringVector.size() >= vollstaendigerNameIndex && stringVector.at(vollstaendigerNameIndex) == "User Admin")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= ereigniskontextIndex && stringVector.at(ereigniskontextIndex).find("(Kopie)") != std::string::npos)
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= komponenteIndex && stringVector.at(komponenteIndex) == "Forum")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= komponenteIndex && stringVector.at(komponenteIndex) == "System")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= komponenteIndex && stringVector.at(komponenteIndex) == "Gruppenwahl")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= komponenteIndex && stringVector.at(komponenteIndex) == "Zoom Meeting")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= komponenteIndex && stringVector.at(komponenteIndex) == "Verzeichnis")
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= ereignisnameIndex && stringVector.at(ereignisnameIndex).find("angezeigt") != std::string::npos)
            {
                exportLine = false;
            }

            if (exportLine && stringVector.size() >= ereignisnameIndex && stringVector.at(ereignisnameIndex).find("heruntergeladen") != std::string::npos)
            {
                exportLine = false;
            }

            if (exportLine)
            {
                filteredOfstream << stringVector.at(zeitIndex) << ","
                    << stringVector.at(vollstaendigerNameIndex) << ","
                    << stringVector.at(ereigniskontextIndex) << " - "
                    << stringVector.at(ereignisnameIndex) << std::endl;
            }
        }

        if (isActivitiesFile)
        {
            ofstream << stringVector.at(zeitIndex) << ","
                << stringVector.at(vollstaendigerNameIndex) << ","
                << stringVector.at(ereigniskontextIndex) << " - "
                << stringVector.at(ereignisnameIndex) << std::endl;
        }

        else
        {
            ofstream << line << std::endl;
        }
    }

    ifstream.close();
    ofstream.close();
    filteredOfstream.close();

    std::cout << "[+] Done" << std::endl;

    return 0;
}

std::string decodeHtmlEntities(std::string string)
{
    // Decodes common HTML/XHTML entities
    // Returns a string containing decoded entities

    std::unordered_map<std::string, std::string> map({
        {"&quot,", "\""}, {"&apos,", "'"}, {"&amp,", "&"},
        {"&gt,", ">"}, {"&lt,", "<"}, {"&frasl,", "/"},
        {"-,", "0,"} });

    std::string result = std::string();

    for (int i = 0; i < string.size(); ++i)
    {
        bool substringFound = false;

        for (auto& pair : map)
        {
            std::string key = pair.first;
            std::string value = pair.second;

            if (i + key.size() - 1 < string.size())
            {
                if (string.substr(i, key.size()) == key)
                {
                    result += value;
                    i += (int)key.size() - 1;
                    substringFound = true;
                    break;
                }
            }
        }

        if (!substringFound)
        {
            result += string[i];
        }
    }

    return result;
}

std::vector<std::string> splitString(std::string string, char delimiter)
{
    // Splits a string into substrings given the delimiter that seperates them
    // Returns a vector of substrings
    // Supports strings containing the given delimiter marked by quotes as well
    // Example: "substring1 <delimiter> substring2" 

    std::vector<std::string> result;
    std::stringstream stringStream = std::stringstream(string);
    std::string subString = std::string();
    std::string subStringInQuotes = std::string();
    bool subStringInQuotesDetected = false;
    bool pushToResult = true;

    while (getline(stringStream, subString, delimiter))
    {
        if (subString.length() > 0 && subString.at(0) == '"' || subStringInQuotesDetected)
        {
            if (!subStringInQuotesDetected)
            {
                subString.erase(subString.begin());
            }

            subStringInQuotesDetected = true;
            subStringInQuotes += subString;

            if (subString.back() == '"')
            {
                subStringInQuotes.pop_back();

                result.push_back(subStringInQuotes);
                subStringInQuotesDetected = false;
                subStringInQuotes = std::string();
                pushToResult = false;
            }
        }

        if (pushToResult && !subStringInQuotesDetected && subStringInQuotes == std::string())
        {
            result.push_back(subString);
        }

        if (!pushToResult)
        {
            pushToResult = true;
        }
    }

    return result;
}

std::string getOutFileName(std::string inFileName)
{
    // Converts inFileName to outFileName
    // Example: <filename>.csv will return <filename>_celonis.csv

    std::string outFileName = std::string();
    std::vector<std::string> outFileNameVector = splitString(inFileName, '.');

    if (outFileNameVector.size() == 1)
    {
        return inFileName + FILE_EXTENSION;
    }

    for (int i = 0; i < outFileNameVector.size() - 1; ++i)
    {
        outFileName += outFileNameVector.at(i);
    }

    return outFileName + FILE_EXTENSION;
}

std::string getFilteredOutFileName(std::string inFileName)
{
    // Converts inFileName to outFileName
    // Example: <filename>.csv will return <filename>_filtered_celonis.csv

    std::string outFileName = std::string();
    std::vector<std::string> outFileNameVector = splitString(inFileName, '.');

    if (outFileNameVector.size() == 1)
    {
        return inFileName + FILTERED_FILE_EXTENSION;
    }

    for (int i = 0; i < outFileNameVector.size() - 1; ++i)
    {
        outFileName += outFileNameVector.at(i);
    }

    return outFileName + FILTERED_FILE_EXTENSION;
}
