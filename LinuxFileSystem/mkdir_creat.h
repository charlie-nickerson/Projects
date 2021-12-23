#include <libgen.h>
#include <time.h>

#include <sys/stat.h>

#include "util.h"

//global vars from main
MINODE minode[NMINODE], *root;
PROC proc[NPROC], *running;
MTABLE mtable[NMTABLE];

int dev, imap, bmap, ninodes, nblocks;


// mountroot.c
// int init();
// int mount_root();
// int quit();

// cd_ls_pwd.c
// int cd(char *pathname);
// int ls_file(MINODE *mip, char *name);
// int ls_dir(MINODE *mip);
// int ls(char *pathname);
// char *rpwd(MINODE *wd);
// char *pwd(MINODE *wd);

//mkdir_creat.c
int make_dir(char *pathname);
int mymkdir(MINODE *pip, char *name);
int enter_name(MINODE *pip, int myino, char *myname);
int creat_file(char *pathname);
int mycreat(MINODE *pip, char *name);
int touch(char* name);

// // rmdir.c
// int rm_dir(char *pathname);
// int rm_name(MINODE *pmip, char *name);

// // symlink.c
// int sym_link(char *src, char *dest);

// // link.c
// int link_file(char *pathname, char *linkname);

// // unlink.c
// int unlink_file(char *filename);