/*
** mrb_jvm_process.c - File class
*/


#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/ext/io.h"

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  #include <stdio.h>
#else
  #include <dlfcn.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <signal.h>
#endif

#include <string.h>
#include "mrb_jvm_windows.h"

mrb_value mrb_find_native_java(mrb_state *mrb, mrb_value obj)
{
  char *java_home = NULL;
  char buff[PATH_MAX];
#if defined(_WIN32) || defined(_WIN64)
  java_home = get_java_home_from_registry("Software\\JavaSoft\\Java Development Kit");
  if (!java_home) {
    java_home = get_java_home_from_registry("Software\\JavaSoft\\Java Runtime Environment");
  }
#elif defined(__APPLE__)
  FILE *fp = popen("/usr/libexec/java_home", "r");
  if (fp == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Failed to run `/usr/libexec/java_home'");
  }

  // Read only the first line of output
  java_home = fgets(buff, sizeof(buff)-1, fp);
  pclose(fp);
#else
  struct stat buf_stat;
  int x;

  ssize_t len = readlink("/usr/bin/java", buff, sizeof(buff)-1);
  buff[len] = '\0';

  x = lstat (buff, &buf_stat);
  if (S_ISLNK(buf_stat.st_mode)) { // /etc/alternatives/java
    char java_link[len];
    strcpy(java_link, buff);
    len = readlink(java_link, buff, sizeof(buff)-1);
    buff[len] = '\0';
  }

  java_home = buff;
#endif
  return mrb_str_new_cstr(mrb, java_home);
}


mrb_value mrb_process_exec(char *const *pargv, int pargc)
{
  int ret, i;

  fflush(stdout);
  fflush(stderr);
#if defined(_WIN32) || defined(_WIN64)
  char cmd[32*1024] = "";
  for (i = 0; i < pargc-1; i++) {
    strcat(cmd, pargv[i]);
    strcat(cmd, " ");
  }

  STARTUPINFO si = {0};
  si.cb = sizeof(STARTUPINFO);
  PROCESS_INFORMATION pi = {0};

  if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)){
    // mrb_raise(mrb, E_RUNTIME_ERROR, "Failed to create process");
    return mrb_false_value();
  }

  disable_folder_virtualization(pi.hProcess);
  ResumeThread(pi.hThread);
  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return mrb_true_value();
#else
  void (*chfunc)(int);

  chfunc = signal(SIGCHLD, SIG_DFL);
  ret = execv(pargv[0], pargv);
  signal(SIGCHLD, chfunc);

  if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
    return mrb_true_value();
  }

  return mrb_false_value();
#endif
}

void launch_jvm_out_of_proc(mrb_state *mrb, char *java_exe, char *java_main_class,
			    char **java_opts, int java_optsc, char **prgm_opts, int prgm_optsc)
{
  int ret, i, pargvc;
  void (*chfunc)(int);

  pargvc = java_optsc + prgm_optsc + 3;
  char **pargv = malloc(pargvc * sizeof(void*));
  pargv[0] = java_exe;
  for (i = 0; i < java_optsc; i++) {
    pargv[i+1] = java_opts[i];
  }
  if (java_main_class && (strcmp(java_main_class, "") != 0)) {
    pargv[java_optsc+1] = java_main_class;
  }
  for (i = 0; i < prgm_optsc; i++) {
    pargv[i+java_optsc+2] = prgm_opts[i];
  }
  pargv[pargvc-1] = NULL;

  mrb_process_exec(pargv, pargvc);
}
