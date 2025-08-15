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

#pragma once

#include <string>
#include <vector>
#include <map>

class ARGC
{
private:
    static std::map<std::string, std::string> args;

    static void showHelp();
    static void parseArguments(int argc, char *argv[]);

public:
    static void Initialize(int argc, char *argv[]);
    static std::string GetArg(const std::string &key, const std::string &defaultValue = "");
    static bool HasArg(const std::string &key);
};
