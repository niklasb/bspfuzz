#include <bits/stdc++.h>

#include <dlfcn.h>
#include <link.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

// dedicated
void (*CBaseFileSystem_Constr)(void*);
void (*CFileSystem_Stdio_Constr)(void*);
void (*CBaseFileSystem_AddMapPackFile)(void*, const char* pPath, const char* pPathID, int addType);
void (*AppFactory)();
void (*DedicatedMain)(int argc, const char** argv);
void (*set_steam_app)(void*);

void (*CSteamAppSystemGroup_Constr)(void*, int);
void (*CDedicatedSteamApplication_Constr)(void*, void*);
void *p_CDedicatedAppSystemGroup_vtable;
void **p_steam_app;

// engine
void (*CModelLoader_GetModelForName)(void*, const char* name, int referencetype);
void (*CDedicatedServerAPI_Connect)(void*, decltype(AppFactory));
void (*Memory_Init)();
void** p_g_baseFs;
void** p_modelloader;
void** p_materialsystem;
void** p_g_pDataCache;
void **p_g_pMaterialSystemHardwareConfig;

// shaderapiempty
void (*CShaderAPIEmpty_Constr)(void*);

// materialsystem
void (*CMaterialSystem_Constr1)(void*);
void** matsystem_p_shader;
void** matsystem_p_shader2;
void** matsystem_p_fs;

// datacache
void* g_dataCache;

template <typename T> void ptr(T*& f, void* so, uint32_t offset) {
    f = (T*)((char*)so + offset);
}

void forkserver() {
    fprintf(stderr, "forkserver()\n");
}

bool dbg;
char *mappath;

void startpoint() {
    fprintf(stderr, "startpoint()\n");

    void* modelloader = *p_modelloader;
    cout << "modelloader @ " << modelloader << endl;

    forkserver();

    if (dbg) {
        cerr << "Press enter to continue" << endl;
        getchar();
    }

    void *buf = alloca(0x10000);

    CModelLoader_GetModelForName(modelloader, mappath, 2);
    cout << "Done" << endl;

    _exit(0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " bspfile [--dbg]" << endl;
        return EXIT_FAILURE;
    }

    dbg = argc > 2 && string(argv[2]) == "--dbg";
    if (dbg) {
        cerr << "Debug mode enabled" << endl;
    }

    struct link_map *lm = (struct link_map*)dlopen("dedicated.so", RTLD_NOW);
    void* dedicated = (void*)lm->l_addr;
    assert(dedicated);
    lm = (struct link_map*)dlopen("engine.so", RTLD_NOW);
    void* engine = (void*)lm->l_addr;
    assert(engine);
    lm = (struct link_map*)dlopen("shaderapiempty.so", RTLD_NOW);
    void* shaderapiempty = (void*)lm->l_addr;
    assert(shaderapiempty);
    lm = (struct link_map*)dlopen("materialsystem.so", RTLD_NOW);
    void* materialsystem = (void*)lm->l_addr;
    assert(materialsystem);
    lm = (struct link_map*)dlopen("datacache.so", RTLD_NOW);
    void* datacache = (void*)lm->l_addr;
    assert(datacache);
    cout << "dedicated.so loaded at " << dedicated << endl;
    cout << "engine.so loaded at " << engine << endl;
    cout << "shaderapiempty.so loaded at " << shaderapiempty << endl;
    cout << "materialsystem.so loaded at " << materialsystem << endl;
    cout << "datacache.so loaded at " << datacache << endl;

    mappath = argv[1];
    if (mappath[0] != '/') {
        char tmp[2048];
        getcwd(tmp, sizeof tmp);
        strcat(tmp, "/");
        strcat(tmp, mappath);
        mappath = strdup(tmp);
    }
    cout << "Reading from " << mappath << endl;

    // dedicated
    ptr(CBaseFileSystem_Constr, dedicated, 0x3fc50);
    ptr(CFileSystem_Stdio_Constr, dedicated, 0x4d750);
    ptr(CBaseFileSystem_AddMapPackFile, dedicated, 0x3def0);
    ptr(AppFactory, dedicated, 0x7F750);
    ptr(DedicatedMain, dedicated, 0x1beb0);

    ptr(CSteamAppSystemGroup_Constr, dedicated, 0x7F650);
    ptr(CDedicatedSteamApplication_Constr, dedicated, 0x20710);
    ptr(set_steam_app, dedicated, 0x80830);
    ptr(p_CDedicatedAppSystemGroup_vtable, dedicated, 0xAA428);
    ptr(p_steam_app, dedicated, 0xf8f00);

    // engine
    ptr(CModelLoader_GetModelForName, engine, 0x180460);
    ptr(CDedicatedServerAPI_Connect, engine, 0x2C8690);
    ptr(Memory_Init, engine, 0x2E3B50);
    ptr(p_g_baseFs, engine, 0x757080);
    ptr(p_g_pMaterialSystemHardwareConfig, engine, 0x785C48);
    ptr(p_g_pDataCache, engine, 0x785BE4);
    ptr(p_modelloader, engine, 0x6E3C80);
    ptr(p_materialsystem, engine, 0x785C64);

    // shaderapiempty
    ptr(CShaderAPIEmpty_Constr, shaderapiempty, 0x92C0);

    // materialsystem
    ptr(CMaterialSystem_Constr1, materialsystem, 0x76c80);
    ptr(matsystem_p_shader, materialsystem, 0x189344);
    ptr(matsystem_p_shader2, materialsystem, 0x18933c);
    ptr(matsystem_p_fs, materialsystem, 0x1a00cc);

    // datacache
    ptr(g_dataCache, datacache, 0xdaf80);

    const char* args[] = {"x", "-game", "csgo", "-nominidumps", "-nobreakpad"};
    DedicatedMain(sizeof args / sizeof *args, args);
}
