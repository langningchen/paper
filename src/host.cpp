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

#include "host.hpp"
#include "define.hpp"
#include "io.hpp"
#include "i18n.hpp"
#include <windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

const std::string HOST::HOST_ENTRY = "192.168.137.1 iotapi.abupdate.com";
const std::string HOST::HOSTS_FILE_PATH = "C:\\Windows\\System32\\drivers\\etc\\hosts";

bool HOST::ReadHostsFile(std::vector<std::string> &lines)
{
    IO::Debug(t("opening_hosts_file") + ": " + HOSTS_FILE_PATH);
    std::ifstream file(HOSTS_FILE_PATH);
    if (!file.is_open())
        DIE(t("cannot_open_hosts_file"));
    IO::Debug(t("reading_hosts_file"));
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);
    file.close();
    return true;
}

bool HOST::WriteHostsFile(const std::vector<std::string> &lines)
{
    IO::Debug(t("writing_hosts_file"));
    std::ofstream file(HOSTS_FILE_PATH);
    if (!file.is_open())
        DIE(t("failed_write_hosts_file"));
    for (const auto &line : lines)
        file << line << std::endl;
    file.close();
    IO::Debug(t("hosts_file_updated"));
    return true;
}

bool HOST::ContainsHostEntry(const std::vector<std::string> &lines)
{
    for (const auto &line : lines)
        if (line.find("iotapi.abupdate.com") != std::string::npos &&
            line.find("192.168.137.1") != std::string::npos)
            return true;
    return false;
}

void HOST::RemoveHostEntry(std::vector<std::string> &lines)
{
    lines.erase(
        std::remove_if(lines.begin(), lines.end(),
                       [](const std::string &line)
                       {
                           return line.find("iotapi.abupdate.com") != std::string::npos &&
                                  line.find("192.168.137.1") != std::string::npos;
                       }),
        lines.end());
}
void HOST::FlushDnsCache()
{
    IO::Debug(t("flushing_dns_cache"));
    IO::Debug(t("loading_dnsapi_dll"));
    HMODULE hDnsApi = LoadLibraryA("dnsapi.dll");
    if (hDnsApi == NULL)
        DIE(t("failed_load_dnsapi"));
    typedef BOOL(WINAPI * DnsFlushResolverCacheFunc)();
    DnsFlushResolverCacheFunc DnsFlushResolverCache =
        (DnsFlushResolverCacheFunc)GetProcAddress(hDnsApi, "DnsFlushResolverCache");
    if (DnsFlushResolverCache == NULL)
    {
        FreeLibrary(hDnsApi);
        DIE(t("failed_get_dns_function"));
    }
    BOOL result = DnsFlushResolverCache();
    FreeLibrary(hDnsApi);
    if (!result)
        DIE(t("failed_flush_dns_cache"));
    IO::Debug(t("dns_cache_flushed"));
}

void HOST::enable()
{
    IO::Debug(t("enabling_host_redirect"));
    std::vector<std::string> lines;
    ReadHostsFile(lines);
    if (ContainsHostEntry(lines))
    {
        IO::Warn(t("host_entry_already_exists"));
        return;
    }
    IO::Debug(t("adding_host_entry") + ": " + HOST_ENTRY);
    lines.push_back(HOST_ENTRY);
    WriteHostsFile(lines);
    IO::Info(t("host_entry_added"));
    FlushDnsCache();
}

void HOST::disable()
{
    IO::Debug(t("disabling_host_redirect"));
    std::vector<std::string> lines;
    ReadHostsFile(lines);
    if (!ContainsHostEntry(lines))
    {
        IO::Warn(t("host_entry_not_found"));
        return;
    }
    IO::Debug(t("removing_host_entry"));
    RemoveHostEntry(lines);
    WriteHostsFile(lines);
    IO::Info(t("host_entry_removed"));
    FlushDnsCache();
}
