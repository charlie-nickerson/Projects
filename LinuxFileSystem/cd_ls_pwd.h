#include <libgen.h>
#include <time.h>

#include <sys/stat.h>

#include "util.h"

MINODE minode[NMINODE], *root;
PROC proc[NPROC], *running;
MTABLE mtable[NMTABLE];

int dev, imap, bmap, ninodes, nblocks;
char* rpwd(MINODE* wd);
char* pwd(MINODE* wd);
int cd(char* path);
int ls(char* path);
