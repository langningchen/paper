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
#include "capture.hpp"
#include "io.hpp"
#include "core.hpp"
#include "download.hpp"
#include "hash.hpp"
#include "httpServer.hpp"
#include "i18n.hpp"
#include "argc.hpp"
#include "host.hpp"
#include <fstream>
#include <filesystem>

int main(int argc, char *argv[])
{
    ARGC::Initialize(argc, argv);
    const std::string imageFile = ARGC::GetArg("image", "image.img");
    I18N::Initialize();
    IO::Debug(t("app_starting"));

    CORE::BindSignal();
    CORE::Header();
    CORE::ElevateNow();
    CAPTURE::CAPTURE_RESULT result;
    CAPTURE capturer;
    IO::Debug(t("starting_packet_capture"));
    capturer.capture(result);
    // result.productUrl = "/product/1708583443/f730c7fa72bd3871/ota/checkVersion";
    // result.request_body = nlohmann::json::parse(R"({ "timestamp": 1755184821, "sign": "4f2a475cdb69b45f76c5fa3cde2fd4ff", "mid": "7E92000008705369", "productId": "1708583443", "version": "4.7.7", "networkType": "WIFI" })");
    //     result.productUrl = "/product/1700649481/8b2d1ce6a5d9e922/ota/checkVersion";
    //     result.request_body = nlohmann::json::parse(R"({
    // "timestamp": 1727433274,
    //  "sign": "0cb6df0e3b04d2c0d90f4d95cd7c2344",
    //  "mid": "7E90400008506319",
    //  "productId": "1700649481",
    //  "version": "1.0.0",
    //  "networkType": "WIFI"
    // })");
    auto updateData = DOWNLOAD::getUpdateData(result);
    std::string deltaUrl = updateData["data"]["version"]["deltaUrl"];
    IO::Debug(t("delta_url_extracted") + ": " + deltaUrl);
    DOWNLOAD::downloadFile(deltaUrl, imageFile);
    HASH::replaceHash(imageFile);
    IO::Info(t("calculating_hash"));
    auto segmentMd5 = nlohmann::json::parse(std::string(updateData["data"]["version"]["segmentMd5"]));
    for (auto &md5 : segmentMd5)
        md5["md5"] = HASH::MD5FileSegment(imageFile, md5["startpos"], md5["endpos"]);
    updateData["data"]["version"]["segmentMd5"] = segmentMd5.dump();
    IO::Debug(t("calculating_full_md5"));
    updateData["data"]["version"]["md5sum"] = HASH::MD5File(imageFile);
    updateData["data"]["version"]["sha"] = HASH::SHA1File(imageFile);
    updateData["data"]["version"]["deltaUrl"] =
        updateData["data"]["version"]["bakUrl"] =
            "http://192.168.137.1/image.img";

    IO::Debug(updateData.dump(2, ' '));
    HOST::enable();
    HTTP_SERVER httpServer(80, imageFile, updateData.dump(), result.productUrl.substr(0, result.productUrl.find_last_of('/')));
    httpServer.start();
    while (_getch() != 'x')
        ;
    httpServer.stop();
    HOST::disable();
    IO::Debug(t("app_terminating"));
    _getch();
    return 0;
}
