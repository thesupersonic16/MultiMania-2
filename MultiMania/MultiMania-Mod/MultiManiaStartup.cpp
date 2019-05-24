#include "stdafx.h"
#include <ManiaModLoader.h>
#include <Trampoline.h>
#include <SonicMania.h>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>
#include "MultiManiaStartup.h"
#include "MultiManiaMenu.h"

DataPointer(BYTE, Controller_A, 0x0044170C);
DataPointer(BYTE, Key_Enter, 0x00441754);
DataPointer(BYTE, Key_Up, 0x004416D8);
DataPointer(BYTE, Key_Down, 0x004416E4);


Devmenu_Image* Logo = nullptr;
static int Page = 0;

void Init()
{
    Logo = new Devmenu_Image("MultiManiaLogo.bin", 153, 120);
}

Devmenu_Image::Devmenu_Image(const char* path, int width, int height)
{
    FILE* fileHandle;
    fopen_s(&fileHandle, path, "rb");
    int bufferSize = width * height;
    ImageData = new unsigned int[bufferSize];
    fread(ImageData, 4, bufferSize, fileHandle);
    Width = width;
    Height = height;
    fclose(fileHandle);
}
Devmenu_Image::~Devmenu_Image()
{
    if (ImageData)
        delete ImageData;
}

void Devmenu_Image::RenderImage(int x, int y)
{
    unsigned int colour = 0;
    for (int xx = 0; xx < Width; ++xx)
    {
        for (int yy = 0; yy < Height; ++yy)
        {
            colour = ImageData[yy * Width + xx];
            if (colour & 0xFF000000)
                SonicMania::DevMenu_DrawRect(x + xx, y + yy, 1, 1, (colour & 0x000000FF) << 16 | (colour & 0x0000FF00) | (colour & 0x00FF0000) >> 16, 255, 0, 1);
        }
    }
}

char MultiManiaStartupMenu()
{

    char result;

    int centerX = *(_DWORD *)(SonicMania::dword_D3CC00 + 614416);
    int centerY = *(_DWORD *)(SonicMania::dword_D3CC00 + 614420);
    int YPosition = centerY - 90;

    SonicMania::DevMenu_DrawRect(centerX - 158, YPosition, 316, 168, 0x00000080, 255, 0, 1);
    YPosition += 8;
    SonicMania::DevMenu_DrawRect(centerX - 154, YPosition - 3, 308, 12, 0x00000000, 255, 0, 1);
    if (Page == 0)
    {
        SonicMania::DevMenu_DrawText(centerX - 150, "1/5", YPosition, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Welcome", YPosition, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 8;

        if (Logo)
            Logo->RenderImage(centerX - 75, YPosition);
        YPosition += 112;
        SonicMania::DevMenu_DrawText(centerX, "Thanks for installing MultiMania!", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Press A to continue.", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
    }
    else if (Page == 1)
    {
        SonicMania::DevMenu_DrawText(centerX - 150, "2/5", YPosition, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Creators of MultiMania", YPosition, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 8;

        SonicMania::DevMenu_DrawText(centerX, "MultiMania Credits:", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "SuperSonic16     Lead Programmer", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "CodenameGamma    Reverse Engineering", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "DaveAce          Logo Design/Artwork", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "Rubberduckycooly Help with Testing", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        YPosition += 8;
        SonicMania::DevMenu_DrawText(centerX, "ManiaAPI Credits:", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "SuperSonic16     Programmer, RE", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "CodenameGamma    Reverse Engineering", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 140, "LJSTAR           Reverse Engineering", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        YPosition += 8;
        SonicMania::DevMenu_DrawText(centerX, "Press A to continue.", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
    }
    else if (Page == 2)
    {
        SonicMania::DevMenu_DrawText(centerX - 150, "3/5", YPosition, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Quick Start for Hosting", YPosition, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;

        SonicMania::DevMenu_DrawText(centerX - 145, "To host you must open the MultiMania", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " menu by pressing ESC, and go down to", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " Host Game, this should give you a", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " code like MM012345. Give this code", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " to your partner so they can connect", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " to you.", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        YPosition += 6;
        SonicMania::DevMenu_DrawText(centerX, "Connecting on Main Menu is recommanded", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;
        SonicMania::DevMenu_DrawText(centerX, "For detailed Instructions, please", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "read to the MultiMania Manual", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;
        SonicMania::DevMenu_DrawText(centerX, "Press A to continue.", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
    }
    else if (Page == 3)
    {
        SonicMania::DevMenu_DrawText(centerX - 150, "4/5", YPosition, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Quick Start for Connecting", YPosition, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;

        SonicMania::DevMenu_DrawText(centerX - 145, "To connect to your partner, you must", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " first open the MultiMania menu by", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " pressing ESC, and selecting", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " Connection code. Pressing right or", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " left will move the red cursor,", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " pressing up and down will change", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " the code, match up the code with", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " your partner's and then go to", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX - 145, " Connect and press A to connect.", YPosition += 12, SonicMania::Alignment_Left, 0xF0F0F0);
        YPosition += 12;
        YPosition += 6 ;
        SonicMania::DevMenu_DrawText(centerX, "Press A to continue.", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
    }
    else if (Page == 4)
    {
        SonicMania::DevMenu_DrawText(centerX - 150, "5/5", YPosition, SonicMania::Alignment_Left, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "Ready!", YPosition, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;

        YPosition += 12;
        YPosition += 12;
        SonicMania::DevMenu_DrawText(centerX, "You are now ready to start playing", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "with your friends. We all hope you", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, "enjoy playing with MultiMania", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 12;
        YPosition += 12;
        SonicMania::DevMenu_DrawText(centerX, "If you are running into any issues then", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, " please check out the MultiMania Manual", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        SonicMania::DevMenu_DrawText(centerX, " included for detailed Instructions", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);
        YPosition += 6;
        SonicMania::DevMenu_DrawText(centerX, "Press A to continue.", YPosition += 12, SonicMania::Alignment_Centre, 0xF0F0F0);


    }
    if ((Key_Enter | Controller_A) == 1)
    {
        if (Page >= 4)
        {
            Page = 0;
            //SonicMania::GameState = *(SonicMania::GameStates*)(baseAddress + 0x002FBB54);
        }
        else
            ++Page;
    }

    return 0;
}