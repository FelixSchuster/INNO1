#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

// C++ Parser for Files containing common HTML/XHTML Entities
// Felix Schuster | 2022-11-30

std::string parse(std::string string);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <logfile>" << std::endl;
        return -1;
    }

    std::ifstream ifstream = std::ifstream();
    std::ofstream ofstream = std::ofstream();
   
    ifstream.open(argv[1]);

    if (!ifstream.is_open())
    {
        std::cout << "[!] Can not open file: " << argv[1] << std::endl;
        return -1;
    }

    ofstream.open(std::string(argv[1]) + ".celonis");

    if(!ofstream.is_open())
    {
        std::cout << "[!] Can not open file: " << argv[1] << ".celonis" << std::endl;
        ifstream.close();
        return -1;
    }

    std::string line = std::string();

    std::cout << "[*] Parsing " << argv[1] << std::endl;

    while (getline(ifstream, line))
    {
        ofstream << parse(line) << std::endl;
    }

    ifstream.close();
    ofstream.close();

    std::cout << "[+] Done" << std::endl;

    return 0;
}

std::string parse(std::string string)
{
    // http://www.elizabethcastro.com/html/extras/entities.html

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
