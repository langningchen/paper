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

#include "httpResponse.hpp"
#include "io.hpp"
#include "i18n.hpp"

void HTTP_RESPONSE::setBody(std::string bodyContent)
{
    IO::Debug(t("setting_response_body") + " (" + std::to_string(bodyContent.length()) + " " + t("bytes") + ")");
    body = bodyContent;
    headers["Content-Length"] = std::to_string(body.length());
    IO::Debug(t("content_length_set") + ": " + std::to_string(body.length()));
}
std::string HTTP_RESPONSE::headerToString()
{
    IO::Debug(t("generating_response_header"));
    IO::Debug(t("response_status_code") + ": " + std::to_string(statusCode));

    std::string header = "HTTP/1.1 " +
                         std::to_string(statusCode) + " " +
                         statusCodes.at(statusCode) + "\r\n";

    for (auto [key, value] : headers)
    {
        header += key + ": " + value + "\r\n";
        IO::Debug(t("adding_response_header") + ": " + key + " = " + value);
    }

    IO::Debug(t("response_header_generated") + " (" + std::to_string(header.length()) + " " + t("bytes") + ")");
    return header;
}
std::string HTTP_RESPONSE::toString()
{
    IO::Debug(t("generating_full_response"));
    std::string fullResponse = headerToString() + "\r\n" + body;
    IO::Debug(t("response_size") + ": " + std::to_string(fullResponse.length()) + " " + t("bytes"));
    return fullResponse;
}
