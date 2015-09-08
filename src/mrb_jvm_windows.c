/*
** mrb_jvm_windows.c - Helper functions for Windows
*/

#include "mrb_jvm_windows.h"

#if defined(_WIN32) || defined(_WIN64)

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/ext/io.h"

#include <windows.h>
#include <stdio.h>

#include <string.h>

void disable_folder_virtualization(HANDLE hProcess) {
  OSVERSIONINFO osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
  if (GetVersionEx(&osvi) && osvi.dwMajorVersion == 6) // check it is Win VISTA
  {
    HANDLE hToken;
    if (OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) {
      DWORD tokenInfoVal = 0;
      if (!SetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS) 24, &tokenInfoVal, sizeof (DWORD))) {
        if (GetLastError() != ERROR_INVALID_PARAMETER) {
            return;
        }
      }
      CloseHandle(hToken);
    } else {
      return;
    }
  }
}

char *get_string_from_registry(HKEY rootKey, const char *keyName, const char *valueName) {
  HKEY hKey = 0;
  if (RegOpenKeyEx(rootKey, keyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
      DWORD valSize = 4096;
      DWORD type = 0;
      char val[4096] = "";
      if (RegQueryValueEx(hKey, valueName, 0, &type, (BYTE *) val, &valSize) == ERROR_SUCCESS && type == REG_SZ) {
          RegCloseKey(hKey);
          return val;
      } else {
          printf("RegQueryValueEx() failed.\n");
      }
      RegCloseKey(hKey);
  } else {
      printf("RegOpenKeyEx() failed.\n");
  }
  return NULL;
}

char *get_java_home_from_registry(char *java_key)
{
  char *version = get_string_from_registry(HKEY_LOCAL_MACHINE, java_key, "CurrentVersion");
  if (version) {
    char *sep = "\\";
    char *full_java_key = malloc(strlen(java_key)+strlen(sep)+strlen(version)+1);
    strcpy(full_java_key, java_key);
    strcat(full_java_key, sep);
    strcat(full_java_key, version);
    return get_string_from_registry(HKEY_LOCAL_MACHINE, full_java_key, "JavaHome");
  }
  return NULL;
}
#endif
