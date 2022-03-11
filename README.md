**File system** from the ground up </br>

### Stage 1. Disk Emulator
Emulate real call to real hard drive:</br>
* write_block
* read_block
  
### Stage 2. Cache buffer
Simple array of blocks with LRU policy
### Stage 3. Inodes
There are two types of them:
* **Disk inode** - on disk data structure
* **Memory inodes** - same inode in memory with additional information
### Stage 4. Directory layer
Gives possibility to name files and build hierarchy

