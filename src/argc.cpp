// Copyright (C) 2025 Langning Chen
// 
// This file is part of paper.
// 
// paper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// paper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with paper.  If not, see <https://www.gnu.org/licenses/>.

#include "argc.hpp"
#include "io.hpp"
#include "i18n.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

std::map<std::string, std::string> ARGC::args = {};

void ARGC::Initialize(int argc, char *argv[])
{
    parseArguments(argc, argv);
    if (HasArg("help"))
    {
        showHelp();
        exit(0);
    }
}

void ARGC::parseArguments(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--verbose" || arg == "-v")
            args["verbose"] = "true";
        else if (arg == "--help" || arg == "-h")
            args["help"] = "true";
        else
        {
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos)
            {
                std::string key = arg.substr(0, equalPos);
                std::string value = arg.substr(equalPos + 1);

                if (key.substr(0, 2) == "--")
                    key = key.substr(2);
                else if (key.substr(0, 1) == "-")
                    key = key.substr(1);

                args[key] = value;
            }
            else if (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-")
            {
                std::string key = arg;

                if (key.substr(0, 2) == "--")
                    key = key.substr(2);
                else if (key.substr(0, 1) == "-")
                    key = key.substr(1);

                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    args[key] = argv[i + 1];
                    i++;
                }
                else
                    args[key] = "true";
            }
        }
    }
}

void ARGC::showHelp()
{
    std::cout << "paper" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "  paper [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v, --verbose      Enable verbose debug output" << std::endl;
    std::cout << "  -h, --help         Show this help message" << std::endl;
    std::cout << "  --port=<port>      Set HTTP server port (default: 80)" << std::endl;
    std::cout << "  --image=<file>     Set image file name (default: image.img)" << std::endl;
    std::cout << "  --no-capture       Skip packet capture and allow manual input" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  paper --verbose" << std::endl;
    std::cout << "  paper --port=8080 --image=firmware.img" << std::endl;
    std::cout << "  paper -v --port=9000" << std::endl;
    std::cout << "  paper --no-capture" << std::endl;
}

std::string ARGC::GetArg(const std::string &key, const std::string &defaultValue)
{
    auto it = args.find(key);
    if (it != args.end())
        return it->second;
    return defaultValue;
}

bool ARGC::HasArg(const std::string &key)
{
    return args.find(key) != args.end();
}
