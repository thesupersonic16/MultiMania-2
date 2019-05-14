#include "stdafx.h"
#include <ManiaModLoader.h>
#include <string>
#include <algorithm>
#include <vector>
#include "MultiMania.h"
#include "MultiManiaMenu.h"
#include <SonicMania.h>
#include <windows.h>

using namespace SonicMania;

using std::string;

int MultiMania_Code[6];
int MultiMania_CodePosition;
int MultiMania_PPS = 40;

static int ConnectionTimer = 0;

DataPointer(BYTE, Controller_A, 0x0044170C);
DataPointer(BYTE, Key_Enter, 0x00441754);
DataPointer(BYTE, Key_Up, 0x004416D8);
DataPointer(BYTE, Key_Down, 0x004416E4);
DataPointer(DWORD, dword_6F0AE4, 0x002FBB4C);

#define DEBUG(str) \
string s = "DEBUG: "; \
s += str; \
DevMenu_DrawRect(0, centerY - 16 + centerY / 2 + centerY / 4, centerX * 2, 32, 0x00004080, 255, 0, 1); \
DevMenu_DrawText(centerX, s.c_str(), centerY - 4 + centerY / 2 + centerY / 4, 1, 0xF0F0F0); 

#define MSG(str) \
DevMenu_DrawRect(0, centerY - 16 + centerY / 2 + centerY / 4, centerX * 2, 32, 0x00004080, 255, 0, 1); \
DevMenu_DrawText(centerX, str, centerY - 4 + centerY / 2 + centerY / 4, 1, 0xF0F0F0); 


char MultiManiaMenu()
{
    char result;

    // Centre of the Screen

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);
    int YPosition = centerY - 84;

    //DevMenu_DrawRect(0, 0, centerX * 2, centerY * 2, 0x000000, 255, 0, 1);
    // Title
    //DevMenu_DrawRect(centerX - 128, centerY - 84, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    //DevMenu_DrawText(centerX, "MultiMania Main Menu", YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    //DevMenu_DrawText(centerX, (std::string("Version ") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    //DevMenu_DrawText(centerX, "Not Connected", YPosition, 1, 0xF0F0F0);
    YPosition += 40;

    // Bottom Panel
    DevMenu_DrawRect(centerX - 128, YPosition - 8, 256, 72, 0x00000080, 255, 0, 1);


    int count = 4;
    int optionColours[6];
    for (int i = 0; i < count; ++i)
        optionColours[i] = 0x808090;
    optionColours[DevMenu_Option - DevMenu_Scroll] = 0xF0F0F0;



    DevMenu_DrawText(centerX - 116, "Connection Code: ", YPosition, 0, optionColours[0]);
    
    DevMenu_DrawRect(centerX + 19, YPosition - 2, 78, 11, 0x00000000, 255, 0, 1);
    DevMenu_DrawRect(centerX + 20, YPosition - 1, 76, 9, 0x00FFFFFF, 255, 0, 1);
    
    char buf[2];
    buf[1] = NULL;
    for (int i = 0; i < 6; ++i)
    {
        buf[0] = '0' + MultiMania_Code[i];
        DevMenu_DrawText(centerX + 43 + (i * 8), buf, YPosition, 0, i == MultiMania_CodePosition ? 0x00FF0000 : 0x00000000);
    }
    DevMenu_DrawText(centerX + 27, "MM", YPosition, 0, 0x000000);               YPosition += 12;
    DevMenu_DrawText(centerX - 116, "Packets Per Secs.", YPosition, 0, optionColours[1]);
    
    string s = "<";
    s += std::to_string(MultiMania_PPS);
    s += ">";
    
    DevMenu_DrawText(centerX + 76, s.c_str(), YPosition, 2, 0xF0F07D);          YPosition += 12;
                                                                                YPosition += 12;
    DevMenu_DrawText(centerX, "Connect", YPosition, 1, optionColours[2]);       YPosition += 12;
    DevMenu_DrawText(centerX, "Host Game", YPosition, 1, optionColours[3]);     YPosition += 12;

    YPosition += 10;

    DevMenu_DrawRect(centerX - 128, YPosition, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    DevMenu_DrawText(centerX, (std::string("MultiMania ") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    DevMenu_DrawText(centerX, "Not Connected", YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    //DevMenu_DrawText(centerX, "Open Menu", YPosition, 1, optionColours[5]);
    YPosition += 40;

    
    bool left = PlayerControllers[0].Left.Down;
    if (Key_Up)
    {
        if (!dword_6F0AE4)
        {
            if (MultiMania_CodePosition == 6 || DevMenu_Option != 0)
            {
                --DevMenu_Option;
                if (DevMenu_Option < 0)
                {
                    DevMenu_Option = count - 1;
                    if (DevMenu_Option >= 18)
                        DevMenu_Scroll = DevMenu_Option - 18;
                    else
                        DevMenu_Scroll = 0;
                }
            }
            else
            {
                ++MultiMania_Code[MultiMania_CodePosition];
                if (MultiMania_Code[MultiMania_CodePosition] > 9)
                    MultiMania_Code[MultiMania_CodePosition] = 0;
            }

        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else if (Key_Down)
    {
        if (!dword_6F0AE4)
        {
            if (MultiMania_CodePosition == 6 || DevMenu_Option != 0)
            {
                ++DevMenu_Option;
                if ((DevMenu_Option - DevMenu_Scroll) > 18)
                    ++DevMenu_Scroll;
                if (DevMenu_Option > count - 1)
                    DevMenu_Option = 0;
            }
            else
            {
                --MultiMania_Code[MultiMania_CodePosition];
                if (MultiMania_Code[MultiMania_CodePosition] < 0)
                    MultiMania_Code[MultiMania_CodePosition] = 9;
            }
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else if (left || PlayerControllers[0].Right.Down)
    {
        if (!dword_6F0AE4)
        {
            if (DevMenu_Option == 0)
            {
                if (left)
                    --MultiMania_CodePosition;
                else
                    ++MultiMania_CodePosition;

                if (MultiMania_CodePosition < 0)
                    MultiMania_CodePosition = 6;
                else if(MultiMania_CodePosition > 6)
                    MultiMania_CodePosition = 0;
            }
            else if (DevMenu_Option == 1)
            {
                if (left)
                {
                    if (MultiMania_PPS > 1)
                        --MultiMania_PPS;
                }
                else
                {
                    if (MultiMania_PPS < 120)
                        ++MultiMania_PPS;
                }
            }
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else if ((GetAsyncKeyState(VK_LCONTROL) & (1 << 15) || GetAsyncKeyState(VK_RCONTROL) & (1 << 15)) && GetAsyncKeyState('V') & (1 << 15))
    {
        OpenClipboard(nullptr);
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == nullptr)
        {
            CloseClipboard();
            return (dword_6F0AE4 + 1) & 7;
        }
        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (pszText == nullptr)
        {
            CloseClipboard();
            return (dword_6F0AE4 + 1) & 7;
        }
        string text(pszText);
        GlobalUnlock(hData);
        CloseClipboard();
        if (text[0] == 'M' && text[1] == 'M')
        {
            bool valid = true;
            for (int i = 0; i < 6; ++i)
                if (text[2 + i] < '0' || text[2 + i] > '9')
                    valid = false;
            if (valid)
                for (int i = 0; i < 6; ++i)
                    MultiMania_Code[i] = text[2 + i] - '0';
        }
        result = (dword_6F0AE4 + 1) & 7;
    }
    else
    {
        result = Key_Enter | Controller_A;
        dword_6F0AE4 = 0;
        if ((Key_Enter | Controller_A) == 1)
        {
            switch (DevMenu_Option)
            {
            case 2:
                // Creates the Connection Code String
                char connectioncode[9];
                connectioncode[0] = connectioncode[1] = 'M';
                for (int i = 0; i < 6; ++i)
                    connectioncode[2 + i] = '0' + MultiMania_Code[i];
                connectioncode[8] = NULL;
                DevMenu_Address = MultiManiaMenu_Host_MMSERVER;
                if (!MultiMania_Connect(connectioncode, MultiMania_PPS))
                {
                    // Failed
                }
                break;
            case 3:
                DevMenu_Address = MultiManiaMenu_Host_MMSERVER;
                MultiMania_Host(MultiMania_PPS);
                //GameState = *(GameStates*)(baseAddress + 0x002FBB54);
                break;
            default:
                break;
            }
        }
    }
    return result;
}

char MultiManiaMenu_Connecting()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);
    
    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00000080, 255, 0, 1);
    DevMenu_DrawText(centerX, "Connecting...", centerY - 4, 1, 0xF0F0F0);

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    if ((Key_Enter | Controller_A) == 1)
        GameState = *(GameStates*)0x002FBB54;
    ++ConnectionTimer;

    if (ConnectionTimer > 120)
    {
        ConnectionTimer = 0;
        MultiMania_Close();
        DevMenu_Address = MultiManiaMenu_ConnectionError_TIMEOUT;
    }
    return result;
}

char MultiManiaMenu_Host_Code()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    char buf[9];
    buf[8] = NULL;
    buf[0] = 'M';
    buf[1] = 'M';
    for (int i = 0; i < 6; ++i)
        buf[i + 2] = '0' + MultiMania_Code[i];
    
    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00008000, 255, 0, 1);
    DevMenu_DrawText(centerX, (string("Connection Code: ") + buf).c_str(), centerY - 4, 1, 0xF0F0F0);

    MSG("Tip: You can Press Ctrl + C to copy the code to the Clipboard.");

    result = Key_Enter | Controller_A;
    if ((Key_Enter | Controller_A) == 1)
    {
        GameState = *(GameStates*)(baseAddress + 0x002FBB54);
    }
    else if ((GetAsyncKeyState(VK_LCONTROL) & (1 << 15) || GetAsyncKeyState(VK_RCONTROL) & (1 << 15)) && GetAsyncKeyState('C') & (1 << 15))
    {

        const size_t len = strlen(buf) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        memcpy(GlobalLock(hMem), buf, len);
        GlobalUnlock(hMem);
        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
        result = (dword_6F0AE4 + 1) & 7;
    }
    dword_6F0AE4 = 0;
    return result;
}

char MultiManiaMenu_Host_MMSERVER()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00004080, 255, 0, 1);
    DevMenu_DrawText(centerX, "Connecting to MultiMania Server...", centerY - 4, 1, 0xF0F0F0);

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    return result;
}

char MultiManiaMenu_ConnectionError_INVALIDCC()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00800000, 255, 0, 1);
    DevMenu_DrawText(centerX, "Connection Failed! Invalid Connection Code!", centerY - 4, 1, 0xF0F0F0);

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    if ((Key_Enter | Controller_A) == 1)
    {
        GameState = *(GameStates*)(baseAddress + 0x002FBB54);
    }
    return result;
}

char MultiManiaMenu_ConnectionWarning_CLOSED()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00805000, 255, 0, 1);
    DevMenu_DrawText(centerX, "Connection Closed!", centerY - 4, 1, 0xF0F0F0);

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    if ((Key_Enter | Controller_A) == 1)
    {
        GameState = *(GameStates*)(baseAddress + 0x002FBB54);
    }
    return result;
}

char MultiManiaMenu_ConnectionError_TIMEOUT()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00800000, 255, 0, 1);
    DevMenu_DrawText(centerX, "Connection Failed! Connection Timed out!", centerY - 4, 1, 0xF0F0F0);

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    if ((Key_Enter | Controller_A) == 1)
    {
        GameState = *(GameStates*)(baseAddress + 0x002FBB54);
    }
    return result;
}

char MultiManiaMenu_Connected()
{
    char result;

    // Centre of the Screen

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);
    int YPosition = centerY - 84;

    // Title
    YPosition += 6;
    YPosition += 14;
    YPosition += 14;
    
    YPosition += 40;

    // Bottom Panel
    DevMenu_DrawRect(centerX - 128, YPosition - 8, 256, 72, 0x00000080, 255, 0, 1);

    if ((DevMenu_Option - DevMenu_Scroll) > 3)
        DevMenu_Option = 0;

    int count = 3;
    int optionColours[4];
    for (int i = 0; i < count; ++i)
        optionColours[i] = 0x808090;
    optionColours[DevMenu_Option - DevMenu_Scroll] = 0xF0F0F0;
    
    DevMenu_DrawText(centerX, "Change Stage", YPosition, 1, optionColours[0]);      YPosition += 12;
    DevMenu_DrawText(centerX, "Change Character", YPosition, 1, optionColours[1]);  YPosition += 12;
                                                                                    YPosition += 12;
                                                                                    YPosition += 12;
    DevMenu_DrawText(centerX, "Disconnect", YPosition, 1, optionColours[2]);        YPosition += 12;

    YPosition += 10;
    DevMenu_DrawRect(centerX - 128, YPosition, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    DevMenu_DrawText(centerX, (std::string("MultiMania ") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    char buff[128];
    memset(buff, 0, 128);
    NetworkInfo info;
    MultiMania_GetNetworkInfo(&info);
    sprintf_s(buff, 128, "U:%d D:%d S:%d R:%d L:%d", info.UpBytesTotal, info.DownBytesTotal, info.UpPacketsTotal, info.DownPacketsTotal, info.LostPacketsTotal);

    DevMenu_DrawText(centerX, buff, YPosition, 1, 0xF0F0F0);


    if (Key_Up)
    {
        if (!dword_6F0AE4)
        {
            --DevMenu_Option;
            if (DevMenu_Option < 0)
            {
                DevMenu_Option = count - 1;
                if (DevMenu_Option >= 18)
                    DevMenu_Scroll = DevMenu_Option - 18;
                else
                    DevMenu_Scroll = 0;
            }
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else if (Key_Down)
    {
        if (!dword_6F0AE4)
        {
            ++DevMenu_Option;
            if ((DevMenu_Option - DevMenu_Scroll) > 18)
                ++DevMenu_Scroll;
            if (DevMenu_Option > count - 1)
                DevMenu_Option = 0;
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else
    {
        result = Key_Enter | Controller_A;
        dword_6F0AE4 = 0;
        if ((Key_Enter | Controller_A) == 1)
        {
            switch (DevMenu_Option)
            {
            case 0: // Change Stage
                DevMenu_Address = Devmenu_StageSelect;
                DevMenu_Option = 0;
                DevMenu_Scroll = 0;
                break;
            case 1: // Change Character
                DevMenu_Address = MultiManiaMenu_ChangeChar;
                DevMenu_Option = 0;
                DevMenu_Scroll = 0;
                break;
            //case 2: // Reserved
            //    break;
            //case 3: // Reserved
            //    break;
            case 2: // Disconnect
                MultiMania_Close();
                GameState = *(GameStates*)(baseAddress + 0x002FBB54);
                break;
            default:
                break;
            }
        }
    }
    return result;
}

char MultiManiaMenu_ChangeChar()
{
    char result;

    // Centre of the Screen

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);
    int YPosition = centerY - 84;

    //DevMenu_DrawRect(0, 0, centerX * 2, centerY * 2, 0x000000, 255, 0, 1);
    // Title
    DevMenu_DrawRect(centerX - 128, centerY - 84, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    DevMenu_DrawText(centerX, "MultiMania Main Menu", YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    DevMenu_DrawText(centerX, (std::string("Version ") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    NetworkInfo info;
    if (MultiMania_GetNetworkInfo(&info))
    {
        char buff[128];
        memset(buff, 0, 128);
        sprintf_s(buff, 128, "U:%d D:%d S:%d R:%d L:%d", info.UpBytesTotal, info.DownBytesTotal, info.UpPacketsTotal, info.DownPacketsTotal, info.LostPacketsTotal);

        DevMenu_DrawText(centerX, buff, YPosition, 1, 0xF0F0F0);
    }
    else
    {
        DevMenu_DrawText(centerX, "Not Connected", YPosition, 1, 0xF0F0F0);
    }
    YPosition += 40;

    // Bottom Panel
    DevMenu_DrawRect(centerX - 128, YPosition - 8, 256, 72, 0x00000080, 255, 0, 1);

    int optionColours[6];
    for (int i = 0; i < 6; ++i)
        optionColours[i] = 0x808090;

    // Give selected Option a lighter colour
    if ((DevMenu_Option - DevMenu_Scroll) < 0)
        DevMenu_Scroll = 0;
    optionColours[DevMenu_Option - DevMenu_Scroll] = 0xF0F0F0;
    YPosition -= 10;
    const char* CharOptions[6]
    {
        "Sonic", "Tails", "Knuckles", "Mighty", "Ray", "Back"
    };
    for (int i = DevMenu_Scroll; i < 7; ++i)
    {
        YPosition += 10;
        if ((i - DevMenu_Scroll) > 5)
            break;
        DevMenu_DrawText(centerX, CharOptions[i], YPosition, 1, optionColours[i - DevMenu_Scroll]);
    }

    if (Key_Up)
    {
        if (!dword_6F0AE4)
        {
            --DevMenu_Option;
            if (DevMenu_Option < 0)
            {
                DevMenu_Option = 5;
                DevMenu_Scroll = DevMenu_Option - 5;
            }
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else if (Key_Down)
    {
        if (!dword_6F0AE4)
        {
            ++DevMenu_Option;
            if ((DevMenu_Option - DevMenu_Scroll) > 5)
                ++DevMenu_Scroll;
            if (DevMenu_Option > 5)
                DevMenu_Option = 0;
        }
        result = (dword_6F0AE4 + 1) & 7;
        dword_6F0AE4 = result;
    }
    else
    {
        result = Key_Enter | Controller_A;
        dword_6F0AE4 = 0;
        if ((Key_Enter | Controller_A) == 1)
        {
            switch (DevMenu_Option)
            {
            case 5:
                DevMenu_Address = MultiManiaMenu_Connected;
                DevMenu_Option = 1;
                DevMenu_Scroll = 0;
                break;
            default:
                try
                {
                    auto character = (Character)(1 << DevMenu_Option);
                    DEBUG(std::to_string(character));
                    *GetAddress((baseAddress + 0x00AA763C), 4) = character;
                    MultiMania_UpdatePlayer(character);
                    FastChangeCharacter(&Player1, character);
                    DevMenu_Address = MultiManiaMenu_Connected;
                    DevMenu_Option = 1;
                    DevMenu_Scroll = 0;
                }
                catch (...)
                {
                }
            }
        }
    }
    return result;
}


void MultiManiaMenu_MMStatus()
{
    if (MultiMania_Devmenu)
    {
        DevMenu_Address = MultiManiaMenu_Connected;
        DevMenu_Option = 5;
        DevMenu_Scroll = 0;
        return;
    }
    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);
    int YPosition = centerY - 84;

    int count = 6;
    int optionColours[6];
    for (int i = 0; i < count; ++i)
        optionColours[i] = 0x808090;
    optionColours[DevMenu_Option - DevMenu_Scroll] = 0xF0F0F0;


    // Title
    YPosition += 6;
    YPosition += 14;
    YPosition += 14;
    YPosition += 40;
    
    bool connected = MultiMania_GetNetworkInfo(nullptr);

    DevMenu_DrawRect(centerX - 128, YPosition - 8, 256, 72, 0x00000080, 255, 0, 1);
    DevMenu_DrawText(centerX, "Resume", YPosition, 1, optionColours[0]);            YPosition += 12;
    DevMenu_DrawText(centerX, "Restart", YPosition, 1, optionColours[1]);           YPosition += 12;
    DevMenu_DrawText(centerX, "Stage Select", YPosition, 1, optionColours[2]);      YPosition += 12;
    DevMenu_DrawText(centerX, "Options", YPosition, 1, optionColours[3]);           YPosition += 12;
    DevMenu_DrawText(centerX, "Exit", YPosition, 1, optionColours[4]);              YPosition += 12;
    YPosition += 10;

    DevMenu_DrawRect(centerX - 128, YPosition, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    DevMenu_DrawText(centerX, (std::string("MultiMania ") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    if (connected)
    {
        char buff[128];
        memset(buff, 0, 128);
        NetworkInfo info;
        MultiMania_GetNetworkInfo(&info);
        sprintf_s(buff, 128, "U:%d D:%d S:%d R:%d L:%d", info.UpBytesTotal, info.DownBytesTotal, info.UpPacketsTotal, info.DownPacketsTotal, info.LostPacketsTotal);

        DevMenu_DrawText(centerX, buff, YPosition, 1, 0xF0F0F0);
    }
    else
        DevMenu_DrawText(centerX, "Not Connected", YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    DevMenu_DrawText(centerX, "Open Menu", YPosition, 1, optionColours[5]);
    YPosition += 40;

    if ((Key_Enter | Controller_A) && DevMenu_Option == 5)
    {
        if (connected)
            DevMenu_Address = MultiManiaMenu_Connected;
        else
            DevMenu_Address = MultiManiaMenu;
        //DevMenu_Option = 0;
        DevMenu_Scroll = 0;
    }
}








