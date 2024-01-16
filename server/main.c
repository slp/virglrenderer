/*
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: MIT
 */

#include "render_context.h"
#include "render_server.h"
#include <sys/mman.h>

/* The main process is the server process.  It enters render_server_main and
 * never returns except on fatal errors.
 *
 * The server process supports only one connection currently.  It creates a
 * render_client to manage the connection.  There is a client process at the
 * other end of the connection.  When the client process requests a new
 * context to be created, the server process creates a worker.  It also sets
 * up a socket pair, with one end owned by the worker and the other end sent
 * to and owned by the client process.
 *
 * A worker can be a subprocess forked from the server process, or a thread
 * created by the server process.  When a worker is a subprocess, the
 * subprocess returns from render_server_main and enters render_context_main.
 *
 * When a worker is a thread, the thread enters render_context_main directly
 * from its start function.  In this case, render_context_main must be
 * thread-safe.
 */
int
main(int argc, char **argv)
{
   //int fd = 25;
   char *buf;

#if 0
   void *ret = mmap(NULL, 1024*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (ret == MAP_FAILED) {
      fprintf(stderr, "YYY - MAP_FAILED");
   } else {
      fprintf(stderr, "YYY - MAP_OK");
   }

   buf = (char *) ret;
   buf[0x69] = 'C';
#endif

   render_log_init();

   struct render_context_args ctx_args;
   bool ok = render_server_main(argc, argv, &ctx_args);

   /* this is a subprocess */
   if (ok && ctx_args.valid)
      ok = render_context_main(&ctx_args);

   return ok ? 0 : -1;
}
