/*************** type.h file ************************/
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>
#include <stdio.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
#define BLKSIZE     1024

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define FREE        0
#define READY       1
#define BUSY        1

#define LINK 			0
#define FILE 			1
#define DIRECTORY		2

#define NMINODE      100
#define NMTABLE       10
#define NPROC          2
#define NFD           10
#define NOFT          40


//Objects
//MINODE, OFT, PROC, MTABLE (page 321)
typedef struct minode{
    //from ext2 inode
    INODE inode;
    int dev, ino;
    int refCount;
    int dirty;
    int mounted;
    struct MTABLE *mntptr;
}MINODE;

/* our inode structure
struct ext2_inode{
    u16 i_mode;
    u16 i_uid;
    u32 i_size;
    u32 i_atime;
    u32 i_ctime;
    u32 i_mtime;
    u32 i_dtime;
    u16 i_gid;
    u16 i_links_count;254
    u32 i_blocks;
    u32 i_flags;
    u32 i_reserved1;
    u32 i_block[15];
    u32 pad[7];
};
*/

typedef struct oft{
    int  mode;
    int  refCount;
    MINODE *minodePtr;
    int  offset;
}OFT;

typedef struct proc{
    struct Proc *next;
    int          pid;
    int          status;
    int          uid, gid;
    struct minode *cwd;
    OFT         *fd[NFD];
}PROC;

typedef struct mtable{
    int dev;
    int ninodes;
    int nblocks;
    int free_blocks;
    int free_inodes;
    int bmap;
    int imap;
    int iblock;
    MINODE *mntDirPtr;
    char devName[64];
    char mntName[64];
}MTABLE;

PROC proc[NPROC], *running;
OFT oft[NOFT];
MTABLE mtable[NMTABLE];

int dev;
/*------------------------------------------------
Use of getino()/iget()/iput(): In a file system, almost every operation begins with a pathname,
e.g. mkdir pathname, cat pathname, etc. Whenever a pathname is specified, its inode must be loaded
into memory for reference. The general pattern of using an inode is
. ino = getino(pathname);
. mip = iget(dev, ino);
.
use mip->INODE, which may modify the INODE;
. iput(mip);

When we recieve pathname we get ino with getino(pathname), then get mip with iget(dev, ino), then if
we want to modify the inode we can access mip->inode with iput(mip). 
--------------------------------------------------*/


