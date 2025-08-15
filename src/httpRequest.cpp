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

#include "httpRequest.hpp"
#include "io.hpp"
#include "i18n.hpp"
#include <sstream>
#include <limits>

HTTP_REQUEST::HTTP_REQUEST(std::string data)
{
    IO::Debug(t("parsing_http_request"));
    std::istringstream ss(data);
    ss >> method >> path >> version;
    IO::Debug(t("parsed_method_path_version") + ": " + method + " " + path + " " + version);
    ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    IO::Debug(t("parsing_http_headers"));
    std::string header;
    while (std::getline(ss, header))
    {
        if (!header.empty() && header.back() == '\r')
            header.pop_back();
        if (header.empty())
            break;
        size_t colonPos = header.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = header.substr(0, colonPos);
            std::string value = header.substr(colonPos + 1);
            headers[key] = value;
            IO::Debug(t("parsed_header") + ": " + key + " = " + value);
        }
    }

    IO::Debug(t("parsing_request_body"));
    std::string line;
    while (std::getline(ss, line))
    {
        if (!body.empty())
            body += "\n";
        body += line;
    }

    IO::Debug(t("request_body_size") + ": " + std::to_string(body.length()) + " " + t("bytes"));
    IO::Debug(t("http_request_parsed"));
}

std::pair<size_t, size_t> HTTP_REQUEST::parseRangeHeader(std::string rangeHeader, size_t fileSize)
{
    IO::Debug(t("parsing_range_header") + ": " + rangeHeader);

    size_t bytesPos = rangeHeader.find("bytes=");
    if (bytesPos == std::string::npos)
    {
        IO::Debug(t("range_header_parsed") + ": " + t("serving_full_file"));
        return {0, fileSize - 1};
    }

    std::string rangeValue = rangeHeader.substr(bytesPos + 6);
    size_t dashPos = rangeValue.find('-');

    if (dashPos == std::string::npos)
    {
        IO::Debug(t("range_header_parsed") + ": " + t("serving_full_file") + " (no dash)");
        return {0, fileSize - 1};
    }

    std::string startStr = rangeValue.substr(0, dashPos);
    std::string endStr = rangeValue.substr(dashPos + 1);

    size_t start = 0;
    size_t end = fileSize - 1;

    if (!startStr.empty())
        start = std::stoull(startStr);

    if (!endStr.empty())
        end = std::stoull(endStr);
    else if (!startStr.empty())
        end = fileSize - 1;

    if (start >= fileSize)
        start = fileSize - 1;
    if (end >= fileSize)
        end = fileSize - 1;
    if (start > end)
        start = end;

    IO::Debug(t("range_header_parsed") + ": " + std::to_string(start) + "-" + std::to_string(end) +
              " (" + t("length") + ": " + std::to_string(end - start + 1) + ")");

    return {start, end};
}
