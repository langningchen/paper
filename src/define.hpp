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

#include <conio.h>
#include "i18n.hpp"

#define DIE(msg)                                                            \
    do                                                                      \
    {                                                                       \
        IO::Error(msg);                                                     \
        IO::Debug(t("occurs_at") + " " + std::string(__PRETTY_FUNCTION__) + \
                  " (" + std::to_string(GetLastError()) + ")");             \
        IO::Info(t("press_any_key_exit"));                                  \
        _getch();                                                           \
        exit(1);                                                            \
    } while (0)
