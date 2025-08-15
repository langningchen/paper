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

#include "httpServer.hpp"
#include <winsock2.h>
#include "io.hpp"
#include "define.hpp"
#include "i18n.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <filesystem>
#include <thread>
#include <map>
#include <ws2tcpip.h>

const int HTTP_SERVER::BUFFER_SIZE;

HTTP_SERVER::HTTP_SERVER(int port, std::string imagePath, std::string otaData, std::string otaUrl)
    : serverPort(port), imagePath(imagePath), otaData(otaData), otaUrl(otaUrl), isRunning(false), serverSocket(INVALID_SOCKET)
{
}

void HTTP_SERVER::start()
{
    IO::Debug(t("initializing_winsock"));
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        DIE("WSAStartup failed");

    IO::Debug(t("creating_server_socket"));
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
        DIE("Failed to create socket");

    IO::Debug(t("setting_socket_options"));
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    IO::Debug(t("binding_to_port") + " " + std::to_string(serverPort) + "...");
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        DIE("Failed to bind socket");
    IO::Debug(t("starting_listen"));
    if (listen(serverSocket, 16) == SOCKET_ERROR)
        DIE("Failed to listen on socket");

    isRunning = true;
    IO::Info(t("server_started_press_x"));
    IO::Debug(t("server_listening") + " " + std::to_string(serverPort));

    serverThread = std::thread(
        [this]()
        {
            while (isRunning)
            {
                sockaddr_in clientAddr;
                int clientAddrLen = sizeof(clientAddr);
                SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
                if (clientSocket == INVALID_SOCKET)
                {
                    if (isRunning)
                        IO::Warn(t("failed_accept_connection"));
                    continue;
                }

                IO::Debug(t("new_client_connected"));
                std::thread(
                    [this, clientSocket]()
                    {
                        char buffer[BUFFER_SIZE];
                        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
                        if (bytesReceived > 0)
                        {
                            buffer[bytesReceived] = '\0';
                            std::string request(buffer);
                            IO::Debug(t("received_request_bytes") + " " + std::to_string(bytesReceived) + " " + t("bytes"));
                            handleRequest(clientSocket, request);
                        }
                        else
                        {
                            IO::Debug(t("no_data_received"));
                        }
                        closesocket(clientSocket);
                        IO::Debug(t("client_connection_closed"));
                    })
                    .detach();
            }
        });
}

void HTTP_SERVER::stop()
{
    IO::Info(t("stopping_http_server"));
    isRunning = false;

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    if (serverThread.joinable())
        serverThread.join();

    WSACleanup();
}

void HTTP_SERVER::handleRequest(int clientSocket, HTTP_REQUEST request)
{
    IO::Debug(t("processing_http_request") + ": " + request.path);
    IO::Debug(t("http_method") + ": " + request.method);
    if (request.path.length() >= 10 && request.path.substr(0, 10) == "/image.img")
    {
        IO::Info(t("serving_image_file"));
        sendFileResponse(clientSocket, request.headers);
    }
    else if (request.path.length() > 10 && request.path.substr(0, 10) == "/register/")
    {
        IO::Info(t("serving_register_data"));
        sendHttpResponse(clientSocket, 200, "application/json;charset=UTF-8", R"({"status":1000,"msg":"success","data":{"deviceSecret":"de8b9bcd0a18afbf25b44f6d4f6c5f23","sha256":"8a6860050ac879171800a8315fc516b46d6baf81f73910ab1ab5d7e9059d427f","deviceId":"f730c7fa72bd3871"}})");
    }
    else if (request.path == otaUrl + "/checkVersion" && request.method == "POST")
    {
        IO::Info(t("serving_ota_data"));
        sendHttpResponse(clientSocket, 200, "application/json;charset=UTF-8", otaData);
    }
    else if (request.path == otaUrl + "/reportDownResult" && request.method == "POST")
    {
        IO::Info(t("serving_ota_report"));
        sendHttpResponse(clientSocket, 200, "application/json;charset=UTF-8", R"({"status":1000,"msg":"success","data":null})");
    }
    else
    {
        IO::Info(t("request_not_found_404"));
        sendHttpResponse(clientSocket, 404, "text/plain", "File Not Found");
    }
}

void HTTP_SERVER::sendHttpResponse(int clientSocket, int statusCode, std::string contentType,
                                   std::string body, std::string extraHeaders)
{
    HTTP_RESPONSE response;
    response.statusCode = statusCode;
    response.headers["Content-Type"] = contentType;
    response.setBody(body);
    std::string responseString = response.toString();
    if (send(clientSocket, responseString.c_str(), responseString.length(), 0) == SOCKET_ERROR)
        IO::Warn(t("failed_send_response"));
    IO::Debug(t("sent_http_response") + ": " + std::to_string(response.statusCode));
}
void HTTP_SERVER::sendFileResponse(int clientSocket, std::map<std::string, std::string> headers)
{
    IO::Debug(t("preparing_file_response") + ": " + imagePath);
    size_t fileSize = std::filesystem::file_size(imagePath);
    IO::Debug(t("file_size") + ": " + std::to_string(fileSize) + " " + t("bytes"));
    std::ifstream file(imagePath, std::ios::binary);
    if (!file.is_open())
        DIE("Failed to open file: " + imagePath);

    HTTP_RESPONSE responseHeader;

    size_t startPos = 0;
    size_t endPos = fileSize - 1;
    size_t contentLength = fileSize;
    responseHeader.statusCode = 200;

    std::string rangeString = headers["Range"];
    if (rangeString != "")
    {
        IO::Debug(t("processing_range_request") + ": " + rangeString);
        auto range = HTTP_REQUEST::parseRangeHeader(rangeString, fileSize);
        startPos = range.first;
        endPos = range.second;
        contentLength = endPos - startPos + 1;
        IO::Debug(t("range") + ": " + std::to_string(startPos) + "-" + std::to_string(endPos) + " (" + t("length") + ": " + std::to_string(contentLength) + ")");

        responseHeader.statusCode = 206;
        responseHeader.headers["Content-Range"] = "bytes " + std::to_string(startPos) + "-" + std::to_string(endPos) + "/" + std::to_string(fileSize);
    }
    else
    {
        IO::Debug(t("serving_full_file"));
    }

    responseHeader.headers["Content-Type"] = "application/octet-stream";
    responseHeader.headers["Content-Disposition"] = "attachment; filename=\"image.img\"";
    responseHeader.headers["Content-Length"] = std::to_string(contentLength);

    std::string responseHeaderString = responseHeader.headerToString() + "\r\n";
    IO::Debug(t("sending_http_headers") + " (" + std::to_string(responseHeaderString.length()) + " " + t("bytes") + ")");
    if (send(clientSocket, responseHeaderString.c_str(), responseHeaderString.length(), 0) == SOCKET_ERROR)
        IO::Warn(t("failed_send_headers"));

    file.seekg(startPos);

    char buffer[BUFFER_SIZE];
    size_t remainingBytes = contentLength;
    size_t totalSent = 0;
    IO::Debug(t("starting_file_transfer"));

    while (remainingBytes > 0 && (file.read(buffer, std::min((size_t)BUFFER_SIZE, remainingBytes)) || file.gcount() > 0))
    {
        size_t bytesToSend = file.gcount();
        if (send(clientSocket, buffer, bytesToSend, 0) == SOCKET_ERROR)
        {
            IO::Warn(t("failed_send_file_data") + ": " + std::to_string(WSAGetLastError()));
            return;
        }
        remainingBytes -= bytesToSend;
        totalSent += bytesToSend;
    }
    file.close();

    IO::Info(t("sent_image_file") + " (" + std::to_string(startPos) + "~" + std::to_string(endPos) + ")");
    IO::Debug(t("file_transfer_completed") + ": " + std::to_string(totalSent) + " " + t("bytes"));
}
