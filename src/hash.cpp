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

#include "define.hpp"
#include "io.hpp"
#include "hash.hpp"
#include "i18n.hpp"
#include <fstream>
#include <picohash.h>
#include <cstring>

bool HASH::isHexChar(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}
std::vector<std::pair<size_t, size_t>> HASH::findHashPatterns(const std::string &filename)
{
    IO::Debug(t("searching_hash_patterns") + ": " + filename);
    std::vector<std::pair<size_t, size_t>> positions;
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        DIE(t("cannot_open_file") + ": " + filename);
    std::streamsize fileSize = file.tellg();
    IO::Debug(t("file_size") + ": " + std::to_string(fileSize) + " " + t("bytes"));
    file.seekg(0, std::ios::beg);

    const size_t bufferSize = 1024 * 1024;
    std::vector<char> buffer(bufferSize);
    size_t totalBytesRead = 0;
    const size_t overlapSize = 70;
    IO::Debug(t("using_buffer_size") + ": " + std::to_string(bufferSize) + " " + t("bytes") + " " + t("with_overlap") + ": " + std::to_string(overlapSize));

    while (totalBytesRead < static_cast<size_t>(fileSize))
    {
        size_t bytesToRead = std::min(bufferSize - overlapSize,
                                      static_cast<size_t>(fileSize) - totalBytesRead);
        if (totalBytesRead > 0)
        {
            std::memmove(buffer.data(), buffer.data() + bufferSize - overlapSize, overlapSize);
            file.read(buffer.data() + overlapSize, bytesToRead);
            bytesToRead += overlapSize;
        }
        else
            file.read(buffer.data(), bytesToRead);
        if (!file && file.gcount() == 0)
            break;
        bytesToRead = file.gcount() + (totalBytesRead > 0 ? overlapSize : 0);

        for (size_t i = 0; i < bytesToRead - overlapSize; i++)
        {
            if (buffer[i] == '#' && i + 67 <= bytesToRead &&
                isValidHashSequence(buffer.data(), i + 1, bytesToRead, 64) &&
                buffer[i + 65] == ' ' && buffer[i + 66] == ' ' && buffer[i + 67] == '-')
            {
                size_t absolutePos = totalBytesRead + i + 1 - (totalBytesRead > 0 ? overlapSize : 0);
                IO::Debug(t("found_sha256_hash_at") + ": " + std::to_string(absolutePos));
                positions.push_back({absolutePos, 64});
            }
            if (buffer[i] == '=' && i + 38 <= bytesToRead && buffer[i + 1] == ' ' && buffer[i + 2] == '"' &&
                isValidHashSequence(buffer.data(), i + 3, bytesToRead, 32) &&
                buffer[i + 35] == ' ' && buffer[i + 36] == ' ' && buffer[i + 37] == '-' && buffer[i + 38] == '"')
            {
                size_t absolutePos = totalBytesRead + i + 3 - (totalBytesRead > 0 ? overlapSize : 0);
                IO::Debug(t("found_md5_hash_at") + ": " + std::to_string(absolutePos));
                positions.push_back({absolutePos, 32});
            }
        }

        totalBytesRead += bytesToRead - (totalBytesRead > 0 ? overlapSize : 0);
    }
    file.close();
    IO::Debug(t("hash_pattern_search_completed") + " " + std::to_string(positions.size()) + " " + t("patterns"));
    return positions;
}
bool HASH::isValidHashSequence(const char *data, size_t pos, size_t dataSize, size_t hashLength)
{
    if (pos + hashLength > dataSize)
        return false;
    for (size_t i = 0; i < hashLength; ++i)
    {
        if (!isHexChar(data[pos + i]))
            return false;
    }
    return true;
}
std::string HASH::toHex(const unsigned char *data, size_t length)
{
    std::string hexString;
    hexString.reserve(length * 2);
    for (size_t i = 0; i < length; ++i)
    {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", data[i]);
        hexString.append(hex);
    }
    return hexString;
}

std::string HASH::MD5(const std::string &input)
{
    picohash_ctx_t ctx;
    unsigned char digest[PICOHASH_MD5_DIGEST_LENGTH];
    picohash_init_md5(&ctx);
    picohash_update(&ctx, input.c_str(), input.size());
    picohash_final(&ctx, digest);
    return toHex(digest, PICOHASH_MD5_DIGEST_LENGTH);
}
std::string HASH::MD5File(const std::string &filename)
{
    IO::Debug(t("calculating_md5_for_file") + ": " + filename);
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        DIE(t("cannot_open_file") + ": " + filename);

    const size_t bufferSize = 1024 * 1024;
    std::vector<char> buffer(bufferSize);

    picohash_ctx_t ctx;
    picohash_init_md5(&ctx);
    size_t bytesRead = 0;
    size_t totalBytes = 0;
    while (file.read(buffer.data(), bufferSize) || (bytesRead = file.gcount()) > 0)
    {
        picohash_update(&ctx, buffer.data(), bytesRead);
        totalBytes += bytesRead;
    }
    unsigned char digest[PICOHASH_MD5_DIGEST_LENGTH];
    picohash_final(&ctx, digest);
    file.close();
    std::string result = toHex(digest, PICOHASH_MD5_DIGEST_LENGTH);
    IO::Debug(t("md5_calculated_for") + " " + std::to_string(totalBytes) + " " + t("bytes") + ": " + result);
    return result;
}
std::string HASH::MD5FileSegment(const std::string &filename, size_t start, size_t end)
{
    IO::Debug(t("calculating_md5_segment") + ": " + filename + " [" + std::to_string(start) + "-" + std::to_string(end) + "]");
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        DIE(t("cannot_open_file") + ": " + filename);

    const size_t bufferSize = 1024 * 1024;
    std::vector<char> buffer(bufferSize);

    picohash_ctx_t ctx;
    picohash_init_md5(&ctx);
    file.seekg(start, std::ios::beg);
    size_t currentPos = start;
    size_t totalProcessed = 0;
    while (currentPos < end)
    {
        size_t bytesToRead = std::min(bufferSize, end - currentPos);
        file.read(buffer.data(), bytesToRead);
        std::streamsize bytesRead = file.gcount();
        if (bytesRead <= 0)
            break;
        picohash_update(&ctx, buffer.data(), bytesRead);
        currentPos += bytesRead;
        totalProcessed += bytesRead;
    }
    unsigned char digest[PICOHASH_MD5_DIGEST_LENGTH];
    picohash_final(&ctx, digest);
    file.close();
    std::string result = toHex(digest, PICOHASH_MD5_DIGEST_LENGTH);
    IO::Debug(t("segment_md5_calculated") + " " + std::to_string(totalProcessed) + " " + t("bytes") + ": " + result);
    return result;
}
std::string HASH::SHA1File(const std::string &filename)
{
    IO::Debug(t("calculating_sha1_for_file") + ": " + filename);
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        DIE(t("cannot_open_file") + ": " + filename);

    const size_t bufferSize = 1024 * 1024;
    std::vector<char> buffer(bufferSize);

    picohash_ctx_t ctx;
    picohash_init_sha1(&ctx);
    size_t bytesRead = 0;
    size_t totalBytes = 0;
    while (file.read(buffer.data(), bufferSize) || (bytesRead = file.gcount()) > 0)
    {
        picohash_update(&ctx, buffer.data(), bytesRead);
        totalBytes += bytesRead;
    }
    unsigned char digest[PICOHASH_SHA1_DIGEST_LENGTH];
    picohash_final(&ctx, digest);
    file.close();
    std::string result = toHex(digest, PICOHASH_SHA1_DIGEST_LENGTH);
    IO::Debug(t("sha1_calculated_for") + " " + std::to_string(totalBytes) + " " + t("bytes") + ": " + result);
    return result;
}
std::string HASH::SHA256(const std::string &input)
{
    picohash_ctx_t ctx;
    unsigned char digest[PICOHASH_SHA256_DIGEST_LENGTH];
    picohash_init_sha256(&ctx);
    picohash_update(&ctx, input.c_str(), input.size());
    picohash_final(&ctx, digest);
    return toHex(digest, PICOHASH_SHA256_DIGEST_LENGTH);
}

void HASH::replaceHash(const std::string &filename)
{
    IO::Info(t("finding_password"));
    IO::Debug(t("starting_password_search") + ": " + filename);
    const std::vector<std::pair<size_t, size_t>> positions = HASH::findHashPatterns(filename);
    if (positions.empty())
        DIE(t("no_passwords_found"));
    if (positions.size() > 1)
        DIE(t("multiple_password_patterns"));
    IO::Debug(t("found_password_at_offset") + " " + std::to_string(positions[0].first));
    IO::Debug(t("hash_length") + ": " + std::to_string(positions[0].second) + " " + t("characters"));
    std::string newPassword;
    while (newPassword == "")
    {
        IO::Input(t("input_new_password") + ": ", newPassword);
        if (newPassword == "")
            IO::Warn(t("password_cannot_be_empty"));
    }
    IO::Debug(t("generating_hash_for_password"));
    const std::string newHash = (positions[0].second == 32 ? HASH::MD5(newPassword + '\n') : HASH::SHA256(newPassword));
    IO::Debug(t("new_hash_generated") + ": " + newHash);
    FILE *file = fopen(filename.c_str(), "r+b");
    if (!file)
        DIE(t("cannot_open_file") + ": " + filename);
    fseek(file, positions[0].first, SEEK_SET);
    fwrite(newHash.c_str(), 1, newHash.size(), file);
    fclose(file);
    IO::Debug(t("password_hash_replacement_completed"));
}
