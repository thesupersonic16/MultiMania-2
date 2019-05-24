#pragma once
class Devmenu_Image
{
public:
    unsigned int* ImageData = nullptr;
    int Width = 0;
    int Height = 0;

    Devmenu_Image(const char* path, int width, int height);
    ~Devmenu_Image();
    void RenderImage(int x, int y);
};


extern char MultiManiaStartupMenu();
extern Devmenu_Image* Logo;
extern void Init();
