#pragma once
#include <ManiaModLoader.h>

static HMODULE MultiManiaCS;
#define MultiManiaFunc(name, ARGS) extern int(__cdecl *name)ARGS;
#define DefineMultiManiaFunc(name, ARGS) int(__cdecl *name)ARGS;

#define LoadMultiManiaFunc(name) name = (decltype(name))GetProcAddress(MultiManiaCS, #name);

MultiManiaFunc(InitMultiMania, ());
MultiManiaFunc(MultiMania_Connect, (const char* connectionCode));
MultiManiaFunc(MultiMania_Host, ());

static void LoadExports()
{
    LoadMultiManiaFunc(InitMultiMania);
    LoadMultiManiaFunc(MultiMania_Connect);
    LoadMultiManiaFunc(MultiMania_Host);
}