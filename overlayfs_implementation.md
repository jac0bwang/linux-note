# PicOS OverlayFS Implementation for Flash Based Switches

## 1. Introduction

There are some low end switches only provide internal flash as the storage space. Only UBIFS can be enabled for storing system rootfs and configuration. UBIFS is not reliable when power down happens and the corrupted files may be removed when system bootup again.

In this case, the UBIFS can be used to store the read-only images and not frequently changed configuration files or other application/user data.

OverlayFS is a memory based file system, which can cache any write operation without write the data onto the underlying physical storage. When the switch is powered down, the data in the cached OverlayFS will disappear unless it is coped to the write partition in advance.

Previous PicOS installation is based USB storage, i.e the system can write any data back to the storage without extra operation. The OverlayFS enabled PicOS has to change the whole workflow of system installation, local upgrade, second partition upgrade and bootup process. The main changes include:

    1. image format
    2. storage partitioning
    3. system installation
    4. system upgrade
    5. second image upgrade(upgrade2)
    6. PicOS system bootup process
    7. User Package/Application Installation

## 2. Image Format

On most of the platforms, PicOS is installed on the normal disk partition ,which can provide the full read and write capability. The OverlayFS image is a little bit different. It is a ramdisk image with kernel, rootfs, as well as OverlayFS function enabled. The image file on the switch flash is in SquashFS format, created with mksquashfs command.

## 3. Storage Partitioning

The extra writable partition is needed for saving the configuration files when the system is up and running. At least two partitions are needed, one for ramdisk image files, and the other two partitions for the configuration files of each running system.

The total space used, two squashfs images, 256MB each, added with one configuration partition, for example 256MB, is about 768MB.

## 4. System Installation

The fresh installation goes through ONIE as well. The image name should be the same without any change. The installation scripts will extract the squashFS file image and put it on the flash.

The uboot bootup string will be updated accordingly to boot the sqashfs image from flash.

## 5. System Upgrade

The local upgrade command is not changed, but just extract the new squashfs image file and put it on the flash to replace the current one.
The configuration files will stay on the configuration partition and not changed. It will be loaded again the new image bootup.

## 6. Second Image Upgrade

The second system upgrade will extract the squashfs file from the picos image and put to the flash with proper name.
The configuration partition will not be changed.

## 7. PicOS Bootup Process

The uboot will load the squashfs file/ramdisk from flash and it will start up the kernel and rootfs within the ramdisk.
The OverlayFS is enabled and the default scripts will be called, which will copy the configuration files from the configuration partition to the OverlayFS file system on the memory.

Start all other services.

If any of the configuration file is changed, it will be saved to the configuration partition by calling the corresponding save scripts.

## 8. User Package/Application Installation

End users can install any third party packages or their own applications on the running system.
The save list should be updated accordingly so as to save and recover the file properly.

## 9. Summary

The PicOS installation, upgrade internal processing logic is changed, but the user interfaces are not changed.
The only thing the end user should take care if they install extra applications, they should make sure they are put into the save/recovery list.

## 10.  Scripts for Installation/Upgrade/System Bootup/Configuration backup and recovery

### 10.1 Check the ubifs open partitions and backup the original data.

```bash
ubiattach -m 6 -d 0 /dev/ubi_ctrl
mkdir /mnt/open /orig
mount -t ubifs /dev/ubi0_0  /mnt/open/
cp /mnt/open/* /orig
umount /mnt/open
```

### 10.2 Make partitions for PICOS

There are four partitions for PICOS :

1. open partition is used for the original data
2. IMAGES partition is used for the PICOS squashfs files
3. CONFIG partition is used for the PICOS config files
4. DATA partition is used for the user data

Uncompress PICOS images to IMAGES partitions

```bash
ubiformat /dev/mtd6
ubiattach -m 6 -d 0 /dev/ubi_ctrl

ubimkvol /dev/ubi0 -N open -s 200000000   # 200M
ubimkvol /dev/ubi0 -N IMAGES -s 500000000 # 500M
ubimkvol /dev/ubi0 -N CONFIG -s 100000000 # 100M
ubimkvol /dev/ubi0 -N DATA  -s 100000000  # 200M
ubinfo -a
mkdir /mnt/open /mnt/images /mnt/conf /mnt/data
mount -t ubifs /dev/ubi0_0  /mnt/open/
mount -t ubifs /dev/ubi0_1  /mnt/images/
mount -t ubifs /dev/ubi0_2  /mnt/conf/
mount -t ubifs /dev/ubi0_3  /mnt/data/
```

### 10.3 Update U-boot environment

1. Modify the root=/dev/ram use the ramdisk for the rootfs.
2. Load the uImage.itb (kernel, dtb, and initrd files)from the UBIFS(IMAGES partition)

```bash
usb start;run platformargs;setenv bootargs root=/dev/ram rw console=$consoledev,$baudrate rootdelay=10 $mtdparts;
ubi part open; ubifsmount IMAGES; ubifsload $loadaddr uImage.itb
bootm $loadaddr
```

### 10.4 Boot into the init ramdisk

1. Make the tmpfs directory to story the PICOS squashfs file
2. Mount the file to the newroot directory
3. Then mount the overlayfs
4. Switch the real rootfs to the newroot

```bash
destdir=/newroot
mkdir $destdir
workdir=$(mktemp -t -d swiprep-XXXXXX)
ubiattach -m 6 -d 0 /dev/ubi_ctrl
mount -t ubifs /dev/ubi0_1  /mnt/images
cp mnt/images/pica.squash /tmp/.rootfs 
mount -t squashfs -o loop /tmp/.rootfs "${destdir}.lower"                  
mount -t tmpfs -o size=15%,mode=0755 none "${destdir}.upper"               
mkdir "${destdir}.upper/upper"                                             
mkdir "${destdir}.upper/work"                                              
mount -t overlay "-olowerdir=${destdir}.lower,upperdir=${destdir}.upper/upper,workdir=${destdir}.upper/work" overlay "$destdir"
exec switch_root -c /dev/console /newroot /sbin/init
```

### 10.5 Backup/Restore PICOS system config files

Because of the newrootfs is based on the ramdisk, so when reboot the machine and the modified config file will disappeared. In order to make it woker well that we need to backup when modified the config file, and restor it when the machine start.

1. Backup the config file to the CONFIG partitions
2. Restore the config file when machine start.

```bash
# CONFIG FILES
./etc/passwd
./etc/shadow
./etc/group
./etc/gshadow
./etc/resolv.conf
./etc/network/interfaces
```