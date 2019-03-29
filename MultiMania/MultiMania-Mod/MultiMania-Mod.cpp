// MultiMania-Mod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ManiaModLoader.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>

using namespace SonicMania;

extern "C"
{

    static HANDLE MultiManiaCS;

    __declspec(dllexport) void OnFrame()
    {
    }

    __declspec(dllexport) void Init(const char *path)
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        SetCurrentDirectoryA(path);

        MultiManiaCS = LoadLibrary("MultiMania.dll");

        SetCurrentDirectoryA(buffer);

    }

}
