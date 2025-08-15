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

#include "json.hpp"
#include "capture.hpp"

#ifdef UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

class DOWNLOAD
{
private:
    static tstring toTString(std::string s);

public:
    static nlohmann::json getUpdateData(CAPTURE::CAPTURE_RESULT captureResult);
    static void downloadFile(std::string url, std::string filename);
};
