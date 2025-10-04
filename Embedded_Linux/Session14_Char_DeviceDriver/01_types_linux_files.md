# Files in linux

### There are multiple types of files.
- Regular files (.txt , ...) [-]
- Directory. [d]
- Symbolic link. [s]
- **Block Device. [b]**
    - The block device is mainly used for storage devices.
    - RAM, SSD, HDD, FLASH, sdcard.
- Character Device. [c]
- **Network Device. [n]**
    - Network Interface cards (wifi, bluetooth(BLE), ...)
    - Network stack and file system has big stack and too many layers.


### Character Device
- It represents element in system that doesn't have a specific stack.
- gpio, i2c, spi, adc, ...etc
- The difference between procfs and char device is that 
    - procfs just gives some information about the running processes.
    - procfs doesn't represent a real device in linux stack.