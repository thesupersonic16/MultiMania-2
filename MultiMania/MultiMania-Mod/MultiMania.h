#pragma once
#include <ManiaModLoader.h>

static HMODULE MultiManiaCS;
#define MultiManiaFunc(name, ARGS) extern int(__cdecl *name)ARGS;
#define DefineMultiManiaFunc(name, ARGS) int(__cdecl *name)ARGS;

#define LoadMultiManiaFunc(name) name = (decltype(name))GetProcAddress(MultiManiaCS, #name);

MultiManiaFunc(InitMultiMania, ());
MultiManiaFunc(MultiMania_Connect, (const char* connectionCode, int PPS));
MultiManiaFunc(MultiMania_Host, (int PPS));
MultiManiaFunc(MultiMania_Update, ());

static void LoadExports()
{
    LoadMultiManiaFunc(InitMultiMania);
    LoadMultiManiaFunc(MultiMania_Connect);
    LoadMultiManiaFunc(MultiMania_Host);
    LoadMultiManiaFunc(MultiMania_Update);
}