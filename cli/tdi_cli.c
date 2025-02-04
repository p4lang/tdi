/*
 * Copyright(c) 2024 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sched.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <semaphore.h>

#include <Python.h>
#include <tdi/common/c_frontend/tdi_init.h>
#include <tdi/common/tdi_defs.h>
#include <target-utils/clish/shell.h>
#include <target-utils/clish/thread.h>
#include <target-sys/bf_sal/bf_sys_mem.h>

#define MAX_PATH_CFG    4096

/*
 * Start TDI Runtime CLI. This program heavily borrows from the python C
 * interface example program.
 */
PyObject *tdipModule = NULL;
static int tdi_start_cli(int in_fd,
                         int out_fd,
                         const char *install_dir,
                         const char *udf,
                         bool interactive) {
  PyObject *pArgs = NULL, *pValue = NULL;
  tdi_dev_id_t *dev_id_list = NULL;
  uint32_t array_size = 0;
  PyObject *pFunc = NULL;
  char *user_cfg = NULL;
  int ret_val = 0;
  PyConfig config;

  PyConfig_InitPythonConfig(&config);
  PyConfig_Read(&config);
  Py_InitializeFromConfig(&config);

  // Setting tdi_python search path dynamically
  // consider user_cfg as maximum path config(4096)
  user_cfg = bf_sys_calloc(1, MAX_PATH_CFG);
  if (user_cfg == NULL) {
	fprintf(stderr, "Failed to allocate user cfg path variable \n");
        ret_val = 1;
	goto cleanup;
  }
  snprintf(user_cfg, MAX_PATH_CFG - 1, "sys.path.append('%s/lib/python')\n", install_dir);
  PyRun_SimpleString("import sys\n");
  PyRun_SimpleString(user_cfg);

  tdi_num_device_id_list_get(&array_size);
  if (array_size) {
    dev_id_list = bf_sys_malloc(array_size * sizeof(tdi_dev_id_t));
    if (!dev_id_list) {
        fprintf(stderr, "Failed tp allocate device id list\n");
	ret_val = 1;
	goto cleanup;
    }
    tdi_device_id_list_get(dev_id_list);
  }

  // first run, initialize python interpreter
  if (tdipModule == NULL) {
    Py_Initialize();
    PyObject *pName;
    /* Load the tdiRtcli python program. Py_Initialize loads its libraries from
    the install dir we installed Python into. */
    pName = PyUnicode_DecodeFSDefault("tdiRtCli");
    /* Error checking of pName left out */
    tdipModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (tdipModule == NULL) {
      printf("cannot import module in tdi\n");
      ret_val = 1;
      goto cleanup;
    }
  }

  if (tdipModule != NULL) {
    // Create a call to the start_rt_tdi function in tdiRtCli.py
    pFunc = PyObject_GetAttrString(tdipModule, "start_tdi_rt");
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      // Use a tuple as our argument list
      if (udf) {
        pArgs = PyTuple_New(6);
      } else {
        pArgs = PyTuple_New(4);
      }

      if (!pArgs) {
          fprintf(stderr, "Failed to allocate pArgs\n");
	  ret_val = 1;
	  goto cleanup;
      }
      // Create python objects from c types.
      // Place references to them in the argument tuple.
      pValue = PyLong_FromLong(in_fd);
      PyTuple_SetItem(pArgs, 0, pValue);
      pValue = PyLong_FromLong(out_fd);
      PyTuple_SetItem(pArgs, 1, pValue);
      /*
       * Convert from the filepath c string to a python string using the
       * filesystem's default encoding
       */
      pValue = PyUnicode_DecodeFSDefault(install_dir);
      PyTuple_SetItem(pArgs, 2, pValue);
      PyObject *pyList = PyList_New(array_size);
      if (!pyList) {
	  fprintf(stderr, "Failed to allocate python list\n");
          ret_val = 1;
	  goto cleanup;
      }
      for (uint32_t i = 0; i < array_size; i++) {
        pValue = PyLong_FromLong(dev_id_list[i]);
        PyList_SetItem(pyList, i, pValue);
      }
      PyTuple_SetItem(pArgs, 3, pyList);
      if (udf) {
        pValue = PyUnicode_DecodeFSDefault(udf);
        PyTuple_SetItem(pArgs, 4, pValue);
        pValue = PyBool_FromLong(interactive);
        PyTuple_SetItem(pArgs, 5, pValue);
      }

      // Actually make the function call.
      // This is where we block until the CLI exits
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);

      // Handle exit codes and decrement references to free our python objects
      if (pValue != NULL) {
        long ret = PyLong_AsLong(pValue);
        if (ret == 0) {
          printf("tdi cli exited normally.\n");
        } else {
          printf("tdi cli exited with error: %ld\n", ret);
        }
        Py_DECREF(pValue);
      } else {
        Py_DECREF(pFunc);
        PyErr_Print();
        fprintf(stderr, "tdi cli python call failed\n");
        ret_val = 1;
        goto cleanup;
      }
    } else {
      if (PyErr_Occurred()) PyErr_Print();
      fprintf(stderr, "Cannot find start_tdi_rt function.\n");
    }
    Py_XDECREF(pFunc);
  } else {
    PyErr_Print();
    fprintf(stderr, "Failed to load tdicli python library\n");
    ret_val = 1;
    goto cleanup;
  }
cleanup:
  // After execution of Py_Fynalize will be needed call
  // Py_Initialize and PyImport_Import that leads to a memory leak
  // because of previous imported lib will not be removed
  PyGC_Collect();
  if (dev_id_list) {
    bf_sys_free(dev_id_list);
  }
  if (user_cfg)
    bf_sys_free(user_cfg);
  return ret_val;
}

// A klish plugin allowing TDI Runtime CLI to be started from tdishell.
CLISH_PLUGIN_SYM(tdi_cli_cmd) {
  (void)script;
  (void)out;
  tdi_status_t sts;
  clish_shell_t *tdishell = clish_context__get_shell(clish_context);
  //bool success = TRY_PYTHON_SHL_LOCK();
  bool success = true;
  if (!success) {
    bfshell_printf(clish_context,
                   "Only one Python shell instance allowed at a time. tdi "
                   "python and debug python share the python shell "
                   "resource.\n");
    return 0;
  }

  clish_pargv_t *pargv = clish_context__get_pargv(clish_context);
  const clish_parg_t *parg = clish_pargv_find_arg(pargv, "py_file");
  const char *udf = NULL;
  if (parg) {
    udf = clish_parg__get_value(parg);
  }
  parg = clish_pargv_find_arg(pargv, "interactive");
  const char *i_str = NULL;
  bool interactive = false;
  if (parg) {
    i_str = clish_parg__get_value(parg);
    if (i_str && strcmp(i_str, "1") == 0) {
      interactive = true;
    }
  }

  tinyrl_t *bftinyrl = clish_shell__get_tinyrl(tdishell);
  sts = tdi_start_cli(fileno(tinyrl__get_istream(bftinyrl)),
                      fileno(tinyrl__get_ostream(bftinyrl)),
                      clish_context__get_install_dir(clish_context),
                      udf,
                      interactive);
  if (sts != TDI_SUCCESS) {
    bfshell_printf(clish_context,
                   "%s:%d could not initialize tdi for the cli. err: %d\n",
                   __func__,
                   __LINE__,
                   sts);
  }
//  RELEASE_PYTHON_SHL_LOCK();
  return 0;
}

CLISH_PLUGIN_SYM(tdi_run_file_cmd) {
  (void)script;
  (void)out;

  clish_shell_t *this = clish_context__get_shell(clish_context);
  clish_pargv_t *pargv = clish_context__get_pargv(clish_context);

  const char *filename = NULL;
  const clish_parg_t *parg = clish_pargv_find_arg(pargv, "filename");
  if (parg) {
    filename = clish_parg__get_value(parg);
  }

  int stop_on_error = 1;
  parg = clish_pargv_find_arg(pargv, "stop_on_error");
  if (parg) {
    const char *i_str = clish_parg__get_value(parg);
    if (i_str && strcmp(i_str, "0") == 0) {
      stop_on_error = 0;
    }
  }

  int result = -1;
  struct stat fileStat;

  /*
   * Check file specified is not a directory
   */
  if (filename && (0 == stat((char *)filename, &fileStat)) &&
      (!S_ISDIR(fileStat.st_mode))) {
    /*
     * push this file onto the file stack associated with this
     * session. This will be closed by clish_shell_pop_file()
     * when it is finished with.
     */
    result = clish_shell_push_file(this, filename, stop_on_error);
  }

  return result ? -1 : 0;
}

CLISH_PLUGIN_INIT(tdi) {
  (void)clish_shell;
  clish_plugin_add_sym(plugin, tdi_cli_cmd, "tdi_cli_cmd");
  clish_plugin_add_sym(plugin, tdi_run_file_cmd, "tdi_run_file_cmd");
  return 0;
}

/**
 * User exposed function to start the CLI shell
 */
int tdi_shell_start(char *install_dir,
                           char **p4_names) {

  /* Note that the CLI thread is responsible for free-ing this data */
  char *install_dir_path = calloc(1024, sizeof(char));

  /* Start the CLI server thread */
  snprintf(install_dir_path, 1023, "%s/", install_dir);
  printf("ipu_p4d: spawning cli server thread. Install path: %s\n", install_dir_path);
  cli_thread_main(install_dir_path, p4_names, true);
  cli_run_bfshell();

  printf("ipu_p4d: server started - listening on port 9999\n");

  return 0;
}

