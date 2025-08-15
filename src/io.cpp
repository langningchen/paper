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

#include "io.hpp"
#include "i18n.hpp"
#include "argc.hpp"
#include <conio.h>
#include <iomanip>
#include <chrono>
#include <cmath>

void IO::SetColor(WORD color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

BOOL IO::Confirm(std::string message)
{
    SetColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << t("confirm_prefix") << " " << message << " " << t("confirm_suffix") << " " << std::flush;
    char res = _getch();
    std::cout << res << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return (res == 'y' || res == 'Y');
}

void IO::Input(std::string message, std::string &input)
{
    SetColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << t("input_prefix") << " " << message << std::flush;
    std::getline(std::cin, input);
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::Debug(std::string message)
{
    if (!ARGC::HasArg("verbose"))
        return;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << t("debug_prefix") << " " << message << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::Info(std::string message)
{
    SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << t("info_prefix") << " " << message << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::Warn(std::string message)
{
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << t("warn_prefix") << " " << message << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::Error(std::string message)
{
    SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << t("error_prefix") << " " << message << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::ShowProgress(double percentage, size_t downloaded, size_t total)
{
    static double lastPercentage = -1;
    static auto lastUpdateTime = std::chrono::steady_clock::now();

    auto now = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();

    if (percentage < 100.0 && timeDiff < 100 && std::abs(percentage - lastPercentage) < 1.0)
        return;

    lastPercentage = percentage;
    lastUpdateTime = now;

    int barWidth = 50;
    int filledWidth = static_cast<int>(percentage * barWidth / 100.0);

    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < filledWidth)
            std::cout << "=";
        else if (i == filledWidth && percentage < 100.0)
            std::cout << ">";
        else
            std::cout << " ";
    }

    std::string downloadedStr, totalStr;
    if (downloaded >= 1024 * 1024)
        downloadedStr = std::to_string(downloaded / (1024 * 1024)) + " MB";
    else if (downloaded >= 1024)
        downloadedStr = std::to_string(downloaded / 1024) + " KB";
    else
        downloadedStr = std::to_string(downloaded) + " B";

    if (total >= 1024 * 1024)
        totalStr = std::to_string(total / (1024 * 1024)) + " MB";
    else if (total >= 1024)
        totalStr = std::to_string(total / 1024) + " KB";
    else
        totalStr = std::to_string(total) + " B";

    std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% ("
              << downloadedStr << "/" << totalStr << ")" << std::flush;

    if (percentage >= 100.0)
        std::cout << std::endl;

    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void IO::FlushProgress()
{
    std::cout << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}
