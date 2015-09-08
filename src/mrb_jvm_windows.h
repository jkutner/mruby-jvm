/*
** mrb_jvm_windows.h - Helper functions for Windows
*/

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#include <string.h>

void disable_folder_virtualization(HANDLE hProcess);

char *get_string_from_registry(HKEY rootKey, const char *keyName, const char *valueName);

char *get_java_home_from_registry(char *java_key);
#endif
