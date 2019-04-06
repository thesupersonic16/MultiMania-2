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
int MultiMania_PPS;

static int ConnectionTimer = 0;

char MultiManiaMenu_ConnectionError_TIMEOUT();

DataPointer(BYTE, Controller_A, 0x0044170C);
DataPointer(BYTE, Key_Enter, 0x00441754);
DataPointer(BYTE, Key_Up, 0x004416D8);
DataPointer(BYTE, Key_Down, 0x004416E4);
DataPointer(DWORD, dword_6F0AE4, 0x002FBB4C);

char MultiManiaMenu()
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
    //DevMenu_DrawText(centerX, "", YPosition, 1, 0xF0F0F0);
    YPosition += 40;

    // Bottom Panel
    DevMenu_DrawRect(centerX - 128, YPosition - 8, 256, 72, 0x00000080, 255, 0, 1);


    int count = 4;
    int optionColours[4];
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
    DevMenu_DrawText(centerX + 27, "MM", YPosition, 0, 0x000000);
    YPosition += 12;
    DevMenu_DrawText(centerX - 116, "Packets Per Secs.", YPosition, 0, optionColours[1]);

    DevMenu_DrawText(centerX  + 80, "<30>", YPosition, 0, 0xF0F07D);
    YPosition += 24;
    DevMenu_DrawText(centerX, "Connect", YPosition, 1, optionColours[2]);
    YPosition += 12;
    DevMenu_DrawText(centerX, "Host Game", YPosition, 1, optionColours[3]);
    
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
                if (!MultiMania_Connect(connectioncode, MultiMania_PPS))
                {
                    // Failed
                }
                break;
            case 3:
                MultiMania_Host(MultiMania_PPS);
                //GameState = (GameStates)(GameState & ~GameState_DevMenu);
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

    result = Key_Enter | Controller_A;
    dword_6F0AE4 = 0;
    return result;
}

char MultiManiaMenu_Host_MMSERVER()
{
    char result;

    int centerX = *(_DWORD *)(dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(dword_D3CC00 + 614420);

    DevMenu_DrawRect(0, centerY - 16, centerX * 2, 32, 0x00000080, 255, 0, 1);
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