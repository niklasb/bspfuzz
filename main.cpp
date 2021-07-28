#include <bits/stdc++.h>

#include <dlfcn.h>
#include <link.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

// dedicated
void (*DedicatedMain)(int argc, const char** argv);

// engine
void (*CModelLoader_GetModelForName)(void*, const char* name, int referencetype);
void** p_modelloader;

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
    if(lm == NULL){
    	
    	fprintf(stderr, dlerror());
    
    }
    void* dedicated = (void*)lm->l_addr;
    assert(dedicated);
    lm = (struct link_map*)dlopen("engine.so", RTLD_NOW);
    void* engine = (void*)lm->l_addr;
    assert(engine);
    

    cout << "dedicated.so loaded at " << dedicated << endl;
    cout << "engine.so loaded at " << engine << endl;

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
    ptr(DedicatedMain, dedicated, 0x1beb0);

    // engine
    ptr(CModelLoader_GetModelForName, engine, 0x180460);
    ptr(p_modelloader, engine, 0x6E3C80);

    const char* args[] = {"x", "-game", "csgo", "-nominidumps", "-nobreakpad"};
    DedicatedMain(sizeof args / sizeof *args, args);
}
