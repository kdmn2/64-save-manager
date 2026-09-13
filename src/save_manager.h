#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <stdbool.h>

#include "app.h"

bool save_manager_backup(save_source_t source, int port, char *status, int status_len);
bool save_manager_restore(save_source_t source, int port, char *status, int status_len);

#endif
