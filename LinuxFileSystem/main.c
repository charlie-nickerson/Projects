

#include "util.h"
#include "mkdir_creat.h"
#include "cd_ls_pwd.h"
#include "rmdir_rm.h"
#include "link_unlink.h"
#include "open_close.h"

// #include "cd_ls_pwd.c"
// #include "mkdir_creat.c"
// #include "rmdir.c"
// #include "symlink.c"
// #include "link.c"
// #include "unlink.c"

// global variables
MINODE minode[NMINODE];
MINODE *root;

PROC proc[NPROC], *running;
OFT oft[NOFT];
MTABLE mtable[NMTABLE];

char default_disk[6]="disk1";
char gline[128]; // global for tokenized components
char *name[64];  // assume at most 32 components in pathname
int   nname;         // number of component strings

int fd;
int nblocks, ninodes, bmap, imap, inode_start; // disk parameters

int quit();



// load root INODE and set root pointer to it
int mount_root()
{  
    printf("mount_root()\n");
    // iget will return the minode in memory containing INODE of (dev, ino)
    root = iget(dev, 2);

    return 0;
}

char *disk;
int main(int argc, char *argv[ ])
{
    //int ino;
    char buf[BLKSIZE];
    char line[128], cmd[32], src[128], dest[128];
    //Only if no argument for disk
    if (argc>1)
        disk=argv[1];
    else{
        printf("[main]: no disk specified, using default=%s\n", default_disk);
        disk=default_disk;
    }
 
    printf("checking EXT2 FS ....");
    if ((fd = open(disk, O_RDWR)) < 0){
        printf("open %s failed\n", disk);
        exit(1);
    }
    dev = fd;    // fd is the global dev 

    /********** read super block  ****************/
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;

    /* verify it's an ext2 file system ***********/
    if (sp->s_magic != 0xEF53){
        printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
        exit(1);
    }     
    printf("EXT2 FS OK\n");
    ninodes = sp->s_inodes_count;
    nblocks = sp->s_blocks_count;

    printf("NUMBER OF inodes=%d\n", ninodes);
    printf("NUMBER OF blocks=%d\n", nblocks);

    /********** read group descriptor ***********/
    get_block(dev, 2, buf); 
    gp = (GD *)buf;

    /*initialize bmap, imap and inode_start using the group descriptor*/
    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    inode_start = gp->bg_inode_table;
    printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

    fs_init();  
    mount_root();
    printf("root refCount = %d\n", root->refCount);

    printf("creating P0 as running process\n");
    running = &proc[0];
    proc[0].uid = 0;
    running->status = READY;
    running->cwd = iget(dev, 2);
    printf("root refCount = %d\n", root->refCount);

    // WRTIE code here to create P1 as a USER process
    proc[1].uid = 1;
    proc[1].status = FREE;
    while(1){
        printf("running p%d: [ls|cd|pwd|mkdir|rmdir|creat|symlink|link|unlink] \n", running->uid);
        printf("\033[0;31m");
        printf("COMMAND : ");
        printf("\033[0m");
        fgets(line, 128, stdin);
        line[strlen(line)-1] = 0;

        *src=*dest=0; // reset src and dest strings

        if (line[0]==0)
        continue;

        sscanf(line, "%s %s %s", cmd, src, dest);
        printf("[main]: cmd=%s src=%s dest=%s\n", cmd, src, dest);

        src[strlen(src)] = 0;
        dest[strlen(dest)] = 0;

        if(strcmp(cmd, "pwd")==0){
            pwd(running->cwd);
        }
        else if(strcmp(cmd, "cd")==0){
            cd(src);
        }
        else if(strcmp(cmd, "mkdir")==0){
            make_dir(src);
        }
        else if(strcmp(cmd, "ls")==0)
            ls(src);
        else if (strcmp(cmd, "creat")==0 || strcmp(cmd, "touch") == 0)
            creat_file(src);
        else if (strcmp(cmd, "rmdir")==0)
            rm_dir(src);
        else if (strcmp(cmd, "link")==0)
            link_files(src, dest);
        else if (strcmp(cmd, "unlink")==0)
            unlink_file(src);
        else if (strcmp(cmd, "symlink")==0)
            sym_link(src, dest);
        else if (strcmp(cmd, "open")==0){
            open_file(src, dest);
        }
        else if (strcmp(cmd, "pfd")==0){
            pfd();
        }
        else if (strcmp(cmd, "close")==0){
            close_file(src);
        }
        else if (strcmp(cmd, "quit")==0 || strcmp(cmd, "q")==0)
            quit();
     
        // else if (strcmp(cmd, "sw")==0)
        // {
        //     if(running->uid == 0)
        //     {
        //         printf("[sw]: switch from p1 to p0\n");
        //         running = &proc[1];
        //         running->status = READY;
        //         running->cwd = iget(dev, 2);
        //         proc[0].status = FREE;
        //     }
        //     else
        //     {
        //         printf("[sw]: switch from p0 to p1\n");
        //         running = &proc[0];
        //         running->status = READY;
        //         running->cwd = iget(dev, 2);
        //         proc[1].status = FREE;
        //     }
             

        //  }
    }
}

int quit()
{
    int i;
    MINODE *mip;
    for (i=0; i<NMINODE; i++){
        mip = &minode[i];
        if (mip->refCount > 0)
        iput(mip);
    }
    exit(0);
}
