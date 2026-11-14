#include "mod.h"

extern "C" {
int pthread_create(unsigned long* thread, const void* attr, void* (*fn)(void*), void* arg);
int pthread_detach(unsigned long thread);
unsigned int usleep(unsigned int usec);
}

ModConfig g_cfg;

static void* modThread(void*) {
    // Give the game time to load libil2cpp and finish il2cpp_init before we touch it.
    for (int i = 0; i < 360; i++) usleep(25000);  // ~9 s

    for (int i = 0; i < 400; i++) {
        if (il2cpp::resolve()) break;
        usleep(25000);
    }
    if (!il2cpp::resolve()) {
        plog("PhiMe: il2cpp API resolve failed");
        return 0;
    }
    plog("PhiMe: api resolved");

    Il2CppDomain* d = il2cpp::domainGet();
    plog("PhiMe: domain=%p", (void*)d);
    if (!d) return 0;
    il2cpp::threadAttach(d);
    plog("PhiMe: thread attached");

    // prove the binding reaches the game assembly
    Il2CppClass* GI = il2cpp::findClass("", "GameInformation");
    Il2CppClass* SM = il2cpp::findClass("", "SaveManagement");
    plog("PhiMe rebuilt v%s: GameInformation=%s SaveManagement=%s", PM_VERSION, GI ? "ok" : "MISSING",
         SM ? "ok" : "MISSING");

    // install (resolve) features first, then apply config (which may trigger unlocks).
    keysInstall();
    autoplayInstall();
    panelInstall();
    configLoad();
    return 0;
}

__attribute__((constructor)) static void phiMeInit() {
    unsigned long t = 0;
    pthread_create(&t, 0, modThread, 0);
    pthread_detach(t);
}
