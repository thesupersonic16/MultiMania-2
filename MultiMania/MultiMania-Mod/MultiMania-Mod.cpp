// MultiMania-Mod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ManiaModLoader.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include "MultiMania.h"
#include "MultiManiaMenu.h"

using namespace SonicMania;

DefineMultiManiaFunc(InitMultiMania, ());
DefineMultiManiaFunc(MultiMania_Connect, (const char* connectionCode));
DefineMultiManiaFunc(MultiMania_Host, ());
extern "C"
{

    __declspec(dllexport) void OpenMenu()
    {
        DevMenu_Address = MultiManiaMenu;
        GameState = GameState | GameState_DevMenu;
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
