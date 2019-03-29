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
DefineMultiManiaFunc(MultiMania_Connect, (const char* connectionCode, int PPS));
DefineMultiManiaFunc(MultiMania_Host, (int PPS));
extern "C"
{

    static bool StillPressed = false;
    static bool StillPressed2 = false;
    static bool KillPlayer2 = false;
    static bool KillPlayer3 = false;
    static bool KillPlayer4 = false;

    __declspec(dllexport) void OpenMenu()
    {
        DevMenu_Address = MultiManiaMenu;
        memset(MultiMania_Code, 0, 6);
        MultiMania_CodePosition = 0;
        GameState = GameState_DevMenu;
    }

    __declspec(dllexport) void MultiMania_Mod_ChangeScene(Scene scene)
    {
        ChangeScene(scene);
    }
    
    __declspec(dllexport) Scene MultiMania_Mod_GetScene()
    {
        return CurrentScene;
    }

    __declspec(dllexport) void MultiMania_Mod_SetCharacter(BYTE slot, Character character)
    {
        *GetCharacter_ptr(slot) = character;
    }

    __declspec(dllexport) Character MultiMania_Mod_GetCharacter(BYTE slot)
    {
        return GetCharacter(slot);
    }

    __declspec(dllexport) void MultiMania_Mod_WritePlayerData(BYTE slot, BYTE* data)
    {
        auto &player = *(EntityPlayer*)(0x00469A10 + 0x458 * slot);
        int counter = 0;

        // Bits
        BYTE value = data[counter++];
        player.Up           = (bool)((value >> 0) & 1);
        player.Down         = (bool)((value >> 1) & 1);
        player.Left         = (bool)((value >> 2) & 1);
        player.Right        = (bool)((value >> 3) & 1);
        player.Jump         = (bool)((value >> 5) & 1);
        player.IsFacingLeft = (bool)((value >> 6) & 1);
        //player.KillFlag   = (bool)((value >> 7) & 1);

        bool b = (bool)((value >> 4) & 1);
        if (!StillPressed2 && b)
        {
            KillPlayer2 = true;
            StillPressed2 = true;
        }

        if (StillPressed2 && !b)
            StillPressed2 = false;

        if (player.Animation.AnimationID != 19 && player.Animation.AnimationID != 20)
        {
            short x = *(short*)data + counter; counter += 2;
            short y = *(short*)data + counter; counter += 2;
            if (x == 0 && y == 0)
            {
                player.LifeCount = 3;
            }
            else
            {
                player.Position.X = x;
                player.Position.Y = y;
            }
        }
        player.LifeCount = (int)data[counter++]; 
        player.RingCount = *(short*)data + counter; counter += 2;
        player.AirLeft = 0;

        if (player.Animation.AnimationID != 19 && player.Animation.AnimationID != 20)
        {
            player.GroundSpeed = *(int*)data + counter; counter += 4;
            player.XSpeed = *(int*)data + counter; counter += 4;
            player.YSpeed = *(int*)data + counter; counter += 4;

            int levelTimer = *(int*)data + counter; counter += 2;

            player.Shield = (ShieldType)data[counter++];
        }
    }


    bool test = false;
    __declspec(dllexport) void OnFrame()
    {
        if (PlayerControllers[0].Down.Down && PlayerControllers[0].Up.Press)
            OpenMenu();
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
