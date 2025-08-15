#pragma once 

#include <string>
#include <csignal>
#include <windows.h>
#include "io.hpp"

class CORE
{
private:
    static void SignalHandler(int signum);
    static BOOL IsAdmin();

public:
    static void BindSignal();
    static void Header();
    static void ElevateNow();
};
