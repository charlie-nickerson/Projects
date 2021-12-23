#ifndef __360PROJ_UTIL__
#define __360PROJ_UTIL__

#include "type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <fcntl.h>
#include "mkdir_creat.h"
#include "rmdir_rm.h"

int get_block(int dev, int blk, char *buf);
int put_block(int dev, int blk, char *buf);
int tokenize(char *pathname);
MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *name);
int getino(char *pathname);
int findmyname(MINODE *parent, u32 myino, char *myname);
int show(MINODE *mip);
int findino(MINODE *mip, u32 *myino);
// our added util functions
int abs_path(char *path);
int tst_bit(char *buf, int bit);
int set_bit(char *buf, int bit);
int clr_bit(char *buf, int bit);
int dec_free_inodes(int dev);
int dec_free_blocks(int dev);
int inc_free_inodes(int dev);
int inc_free_blocks(int dev);
int ialloc(int dev);
int idalloc(int dev, int ino);
int balloc(int dev);
int bdalloc(int dev, int blk);
int fs_init();
int my_strlen(char *p);

int pfd();

int faccess(char *pathname, char mode);
int maccess(MINODE *mip, char mode);
int startatroot;
#endif
