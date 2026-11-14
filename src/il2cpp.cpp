#include "mod.h"

extern "C" {
void* dlopen(const char*, int);
void* dlsym(void*, const char*);
void* calloc(size_t, size_t);
void* malloc(size_t);
void free(void*);
void* memcpy(void*, const void*, size_t);
void* memset(void*, int, size_t);
int strcmp(const char*, const char*);
size_t strlen(const char*);
char* strcpy(char*, const char*);
char* strchr(const char*, int);
char* strrchr(const char*, int);
int open(const char*, int);
long read(int, void*, unsigned long);
int close(int);
}

// Find the full on-disk path of a loaded shared library from /proc/self/maps.
static const char* findLib(const char* name) {
    static char out[512];
    static char buf[1 << 19];  // 512 KB: /proc/self/maps is large for a Unity process
    int fd = open("/proc/self/maps", 0 /* O_RDONLY */);
    if (fd < 0) return 0;
    long total = 0;
    while (total < (long)(sizeof buf - 1)) {
        long n = read(fd, buf + total, (unsigned long)(sizeof buf - 1 - total));
        if (n <= 0) break;
        total += n;
    }
    close(fd);
    if (total <= 0) return 0;
    buf[total] = 0;
    size_t nameLen = strlen(name);
    char* p = buf;
    while (p && *p) {
        char* nl = strchr(p, '\n');
        if (nl) *nl = 0;
        char* path = strrchr(p, ' ');
        if (path) {
            path++;
            size_t pl = strlen(path);
            if (pl >= nameLen && !strcmp(path + pl - nameLen, name)) {
                strcpy(out, path);
                return out;
            }
        }
        p = nl ? nl + 1 : 0;
    }
    return 0;
}

namespace il2cpp {

// ---- resolved API function pointers ----
typedef Il2CppDomain* (*FnDomainGet)(void);
typedef Il2CppAssembly** (*FnDomainAssemblies)(const Il2CppDomain*, size_t*);
typedef Il2CppImage* (*FnAssemblyImage)(const Il2CppAssembly*);
typedef void* (*FnThreadAttach)(Il2CppDomain*);
typedef Il2CppClass* (*FnClassFromName)(const Il2CppImage*, const char*, const char*);
typedef const char* (*FnClassName)(Il2CppClass*);
typedef const char* (*FnClassNamespace)(Il2CppClass*);
typedef const MethodInfo* (*FnMethodFromName)(Il2CppClass*, const char*, int);
typedef FieldInfo* (*FnFieldFromName)(Il2CppClass*, const char*);
typedef size_t (*FnFieldOffset)(FieldInfo*);
typedef Il2CppObject* (*FnObjectNew)(const Il2CppClass*);
typedef Il2CppClass* (*FnObjectClass)(Il2CppObject*);
typedef Il2CppString* (*FnStringNew)(const char*);
typedef uint32_t (*FnStringLength)(Il2CppString*);
typedef uint16_t* (*FnStringChars)(Il2CppString*);
typedef Il2CppObject* (*FnRuntimeInvoke)(const MethodInfo*, void*, void**, void**);
typedef void (*FnRuntimeClassInit)(Il2CppClass*);
typedef void (*FnFieldStaticGet)(FieldInfo*, void*);
typedef void (*FnFieldStaticSet)(FieldInfo*, void*);
typedef Il2CppArray* (*FnArrayNew)(const Il2CppClass*, uintptr_t);
typedef uintptr_t (*FnArrayLength)(Il2CppArray*);
typedef uint32_t (*FnGchandleNew)(Il2CppObject*, bool);
typedef void (*FnGchandleFree)(uint32_t);
typedef Il2CppObject* (*FnGchandleTarget)(uint32_t);
typedef void (*FnFree)(void*);

static FnDomainGet pDomainGet;
static FnDomainAssemblies pDomainAssemblies;
static FnAssemblyImage pAssemblyImage;
static FnThreadAttach pThreadAttach;
static FnClassFromName pClassFromName;
static FnClassName pClassName;
static FnClassNamespace pClassNamespace;
static FnMethodFromName pMethodFromName;
static FnFieldFromName pFieldFromName;
static FnFieldOffset pFieldOffset;
static FnObjectNew pObjectNew;
static FnObjectClass pObjectClass;
static FnStringNew pStringNew;
static FnStringLength pStringLength;
static FnStringChars pStringChars;
static FnRuntimeInvoke pRuntimeInvoke;
static FnRuntimeClassInit pRuntimeClassInit;
static FnFieldStaticGet pFieldStaticGet;
static FnFieldStaticSet pFieldStaticSet;
static FnArrayNew pArrayNew;
static FnArrayLength pArrayLength;
static FnGchandleNew pGchandleNew;
static FnGchandleFree pGchandleFree;
static FnGchandleTarget pGchandleTarget;
static FnFree pFree;

static bool g_resolved = false;
static void* g_lib = 0;

bool resolve() {
    if (g_resolved) return true;
    static bool logged = false;
    if (!g_lib) {
        const char* path = findLib("libil2cpp.so");
        if (!logged) plog("PhiMe: findLib=%s", path ? path : "NULL");
        if (path) g_lib = dlopen(path, 0x2 /* RTLD_NOW */);
        if (!g_lib) g_lib = dlopen("libil2cpp.so", 0x4 /* RTLD_NOLOAD */);
        if (!g_lib) g_lib = dlopen("libil2cpp.so", 0x2);
        if (!g_lib) g_lib = (void*)0;  // RTLD_DEFAULT (global scope) as a last resort
        if (!logged) plog("PhiMe: dlopen handle=%p", g_lib);
    }
    #define R_(sym, p) do { if (!p) p = (decltype(p))dlsym(g_lib, sym); } while (0)
    R_("il2cpp_domain_get", pDomainGet);
    R_("il2cpp_domain_get_assemblies", pDomainAssemblies);
    R_("il2cpp_assembly_get_image", pAssemblyImage);
    R_("il2cpp_thread_attach", pThreadAttach);
    R_("il2cpp_class_from_name", pClassFromName);
    R_("il2cpp_class_get_name", pClassName);
    R_("il2cpp_class_get_namespace", pClassNamespace);
    R_("il2cpp_class_get_method_from_name", pMethodFromName);
    R_("il2cpp_class_get_field_from_name", pFieldFromName);
    R_("il2cpp_field_get_offset", pFieldOffset);
    R_("il2cpp_object_new", pObjectNew);
    R_("il2cpp_object_get_class", pObjectClass);
    R_("il2cpp_string_new", pStringNew);
    R_("il2cpp_string_length", pStringLength);
    R_("il2cpp_string_chars", pStringChars);
    R_("il2cpp_runtime_invoke", pRuntimeInvoke);
    R_("il2cpp_runtime_class_init", pRuntimeClassInit);
    R_("il2cpp_field_static_get_value", pFieldStaticGet);
    R_("il2cpp_field_static_set_value", pFieldStaticSet);
    R_("il2cpp_array_new", pArrayNew);
    R_("il2cpp_array_length", pArrayLength);
    R_("il2cpp_gchandle_new", pGchandleNew);
    R_("il2cpp_gchandle_free", pGchandleFree);
    R_("il2cpp_gchandle_get_target", pGchandleTarget);
    R_("il2cpp_free", pFree);
    #undef R_
    if (!pDomainGet || !pDomainAssemblies || !pAssemblyImage || !pClassFromName || !pClassName ||
        !pMethodFromName || !pFieldFromName || !pFieldOffset || !pObjectNew || !pObjectClass ||
        !pStringNew || !pRuntimeInvoke || !pArrayNew || !pArrayLength ||
        !pGchandleNew || !pGchandleFree || !pGchandleTarget) {
        if (!logged) plog("PhiMe: missing sym domain=%p cls=%p obj=%p", (void*)pDomainGet, (void*)pClassFromName, (void*)pObjectNew);
        return false;
    }
    logged = true;
    g_resolved = true;
    return true;
}

Il2CppDomain* domainGet() { return pDomainGet ? pDomainGet() : 0; }
void* threadAttach(Il2CppDomain* d) { return pThreadAttach ? pThreadAttach(d) : 0; }

Il2CppImage* gameImage() {
    Il2CppDomain* d = domainGet();
    if (!d) return 0;
    size_t count = 0;
    Il2CppAssembly** asms = pDomainAssemblies(d, &count);
    if (!asms) return 0;
    for (size_t i = 0; i < count; i++) {
        Il2CppImage* img = pAssemblyImage(asms[i]);
        if (!img) continue;
        // The game's Assembly-CSharp image is the one that contains GameInformation.
        if (pClassFromName(img, "", "GameInformation")) return img;
    }
    return 0;
}

Il2CppClass* findClass(const char* ns, const char* name) {
    // Search every assembly (not just Assembly-CSharp): PlayerPrefs etc. live in the
    // UnityEngine assemblies, GameInformation in Assembly-CSharp.
    Il2CppDomain* d = domainGet();
    if (!d) return 0;
    size_t count = 0;
    Il2CppAssembly** asms = pDomainAssemblies(d, &count);
    if (!asms) return 0;
    for (size_t i = 0; i < count; i++) {
        Il2CppImage* img = pAssemblyImage(asms[i]);
        if (!img) continue;
        Il2CppClass* c = pClassFromName(img, ns, name);
        if (c) return c;
    }
    return 0;
}

MethodInfo* findMethod(Il2CppClass* k, const char* name, int argc) {
    if (!k) return 0;
    return (MethodInfo*)pMethodFromName(k, name, argc);
}

MethodInfo* findMethodWithSig(Il2CppClass* k, const char* name, int argc, uint8_t) {
    return findMethod(k, name, argc);
}

FieldInfo* findField(Il2CppClass* k, const char* name) {
    return k ? pFieldFromName(k, name) : 0;
}

int fieldOffset(FieldInfo* f) { return f ? (int)pFieldOffset(f) : -1; }
int fieldOffset(Il2CppClass* k, const char* name) { return fieldOffset(findField(k, name)); }

Il2CppObject* objectNew(const Il2CppClass* k) { return pObjectNew(k); }
Il2CppClass* objectClass(Il2CppObject* o) { return pObjectClass(o); }
Il2CppString* stringNew(const char* utf8) { return pStringNew(utf8); }

int stringBytes(Il2CppString* s, char* out, int cap) {
    if (!s || !out || cap <= 0) return 0;
    uint32_t len = pStringLength(s);
    uint16_t* ch = pStringChars(s);
    int w = 0;
    for (uint32_t i = 0; i < len; i++) {
        uint32_t c = ch[i];
        // surrogate pairs
        if (c >= 0xD800 && c <= 0xDBFF && i + 1 < len && ch[i + 1] >= 0xDC00 && ch[i + 1] <= 0xDFFF) {
            c = 0x10000 + ((c - 0xD800) << 10) + (ch[++i] - 0xDC00);
        }
        if (c < 0x80) {
            if (w + 1 >= cap) break; out[w++] = (char)c;
        } else if (c < 0x800) {
            if (w + 2 >= cap) break; out[w++] = (char)(0xC0 | (c >> 6)); out[w++] = (char)(0x80 | (c & 0x3F));
        } else if (c < 0x10000) {
            if (w + 3 >= cap) break; out[w++] = (char)(0xE0 | (c >> 12)); out[w++] = (char)(0x80 | ((c >> 6) & 0x3F)); out[w++] = (char)(0x80 | (c & 0x3F));
        } else {
            if (w + 4 >= cap) break; out[w++] = (char)(0xF0 | (c >> 18)); out[w++] = (char)(0x80 | ((c >> 12) & 0x3F)); out[w++] = (char)(0x80 | ((c >> 6) & 0x3F)); out[w++] = (char)(0x80 | (c & 0x3F));
        }
    }
    out[w] = 0;
    return w;
}

Il2CppObject* runtimeInvoke(MethodInfo* m, void* obj, void** params, bool* ok) {
    if (ok) *ok = false;
    if (!m) return 0;
    void* exc = 0;
    Il2CppObject* r = pRuntimeInvoke((const MethodInfo*)m, obj, params, &exc);
    if (ok) *ok = (exc == 0);
    return r;
}

void* hookMethod(MethodInfo* m, void* replacement) {
    if (!m) return 0;
    void* prev = *(void**)m;   // methodPointer is the first field of MethodInfo
    *(void**)m = replacement;
    return prev;
}

void runtimeClassInit(Il2CppClass* k) { if (pRuntimeClassInit && k) pRuntimeClassInit(k); }
void fieldStaticGet(FieldInfo* f, void* out) { if (pFieldStaticGet && f) pFieldStaticGet(f, out); }
void fieldStaticSet(FieldInfo* f, void* value) { if (pFieldStaticSet && f) pFieldStaticSet(f, value); }
Il2CppArray* arrayNew(const Il2CppClass* e, uintptr_t n) { return pArrayNew(e, n); }
uintptr_t arrayLen(Il2CppArray* a) { return pArrayLength(a); }
// Il2CppArray = { klass, monitor, bounds, max_length } then the element vector.
void* arrayAddr(Il2CppArray* a, uintptr_t idx) { return (char*)a + 0x20 + idx * sizeof(void*); }
uint32_t gchandleNew(Il2CppObject* o, bool pinned) { return pGchandleNew(o, pinned); }
void gchandleFree(uint32_t h) { pGchandleFree(h); }
Il2CppObject* gchandleTarget(uint32_t h) { return pGchandleTarget(h); }
void ilFree(void* p) { if (pFree) pFree(p); }

}  // namespace il2cpp

// ---- C-string helpers outside the namespace ----
Il2CppString* mstr(const char* s) { return il2cpp::stringNew(s); }
