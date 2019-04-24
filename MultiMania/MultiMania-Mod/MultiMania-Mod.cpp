// MultiMania-Mod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ManiaModLoader.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>
#include "MultiMania.h"
#include "MultiManiaMenu.h"

using namespace SonicMania;
using namespace std::chrono;
using namespace std::chrono_literals;

DefineMultiManiaFunc(InitMultiMania, ());
DefineMultiManiaFunc(MultiMania_Connect, (const char* connectionCode, int PPS));
DefineMultiManiaFunc(MultiMania_Close, ());
DefineMultiManiaFunc(MultiMania_Host, (int PPS));
DefineMultiManiaFunc(MultiMania_Update, ());
DefineMultiManiaFunc(MultiMania_IsHost, ());
DefineMultiManiaFunc(MultiMania_IsConnected, ());
DefineMultiManiaFunc(MultiMania_SpawnObject, (short objectID, short subObject, DWORD x, DWORD y));

#define GetTime duration_cast<milliseconds>(system_clock::now().time_since_epoch())
extern "C"
{

    static bool StillPressed = false;
    static bool StillPressed2 = false;
    static bool StillPressed3 = false;
    static bool KillPlayer2 = false;
    static bool KillPlayer3 = false;
    static bool KillPlayer4 = false;
    static BYTE LastPacket_old[25];
    static BYTE LastPacket[25];
    static bool Processed = true;
    static milliseconds Time = GetTime;

    // TODO: Write comment on how these offsets are made
    __declspec(dllexport) void MultiMania_Mod_SetResultData(int score, int finalRings, int totalRings, int itemboxes, int playerID)
    {
        // Player 2 Score
        if (*(int*)(baseAddress + 0x00AA763C) != 0)
        {
            *GetAddress(baseAddress + 0x00AA763C, 0x00031184) = score;
            *GetAddress(baseAddress + 0x00AA763C, 0x00031174) = finalRings;
            *GetAddress(baseAddress + 0x00AA763C, 0x000311D8) = totalRings;
            *GetAddress(baseAddress + 0x00AA763C, 0x00031194) = itemboxes;
        }
    }

    __declspec(dllexport) void MultiMania_Mod_ReadResultData(int playerID, BYTE* data)
    {
        int counter = 0;

        if (*(int*)(baseAddress + 0x00AA763C) != 0)
        {
            *(int*)(data + counter) = *GetAddress(baseAddress + 0x00AA763C, 0x00031180); counter += 4;
            *(int*)(data + counter) = *GetAddress(baseAddress + 0x00AA763C, 0x00031170); counter += 4;
            *(int*)(data + counter) = *GetAddress(baseAddress + 0x00AA763C, 0x000311D4); counter += 4;
            *(int*)(data + counter) = *GetAddress(baseAddress + 0x00AA763C, 0x00031190); counter += 4;
        }
    }


    __declspec(dllexport) void OpenMenu()
    {
        if (MultiMania_IsConnected())
            DevMenu_Address = MultiManiaMenu_Connected;
        else
            DevMenu_Address = MultiManiaMenu;
        memset(MultiMania_Code, 0, 6);
        MultiMania_CodePosition = 6;
        *(GameStates*)(baseAddress + 0x002FBB54) = GameState;
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

    __declspec(dllexport) int MultiMania_Mod_GetLevelTimer()
    {
        return *GetAddress(baseAddress + 0x00AC690C, 0x7C);
    }

    __declspec(dllexport) void MultiMania_Mod_SetLevelTimer(int timer)
    {
        *GetAddress(baseAddress + 0x00AC690C, 0x7C) = timer;
    }

    __declspec(dllexport) void MultiMania_Mod_SendHostConnectionCode(const char* connectionCode)
    {
        DevMenu_Address = MultiManiaMenu_Host_Code;
        for (int i = 0; i < 6; ++i)
            MultiMania_Code[i] = connectionCode[2 + i] - '0';
    }


    __declspec(dllexport) void MultiMania_Mod_SendEvent(int errorcode)
    {
        switch (errorcode)
        {
        case 0:
            GameState = *(GameStates*)(baseAddress + 0x002FBB54);
            break;
        case 1:
            DevMenu_Address = MultiManiaMenu_ConnectionError_INVALIDCC;
            break;
        default:
            break;
        }
    }

    __declspec(dllexport) void MultiMania_Mod_WritePlayerData(BYTE slot, BYTE* data)
    {
        memcpy(LastPacket, data, 25);
        Processed = false;
    }

    __declspec(dllexport) void MultiMania_Mod_WritePlayerData_Process(BYTE slot, BYTE* data, bool host)
    {
        auto &player = *(EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
        int counter = 0;
        if (*(int*)(baseAddress + 0x00AC690C) != 0)
        {
            short x = *(short*)(data + counter); counter += 2;
            short y = *(short*)(data + counter); counter += 2;
            player.LifeCount = (int)data[counter++];
            player.RingCount = *(short*)(data + counter); counter += 2;
            short animID = *(short*)(data + counter); counter += 2;
            short frameID = *(short*)(data + counter); counter += 2;
            SetSpriteAnimation(player.SpriteIndex, animID, &player.Animation, true, frameID);
            player.InputStatus = InputStatus_None;
            player.Status = PlayerStatus_None;

            if (x == 0 && y == 0)
            {
                player.LifeCount = 3;
            }
            else
            {
                player.Position.X = x;
                player.Position.Y = y;
            }
            if (!host)
                MultiMania_Mod_SetLevelTimer(*(int*)(data + counter));
            counter += 4;

            player.Shield = (ShieldType)data[counter++];
            player.IsFacingLeft = data[counter++];
            player.Angle = *(int*)(data + counter); counter += 4;
        }
    }

    __declspec(dllexport) void MultiMania_Mod_ReadPlayerData(BYTE slot, BYTE* data)
    {
        auto &player = *(EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
        int counter = 0;

        if (*(int*)(baseAddress + 0x00AC690C) != 0)
        {
            *(short*)(data + counter) = player.Position.X; counter += 2;
            *(short*)(data + counter) = player.Position.Y; counter += 2;
            *(BYTE*)(data + counter) = player.LifeCount; counter += 1;
            *(short*)(data + counter) = player.RingCount; counter += 2;
            *(short*)(data + counter) = player.Animation.AnimationID; counter += 2;
            *(short*)(data + counter) = player.Animation.FrameID; counter += 2;
            *(int*)(data + counter) = MultiMania_Mod_GetLevelTimer(); counter += 4;
            *(BYTE*)(data + counter) = player.Shield; counter += 1;
            *(BYTE*)(data + counter) = player.IsFacingLeft; counter += 1;
            *(int*)(data + counter) = player.Angle; counter += 4;
        }
    }

    __declspec(dllexport) void OnFrame()
    {
        if (PlayerControllers[0].Down.Down && PlayerControllers[0].Up.Press && (GameState & GameState_DevMenu) != GameState_DevMenu)
            OpenMenu();
        if (MultiMania_IsConnected)
        {
            if ((GetTime - Time) > (milliseconds)(long long)((float)1000.0f / MultiMania_PPS))
            {
                Time = GetTime;
                
                MultiMania_Update();
            }

            if (LastPacket)
                MultiMania_Mod_WritePlayerData_Process(1, LastPacket, MultiMania_IsHost());
        }
    }

    Entity* SpawnObject_r(short objectID, short subObject, DWORD x, DWORD y)
    {
        if (CurrentScene >= Scene_GHZ1)
            MultiMania_SpawnObject(objectID, subObject, x, y);
        return SpawnObject_Internal(objectID, subObject, x, y);
    }

    __declspec(dllexport) void MultiMania_Mod_SpawnObject(short objectID, short subObject, DWORD x, DWORD y)
    {
        SpawnObject_Internal(objectID, subObject, x, y);
    }

    __declspec(dllexport) void Init(const char *path)
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        SetCurrentDirectoryA(path);
        MultiManiaCS = LoadLibrary("MultiMania.dll");
        SetCurrentDirectoryA(buffer);
        LoadExports();
        *(int*)(baseAddress + 0x00AA7768) = (int)SpawnObject_r;
    }

    __declspec(dllexport) ModInfo ManiaModInfo = { ModLoaderVer, GameVer };

}
