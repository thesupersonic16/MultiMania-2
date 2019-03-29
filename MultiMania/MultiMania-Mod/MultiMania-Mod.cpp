// MultiMania-Mod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ManiaModLoader.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include "MultiMania.h"

using namespace SonicMania;

extern "C"
{

    __declspec(dllexport) void Test(const char *code)
    {
        printf(code);
    }

    bool test = false;
    __declspec(dllexport) void OnFrame()
    {
        if (!test)
        {
            test = true;
            InitMultiMania();
        }
    }

    __declspec(dllexport) void Init(const char *path)
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        SetCurrentDirectoryA(path);
        MultiManiaCS = LoadLibrary("MultiMania.dll");
        SetCurrentDirectoryA(buffer);
        LoadExports();

    }

    __declspec(dllexport) ModInfo ManiaModInfo = { ModLoaderVer, GameVer };

}
