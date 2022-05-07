### **File system** from the ground up </br>

### Stage 1. Disk Emulator
Emulate real call to real hard drive:</br>
* write_block
* read_block
### Stage 2. Cache buffers
Simple array of blocks with LRU policy
### Stage 3. Inodes<br>
Inode represents either file, folder or device.</br>
Under the hood, inode is just organized structure of data blocks.</br>
Basically, there are two types of inodes:
* **Disk inode** - on disk data structure
* **Memory inodes** - same data structure with additional fields, all processes use that type of inodes
### Stage 4. Directory layer
Being able to create files and folders to build hierarchical file system