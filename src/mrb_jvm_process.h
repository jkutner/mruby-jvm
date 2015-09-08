/*
** mrb_jvm_process.h - File class
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
  #include <unistd.h>
  #include <signal.h> // This is specific to mrb_p_exec
#endif

#include <string.h>
#include <jni.h>

// This is specific to mrb_p_exec
static struct {
  const char *name;
  int no;
} signals[] = {
#include "signals.cstub"
  { NULL, 0 }
};

mrb_value
mrb_find_native_java(mrb_state *mrb, mrb_value obj);

mrb_value
mrb_p_exec(const char **pargv, int pargc);

void
launch_jvm_out_of_proc(mrb_state *mrb, const char *java_exe, const char *java_main_class, const char **java_opts, int java_optsc, const char **prgm_opts, int prgm_optsc);
