#include "mod.h"

// libc / libdl / libm / liblog are resolved at load time from the already-loaded system libs.
extern "C" {
void* dlopen(const char*, int);
void* dlsym(void*, const char*);
int vsnprintf(char*, size_t, const char*, va_list);
void __android_log_print(int prio, const char* tag, const char* fmt, ...);
}

void plog(const char* fmt, ...) {
    char b[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    __android_log_print(4 /* ANDROID_LOG_INFO */, "PhiMeMod", "%s", b);
}
void plogv(const char* fmt, va_list ap) {
    char b[2048];
    vsnprintf(b, sizeof b, fmt, ap);
    __android_log_print(4, "PhiMeMod", "%s", b);
}
