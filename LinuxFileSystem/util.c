/*********** util.c file ****************/
#include "util.h"

extern PROC *running;
extern MINODE *root, minode[NMINODE];
extern MTABLE mtable[NMTABLE];
extern char gline[128], *name[64];
extern int nname, inode_start, dev, imap, bmap, ninodes, nblocks;
int r;

//Given from book, reads from the lseek on the buf. put_block will write to it. 
int fs_init()
{
    // Initialize variabels
    int i, j;
    MINODE *mip;
    PROC   *p;
    MTABLE *minodePtr; 

    printf("init: init()\n");

    for (i=0; i<NMINODE; i++){ // initialize all minodes as FREE
        mip = &minode[i];
        mip->dev = FREE;
        mip->ino = FREE;
        mip->refCount = FREE;
        mip->mounted  = FREE;
        mip->mntptr   = FREE;
    }
    for (i=0; i<NPROC; i++){ // Initialize PROCs as FREE
        p = &proc[i];
        p->pid = i;
        p->uid = FREE;
        p->gid = FREE;
        p->cwd = FREE;
        p->status = FREE;
        for (j=0; j<NFD; j++) 
            p->fd[j] = FREE;
    }
    for (i=0; i<NMTABLE; i++){ // Initialize mtable to FREE 
        minodePtr = &mtable[i];
        minodePtr->dev = FREE;
        minodePtr->mntDirPtr = FREE;
    }

    return 0;
}

// Reads a block from the virtual disk
int get_block(int dev, int blk, char *buf){
    lseek(dev, (long)blk*BLKSIZE, 0);
    r = read(dev, buf, BLKSIZE);
    if (r < 0) { printf("get_block: [%d %d] error\n", dev, blk); }

    return r;
}   
// Writes a block to the virtual disk
int put_block(int dev, int blk, char *buf){
    lseek(dev, (long)blk*BLKSIZE, 0);
    r = write(dev, buf, BLKSIZE);
    if(r != BLKSIZE) { printf("put_block: [%d %d] error\n", dev, blk); }

    return r;
}   

//FROM THE BOOK
int tokenize(char *pathname){
    printf("tokenize: TOKENIZING %s...", pathname);
    int i;
    char *s;
    if(pathname[0] == '/'){
        startatroot = 1;
    }
    strcpy(gline, pathname);   // tokens are in global gline[ ]
    nname = 0;

    s = strtok(gline, "/");
    while(s){
        name[nname++] = s;
        s = strtok(0, "/");
    }

    for (i= 0; i<nname; i++)
        printf("%s  ", name[i]);
    printf("\n");

    return name;
}

MINODE *mialloc(){
    int i;
    for (i=0; i<NMINODE; i++){
        MINODE *mp = &minode[i];
        if (mp->refCount == 0){
            mp->refCount = 1;
            return mp;
        }
    }
    printf("mialloc: FS panic: out of minodes\n");
    return 0;
}

int midalloc(MINODE *mip) // Releases a used minode
{
    mip->refCount = 0;
}

// Function returns a pointer to the in-memory minode
// containing the INODE of (dev, ino)This is a unique
// minode meaning there is only one copy of the INODE
// memory.
MINODE *iget(int dev, int ino){
    int i;
    MINODE *mip;
    char buf[BLKSIZE];
    int blk, offset;
    INODE *ip;

    for (i=0; i<NMINODE; i++){
        mip = &minode[i];
        if (mip->refCount && mip->dev == dev && mip->ino == ino){
            //printf("\nmip=%s refCount++ now=%d\n", mip->ino, mip->refCount);
            mip->refCount++;
            //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
            return mip;
        }
    }

    mip = mialloc(); // Allocate a FREE minode
    mip->dev = dev; mip->ino = ino; // Assign to (dev, ino)
    blk = (ino-1)/8 + inode_start; // Disk block containg this inode
    offset = (ino-1)%8; // Which inode in this block
    get_block(dev, blk, buf);
    ip = (INODE *)buf + offset;
    mip->inode = *ip;
    // initialize minode
    mip->refCount = 1;
    mip->mounted  = FREE;
    mip->dirty    = FREE;
    mip->mntptr   = FREE;
    return mip;

    printf("iget: PANIC: no more free minodes\n");
    return 0;
}

    
void iput(MINODE *mip){
    int block, offset;
    char buf[BLKSIZE];
    INODE *ip;
    if (mip==0) return;

    //printf("\nmip=%d refCount-- now=%d\n", mip->ino, mip->refCount);
    mip->refCount--;
    if (mip->refCount > 0) return; // minode is still in use
    if (mip->dirty == 0) return; // No need to write back

    printf("iput: inode_start=%d\n", inode_start);
    // Write the inode back to the disk
    block = (mip->ino - 1) / 8 + inode_start;
    offset = (mip->ino -1) % 8;

    printf("iput: block=%d, offset=%d\n", block, offset);

    get_block(mip->dev, block, buf);
    ip = (INODE *)buf + offset; // ip points at INODE
    *ip = mip->inode; // Copy mip->inode to inode in block
    put_block(mip->dev, block, buf); // Write back to disk
    midalloc(mip);
} 

// Taken from the book
int search(MINODE *mip, char *name){
    int i;
    char *cp, temp[256], sbuf[BLKSIZE];
    DIR *dp;
    for(i=0; i<12;i++){
        if(mip->inode.i_block[i] == 0) // Search DIR direct blocks
            return 0;
        get_block(mip->dev, mip->inode.i_block[i], sbuf); // Read the directory pointer into memory
        dp = (DIR *)sbuf;
        cp = sbuf;
        printf("search: Searching Directories...\n");
        printf("search: inode rlen nlen name\n");
        while (cp < sbuf + BLKSIZE){
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            printf("%3d%6d%5u    %s\n", dp->inode, dp->rec_len, dp->name_len, temp); // Print the directory
            if (strcmp(name, temp)==0){
                printf("\033[0;32m");
                printf("\nsearch: Found %s with inode #%d\n", name, dp->inode);
                printf("\033[0m");
                printf("EXITING search FUNCTION...\n");
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        printf("EXITING search FUNCTION...\n");
        return 0;
    }
}

//From book but had to find whether it was directory or not
//we used same implementation as in the given ls_file code by
//checking that (inode.i_mode & 0xF000) == 0x4000
int getino(char *pathname)
{
    MINODE *mip;
    int i, ino;

    if (strcmp(pathname, "/")==0){
    return 2;
    // return root
    }
    if (pathname[0] == "/"){
        ip = &(root->inode);
        mip = root;
    }
    // if absolute
    else{
        ip = &(running->cwd->inode);
        mip = running->cwd;
    }
    // if relative
    mip->refCount++;
    // in order to
    tokenize(pathname);
    iput(mip); 
    // assume: name[ ], nname are globals
    for (i=0; i<nname; i++){
        // search for each component string
        //S_ISDIR(mip->inode.i_mode in textbook, we need to prove it is not a Dir Type
        if (!(mip->inode.i_mode & 0xF000) == 0x4000){ // check DIR type
            printf("getino: %s is not a directory\n", name[i]);
            iput(mip);
            return 0;
        }
        ino = search(mip, name[i]);
        if (!ino){
            printf("getino: no such component name %s\n", name[i]);
            iput(mip);
            return 0;
        }
        iput(mip);
        // release current minode
        mip = iget(dev, ino);
        ip = &(mip->inode);
    // switch to new minode
    }
    iput(mip);
    return ino;
}
    
int findmyname(MINODE *parent, u32 myino, char* myname){
    int i;
    char *cp, c, sbuf[BLKSIZE];
    DIR *dp;
    INODE *ip;

    ip = &(parent->inode);
    //search for file name

    for (int i = 0; i < 12; i++)
    {//search direct block only
        if (ip->i_block[i] == 0)
            return 0;

        get_block(dev, ip->i_block[i], sbuf);
        dp = (DIR*)sbuf;
        cp = sbuf;

        while (cp < sbuf + BLKSIZE)
        {
            c = dp->name[dp->name_len];
            dp->name[dp->name_len] = 0;

            if (myino == dp->inode)
            {
                strcpy(myname, dp->name);
                //printf("myname found, %s\n", myname);
                return 1;
            }
            dp->name[dp->name_len] = c;
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
    return 0;
}

//find ino of the MINODE structure from entry in "INODE.i_block[0]"
int findino(MINODE *mip, u32 *myino){ // myino = ino of . return ino of ..
    char buf[BLKSIZE], *cp;   
    DIR *dp;

    //We take the mip pass in it's ->dev and "INODE.i_block[0]" get_block
    get_block(mip->dev, mip->inode.i_block[0], buf);
    cp = buf; 
    dp = (DIR *)buf;
    *myino = dp->inode;
    cp += dp->rec_len;
    dp = (DIR *)cp;
    return dp->inode;
}

// LEVEL 1
int abs_path(char *path){
    if (path[0] == '/')
        return 0;
    else
        return -1;
}

int tst_bit(char *buf, int bit){
    int i = bit / 8;
    int j = bit % 8;

    if (buf[i] & (1 << j))
        return 1;

    return 0;
}

int set_bit(char *buf, int bit){
    int i = bit / 8;
    int j = bit % 8;

    buf[i] |= (1 << j);

    return 0;
}

int clr_bit(char *buf, int bit){
    int i = bit / 8, j = bit % 8;

    buf[i] &= ~(1 << j);

    return 0;
}

int dec_free_inodes(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // dec the super table
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);

    return 0;
}

int dec_free_blocks(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // dec the super table
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);

    return 0;
}

int inc_free_inodes(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); // get the super table
    sp = (SUPER*)buf;
    sp->s_free_inodes_count++; // inc free inodes
    put_block(dev, 1, buf); // put it back

    get_block(dev, 2, buf); // get the gd table
    gp = (GD*)buf;
    gp->bg_free_inodes_count++; // inc free inodes
    put_block(dev, 2, buf); // put it back

    return 0;
}

int inc_free_blocks(int dev){
    char buf[BLKSIZE];

    get_block(dev, 1, buf); 
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++; // dec the super table
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf; // dec the GD table
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);

    return 0;
}

int ialloc(int dev){
    int i;
    char buf[BLKSIZE];

    get_block(dev, imap, buf);

    for (i=0; i < ninodes; i++){
        if (tst_bit(buf, i) == 0){
            set_bit(buf, i);
            put_block(dev, imap, buf);
            dec_free_inodes(dev);
            printf("ialloc: allocated ino = %d\n", i+1); // bits 0..n, ino 1..n+1
            return i+1;
        }
    }
    return 0;
}

int idalloc(int dev, int ino){
    char buf[BLKSIZE];

    if (ino > ninodes){
        printf("idalloc: inumber=%d out of range.\n", ino);
        return 0;
    }

    get_block(dev, imap, buf);
    clr_bit(buf, ino-1);
    put_block(dev, imap, buf);

    inc_free_inodes(dev);
    printf("idalloc: deallocated ino=%d\n", ino);

    return 0;
}

int balloc(int dev){
    int i;
    char buf[BLKSIZE];

    get_block(dev, bmap, buf);

    for (i=0; i < nblocks; i++){
        if (tst_bit(buf, i) == 0){
            set_bit(buf, i);
            put_block(dev, bmap, buf);
            printf("balloc: allocated block = %d\n", i+1); // bits 0..n, ino 1..n+1
            dec_free_blocks(dev);
            return i+1;
        }
    }
    return 0;
}

int bdalloc(int dev, int blk){
    char buf[BLKSIZE];

    if (blk > nblocks){
        printf("bdalloc: block=%d out of range.\n", blk);
        return 0;
    }

    get_block(dev, bmap, buf);
    clr_bit(buf, blk-1);
    put_block(dev, bmap, buf);

    inc_free_blocks(dev);
    printf("bdalloc: deallocated block=%d\n", blk-1);

    return 0;
}

int faccess(char *pathname, char mode)
{
    char t1[9] = "xwrxwrxwr", t2[9] = "---------";
    char permi[9];
    int offset = 0;
    int ino = getino(pathname);
    MINODE *mip = iget(dev,ino);
    INODE *ip = &mip->inode;

    for (int i=8; i >= 0; i--) // permissions
        if (ip->i_mode & (1 << i))
            permi[i]=t1[i];
        else
            permi[i]='-';
    
    //printf("\npermission %s\n", permi);
    if(mode == 'w')
        offset = 1;
    if(mode == 'x')
        offset = 2;

    // Super User
    if(running->uid ==0)
        return 1;

    // Owner
    if(ip->i_uid == running->uid)
        if(mode == permi[offset])
            return 1;
    // Same group
    else if(ip->i_gid == running->gid)
        if(mode == permi[offset + 3])
            return 1;
    // Other
    else
        if(mode == permi[offset + 6])
            return 1;

    return 0;
}

int maccess(MINODE *mip, char mode)
{
    char t1[9] = "xwrxwrxwr", t2[9] = "---------";
    char permi[9];
    int offset = 0;
    INODE *ip = &mip->inode;

    for (int i=8; i >= 0; i--) // permissions
        if (ip->i_mode & (1 << i))
            permi[i]=t1[i];
        else
            permi[i]='-';
    
    //printf("\npermission %s\n", permi);
    if(mode == 'w') offset = 1;
    if(mode == 'x') offset = 2;

     // Super User
    if(running->uid == 0)
        return 1;    

    // Owner
    if(ip->i_uid == running->uid)
        if(mode == permi[offset])
            return 1;
    // Group
    else if(ip->i_gid == running->gid)
        if(mode == permi[offset + 3])
            return 1;
    // Other
    else
        if(mode == permi[offset + 6])
            return 1;

    return 0;
}

int my_strlen(char *p)
{
    int count = 0;

    while(*p!='\0')
    {
        count++;
        p++;
    }

    return count;
}

int check_empty(MINODE *mip)
{
    char *cp, buf[BLKSIZE], temp[256];
    DIR *dp;
    for(int i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0)
            return 0;
        
        get_block(dev, ip->i_block[i], buf);
        dp = (DIR*)buf;
        cp = buf;

        while(cp < buf[BLKSIZE])
        {
            memset(temp, 0, 256);
            strncpy(temp, dp->name, dp->name_len);
            
            if(strncmp(".", temp, 1) != 0 && strncmp("..", temp, 2) != 0)
                return 1;

            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
}

