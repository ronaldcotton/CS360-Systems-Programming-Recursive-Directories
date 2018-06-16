/*	Ronald Cotton
	CS360 - Systems Programming - Instructor: Dick Lang, Ph.D

	README:
		compiled and tested with -g -Wall -pedantic -ansi -std=c99

Write program readable:
	Takes one or zero command line arguments.

	Argument, if present, is a pathname (relative or absolute).
	If no argument is present, the pathname of the present
	working directory is assumed (getcwd).
	
	Readable recursively traverses the directories and their
	files, starting with the pathname, and lists every regular
	file that is readable by the current process.
	
	Each regular file is listed to stdout as a full pathname,
	one pathname per line.
	
	No order is defined for the pathnames that are output.
	Other stuff:
	
		- Ignore symbolic links (Why?) - Symbolic link loop or
										 Symoblic link to parent directory
		- Submit one source file via email: readable.c
		- Design your solution in advance of coding.
		- You will need:
			*chmod - change directory
			*access - check user's permissions for a file
			*opendir - open a directory
			*readdir - read a directory
			*closedir - close a directory
			*lstat - get file status
*/

/* libraries - NOTE C99 required */
#define _BSD_SOURCE
#include <stdio.h>				/* for fprintf() */
#include <unistd.h>				/* for pathconf(), access(), lstat(), chdir() */
#include <string.h>				/* for strcmp() */
#include <stdlib.h>				/* for calloc(), exit(), realpath() */
#include <fcntl.h>				/* AT_* constraints for access() */
#include <errno.h>				/* for error resolution */
#include <sys/types.h>			/* for opendir(), closedir(), lstat() */
#include <sys/stat.h>			/* for lstat() */
#include <dirent.h>				/* for opendir(), readdir(), closedir() */
#include <stdint.h>				/* for SIZE_MAX - largest size for size_t */
#include <stdbool.h>			/* for boolean types */
#include <limits.h>				/* for realpath() */

/* defines */
#define NUMOFARGS 2
#define FUNCTIONERR -1
#define PROGEXIT 1
#define PMAXFILESIZE 4096

/* function prototypes */
/* print the usage for the program */
inline void printUsage(char *cl);
/* print an error if the path is not valid */
inline void errorPathNotValid(char *path);
/* print an error if the first directory is a Symbolic Link */
inline void errorStartingWithSymbolicLink(char *path);
/* check if it's a Directory */
bool isDirectory(char *path);
/* check if it is a segular file vs. directory*/
bool isRegularFile(char *fn);
/* check if it is a symbolic file/direcory */
bool isSymbolicLink(char *sl);
/* check if the permissions allow for the file/directory can be read */
bool isReadable(char *r);
/* handles the errno returns for handling files/directories */
void errorMsg(char *str, int e);
/* recursive program that travels the directory structure */
void traverse(char *pathname);

/* main()
 * 
 * Main handles the arguments - the number of the arguments taken in,
 * return usage if there's more than 2 arguments. if this has the number
 * of argments is one, use the initial directory.
 * 
 * After all initial checks, then run the recursive program.
 */
int main(int argc, char* argv[]) {
	if (argc > NUMOFARGS) printUsage(argv[0]);
	
	char abspath[PMAXFILESIZE];
	
	if (argc == NUMOFARGS) {
		if (realpath((const char *)argv[1],abspath) == NULL) errorMsg("ERROR: realpath() -",errno);
		if (chdir(abspath) == FUNCTIONERR) errorMsg("ERROR: chdir() -", errno);
	} else {
		if (realpath(".",abspath) == NULL) errorMsg("ERROR: realpath() -",errno);
		/* do not need to run chdir(), we are in the correct direcory */
	}
	
	if ((!isDirectory(abspath))&&(!isRegularFile(abspath)))
		errorPathNotValid(abspath); /* if not a directory and is not a regular file */
	if (isSymbolicLink(abspath))
		errorStartingWithSymbolicLink(abspath); /* if the first directory/file is a symbolic link */

	if (getcwd(abspath, PMAXFILESIZE) == NULL) {
		errorMsg("ERROR: getcwd() -",errno);
		exit(1);
	}

	/* recursive function */
	traverse(abspath);

return 0;
} /* end main() */

/* printUsage(char *)
 * prints the usage of the program if more than two argments are given
 */
inline void printUsage(char *cl) {
	fprintf(stderr, "USAGE: %s [optional:<path>]\n",cl);
	exit(PROGEXIT);
} /* end printUsage() */

/* errorPathNotValid()
 * if the pathname is not valid, print this error
 */
inline void errorPathNotValid(char* path) {
	fprintf(stderr, "ERROR - pathname %s not valid\n", path);
	exit(PROGEXIT);
} /* end errorPathNotValid() */

/* errorStartingWithSymbolLink(char *)
 * if the initial directory is a symbolic link, return this error
 */
inline void errorStartingWithSymbolicLink(char *path) {
	fprintf(stderr, "ERROR - starting with a symbolic link.\n");
} /* end errorStartingWithSymbolicLink() */

/* checkValidDirectory(char *)
 * check to see if that is a valid direcory.
 */
bool isDirectory(char *path) {
	struct stat s;
	return ((lstat (path, &s) == 0) && S_ISDIR (s.st_mode));
} /* end checkValidDirectory() */

/* isRegularFile(char *)
 * if the file is a regular file
 */
bool isRegularFile(char *fn) {
	struct stat s;
	return ((lstat (fn, &s) == 0) && S_ISREG(s.st_mode));
} /* end isFile() */

/* isSymbolicLink()
 * if the file is a symbolic link
 */
bool isSymbolicLink(char *sl) {
	struct stat s;
	return ((lstat (sl, &s) == 0) && S_ISLNK(s.st_mode)); 
} /* end isSymbolicLink() */

/* errorMsg(char *, int)
 * This is the return values for all the error number codes.
 * I understand there's a library that does it but the error codes are
 * not readable.
 */
void errorMsg(char *str, int e) {
	const char *str2;
	
	switch(e) {
		case EACCES:
			str2 = "EACCES - Permission Denied.\n";
			break;
		case EBADF:
			str2 = "EBADF - Invalid File Discriptor.\n";
			break;
		case EFAULT:
			str2 = "EFAULT - Buffer Pointing to Invalid Address.\n";
			break;
		case EINVAL:
			str2 = "EINVAL - Buffer is null.\n";
			break;
		case EIO:
			str2 = "EIO - I/O Error.\n";
			break;
		case EMFILE:
			str2 = "EMFILE - Exceeded process limit on open file descriptors.\n";
			break;
		case ENAMETOOLONG:
			str2 = "ENAMETOOLONG - pathname exceeds PATH_MAX bytes.\n";
			break;
		case ENFILE:
			str2 = "ENFILE - Exceeded process limit on open file descriptors.\n";
			break;
		case ENOENT:
			str2 = "ENOENT - directory doesn't exists or name is empty.\n";
			break;
		case ENOMEM:
			str2 = "ENOMEM - Out of memory to complete operation.\n";
			break;
		case ELOOP:
			str2 = "ELOOP - Loop exists in Symbolic Links.\n";
			break;
		case ENOTDIR:
			str2 = "ENOTDIR - Not a directory.\n";
			break;
		case ERANGE:
			str2 = "ERANGE - buffer too small for absolute pathname.\n";
			break;
		default:
			str2 = "OTHER - The error is unknown.\n";
	} /* end switch() */
	fprintf(stderr, "%s %s", str, str2);
	exit(PROGEXIT);
} /* end errorMsg() */

/* traverse(char *)
 * This is the recursive function that will change the pathname to the
 * current pathname, then we open up the directory and get information
 * from that by entering the information into dirent.
 * 
 * We then enter the loop for the readdir, and get the current working 
 * pathname.  If it is directory, and it is not a symbolic link, and is
 * not . or .. - then do not traverse.  If the file is readable and the
 * file is considered a regular file, then print...
 */
void traverse(char *pathname) {
	if (chdir(pathname)) errorMsg("ERROR: chdir() -", errno);
	DIR* dir = opendir(pathname);
	
	/* if the directory is not openable, keep running the recursive
	 * function and close this... */
	if (!dir) return;
	struct dirent *d;
	
	while ((d = readdir(dir)) != NULL) {
		if (FUNCTIONERR == access(d->d_name,R_OK)) continue;
		if (!dir) errorMsg("ERROR: readdir() -", errno);
		if (getcwd(pathname, PMAXFILESIZE)==NULL) errorMsg("ERROR: getcwd() -", errno);
		if ((isDirectory(d->d_name))&&(!isSymbolicLink(d->d_name))&&((strcmp(d->d_name,"."))!=0)&&((strcmp(d->d_name,".."))!=0))
			traverse(strcat(strcat(pathname,"/"),d->d_name));
		else if (isRegularFile(d->d_name))
			printf("%s/%s\n",pathname,d->d_name);
	}
	if (chdir("..") == FUNCTIONERR) errorMsg("ERROR: chdir() -", errno);
	closedir(dir);
} /* end traverse() */
