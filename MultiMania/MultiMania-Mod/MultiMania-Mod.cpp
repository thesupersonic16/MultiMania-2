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
DefineMultiManiaFunc(MultiMania_Close, ());
DefineMultiManiaFunc(MultiMania_Host, (int PPS));
DefineMultiManiaFunc(MultiMania_Update, ());
DefineMultiManiaFunc(MultiMania_IsHost, ());
DefineMultiManiaFunc(MultiMania_SpawnObject, (short objectID, short subObject, DWORD x, DWORD y));
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

    __declspec(dllexport) void OpenMenu()
    {
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

    __declspec(dllexport) void MultiMania_Mod_WritePlayerData_Process_old(BYTE slot, BYTE* data)
    {
        auto &player = *(EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
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

        //player.Jump = 1;

        player.InputStatus = InputStatus_None;

        bool b = (bool)((value >> 4) & 1);
        if (!StillPressed2 && b)
        {
            KillPlayer2 = true;
            StillPressed2 = true;
        }

        if (StillPressed2 && !b)
            StillPressed2 = false;

        //bool a = (bool)((value >> 5) & 1);
        //
        //if (a && !StillPressed3)
        //{
        //    StillPressed3 = true;
        //    player.Jump = true;
        //}
        //else
        //    player.Jump = false;
        //
        //if (StillPressed3 && !a)
        //    StillPressed3 = false;

        if (player.Animation.AnimationID != 19 && player.Animation.AnimationID != 20)
        {
            short x = *(short*)(data + counter); counter += 2;
            short y = *(short*)(data + counter); counter += 2;
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
        else
        {
            counter += 4;
        }
        player.LifeCount = (int)data[counter++]; 
        player.RingCount = *(short*)data + counter; counter += 2;
        player.AirLeft = 0;

        if (player.Animation.AnimationID != 19 && player.Animation.AnimationID != 20)
        {
            player.GroundSpeed = *(int*)(data + counter); counter += 4;
            player.XSpeed = *(int*)(data + counter); counter += 4;
            player.YSpeed = *(int*)(data + counter); counter += 4;

            int levelTimer = *(int*)(data + counter); counter += 2;

            player.Shield = (ShieldType)data[counter++];
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

    __declspec(dllexport) void MultiMania_Mod_ReadPlayerData_old(BYTE slot, BYTE* data)
    {
        auto &player = *(EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
        int counter = 0;

        // Bits
        BYTE value = 0;
        value |= (BYTE)((player.Up << 0));
        value |= (BYTE)((player.Down << 1));
        value |= (BYTE)((player.Left << 2));
        value |= (BYTE)((player.Right << 3));
        value |= (BYTE)((player.Jump << 5));
        value |= (BYTE)((player.IsFacingLeft << 6));
        value |= (BYTE)((((
            player.Animation.AnimationID == 19 ||
            player.Animation.AnimationID == 20
            ) ? 1 : 0) & 0x01) << 4);
        data[counter++] = value;

        *(short*)(data + counter) = player.Position.X; counter += 2;
        *(short*)(data + counter) = player.Position.Y; counter += 2;
        *(BYTE*)(data + counter) = player.LifeCount; counter += 1;
        *(short*)(data + counter) = player.RingCount; counter += 2;
        *(int*)(data + counter) = player.GroundSpeed; counter += 4;
        *(int*)(data + counter) = player.XSpeed; counter += 4;
        *(int*)(data + counter) = player.YSpeed; counter += 4;
        *(int*)(data + counter) = 0; counter += 4;
        *(BYTE*)(data + counter) = player.Shield; counter += 1;
    }


    bool test = false;
    __declspec(dllexport) void OnFrame()
    {
        test = !test;
        if (PlayerControllers[0].Down.Down && PlayerControllers[0].Up.Press && (GameState & GameState_DevMenu) != GameState_DevMenu)
            OpenMenu();
        //if (!test)
        {
            MultiMania_Update();
            if (!Processed && LastPacket)
            {
                MultiMania_Mod_WritePlayerData_Process(1, LastPacket, MultiMania_IsHost());
                //MultiMania_Mod_WritePlayerData_Process(1, LastPacket);
                Processed = false;
            }
        }
        //MultiMania_Mod_WritePlayerData_Process_old(1, LastPacket);
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
