# FS

## Data region

Most of the space in any file system is (and should be) user data. Let’s call the region of the disk we use for user data the data region

## Inode

As we learned about (a little) last chapter, the file system has to track information about each file. This information is a key piece of metadata, and tracks things like which data blocks (in the data region) comprise a file, the size of the file, its owner and access rights, access and modify times, and other similar kinds of information. To store this information, file systems usually have a structure called an inode (we’ll read more about inodes below).

To accommodate inodes, we’ll need to reserve some space on the disk for them as well. Let’s call this portion of the disk the inode table, which simply holds an array of on-disk inodes. 

## Bitmap

One primary component that is still needed, as you might have guessed, is some way to track whether inodes or data blocks are free or allocated. Such allocation structures are thus a requisite element in any file system.

Many allocation-tracking methods are possible, of course. For example, we could use a free list that points to the first free block, which then points to the next free block, and so forth. We instead choose a simple and popular structure known as a bitmap, one for the data region (the data bitmap), and one for the inode table (the inode bitmap). A bitmap is a simple structure: each bit is used to indicate whether the corresponding object/block is free (0) or in-use (1). And thus our new on-disk layout, with an inode bitmap (i) and a data bitmap (d)

## SuperBlock

The superblock contains information about this particular file system, including, for example, how many inodes and data blocks are in the file system (80 and 56, respectively in this instance), where the inode table begins (block 3), and so forth. It will likely also include a magic number of some kind to identify the file system type (in
this case, vsfs).

when mounting a file system, the operating system will read the superblock first, to initialize various parameters, and then attach the volume to the file-system tree. When files within the volume are accessed, the system will thus know exactly where to look for the needed on-disk structures.

## File System Measurement Summary

* Most files are small -- Roughly 2K is the most common size
* Average file size is growing -- Almost 200K is the average
* Most bytes are stored in large files -- A few big files use most of the space
* File systems contains lots of files -- Almost 100K on average
* File systems are roughly half full -- Even as disks grow, file systems remain ˜50% full
* Directories are typically small -- Many have few entries; most have 20 or fewer

## Directory Organization

In vsfs (as in many file systems), directories have a simple organization; a directory basically just contains a list of (entry name, inode number) pairs. For each file or directory in a given directory, there is a string and a number in the data block(s) of the directory. For each string, there may also be a length (assuming variable-sized names).

For example, assume a directory dir (inode number 5) has three files in it (foo, bar, and foobar is a pretty longname), with inode numbers 12, 13, and 24 respectively. The on-disk data for dir might look like:

inum | reclen | strlen | name
5 12 2 .
2 12 3 ..
12 12 4 foo
13 12 4 bar
24 36 28 foobar_is_a_pretty_longname

## FREE SPACE MANAGEMENT

There are many ways to manage free space; bitmaps are just one way. Some early file systems used free lists, Modern file systems use more sophisticated data structures. For example, SGI’s XFS [S+96] uses some form of a B-tree to compactly represent which chunks of the disk are free. As with any data structure, different timespace trade-offs are possible.
