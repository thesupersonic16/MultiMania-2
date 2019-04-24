#pragma once
#include <ManiaModLoader.h>

static HMODULE MultiManiaCS;
#define MultiManiaFunc(name, ARGS) extern int(__cdecl *name)ARGS;
#define DefineMultiManiaFunc(name, ARGS) int(__cdecl *name)ARGS;

#define LoadMultiManiaFunc(name) name = (decltype(name))GetProcAddress(MultiManiaCS, #name);

MultiManiaFunc(InitMultiMania, ());
MultiManiaFunc(MultiMania_Connect, (const char* connectionCode, int PPS));
MultiManiaFunc(MultiMania_Close, ());
MultiManiaFunc(MultiMania_Host, (int PPS));
MultiManiaFunc(MultiMania_Update, ());
MultiManiaFunc(MultiMania_IsHost, ());
MultiManiaFunc(MultiMania_IsConnected, ());
MultiManiaFunc(MultiMania_SpawnObject, (short objectID, short subObject, DWORD x, DWORD y));

static void LoadExports()
{
    LoadMultiManiaFunc(InitMultiMania);
    LoadMultiManiaFunc(MultiMania_Connect);
    LoadMultiManiaFunc(MultiMania_Close);
    LoadMultiManiaFunc(MultiMania_Host);
    LoadMultiManiaFunc(MultiMania_Update);
    LoadMultiManiaFunc(MultiMania_IsHost);
    LoadMultiManiaFunc(MultiMania_IsConnected);
    LoadMultiManiaFunc(MultiMania_SpawnObject);
}