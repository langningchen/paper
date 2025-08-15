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

class HASH
{
private:
    static bool isHexChar(char c);
    static std::vector<std::pair<size_t, size_t>> findHashPatterns(const std::string &filename);
    static bool isValidHashSequence(const char *data, size_t pos, size_t dataSize, size_t hashLength);
    static std::string toHex(const unsigned char *data, size_t length);

public:
    static std::string MD5(const std::string &input);
    static std::string MD5File(const std::string &filename);
    static std::string MD5FileSegment(const std::string &filename, size_t start, size_t end);
    static std::string SHA1File(const std::string &filename);
    static std::string SHA256(const std::string &input);

    static void replaceHash(const std::string &filename);
};
