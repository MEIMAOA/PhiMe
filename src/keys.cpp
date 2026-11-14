#include "mod.h"

// Unlock features, driven by the game's own save:
//  - Igallta (chapter 6 hidden song): SaveManagement.SaveBool on its flags.
//  - Head portrait / illustration / collection keys: each key is a SaveManagement int
//    named "<kindOfKey>key<keyName>" (3 = head portrait, 2 = illustration, 1 = collections
//    / clues, 0 = songs). Writing 1 there + GameInformation.CheckKeys() unlocks them, and
//    it survives a restart. The panel sets PhiMeMod.unlock* prefs to trigger these.

using namespace il2cpp;

static Il2CppClass* g_GI = nullptr;
static FieldInfo* g_giMain = nullptr;
static int g_giKeyStore = -1;
static MethodInfo* g_checkKeys = nullptr;
static Il2CppClass* g_Key = nullptr;
static int g_keyName = -1, g_keyUnlocked = -1, g_keyKind = -1, g_keyTimes = -1;
static MethodInfo* g_saveBool = nullptr;
static MethodInfo* g_saveInt = nullptr;
static int g_listItems = -1, g_listSize = -1;

static Obj* gameInfo() {
    Obj* g = nullptr;
    if (g_giMain) il2cpp::fieldStaticGet(g_giMain, &g);
    return g;
}

static int listView(Obj* list, Obj*** out) {
    *out = nullptr;
    if (!list) return 0;
    if (g_listItems < 0) {
        Il2CppClass* k = il2cpp::objectClass(list);
        g_listItems = il2cpp::fieldOffset(k, "_items");
        g_listSize = il2cpp::fieldOffset(k, "_size");
    }
    if (g_listItems < 0 || g_listSize < 0) return 0;
    Il2CppArray* a = fld<Il2CppArray*>(list, g_listItems);
    int n = fld<int>(list, g_listSize);
    if (!a || n <= 0 || (uintptr_t)n > il2cpp::arrayLen(a)) return 0;
    *out = (Obj**)il2cpp::arrayAddr(a, 0);
    return n;
}

static void saveInt(const char* key, int v) {
    if (!g_saveInt) return;
    Il2CppString* ks = il2cpp::stringNew(key);
    void* p[2] = { ks, &v };
    il2cpp::runtimeInvoke(g_saveInt, nullptr, p, nullptr);
}

static void saveBool(const char* key, bool v) {
    if (!g_saveBool) return;
    Il2CppString* ks = il2cpp::stringNew(key);
    bool b = v;
    void* p[2] = { ks, &b };
    il2cpp::runtimeInvoke(g_saveBool, nullptr, p, nullptr);
}

static void unlockIgallta() {
    saveBool("unlockFlagOfIgalltaEZ", true);
    saveBool("unlockFlagOfIgalltaHD", true);
    saveBool("unlockFlagOfIgalltaIN", true);
    saveBool("unlockFlagOfIgalltaAT", true);
    saveBool("VRUnlockFlagOfIgallta", true);
    saveBool("重生KeyCompleted", true);
    plog("keys: Igallta unlocked");
}

// Build "<kind>key<name>".
static void keyName(int kind, const char* name, char* out, int cap) {
    int p = 0;
    if (p < cap - 1) out[p++] = (char)('0' + kind);
    const char* tag = "key";
    for (int i = 0; tag[i] && p < cap - 1; i++) out[p++] = tag[i];
    for (int i = 0; name[i] && p < cap - 1; i++) out[p++] = name[i];
    out[p] = 0;
}

// kind: 1 collections/clues, 2 illustration, 3 head portrait
static void unlockKind(int kind) {    Obj* g = gameInfo();
    if (!g || g_giKeyStore < 0) return;
    Obj* store = fld<Obj*>(g, g_giKeyStore);
    Obj** e = nullptr;
    int n = listView(store, &e);
    int done = 0;
    for (int i = 0; i < n; i++) {
        Obj* k = e[i];
        if (!k) continue;
        if (fld<int>(k, g_keyKind) != kind) continue;
        char name[200];
        name[0] = 0;
        Il2CppString* ns = fld<Il2CppString*>(k, g_keyName);
        if (ns) il2cpp::stringBytes(ns, name, sizeof name);
        if (!*name) continue;
        char key[260];
        keyName(kind, name, key, sizeof key);
        saveInt(key, 1);
        fld<int>(k, g_keyUnlocked) = 1;
        if (fld<int>(k, g_keyTimes) < 1) fld<int>(k, g_keyTimes) = 1;
        done++;
    }
    if (g_checkKeys) il2cpp::runtimeInvoke(g_checkKeys, g, nullptr, nullptr);
    plog("keys: unlocked %d kind-%d entries", done, kind);
}

static void applyUnlocks() {
    if (prefGetInt("PhiMeMod.unlockIgallta", 0)) unlockIgallta();
    if (prefGetInt("PhiMeMod.unlockClues", 0)) unlockKind(1);
    if (prefGetInt("PhiMeMod.unlockIllustrations", 0)) unlockKind(2);
    if (prefGetInt("PhiMeMod.unlockAvatars", 0)) unlockKind(3);
}

// Re-read the save values into the key table (so unlocks written earlier stay applied
// after a restart even if the game's own module save hasn't run).
static void refreshKeys() {
    Obj* g = gameInfo();
    if (g && g_checkKeys) il2cpp::runtimeInvoke(g_checkKeys, g, nullptr, nullptr);
}

void unlockAction(int which) {
    if (!g_giMain || !g_giKeyStore || !g_saveInt) { plog("keys: not ready"); return; }
    switch (which) {
        case 0: unlockIgallta(); break;
        case 1: unlockKind(1); break;
        case 2: unlockKind(2); break;
        case 3: unlockKind(3); break;
    }
}

bool keysInstall() {
    g_GI = il2cpp::findClass("", "GameInformation");
    g_Key = il2cpp::findClass("", "Key");
    Il2CppClass* SM = il2cpp::findClass("", "SaveManagement");
    if (g_GI) {
        g_giMain = il2cpp::findField(g_GI, "_main");
        g_giKeyStore = il2cpp::fieldOffset(g_GI, "keyStore");
        g_checkKeys = il2cpp::findMethod(g_GI, "CheckKeys", 0);
    }
    if (g_Key) {
        g_keyName = il2cpp::fieldOffset(g_Key, "keyName");
        g_keyUnlocked = il2cpp::fieldOffset(g_Key, "unlockedTimes");
        g_keyKind = il2cpp::fieldOffset(g_Key, "kindOfKey");
        g_keyTimes = il2cpp::fieldOffset(g_Key, "unlockTimes");
    }
    if (SM) {
        g_saveInt = il2cpp::findMethod(SM, "SaveInt", 2);
        g_saveBool = il2cpp::findMethod(SM, "SaveBool", 2);
    }
    bool ok = g_giMain && g_giKeyStore >= 0 && g_checkKeys && g_keyName >= 0 && g_keyUnlocked >= 0 &&
              g_keyKind >= 0 && g_keyTimes >= 0 && g_saveInt && g_saveBool;
    plog("keys: %s (giMain=%d keyStore=%d checkKeys=%d key=%d/%d/%d/%d saveInt=%d saveBool=%d)", ok ? "ready" : "off",
         g_giMain != nullptr, g_giKeyStore, g_checkKeys != nullptr, g_keyName, g_keyUnlocked, g_keyKind, g_keyTimes,
         g_saveInt != nullptr, g_saveBool != nullptr);
    return ok;
}

void configLoad() {
    g_cfg.autoplay = prefGetInt("PhiMeMod.autoplay", 0) != 0;
    g_cfg.autoRank = prefGetInt("PhiMeMod.autoRank", 0);
    plog("config: autoplay=%d autoRank=%d", (int)g_cfg.autoplay, g_cfg.autoRank);
    applyUnlocks();   // pending one-shot unlocks from prefs
    refreshKeys();    // keep previously written unlocks applied every launch
}
