#ifndef RENAME_H
#define RENAME_H 1

#include "persist-tool.h"
#include "syslog-ng.h"
#include "mainloop.h"
#include "persist-state.h"
#include "plugin.h"
#include "state.h"
#include "cfg.h"

gchar *old_key;
gchar *new_key;

gint rename_main(int argc, char *argv[]);

#endif
