#include "symlink.h"

// 1. Check if the old_file exist and if the new file doesnt exist
// 2. creat new file and change to link type
// 3. Stor old_file name in newfiles inode.i_block
// 4. Sete file size to length of old file name
// 5. mark the new file as dirty
// 6. Write the new file minode in to an inode in memory
// 7. Mark parent minode as dirty
// 8. write the new_file parent minode to inode in the block
int sym_link(char* old_file, char* new_file){
    printf("ENTERING sym_link FUNCTION...\n");
    char* name = basename(old_file);
    char buf[BLKSIZE];
    name[strlen(name)] = 0;
    // Check if the old file exists
    int old_ino = getino(old_file);
    if(old_ino <= 0){
        printf("sym_link: File %s does not exist.\n", old_file);
        printf("EXITING sym_link FUNCTION...\n");
        return 0;
    }
    // Check if the new file does not exist
    int new_ino = getino(new_file);
    if(new_ino > 0){
        printf("sym_link: File %s already exists.\n", new_file);
        printf("EXITING sym_link FUNCTION...\n");
        return 0;
    }
    creat_file(new_file);
    new_ino = getino(new_file);
    MINODE* mip = iget(dev, new_ino);
    // Set inode to symbolic link
    mip->inode.i_mode = 0120000;
    get_block(mip->dev, mip->inode.i_block[0], buf);
    strcpy(buf, name);
    put_block(mip->dev, mip->inode.i_block[0], buf);
    mip->inode.i_size = strlen(name);
    mip->dirty = 1;
    iput(mip);
    printf("EXITING sym_link FUNCTION...\n");
    return 1;
}