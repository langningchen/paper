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
#include <regex>
#include "json.hpp"
#include "network_headers.hpp"
#include "io.hpp"
#include <pcap/pcap.h>

class CAPTURE
{
public:
    struct CAPTURE_RESULT
    {
        std::string productUrl;
        nlohmann::json request_body;
    };

    static void capture(CAPTURE_RESULT &result);

private:
    static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
    static CAPTURE_RESULT *global_capture_result;
    static pcap_t *global_pcap_handle;
    static bool IsWantedRequest_NetworkLayer(const u_char **buf, int &len);
    static bool IsWantedRequest_TransportLayer(const u_char **buf, int &len);
    static bool IsWantedRequest(const u_char *pkt_data, int data_len, CAPTURE_RESULT &result);
};
