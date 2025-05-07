/*
    Данная библиотека полностью свободная и под лицензией GPL 3.0.
    Авторские права - ITlopa
*/
#define OPENHL_EXPORTS
#include "openhl.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

constexpr size_t HOOK_JMP_SIZE = 5;

struct HookData {
    void* target;
    std::vector<unsigned char> original_bytes;
    void* hook_func;
    bool installed;

    HookData() : target(nullptr), hook_func(nullptr), installed(false) {}
};

static std::unordered_map<void*, HookData> g_hooks;
static std::mutex g_hooks_mutex;
static bool g_initialized = false;

bool ChangeMemoryProtection(void* address, size_t size, int newProtect) {
    return mprotect(static_cast<void*>(static_cast<char*>(address) - ((uintptr_t)address % sysconf(_SC_PAGESIZE))), 
                    size + ((uintptr_t)address % sysconf(_SC_PAGESIZE)), newProtect) == 0;
}

void WriteJmp(void* from, void* to) {
    unsigned char* p = (unsigned char*)from;
    uintptr_t relAddress = (uintptr_t)to - (uintptr_t)from - HOOK_JMP_SIZE;

    p[0] = 0xE9;
    *(uint32_t*)(p + 1) = (uint32_t)relAddress;
}

extern "C" {

OPENHL_API OpenHL_Result OpenHL_Init(void) {
    if (g_initialized) return OPENHL_OK;

    std::lock_guard<std::mutex> lock(g_hooks_mutex);
    g_hooks.clear();
    g_initialized = true;
    return OPENHL_OK;
}

OPENHL_API void OpenHL_Shutdown(void) {
    if (!g_initialized) return;

    std::lock_guard<std::mutex> lock(g_hooks_mutex);

    for (auto& pair : g_hooks) {
        HookData& data = pair.second;
        if (data.installed) {
            if (ChangeMemoryProtection(data.target, HOOK_JMP_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
                memcpy(data.target, data.original_bytes.data(), HOOK_JMP_SIZE);
                data.installed = false;
            }
        }
    }

    g_hooks.clear();
    g_initialized = false;
}

OPENHL_API OpenHL_Result OpenHL_InstallHook(void* target_address, void* hook_function) {
    if (!g_initialized || target_address == nullptr || hook_function == nullptr) return OPENHL_ERROR;

    std::lock_guard<std::mutex> lock(g_hooks_mutex);
    if (g_hooks.find(target_address) != g_hooks.end()) return OPENHL_ERROR;

    HookData data;
    data.target = target_address;
    data.hook_func = hook_function;
    unsigned char* pTarget = (unsigned char*)target_address;
    data.original_bytes.assign(pTarget, pTarget + HOOK_JMP_SIZE);

    if (!ChangeMemoryProtection(target_address, HOOK_JMP_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) return OPENHL_ERROR;

    WriteJmp(target_address, hook_function);

    data.installed = true;
    g_hooks[target_address] = data;
    return OPENHL_OK;
}

OPENHL_API OpenHL_Result OpenHL_RemoveHook(void* target_address) {
    if (!g_initialized || target_address == nullptr) return OPENHL_ERROR;

    std::lock_guard<std::mutex> lock(g_hooks_mutex);

    auto it = g_hooks.find(target_address);
    if (it == g_hooks.end()) return OPENHL_ERROR;

    HookData& data = it->second;
    if (!data.installed) return OPENHL_OK;

    if (ChangeMemoryProtection(data.target, HOOK_JMP_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        memcpy(data.target, data.original_bytes.data(), HOOK_JMP_SIZE);
        data.installed = false;
    }

    g_hooks.erase(it);
    return OPENHL_OK;
}
}
