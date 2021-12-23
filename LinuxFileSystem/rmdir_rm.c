#include "rmdir_rm.h"
#include "mkdir_creat.h"
#include "link_unlink.h"

// 1. Check path
// 2. Check if file is a DIR
// 3. Set mip with get(dev, ino)
// 4. Check if the DIR is busy
// 5. 
int rm_dir(char *path){
    printf("ENTERING rm_dir FUNCTION...\n");
    //Check dirname
    if(path)
    {
        int ino, pino;
        char name[256], buf[BLKSIZE], *cp;
        MINODE *mip, *pmip;
        char dir_name[256];
        strcpy(dir_name, dirname(path));
        
        // Path is absolute if true and relative if false
        if (abs_path(path)==0)
        { 
          dev = root->dev; 
          mip = root;
        } 
        else 
        {                   
          mip = running->cwd;
          dev = running->cwd->dev;
          printf("rm_dir: dev: %d running->cwd->dev: %d\n", dev, running->cwd->dev);
        }
        // 1. Get in-memory inode of pathname
        ino = getino(path);
        mip = iget(dev, ino);
        printf("mip refcount=%d\n", mip->refCount);

        findmyname(mip, ino, name);
        printf("rm_dir: dir name = %s pino = %d parent name = %s\n", path, mip->ino, name);
        // Verify inode is a DIR
        if(!S_ISDIR(mip->inode.i_mode)){
            printf("rm_dir: File is not dir type. Try using rm.\n");
            iput(mip);
            printf("EXITING rm_dir FUNCTION...\n");
            return -1;
        }
        // Check if DIR is busy
        if(mip->refCount > 1 || mip->mounted || mip == running->cwd){
          printf("rm_dir: refcount=%d mip->mounted=%d mip==running->cwd=%d\n", mip->refCount, mip->mounted, mip==running->cwd);
          printf("rm_dir: Error: %s is busy\n", path);
          iput(mip);
          return -1;
        }
        printf("rm_dir:  Not busy refcount=%d mip->mounted=%d mip==running->cwd=%d\n", mip->refCount, mip->mounted, mip==running->cwd);
        // If S_ISDIR() & refCount <= 1
        if ((mip->inode.i_mode & 0xF000) == 0x4000)
        {
          printf("rm_dir: mip is a directory.\n");
          printf("rm_dir: mip link count = %d.\n", mip->inode.i_links_count);

          if(mip->inode.i_links_count < 3){
            printf("rm_dir: Link count is less than 3\n");
            int actual_links_count = 0;

            get_block(dev, mip->inode.i_block[0], buf);
            dp = (DIR*)buf;
            cp = buf;

            while(cp < (buf + BLKSIZE)){
              if(!strcmp(dp->name, ".") || !strcmp(dp->name, "..")){
              actual_links_count++;
              cp += dp->rec_len;
              dp = (DIR*)cp;
              }
            }

            if(actual_links_count < 3){
              for(int i = 0; i < 12; i++){
                if(mip->inode.i_block[i] == 0){
                  continue;
                }
                else{
                  // Deallocates data block
                  bdalloc(mip->dev, mip->inode.i_block[i]);
                }
              }
              // deallocate its inode and data block (numbers).
              idalloc(mip->dev, mip->ino); 
              mip->dirty = 1;
              iput(mip);
              // Get parent's ino and inode
              pino = getino(dir_name);
              pmip = iget(mip->dev, pino);
              // Get the name from parent DIR's data block
              findmyname(pmip, ino, name);
              printf("rm_dir: pino = %d ino = %d name = %s\n", pino, ino, name);
              // If name is new:
              // 1. Remove the name from the parent directory
              // 2. Decrement the parent link_count
              // 3. Mark the parent mip as dirty and then write the pmip into the block
              // 4. Deallocate parent mip's data blocks and inode
              if(strcmp(name, ".") != 0 && strcmp(name, "..") != 0 && strcmp(name, "/") != 0){
                // Remove name from parent directory
                rm_child(pmip, name); 
                // Decrement parent links_count
                pmip->inode.i_links_count--; 
                pmip->inode.i_mtime = pmip->inode.i_atime = time(0L); // touch a/mtime
                // mark dirty
                pmip->dirty = 1; 
                // bdalloc(pmip->dev, mip->inode.i_block[0]);
                // idalloc(pmip->dev, mip->ino);
                iput(pmip);
                }
                else{
                  printf("rm_dir: mip has the link count %d which is greater than 3\n", actual_links_count);
                }
            }
            else{
              printf("rm_dir: mips children has the link count %d which is greater than 3\n", actual_links_count);
            }
          }
          else if(mip->refCount > 1){
            printf("rm_dir: mip=%d was busy.\n", mip->ino);
            iput(mip);
          }
          else{
            printf("rm_dir: File is not a directory. Try using rm.\n");
            iput(mip);
          }
        }
      }
      printf("EXITING rm_dir FUNCTION...\n");
      return 0;
}

// 1. Search through parent inode's data blocks for the filename to be deleted
// 2. Find the block index of the file to be deleted
// 3a. If the file is the first entry
//     - Using the block index, deallocate the i_block in the parent inode
//     - Move blocks up
// 3b. If the file is in the last entry
//     - Subtract the length of the last entry from the child pointer
//     - Set dp to the new cp
// 3c. If tje file is in the middle
//     - Move all the enteries right of the deleted file to the left
int rm_child(MINODE *pmip, char *name){
    printf("ENTERING rm_child FUNCTION...\n");
    char buf[BLKSIZE], *cp, *rm_cp, temp[256];
    DIR *last_dp;
    int flag = 0;
    // DIR *dp;
    int entered = 0;
    int block_i, i, j, size, last_len, rm_len;
    // Search parent inode's data blocks for the name entered
    for (i=0; i < 12; i++){
        if (pmip->inode.i_block[i]==0) break;
        get_block(pmip->dev, pmip->inode.i_block[i], buf); // Get next block
        printf("rm_child: get_block i=%d\n", i);
        printf("rm_child: name=%s\n", name);
        dp = (DIR*)buf;
        cp = buf;
        // Keep track of block index
        block_i = i;
        i=0;
        j=0;
        printf("rm_child: ");
        // Search through the data block to find which file needs to be deleted
        printf("enterd %d times\n", entered);
        while (cp < buf + BLKSIZE){
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            if (!strcmp(name, temp)){
                rm_cp = cp;
                rm_len = dp->rec_len;
                printf("rm=[%d %s] ", dp->rec_len, temp);
                flag = 1;
                break;
            }
            last_len = dp->rec_len;
            last_dp = dp;
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
        if (flag == 1) break;

        printf("[%d %s]\n", dp->rec_len, temp);
        printf("rm_child: block_i=%d\n", block_i);
    }
         if (dp->rec_len==BLKSIZE){ // first entry
            printf("rm_child: First entry!\n");

            printf("rm_child: deallocating data block=%d\n", block_i);
            bdalloc(pmip->dev, pmip->inode.i_block[block_i]); // dealloc this block

            for (i=block_i; i < pmip->inode.i_blocks; i++); // move other blocks up
         }
        else if (cp + dp->rec_len >= buf + BLKSIZE) { // last entry
            printf("Last entry!\n");
            last_dp->rec_len += dp->rec_len;
            dp->rec_len = 0;
            dp->inode = 0;
            bzero(dp->name, dp->name_len);
            dp->name_len = 0;            
            printf("rm_child: dp->rec_len=%d\n", dp->rec_len);
        } else { // middle entry
            printf("Middle entery!\n");
            size = buf+BLKSIZE - (last_dp->rec_len + cp); //Calculates how many bits should be shifted
            printf("rm_child: copying n=%d bytes\n", size);
            // Move all trailing entries to he left to overlay the deleted entry
            memmove(dp, cp, size);
            cp -= dp->rec_len; 
            dp = (DIR*)cp;
            // Add the previous rec_len to the last entry
            last_dp->rec_len += dp->rec_len;

            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
        }
        put_block(pmip->dev, pmip->inode.i_block[block_i], buf);
        printf("EXITING rm_child FUNCTION...\n");
        return 1;
}