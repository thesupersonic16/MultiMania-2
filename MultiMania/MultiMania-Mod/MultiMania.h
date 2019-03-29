#pragma once
#include <ManiaModLoader.h>

static HMODULE MultiManiaCS;
#define MultiManiaFunc(name, ARGS) static int(__cdecl *name)ARGS;

#define LoadMultiManiaFunc(name) name = (decltype(name))GetProcAddress(MultiManiaCS, #name);

MultiManiaFunc(InitMultiMania, ());

static void LoadExports()
{
    LoadMultiManiaFunc(InitMultiMania);
}