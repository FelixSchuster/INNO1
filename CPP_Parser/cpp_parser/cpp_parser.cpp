#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

// C++ Parser for Moodle-Logfiles containing common HTML/XHTML Entities
// Prepares Data to be used for Celonis Process Mining
// Felix Schuster | 2022-11-30

std::string decodeHtmlEntities(std::string string);
std::vector<std::string> splitString(std::string string, char delimiter);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <logfile>" << std::endl;
        return -1;
    }

    std::ifstream ifstream = std::ifstream();
    std::ofstream ofstream = std::ofstream();
   
    ifstream.open(argv[1]);

    if (!ifstream.is_open())
    {
        std::cout << "[!] Can not open File: " << argv[1] << std::endl;
        return -1;
    }

    ofstream.open(std::string(argv[1]) + ".celonis");

    if(!ofstream.is_open())
    {
        std::cout << "[!] Can not open File: " << argv[1] << ".celonis" << std::endl;
        ifstream.close();
        return -1;
    }

    std::string line = std::string();
    bool offsetIsSet = false;
    unsigned long long offset = 0;

    std::cout << "[*] Parsing " << argv[1] << std::endl;

    while (getline(ifstream, line))
    {
        line = decodeHtmlEntities(line);

        bool exportLine = true;
        std::vector<std::string> stringVector = splitString(line, ',');

        if (!offsetIsSet) // Set Offset by searching for Keyword: "Zeit"
        {
            for (int i = 0; i < stringVector.size(); ++i)
            {
                if (stringVector.at(i) == "Zeit")
                {
                    offset = i;
                }
            }

            offsetIsSet = true;
        }

        // Filter Options
        if (exportLine && stringVector.size() >= 1 + offset && stringVector.at(1 + offset) == "User Admin")
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 3 + offset && stringVector.at(3 + offset) == "Forum")
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 3 + offset && stringVector.at(3 + offset) == "System")
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 3 + offset && stringVector.at(3 + offset) == "Gruppenwahl")
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 3 + offset && stringVector.at(3 + offset) == "Zoom Meeting")
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 4 + offset && stringVector.at(4 + offset).find("angezeigt") != std::string::npos)
        {
            exportLine = false;
        }

        if (exportLine && stringVector.size() >= 4 + offset && stringVector.at(4 + offset).find("heruntergeladen") != std::string::npos)
        {
            exportLine = false;
        }

        if (exportLine)
        {
            ofstream << line << std::endl;
        }
    }

    ifstream.close();
    ofstream.close();

    std::cout << "[+] Done" << std::endl;

    return 0;
}

std::string decodeHtmlEntities(std::string string)
{
    // Decodes common HTML/XHTML Entities
    // Returns a String containing decoded Entities

    std::unordered_map<std::string, std::string> map({
        {"&quot,", "\""}, {"&apos,", "'"}, {"&amp,", "&"},
        {"&gt,", ">"}, {"&lt,", "<"}, {"&frasl,", "/"} });

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
    // Splits a String into Substrings given the Delimiter that seperates them
    // Returns a Vector of Substrings
    // Supports Strings containing the given Delimiter marked by Quotes as well
    // Example: "substring1 <delimiter> substring2" 

    std::vector<std::string> result;
    std::stringstream stringStream = std::stringstream(string);
    std::string subString = std::string();
    std::string actualSubString = std::string();

    while (getline(stringStream, subString, delimiter))
    {
        if (actualSubString == std::string() && subString.size() >= 1 && subString.at(0) == '"')
        {
            actualSubString = subString + ",";
        }

        else if (actualSubString != std::string())
        {
            actualSubString += subString;

            if (actualSubString.back() != '"')
            {
                actualSubString += ",";
            }

            else
            {
                if (actualSubString.at(0) == '"')
                {
                    actualSubString.erase(actualSubString.begin());
                }

                if (actualSubString.length() >= 1 && actualSubString.at(actualSubString.length() - 1) == '"')
                {
                    actualSubString.erase(actualSubString.length() - 1);
                }

                result.push_back(actualSubString);
                actualSubString = std::string();
            }
        }

        else
        {
            result.push_back(subString);
        }
    }

    return result;
}
