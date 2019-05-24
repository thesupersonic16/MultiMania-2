// MultiMania-Mod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ManiaModLoader.h>
#include <Trampoline.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>
#include "MultiMania.h"
#include "MultiManiaMenu.h"
#include "MultiManiaStartup.h"

using namespace SonicMania;
using namespace std::chrono;
using namespace std::chrono_literals;

DefineMultiManiaFunc(InitMultiMania, ());
DefineMultiManiaFunc(MultiMania_Connect, (const char* connectionCode, int PPS));
DefineMultiManiaFunc(MultiMania_Close, ());
DefineMultiManiaFunc(MultiMania_Host, (int PPS));
DefineMultiManiaFunc(MultiMania_Update, ());
DefineMultiManiaFunc(MultiMania_IsHost, ());
DefineMultiManiaFunc(MultiMania_GetNetworkInfo, (NetworkInfo* networkInfo));
DefineMultiManiaFunc(MultiMania_SendSoundFX, (short SoundFXID, int a2, BYTE a3));
DefineMultiManiaFunc(MultiMania_UpdatePlayer, (Character character));
DefineMultiManiaFunc(MultiMania_UpdateStage, (SonicMania::Scene scene));

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
    static Character Player2Character = Character_None;
    static bool Player2Set = false;
    static bool KillDone = false;
    static int SendKill = false;
    Trampoline *SoundFXTrampo;
    bool MultiMania_Devmenu = false;

    // TODO: Write comment on how these offsets are made
    __declspec(dllexport) void MultiMania_Mod_SetResultData(int score, int finalRings, int totalRings, int itemboxes, int playerID, BYTE character)
    {
        // Player 2 Score
        if (*(int*)(baseAddress + 0x00AA763C) != 0)
        {
            *GetAddress(baseAddress + 0x00AA763C, 0x00031184) = score;
            *GetAddress(baseAddress + 0x00AA763C, 0x00031174) = finalRings;
            *GetAddress(baseAddress + 0x00AA763C, 0x000311D8) = totalRings;
            *GetAddress(baseAddress + 0x00AA763C, 0x00031194) = itemboxes;
            
            //auto characterPtr = (BYTE*)GetCharacter_ptr(playerID);
            //
            //if (*characterPtr != character && CurrentScene >= Scene_GHZ1 && CurrentScene <= Scene_TMZ3_e)
            //{
            //    auto player = (EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * playerID);
            //    FastChangeCharacter(player, (Character)character);
            //}
            //*characterPtr = (Character)character;
            //Player2Character = (Character)character;

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
            *(int*)(data + counter) = 1; counter += 4;
            *(int*)(data + counter) = *GetCharacter_ptr(playerID); counter += 4;
        }
    }

    __declspec(dllexport) void OpenMenu()
    {
        if (MultiMania_GetNetworkInfo(nullptr))
            DevMenu_Address = MultiManiaMenu_Connected;
        else
            DevMenu_Address = MultiManiaStartupMenu;//MultiManiaMenu;
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
        if (*GetCharacter_ptr(slot) != character)
        {
            auto player = (EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
            FastChangeCharacter(player, (Character)(character & 0xFF));
        }
        *(BYTE*)GetCharacter_ptr(slot) = character;
        Player2Character = character;
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

    static const uint8_t restorePlayer2Respawn[] = { 0x0F, 0x8Cu, 0xCEu, 0x00, 0x00, 0x00 };
    __declspec(dllexport) void MultiMania_Mod_SendEvent(int errorcode)
    {
        switch (errorcode)
        {
        case 0: // Connected
            GameState = *(GameStates*)(baseAddress + 0x002FBB54);
            WriteJump((void*)(baseAddress + 0xC5449), (void*)(baseAddress + 0xC551D));
            break;
        case 1: // Invalid Connection Code
            DevMenu_Address = MultiManiaMenu_ConnectionError_INVALIDCC;
            break;
        case 2: // Disconnected
            WriteData((void*)(baseAddress + 0xC5449), restorePlayer2Respawn, 6);
            DevMenu_Address = MultiManiaMenu_ConnectionWarning_CLOSED;
            if (!(GameState | GameState_DevMenu))
                *(GameStates*)(baseAddress + 0x002FBB54) = GameState;
            GameState = GameState_DevMenu;
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
            if (*(bool*)(data + counter))
            {
                player.Position.Y = 99999;
                player.Status = PlayerStatus_Dead;
                return;
            }
            if (player.Status != PlayerStatus_Dead)
            {
                player.Status = PlayerStatus_None;
            }
            else
            {
                player.Position.Y = 999999;
                return;
            }
            counter += 1;
            short x = *(short*)(data + counter); counter += 2;
            short y = *(short*)(data + counter); counter += 2;
            player.LifeCount = (int)data[counter++];
            player.RingCount = *(short*)(data + counter); counter += 2;
            short animID = *(short*)(data + counter); counter += 2;
            short frameID = *(short*)(data + counter); counter += 2;
            SetSpriteAnimation(player.SpriteIndex, animID, &player.Animation, true, frameID);
            player.InputStatus = InputStatus_None;
            player.SetVelocity(0, 0);
            
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
            
            if (Player2Set)
            {
                if (*GetCharacter_ptr(slot) != Player2Character)
                    FastChangeCharacter(&player, Player2Character);
                *GetCharacter_ptr(slot) = Player2Character;
            }
            else
            {
                *(BYTE*)GetCharacter_ptr(slot) = Character_Tails;
                Player2Character = Character_Tails;
                Player2Set = true;
            }
        }
    }

    __declspec(dllexport) void MultiMania_Mod_ReadPlayerData(BYTE slot, BYTE* data)
    {
        auto &player = *(EntityPlayer*)(baseAddress + 0x00469A10 + 0x458 * slot);
        int counter = 0;

        if (*(int*)(baseAddress + 0x00AC690C) != 0)
        {
            *(bool*)(data + counter) = (SendKill--) != 0; counter += 1;
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
            if (SendKill < 0 )
                SendKill = 0;
        }
    }
    int boot = false;
    __declspec(dllexport) void OnFrame()
    {
        if (!boot)
        {
            if (!DevMenu_Enabled)
            {
                MultiMania_Devmenu = true;
                DevMenu_Enabled = true;
                printf("[MultiMania-Mod] Devmenu is not active! MultiMania will attempt to enable it for MM use only.");
            }
            *(GameStates*)(baseAddress + 0x002FBB54) = GameState;
            GameState = GameState_DevMenu;
            DevMenu_Address = MultiManiaStartupMenu;
            boot = true;
        }

        if (PlayerControllers[0].Down.Down && PlayerControllers[0].Up.Press && (GameState & GameState_DevMenu) != GameState_DevMenu)
            OpenMenu();
        if (MultiMania_GetNetworkInfo(nullptr))
        {
            if (Player1.KillFlag)
                SendKill = 5;
            if ((GetTime - Time) > (milliseconds)(long long)((float)1000.0f / MultiMania_PPS))
            {
                Time = GetTime;
                
                MultiMania_Update();
            }

            if (LastPacket)
                MultiMania_Mod_WritePlayerData_Process(1, LastPacket, MultiMania_IsHost());
        }
    }

    __declspec(dllexport) int MultiMania_Mod_PlaySoundFX(short SoundFXID, int a2, BYTE a3)
    {
        auto orig = (decltype(MultiMania_Mod_PlaySoundFX)*)SoundFXTrampo->Target();
        return orig(SoundFXID, a2, a3);
    }

    __declspec(dllexport) int MultiMania_Mod_PlaySoundFX_r(short SoundFXID, int a2, BYTE a3)
    {
        //printf("playing SoundFX: %d\n", SoundFXID);
        if (SoundFXID == SonicMania::GetSoundFXID("Global/Jump.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/BlueShield.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/BubbleBounce.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/BubbleShield.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Charge.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Destroy.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/DropDash.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/FireDash.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/FireShield.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Grab.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Hurt.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/HyperRing.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/InstaShield.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Land.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/LightningJump.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/LightningShield.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/LoseRings.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/MightyDeflect.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/MightyDrill.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/MightyLand.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/MightyUnspin.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/OuttaHere.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/PeelCharge.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/PeelRelease.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Push.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Recovery.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Release.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Roll.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/ScoreAdd.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/ScoreTotal.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SignPost.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SignPost2p.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Skidding.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Slide.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Sliding.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SpecialRing.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SpecialWarp.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Spike.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SpikesMove.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Spring.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/StarPost.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Swap.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/SwapFail.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Teleport.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Twinkle.wav") ||
            SoundFXID == SonicMania::GetSoundFXID("Global/Warp.wav"))
        {
            MultiMania_SendSoundFX(SoundFXID, a2, a3);
        }
        auto orig = (decltype(MultiMania_Mod_PlaySoundFX_r)*)SoundFXTrampo->Target();
        return orig(SoundFXID, a2, a3);
    }


    __declspec(dllexport) void MultiMania_Mod_SyncAndRestart()
    {
        if (MultiMania_GetNetworkInfo(nullptr))
            MultiMania_UpdateStage(CurrentScene);
        GameState = GameState_NotRunning;
    }

    __declspec(dllexport) void Init(const char *path)
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        SetCurrentDirectoryA(path);
        printf("[MultiMania-Mod] Loading MultiMania IL... ");
        MultiManiaCS = LoadLibrary("MultiMania.dll");
        if (!MultiManiaCS)
        {
            printf("Failed!\n");
            DWORD error = GetLastError();
            LPSTR buffer;
            size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, nullptr);
            std::string message = "MultiMania has ran into a fatal error while loading its IL library, Please make sure MultiMania.dll exists and is updated. Error: " + std::string(buffer, size);
            LocalFree(buffer);
            MessageBox(nullptr, message.c_str(), "MultiMania Error", MB_ICONERROR);
            exit(error);
        }
        else
            printf("Done.\n");
        printf("[MultiMania-Mod] Loading MultiMania Logo... ");
        Init();
        printf("Done.\n");
        SetCurrentDirectoryA(buffer);
        LoadExports();
        WriteData<7>((void*)(baseAddress + 0x1C3064), 0x90);
        WriteCall((void*)(baseAddress + 0x1C3064), MultiMania_Mod_SyncAndRestart);
        WriteCall((void*)(baseAddress + 0x001C25DB), MultiManiaMenu_MMStatus);
        WriteJump((void*)(baseAddress + 0x001C25E0), (void*)(baseAddress + 0x001C2A25));
        WriteData((char*)(baseAddress + 0x001C2A6E), (char)6);
        WriteData((char*)(baseAddress + 0x001C2A4E), (char)6);

        WriteData<7>((void*)(baseAddress + 0x001C2AD7), 0x90);
        WriteCall((void*)(baseAddress + 0x001C2AD7), MultiMania_Mod_SyncAndRestart);

        
        SoundFXTrampo = new Trampoline((baseAddress + 0x001BC390), (baseAddress + 0x001BC396), MultiMania_Mod_PlaySoundFX_r);
    }

    __declspec(dllexport) ModInfo ManiaModInfo = { ModLoaderVer, GameVer };

}
