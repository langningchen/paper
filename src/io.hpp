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
#include <iostream>
#include <windows.h>
#include <limits>

class IO
{
private:
    static void SetColor(WORD color);

public:
    static BOOL Confirm(std::string message);
    static void Input(std::string message, std::string &input);
    static void Debug(std::string message);
    static void Info(std::string message);
    static void Warn(std::string message);
    static void Error(std::string message);
    static void ShowProgress(double percentage, size_t downloaded, size_t total);
    static void FlushProgress();
};
