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

#include <vector>
#include <string>

class HOST
{
private:
    static const std::string HOST_ENTRY;
    static const std::string HOSTS_FILE_PATH;

    static bool ReadHostsFile(std::vector<std::string> &lines);
    static bool WriteHostsFile(const std::vector<std::string> &lines);
    static bool ContainsHostEntry(const std::vector<std::string> &lines);
    static void RemoveHostEntry(std::vector<std::string> &lines);
    static void FlushDnsCache();

public:
    static void enable();
    static void disable();
};
