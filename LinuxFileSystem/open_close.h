#include <libgen.h>
#include <time.h>

#include <sys/stat.h>

#include "util.h"

MINODE minode[NMINODE], *root;
PROC proc[NPROC], *running;
MTABLE mtable[NMTABLE];

int open_file(char *pathname, char *mode);
int my_trunc(MINODE *mip);
int close_file(int fd);
int l_seek(int fd, int pos);
int pfd();