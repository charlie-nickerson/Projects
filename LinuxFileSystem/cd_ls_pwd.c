#include "util.h"
#include "cd_ls_pwd.h"


// 1. Check path
// 2a. if the path is not specified just print the contents of the cwd
// 2b. If the path is specified read to mip with the path name and the inode
// 3. Check if the file is a REG or a DIR
// 3a. If a REG, call printfFile
// 3b. If a DIR call printChild
int ls(char * path){
    printf("ENTERING ls FUNCTION...\n");
    unsigned long ino;
	MINODE *mip, *pip;
	int device;
	char *child;

	// if there was no additional argument, print the cwd
	if(path[0] == 0)
	{
        device = running->cwd->dev;
		mip = iget(device, running->cwd->ino);
		printChild(device, mip);
        iput(mip);
	}
	// otherwise...
	else
	{
		if(path[0] == '/')
			device = root->dev;

		ino = getino(path);
       
		if(!ino){  // DIR name does not exist
            printf("ls: DIR does not exist.\n");
            return 1;
        }

		mip = iget(device, ino);

		// if we're not looking at a directory...
		if(((mip->inode.i_mode) & 0040000)!= 0040000) //print file
		{
			// find the parent of the path part and get its's basename
			// if it doesn't exist, get the name of the child path
            char* child = (char *)malloc((strlen(path) + 1) * sizeof(char));
            strcpy(child, path);
			// print the child path
			printFile(mip, child);
            printf("EXITING ls FUNCTION...\n");
            iput(mip);
			return 0;
		}

		// print child part
		printChild(device, mip);
        iput(mip);
	}
    printf("MIP REF COUNT = %d\n", mip->refCount);
    printf("EXITING ls FUNCTION...\n");
	return 0;
}

// 1.
int cd(char *path){
    printf("ENTERING cd FUNCTION...\n");
    MINODE *mip;
    unsigned int ino;

    if (!path){ // Check for empty path
        printf("EXITING cd FUNCTION...\n");
        return 0;
    }
    if (strcmp(path, "/") == 0) // Check for root
        ino = root->ino;
    else // Search for inode
        ino = getino(path);

    //load inode
    mip = iget(dev, ino); 
    //check if dir
    if(!S_ISDIR(mip->inode.i_mode))
    {
        printf("cd: Error: path name does not match a directory type\n");
        iput(dev);
        printf("EXITING cd FUNCTION...\n");
        return 0;
    }
    iput(running->cwd); // release old cwd
    running->cwd = mip;   //new minode to mip
    printf("EXITING cd FUNCTION...\n");
    return 1;
}

char *rpwd(MINODE *wd)
{
    MINODE * pip, *nmip;
    char mname[256];
    unsigned int ino, pino;
    MTABLE *mntptr;

    if(wd==root)
        return 0;

    pino = findino(wd, &ino);
    pip  = iget(dev, pino);

    findmyname(pip, ino, mname);
    rpwd(pip);

    printf("/%s", mname);
    return 0;

}

char *pwd(MINODE *wd){
    printf("ENTERING pwd FUNCTION...\n");
    int current_dev = dev;
    if(wd == root){
        printf("pwd: /\n");
        printf("EXITING pwd FUNCTION...\n");
        return 0;
    }
    printf("pwd: ");
    rpwd(wd);
    printf("\n");

    dev = current_dev;
    printf("EXITING pwd FUNCTION...\n");
}

void printChild(int dev_name, MINODE *mp)
{
    printf("ENTERING printChild FUNCTION...\n");
	char buf[BLKSIZE], nbuf[256], *cp;
	DIR *dptr;
	int i, ino;
	MINODE *temp;

	// cycle through direct blocks and print them
	for(i = 0; i < 12; i++) 
	{
		if(mp->inode.i_block[i])
		{
			get_block(dev_name, mp->inode.i_block[i], buf);
			cp = buf;
			dptr = (DIR *)buf;
            printf("In i_block: %d\n", i);
			while(cp < (buf + BLKSIZE))
			{
				strncpy(nbuf, dptr->name, dptr->name_len);
				nbuf[dptr->name_len] = 0;

				ino = dptr->inode;
				temp = iget(dev_name, ino); 
				printFile(temp, nbuf);		
				cp+=dptr->rec_len;
				dptr = (DIR *)cp;
                iput(temp);
			}
		}
	}
    printf("EXITING printChild FUNCTION...\n");
    return 1;
}

void printFile(MINODE* mip, char* name){
    INODE* pip = &mip->inode;
    unsigned int mode = pip->i_mode;
    time_t val = pip->i_ctime;
    char *mtime = ctime(&val);
    char buf[BLKSIZE], link_name[256];
    mtime[strlen(mtime) - 1] = '\0';
    unsigned int type = mode & 0xF000;
    switch(type)
    {
        case 0x4000: // IS DIRECTORY
            printf("d");
            break;
        case 0x8000:
            printf("-"); // IS REGULAR FILE
            break;
        case 0xA000: // IS LINK
            printf("l");
            get_block(mip->dev, mip->inode.i_block[0], buf);
            strcpy(link_name, buf);
            put_block(mip->dev, mip->inode.i_block[0], buf);
            link_name[strlen(link_name)] = 0;
            break;
        default:
            printf("-");
            break;
    }
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
    printf("%4d%4d%4d  %s%8d    %s", pip->i_links_count, pip->i_gid, pip->i_uid, mtime, pip->i_size, name);
    // if this is a symlink file, show the file it points to
    if((mode & 0120000) == 0120000)
    printf(" => %s\n", link_name);
    else
    printf("\n");
}

int findparent(char *path)
{
    printf("ENTERING findparent FUNCTION...\n");
	int i = 0;
	while(i < strlen(path))
	{
		if(path[i] == '/')
			return 1;
		i++;
	}
    printf("EXITING findparent FUNCTION...\n");
	return 0;
}