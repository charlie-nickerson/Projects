#include "open_close.h"

// OPEN FILE FUNCTION:
// 
//   1. ask for a pathname and mode to open:
//          You may use mode = 0|1|2|3 for R|W|RW|APPEND
//   2. get pathname's inumber, minode pointer:
//          ino = getino(pathname); 
//          mip = iget(dev, ino);  
//   3. check mip->INODE.i_mode to verify it's a REGULAR file and permission OK.   
//      Check whether the file is ALREADY opened with INCOMPATIBLE mode:
//            If it's already opened for W, RW, APPEND : reject.
//            (that is, only multiple R are OK)
//   4. allocate a FREE OpenFileTable (OFT) and fill in values:
//          oftp->mode = mode;      // mode = 0|1|2|3 for R|W|RW|APPEND 
//          oftp->refCount = 1;
//          oftp->minodePtr = mip;  // point at the file's minode[]
//   5. Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
//       switch(mode){
//          case 0 : oftp->offset = 0;     // R: offset = 0
//                   break;
//          case 1 : truncate(mip);        // W: truncate file to 0 size
//                   oftp->offset = 0;
//                   break;
//          case 2 : oftp->offset = 0;     // RW: do NOT truncate file
//                   break;
//          case 3 : oftp->offset =  mip->INODE.i_size;  // APPEND mode
//                   break;
//          default: printf("invalid mode\n");
//                   return(-1);
//       }
//    7. find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
//       Let running->fd[i] point at the OFT entry
//    8. update INODE's time field
//          for R: touch atime. 
//          for W|RW|APPEND mode : touch atime and mtime
//       mark Minode[ ] dirty
//    9. return i as the file descriptor

int open_file(char *pathname, char *mode){
    OFT *oftp;
    MINODE *mip;
    int i, ino, i_mode = atoi(mode);

    printf("open_file: i_mode=%d\n", i_mode);

    if (pathname[0] == '/')
        dev = root->dev;
    else
        dev = running->cwd->dev;

    // Set the ino
    ino = getino(pathname);
    // Set the mip
    mip = iget(dev, ino); 

    // 
    if(i_mode == 0)
    {
        if (!maccess(mip, 'r'))
        {  
            iput(mip); 
            printf("EXITING open_file FUNCTION...\n");
            return -1;
        }
    }
    else
    {
        if (!maccess(mip, 'w'))
        { // char = 'r' for R; 'w' for W, RW, APPEND
            iput(mip);
            printf("EXITING open_file FUNCTION...\n"); 
            return -1;
        }
    }
    

    if (!S_ISREG(mip->inode.i_mode)) // check if regular file
    {
        printf("open_file: Error, file is not REG.\n");
        printf("EXITING open_file FUNCTION...\n");
        return -1;
    }
    // Check if the file is open with an incompatible mode
    if (i_mode > 0)
    { 
        for (i=0; i<NFD; i++)
        {
            if (running->fd[i] != NULL && running->fd[i]->minodePtr == mip)
            {
                if (running->fd[i]->mode > 0)
                {
                    printf("open_file: File %s already open for write!\n\n", pathname);
                    printf("EXITING open_file FUNCTION...\n");
                    return -1;
                }
            }   else break;
        }
    }
    
    oftp = (OFT*)malloc(sizeof(OFT)); // build the open fd
    oftp->mode = i_mode;
    oftp->refCount = 1;
    oftp->minodePtr = mip;

    switch (i_mode){ // offset
        case 0: oftp->offset = 0;                 break; // MODE == R
        case 1: my_trunc(mip); oftp->offset = 0;  break; // MODE == W
        case 2: oftp->offset = 0;                 break; // MODE == R/W
        case 3: oftp->offset = mip->inode.i_size; break; // MODE == APPEND
        default:
            printf("open_file: Mode invalid\n");
            printf("EXITING open_file FUNCTION...\n");
            return -1;
    }

    for (i=0; i<NFD; i++)
    {   // find empty fd in running PROC's fd array
        if (running->fd[i] == NULL)
        {
            running->fd[i] = oftp;
            break;
        }
    }
    
    mip->inode.i_atime = time(0L); // update inode access time

     // Modify file time
    if (i_mode > 0) mip->inode.i_mtime = time(0L);

    mip->dirty = 1;
    printf("EXITING open_file FUNCTION...\n");
    return i; // return fd i
}

// TRUNCATE FUNCTION
//  1. release mip->INODE's data blocks;
//      a file may have 12 direct blocks, 256 indirect blocks and 256*256
//      double indirect data blocks. release them all.
//  2. update INODE's time field
//  3. set INODE's size to 0 and mark Minode[ ] dirty
int my_trunc(MINODE *mip)
{
    INODE *ip = &mip->inode;      
    // iterate through blocks
    for (int i=0; i<ip->i_blocks; i++)
    {   
        if (ip->i_block[i] != 0) bzero(ip->i_block, BLKSIZE); // Set i_block to zero
        else break; // Exit loop when i_block = zero
    }
    // update modified time
    ip->i_mtime = time(0L); 
    ip->i_size = 0;
    mip->dirty = 1;
    printf("EXITING my_trunc FUNCTION...\n");
    return 0;
}

// CLOSE FILE FUNCTION
//   1. verify fd is within range.
//   2. verify running->fd[fd] is pointing at a OFT entry
//   3. The following code segments should be fairly obvious:
//      oftp = running->fd[fd];
//      running->fd[fd] = 0;
//      oftp->refCount--;
//      if (oftp->refCount > 0) return 0;
//      // last user of this OFT entry ==> dispose of the Minode[]
//      mip = oftp->inodeptr;
//      iput(mip);
//      return 0; 
// }
int close_file(int fd)
{
    printf("ENTERING close_file FUNCTION...\n");
    // See if the file descriptor exists
    printf("%d\n", fd);
    if(fd < 0 || fd >=NFD)
    {
        printf("close_file: Error file is out of range\n");
        printf("EXITING close_file FUNCTION...\n");
        return -1;
    }
    if(running->fd[fd] == NULL)
    {
        printf("close_file: Error file does not exist\n");
        printf("EXITING close_file FUNCTION...\n");
        return -1;
    }

    OFT *oftp;
    oftp = running->fd[fd];
    running->fd[fd] = 0;
    oftp->refCount--;
    if(oftp->refCount > 0)
    {
        printf("EXITING close_file FUNCTION...\n");
        return 0;
    } 
    MINODE *mip = oftp->minodePtr;
    iput(mip);
    printf("EXITING close_file FUNCTION...\n");
    return 0;
}                                                                                                    


// l_seek FUNCTION:
//  1. From fd, find the OFT entry. 
//  2. change OFT entry's offset to position but make sure NOT to over run either end
//   of the file.
//  3. return originalPosition
//

int l_seek(int fd, int pos)
{
    printf("ENTERING l_seek FUNCTION...\n");
    if(fd < 0 || running->fd[fd] == NULL)
    {
        printf("l_seek: Error file not found\n");
        printf("EXITING l_seek FUNCTION...\n");
        return -1;
    }
    if(pos > 0)
    {
        if(pos >running->fd[fd]->minodePtr->inode.i_size)
        {
            printf("l_seek: Error position is larger than the size of fd\n");
            printf("EXITING l_seek FUNCTION...\n");
            return -1;
        }
        int offset = running->fd[fd]->offset;
        running->fd[fd]->offset = pos;
        printf("EXITING l_seek FUNCTION...\n");
        return offset;
    }
    printf("l_seek: Error postion less than 0\n");
    printf("EXITING l_seek FUNCTION...\n");
    return 0;
}

int pfd()
{
    printf("ENTERING pfd FUNCTION...\n");
    int i;
    printf("\npfd:  fd   mode   offset   inode\n");
    printf("pfd: ---- ------ -------- -------\n");
    for (i=0; i<NFD; i++){
        if (running->fd[i] != 0)
        {
            OFT *current = running->fd[i];
            char mode[8];

            switch(current->mode)
            {
                case 0: strcpy(mode, "READ"); break;
                case 1: strcpy(mode, "WRITE"); break;
                case 2: strcpy(mode, "R/W"); break;
                case 3: strcpy(mode, "APPEND"); break;
            }
            printf("pfd:   %d %6s  %4d     [%d, %d]\n", i, mode, current->offset, current->minodePtr->dev, current->minodePtr->ino);
        } else  break; // no open fd's
    }
    putchar('\n');
    printf("EXITING pfd FUNCTION...\n");
    return 0;
}

// Open File Data Structures:
//     running
//       |                                                  
//       |                                                    ||****************** 
//     PROC[ ]              OFT[64]            MINODE[128]    ||      Disk dev
//   ===========    |---> ===========    |--> ============    || ===============
//   |nextPtr  |    |     |mode     |    |    |  INODE   |    || |      INODE   
//   |pid, ppid|    |     |refCount |    |    | -------  |    || =============== 
//   |uid, gid |    |     |minodePtr|---->    | dev,ino  |    || 
//   |cwd      |    |     |offset   |         | refCount |    ||******************
//   |         |    |     ====|======         | dirty    |
//   |fd[10]   |    |         |               | mounted  |         
//   | ------  |    |         |               ============
// 0 |   ----->|--->|         |
//   | ------  |              |   
// 1 |         |              |
//   | ------  |             --------------------------------
// 2 |         |             |0123456.............
//   | ------  |             --------------------------------    
//   ===========        logical view of file: a sequence of bytes