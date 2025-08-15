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

#include "download.hpp"
#include "io.hpp"
#include "i18n.hpp"
#include "define.hpp"
#include <wininet.h>
#include <fstream>

tstring DOWNLOAD::toTString(std::string s) { return tstring(s.begin(), s.end()); }

nlohmann::json DOWNLOAD::getUpdateData(CAPTURE::CAPTURE_RESULT captureResult)
{
    IO::Debug(t("preparing_modified_request"));
    nlohmann::json modifiedBody = captureResult.request_body;
    modifiedBody["version"] = "99.99.90";
    modifiedBody["networkType"] = "WIFI";
    IO::Debug(t("modified_version_to") + ": " + std::string(modifiedBody["version"]));

    IO::Debug(t("initializing_wininet"));
    HINTERNET hInternet = InternetOpen(TEXT(""), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    IO::Debug(t("connecting_to_server"));
    HINTERNET hConnect = InternetConnect(hInternet, TEXT("iotapi.abupdate.com"), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    IO::Debug(t("opening_http_request_for") + ": " + captureResult.productUrl);
    HINTERNET hRequest = HttpOpenRequest(hConnect, TEXT("POST"), toTString(captureResult.productUrl).c_str(), NULL, NULL, NULL, 0, 0);
    HttpAddRequestHeaders(hRequest, TEXT("Content-Type: application/json;charset=UTF-8"), -1, HTTP_ADDREQ_FLAG_ADD);

    std::string bodyStr = modifiedBody.dump();
    DWORD bodyLength = static_cast<DWORD>(bodyStr.length());
    IO::Debug(t("request_body_size") + ": " + std::to_string(bodyLength) + " " + t("bytes"));

    IO::Debug(t("sending_http_request"));
    HttpSendRequest(hRequest, NULL, 0, (LPVOID)bodyStr.c_str(), bodyLength);

    IO::Debug(t("reading_http_response"));
    std::string response;
    char buffer[4096];
    DWORD dwRead = 0;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer), &dwRead) && dwRead > 0)
        response.append(buffer, dwRead);

    IO::Debug(t("response_size") + ": " + std::to_string(response.length()) + " " + t("bytes"));
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    IO::Debug(t("parsing_json_response"));
    nlohmann::json responseJson = nlohmann::json::parse(response);
    IO::Debug(t("json_response_parsed"));
    IO::Debug(t("response_json") + ": " + responseJson.dump(2, ' '));
    return responseJson;
}
void DOWNLOAD::downloadFile(std::string url, std::string filename)
{
    if (std::filesystem::exists(filename) && IO::Confirm(t("file_exists_skip_download")))
        return;

    IO::Info(t("downloading_image_file"));

    HINTERNET hInternet = InternetOpen(TEXT(""), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        DIE(t("failed_initialize_wininet"));

    HINTERNET hUrl = InternetOpenUrl(hInternet, toTString(url).c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl)
        DIE(t("failed_open_url"));

    DWORD contentLength = 0;
    DWORD bufferSize = sizeof(contentLength);
    HttpQueryInfo(hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                  &contentLength, &bufferSize, NULL);

    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open())
        DIE(t("failed_create_output_file"));

    char buffer[1024 * 1024];
    DWORD bytesRead = 0;
    size_t totalDownloaded = 0;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
    {
        outFile.write(buffer, bytesRead);
        totalDownloaded += bytesRead;

        if (contentLength > 0)
        {
            double percentage = (static_cast<double>(totalDownloaded) / contentLength) * 100.0;
            IO::ShowProgress(percentage, totalDownloaded, contentLength);
        }
    }

    outFile.close();
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
}
