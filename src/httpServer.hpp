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

#include <winsock2.h>
#include <string>
#include <functional>
#include <thread>
#include "httpRequest.hpp"
#include "httpResponse.hpp"

class HTTP_SERVER
{
public:
    HTTP_SERVER(int port, std::string imagePath, std::string otaData, std::string otaUrl);
    void start();
    void stop();

private:
    void handleRequest(int clientSocket, HTTP_REQUEST request);

    void sendHttpResponse(int clientSocket, int statusCode, std::string contentType,
                          std::string body, std::string extraHeaders = "");
    void sendFileResponse(int clientSocket, std::map<std::string, std::string> headers);

    int serverPort;
    std::string imagePath;
    std::string otaData;
    std::string otaUrl;
    std::thread serverThread;
    bool isRunning;
    SOCKET serverSocket;
    static const int BUFFER_SIZE = 8192;
};
