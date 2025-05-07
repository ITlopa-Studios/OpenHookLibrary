/*
    Данная библиотека полностью свободная и под лицензией GPL 3.0.
    Авторские права - ITlopa
*/
#ifndef OPENHL_H
#define OPENHL_H

#ifdef _WIN32
  #ifdef OPENHL_EXPORTS
    #define OPENHL_API __declspec(dllexport)
  #else
    #define OPENHL_API __declspec(dllimport)
  #endif
#else
  #ifdef OPENHL_EXPORTS
    #define OPENHL_API __attribute__((visibility("default")))
  #else
    #define OPENHL_API
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum OpenHL_Result {
    OPENHL_OK = 0,
    OPENHL_ERROR = -1
} OpenHL_Result;

OPENHL_API OpenHL_Result OpenHL_Init(void);
OPENHL_API void OpenHL_Shutdown(void);
OPENHL_API OpenHL_Result OpenHL_InstallHook(void* target_address, void* hook_function);
OPENHL_API OpenHL_Result OpenHL_RemoveHook(void* target_address);

#ifdef __cplusplus
}
#endif

#endif // OPENHL_H

