#include <libgen.h>
#include <time.h>

#include <sys/stat.h>

#include "util.h"

MINODE minode[NMINODE], *root;
PROC proc[NPROC], *running;
MTABLE mtable[NMTABLE];