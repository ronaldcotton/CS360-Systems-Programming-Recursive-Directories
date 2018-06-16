# WSU Vancouver - CS360 - File Systems
## Systems Programming - Assignment 3 - reading files recusively
## C/Network Program
### Abstract
This program does the following:
 - Takes one or zero command line arguments.
 - Argument, if present, is a pathname (relative or absolute). If no argument is present, the pathname of the present working directory is assumed (getcwd).
 - recursively traverses the directories and their files, starting with the pathname, and lists every regular file that is readable by the current process.
 - each regular file is listed to stdout as a full pathname, ne pathname per line.
 - No order is defined for the pathnames that are output.

Other:
 - Ignore symbolic links (Why?) - Symbolic link loop - Symoblic link to parent directory

C Functions Used:
 - chmod - change directory
 - access - check user's permissions for a file
 - opendir - open a directory
 - readdir - read a directory
 - closedir - close a directory
 - lstat - get file status


### Requirements
*GNU C and GNU make*

### Executing
```sh
    make
    ./readable <optional pathname>
```
*to clean the project when done:*
```sh
    make clean
```

### Note
Better solutions exist, but constrained because of the restrictions of the assignment.  Modern methods of file traversing can be found online, but **_BSD_SOURCE** traversial is not recommended for production code.
