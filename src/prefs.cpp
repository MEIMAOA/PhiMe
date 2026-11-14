#include "mod.h"

// Unity PlayerPrefs via IL2CPP. The boxed int return of GetInt sits right after the
// object header (klass + monitor = 16 bytes).
using namespace il2cpp;

static MethodInfo* g_getInt = nullptr;
static MethodInfo* g_setInt = nullptr;
static MethodInfo* g_save = nullptr;
static bool g_tried = false;

static void resolvePrefs() {
    if (g_tried) return;
    g_tried = true;
    Il2CppClass* PP = il2cpp::findClass("UnityEngine", "PlayerPrefs");
    if (!PP) return;
    g_getInt = il2cpp::findMethod(PP, "GetInt", 2);
    g_setInt = il2cpp::findMethod(PP, "SetInt", 2);
    g_save = il2cpp::findMethod(PP, "Save", 0);
}

static int unboxInt(Il2CppObject* r) {
    if (!r) return 0;
    return *(int*)((char*)r + 0x10);
}

int prefGetInt(const char* key, int def) {
    resolvePrefs();
    if (!g_getInt) return def;
    Il2CppString* ks = il2cpp::stringNew(key);
    void* params[2] = { ks, &def };
    Il2CppObject* r = il2cpp::runtimeInvoke(g_getInt, nullptr, params, nullptr);
    return unboxInt(r);
}

void prefSetInt(const char* key, int val) {
    resolvePrefs();
    if (!g_setInt) return;
    Il2CppString* ks = il2cpp::stringNew(key);
    void* params[2] = { ks, &val };
    il2cpp::runtimeInvoke(g_setInt, nullptr, params, nullptr);
    if (g_save) il2cpp::runtimeInvoke(g_save, nullptr, nullptr, nullptr);
}
