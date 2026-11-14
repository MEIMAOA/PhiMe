#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define PM_VERSION "1.6.0"
#define PM_VERSION_NUM 10600

// ---- forward declarations of the IL2CPP opaque types we pass around ----
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppClass Il2CppClass;
typedef struct Il2CppObject Il2CppObject;
typedef struct Il2CppString Il2CppString;
typedef struct Il2CppArray Il2CppArray;
typedef struct MethodInfo MethodInfo;
typedef struct FieldInfo FieldInfo;
typedef struct Il2CppType Il2CppType;

typedef Il2CppObject Obj;

// ---- logging ----
void plog(const char* fmt, ...);
void plogv(const char* fmt, va_list ap);

// ---- the IL2CPP binding (il2cpp.cpp) ----
namespace il2cpp {
bool resolve();                       // dlsym the il2cpp_* API; returns true once all found
Il2CppImage* gameImage();             // the Assembly-CSharp image (or null)
Il2CppClass* findClass(const char* ns, const char* name);
MethodInfo* findMethod(Il2CppClass* k, const char* name, int argc);
MethodInfo* findMethodWithSig(Il2CppClass* k, const char* name, int argc, uint8_t firstParamType);
FieldInfo* findField(Il2CppClass* k, const char* name);
int fieldOffset(Il2CppClass* k, const char* name);   // -1 if missing
int fieldOffset(FieldInfo* f);

// raw API (resolved at runtime)
Il2CppDomain* domainGet();
void* threadAttach(Il2CppDomain* d);   // attach this native thread to the IL2CPP domain
Il2CppObject* objectNew(const Il2CppClass* k);
Il2CppClass* objectClass(Il2CppObject* o);
Il2CppString* stringNew(const char* utf8);
int stringBytes(Il2CppString* s, char* out, int cap);   // UTF-16 -> UTF-8, returns length (excl NUL)
Il2CppObject* runtimeInvoke(MethodInfo* m, void* obj, void** params, bool* ok);
void runtimeClassInit(Il2CppClass* k);
// patch a method's methodPointer; returns the previous pointer (for chaining)
void* hookMethod(MethodInfo* m, void* replacement);
void fieldStaticGet(FieldInfo* f, void* out);
void fieldStaticSet(FieldInfo* f, void* value);
Il2CppArray* arrayNew(const Il2CppClass* elementClass, uintptr_t len);
uintptr_t arrayLen(Il2CppArray* a);
void* arrayAddr(Il2CppArray* a, uintptr_t idx);         // element pointer (object-sized)
uint32_t gchandleNew(Il2CppObject* o, bool pinned);
void gchandleFree(uint32_t h);
Il2CppObject* gchandleTarget(uint32_t h);
void ilFree(void* p);
}

// ---- small helpers ----
template <typename T> inline T& fld(void* obj, int off) { return *(T*)((char*)obj + off); }

// Create a managed string from a C string (used as an argument).
Il2CppString* mstr(const char* s);

// config
struct ModConfig {
    bool autoplay = false;
    int autoRank = 0;   // 0 none, 1 V, 2 S, 3 A, 4 B, 5 C
    bool unlockIgallta = false;
    bool unlockAvatars = false;
    bool unlockClues = false;
    bool unlockIllustrations = false;
};
extern ModConfig g_cfg;
void configLoad();

// feature installs
bool autoplayInstall();
bool keysInstall();
bool panelInstall();

// one-click unlocks (0 Igallta, 1 线索/收藏品, 2 曲绘, 3 头像)
void unlockAction(int which);

// PlayerPrefs helpers (Unity PlayerPrefs via IL2CPP)
int prefGetInt(const char* key, int def);
void prefSetInt(const char* key, int val);
