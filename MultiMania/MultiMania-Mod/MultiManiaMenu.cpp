#include "stdafx.h"
#include <ManiaModLoader.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include "MultiMania.h"
#include "MultiManiaMenu.h"
#include <SonicMania.h>

using namespace SonicMania;

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

    DevMenu_DrawRect(0, 0, centerX * 2, centerY * 2, 0x000000, 255, 0, 1);
    // Title
    DevMenu_DrawRect(centerX - 128, centerY - 84, 256, 48, 0x00000080, 255, 0, 1);
    YPosition += 6;
    DevMenu_DrawText(centerX, "MultiMania Main Menu", YPosition, 1, 0xF0F0F0);
    YPosition += 14;
    DevMenu_DrawText(centerX, (std::string("MultiMania v") + MMVER).c_str(), YPosition, 1, 0xF0F0F0);
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



    DevMenu_DrawText(centerX - 116, "Conn Code: ", YPosition, 0, optionColours[0]);
    
    DevMenu_DrawRect(centerX - 33, YPosition - 2, 148, 11, 0x00000000, 255, 0, 1);
    DevMenu_DrawRect(centerX - 32, YPosition - 1, 146, 9, 0x00FFFFFF, 255, 0, 1);
    DevMenu_DrawText(centerX - 30, "MM000000", YPosition, 0, 0x000000);
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
    else if (left || PlayerControllers[0].Right.Down)
    {
        if (!dword_6F0AE4)
        {
            if (DevMenu_Option == 1)
            {
            }
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
            case 2:
                if (!MultiMania_Connect("M000000"))
                {
                    // Failed
                }
                break;
            case 3:
                MultiMania_Host();
                GameState = (GameStates)(GameState & ~GameState_DevMenu);
                break;
            default:
                break;
            }
        }
    }
    return result;
}