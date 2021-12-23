#include "mkdir_creat.h"

//   1. checking: parent, child
//   2. mymkdir
//      allocate ino, bno;
//      Write DIR INDOE on disk; disk block bno with . and ..
//      enter child name into parent data block
int make_dir(char *pathname)
{
    printf("ENTERING make_dir FUNCTION...\n");
    char *dir_name, *base_name, cpy[128];
    int pino, r, dev;
    MINODE *pmip, *mip;

    strcpy(cpy, pathname); // dirname/basename destroy pathname, must make copy
    // STEP 1: Determine if the path is absolute or relative
    if (abs_path(pathname) == 0)
    { // Path is absolute if true and relative if false
        printf("mkdir: Path is absolute.\n");
        dev = root->dev;
        mip = root;
    }
    else
    {
        printf("mkdir: Path is relative.\n");
        mip = running->cwd;
        dev = running->cwd->dev;
        printf("make_dir: dev: %d running->cwd->dev: %d\n", dev, running->cwd->dev);
    }

    // STEP 2: Divide path into dirname and basename
    dir_name = dirname(cpy);
    base_name = basename(pathname);
    printf("mkdir: dirname=%s\n", dir_name);
    printf("mkdir: basename=%s\n", base_name);

    pino = getino(dir_name);
    pmip = iget(dev, pino);

    if (S_ISDIR(mip->inode.i_mode))
    { // is_dir
        printf("make_dir: pmip is a dir\n");
        if (search(pmip, base_name) == 0)
        { // if can't find child name in start MINODE
            r = mymkdir(pmip, base_name);
            pmip->inode.i_links_count++;    // increment link count
            pmip->inode.i_atime = time(0L); // touch atime
            pmip->dirty = 1;                // make dirty
            iput(pmip);                     // write to disk

            printf("\nmake_dir: new directory = %s\n\n", pathname);
            printf("EXITING make_dir FUNCTION...\n");
            return r;
        }
        else
        {
            printf("\nmake_dir: Dir %s already exists.\n\n", base_name);
        }
    }

    iput(pmip);

    return 0;
}

int mymkdir(MINODE *pip, char *name)
{
    printf("ENTERING mymkdir FUNCTION...\n");
    char buf[BLKSIZE], *cp;
    MINODE *mip;
    INODE *ip;
    int i;
    // STEP 1: Allocate an inode and a disk block
    int ino = ialloc(dev);
    int blk = balloc(dev);
    printf("mymkdir: ino=%d bno=%d\n", ino, blk);
    // STEP 2: Load inode into a minode
    mip = iget(dev, ino);
    ip = &mip->inode;

    char temp[256];
    // findmyname(pip, pip->ino, temp);
    printf("mymkdir: ino=%d name=%s\n", pip->ino, temp);
    // Initizalize  mip->inode as a DIR inode
    ip->i_mode = 0x41ED;                                // set to dir type and set perms
    ip->i_uid = running->uid;                           // set owner uid
    ip->i_gid = running->cwd->inode.i_gid;              // set group id
    ip->i_size = BLKSIZE;                               // set byte size
    ip->i_links_count = 2;                              // . and ..
    ip->i_blocks = 2;                                   // each block = 512 bytes
    ip->i_block[0] = blk;                               // new dir has only one data block
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); // set to current time

    for (i = 1; i <= 14; i++) // Clear all i_blocks after 0
        ip->i_block[i] = 0;

    mip->dirty = 1; // make dirty
    iput(mip);      // write to disk
    printf("mymkdir: Wrote mip to disk\n");

    bzero(buf, BLKSIZE);

    dp = (DIR *)buf; // write . to buf
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';

    dp = (char *)dp + 12; // move pointer to end of last entry into buf

    dp->inode = pip->ino;
    dp->rec_len = BLKSIZE - 12;
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';

    put_block(dev, blk, buf); // write buf to disk at blk
    printf("mymkdir: Write data block %d to disk\n", blk);
    enter_name(pip, ino, name); // Enters (ino, basename) as a dir_entry to the parent inode
    printf("EXITING mymkdir FUNCTION...\n");
    return 0;
}

// Enter a new dir_entry into a parent directory
int enter_name(MINODE *pip, int myino, char *myname)
{
    printf("ENTERING enter_name FUNCTION...\n");
    char buf[BLKSIZE], *cp, temp[256];
    int n_len = strlen(myname);
    DIR *dp;
    int block_i, i, ideal_len, need_len, remain, blk;
    need_len = 4 * ((8 + (n_len) + 3) / 4);
    printf("enter_name: needed len for %s is %d\n", myname, need_len);

    for (i = 0; i < 12; i++)
    { // find empty block
        if (pip->inode.i_block[i] == 0)
            break;
        // Get parents data block into buf
        get_block(pip->dev, pip->inode.i_block[i], buf); // get that empty block
        printf("enter_name: get_block : i=%d\n", i);
        block_i = i;
        dp = (DIR *)buf;
        cp = buf;

        blk = pip->inode.i_block[i];

        printf("enter_name: Stepping through parent data block[i] = %d\n", blk);
        printf("enter_name: Directory contents: ");
        while (cp + dp->rec_len < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            printf("[%d %s] ", dp->rec_len, temp);

            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        printf("[%d %s]\n", dp->rec_len, dp->name);

        ideal_len = 4 * ((8 + dp->name_len + 3) / 4);

        printf("enter_name: ideal_len=%d\n", ideal_len);
        remain = dp->rec_len - ideal_len; // Remain represents the last entry's rec_len
        printf("enter_name: remain=%d\n", remain);

        if (remain >= need_len)
        {
            // trim last rec_len to ideal_len
            printf("name: %s\n", dp->name );
            dp->rec_len = ideal_len;
            // Enter the new entry as the LAST entry
            cp += dp->rec_len;
            dp = (DIR *)cp;
            dp->inode = myino;
            dp->rec_len = remain;
            dp->name_len = n_len;
            strcpy(dp->name, myname);
            printf("enter_name: put_block : i=%d\n", block_i);
            // Write data block to disk
            put_block(pip->dev, pip->inode.i_block[block_i], buf);
            printf("enter_name: write parent data block=%d to disk\n", blk);
            printf("EXITING enter_name FUNCTION...\n");
            return 0;
        }
    }
    get_block(pip->dev, pip->inode.i_block[i], buf);
    dp = (DIR*)buf;
    dp->inode = myino;
    dp->rec_len = BLKSIZE;
    strcpy(dp->name, myname);
    dp->name_len = n_len;
    printf("enter_name: write parent data block=%d to disk\n", blk);
    printf("EXITING enter_name FUNCTION...\n");
    put_block(pip->dev, pip->inode.i_block[i], buf);
    return 0;
}

int creat_file(char *pathname)
{
    printf("ENTERING creat_file FUNCTION...\n");
    char *dir_name, *file_name, cpy[128];
    int pino, r, dev;
    MINODE *pmip;

    strcpy(cpy, pathname); // dirname/basename destroy pathname, must make copy

    if (abs_path(pathname) == 0)
    {
        pmip = root;
        dev = root->dev;
    }
    else
    {
        pmip = running->cwd;
        dev = running->cwd->dev;
    }

    dir_name = dirname(cpy);
    file_name = basename(pathname);

    printf("creat_file: path=%s\n", dir_name);
    printf("creat_file: file=%s\n", file_name);

    pino = getino(dir_name);
    pmip = iget(dev, pino);

    if (!maccess(pmip, 'w'))
    {
        printf("creat_file: Couldn't create file");
        iput(pmip);
        printf("EXITING creat_file FUNCTION...\n");
        return 0;
    }
    if (S_ISDIR(pmip->inode.i_mode))
    { // is_dir
        if (search(pmip, file_name) == 0)
        { // if can't find child name in start MINODE
            r = mycreat(pmip, file_name);
            pmip->inode.i_atime = time(0L); // touch atime
            pmip->dirty = 1;                // make dirty
            iput(pmip);                     // write to disk

            printf("\ncreat_file: File Created %s\n", pathname);
            printf("EXITING creat_file FUNCTION...\n");
            return r;
        }
        else
        {
            printf("\ncreat_file: File %s already exists\n", file_name);
            iput(pmip); // Release parent nodee
        }
    }

    printf("EXITING creat_file FUNCTION...\n");
    return 0;
}

int mycreat(MINODE *pip, char *name)
{
    printf("ENTERING mycreat FUNCTION...\n");
    MINODE *mip;
    INODE *ip;
    int ino = ialloc(dev);

    mip = iget(dev, ino);
    ip = &mip->inode;
    ip->i_mode = 0x81A4;      // set to file type and set perms
    ip->i_uid = running->uid; // set owner uid
    ip->i_gid = running->gid; // set group id
    ip->i_size = 0;           // no data blocks
    ip->i_links_count = 1;    // . and ..
    //ip->i_blocks = 2; // each block = 512 bytes
    //ip->i_block[0] = bno; // new dir has only one data block
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L); // set to current time

    mip->dirty = 1;
    printf("mycreat: write INODE to disk\n");

    enter_name(pip, ino, name);

    iput(mip);
    printf("EXITING mycreat FUNCTION...\n");
    return 0;
}
