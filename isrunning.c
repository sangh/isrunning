#include <string.h> 
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

// The location of the proc filesystem.
#define PROCDIR "/proc"
// The file used to find the name of each process (e.g. /proc/PID/status).
#define STATUSFILE "status"
/* The character the name starts on in STATUSFILE, as in how many chars out.
 * On my system the first line in the file is something like:
 * Name:   init
 * where "init" is the name of the process and there is a tab after the colon,
 * so the name would start on character 6. */
#define N_START_CHAR 6
/* The file in PROCDIR the command line is stored in. This file should only
 * have the command line, no leading chars, and all white space converted to
 * the following WHITESPACE character. */
#define CMDFILE "cmdline"
// The character white space is converted to.
#define WHITESPACE '\0'

/* Takes in a char* pointing to a string contained within the name of
 * process.  That is, in *just* the name, not in the whole command line,
 * if you want that use isrunningcmdline() for that (and see the wrapper).
 * For instance, if the command line is:
 *     MyProg arg1 arg2 arg3
 * and then isrunning("arg") is called it would return 0, or not
 * running. Where as isrunning("rog") would return the pid of MyProg.
 * Returns a negative int for error, 0 for not found and the pid if found.
 * Requires that PROCDIR exist and is readable. the file looked
 * for is /PROCDIR/{pid}/STATUSFILE, the first line of witch should contain:
 *     Name:  {name-of-program}
 *
 * Return values:
 * num  The pid of the found process.
 *  0   No process containing prog was found.
 * -2   The call to opendir(PROCDIR) failed (No PROCDIR directory?).
 * -3   The call to open( PROCDIR / dent->d_name / STATUSFILE ) failed.
 * -4   The read from STATUSFILE to N_START_CHAR failed.
 */
int isrunning(char* prog) {

    DIR * dir;
    struct dirent * dent;
    /* The struct dirent has a member called d_type.
     * This is supposed to be set to the mode of the named entry; *but* this
     * is not consistent across platforms or version or anything else.
     * For instance on my system the variable
     * _DIRENT_HAVE_D_TYPE is defined, but every entry is set to zero, making
     * the use of it worse than useless.  So, we don't use any entry of
     * dirent other than dirent->d_name.
     */
    int i, fd;
    struct stat stbuf;
    /* This version only implements the standard stat function.
     * Stat64 should not be necessary, as the maximum number of processes is
     * the maximum value if the int type.  It should not however be too hard
     * change it to use stat64 instead.
     */
    char c;
    char statusfile[
        sizeof(PROCDIR)
        + sizeof(STATUSFILE)
        + 10 // 2^32 takes 10 decimal digits.
        + 3  // 2 for the null and the two slashes.
        ];


    if ((dir = opendir(PROCDIR)) == NULL) {
        // Try closing it anyway.
        closedir(dir);
        return -2;
    } // if

    while((dent = readdir(dir)) != NULL) {

        strcpy(statusfile, PROCDIR);
        strcat(statusfile, "/");
        strcat(statusfile, dent->d_name);


        // stat stuff
        if((stat(statusfile, &stbuf) == -1) ||
            ((stbuf.st_mode & S_IFMT) != S_IFDIR))
            continue;

        i = sizeof(PROCDIR);
        while ((c = statusfile[i++]) != '\0')
            if(!isdigit(c))
                break;
        if(c != '\0')
            continue;


        strcat(statusfile, "/");
        strcat(statusfile, STATUSFILE); // Statusfile is now complete.

        if((fd = open(statusfile,O_RDONLY)) == -1) {
            // Close dir and file, or try.
            close(fd);
            closedir(dir);
            return -3;
        } // if


        for(i=0; i<N_START_CHAR; i++ )
            if(read(fd, &c, 1) != 1) {
                // Close stuff.
                close(fd);
                closedir(dir);
                return -4;
            } // if

        i = 0;
        while(read(fd,&c,1) == 1 &&
            c != '\n' &&
            prog[i] != '\0' &&
            c != EOF) {
            if(c == prog[i])
                i++;
            else
                i = 0;
        }
        // We need to close this file nomatter what.
        close(fd);
        if(prog[i] == '\0') {
            closedir(dir);
            return atoi(dent->d_name);
        } // if

    } // while

    close(fd);
    closedir(dir);
    // no process found
    return 0;
} // isrunning


/* This is the int isrunningcmdline(char*) function.
 * It parses the /proc/PID/cmdline file, which should only contain the cmdline.
 * Most(all) Linux distros combine all null space in the command line and
 * replace it with a white space delimiter.  On my system this is the null char.
 * This is essentially the same as isrunning except it checks the cmdline (which
 * includes the arguments). For instancne if:
 *    MyProg arg1 arg2
 * was running, isrunningcmdline("arg") would return the pid of MyProg, (unless
 * another process was found first). This is useful if you have many of the same
 * program running with different arguments and want to find a particular one.
 *
 * The conversion algorithm in isrunningcmdline to convert the given char* to
 * one with only "null" spaces is the int isspace(char) function from ctype.h
 *
 * Otherwise this function behaves the same as isrunning() above with the same
 * return values (except -4).
 */
int isrunningcmdline(char* cmd) {

    DIR * dir;
    struct dirent * dent;
    // See note in isrunning().
    int i, fd;
    struct stat stbuf;
    // See note in isrunning().
    char c, cmdfile[sizeof(PROCDIR)
        + sizeof(CMDFILE)
        + 10 //2^32 takes 10 decimal digits
        + 3]; //2 for the null and the two slashes


    if ((dir = opendir(PROCDIR)) == NULL) {
        // Try closing it anyway.
        closedir(dir);
        return -2;
    } // if

    while((dent = readdir(dir)) != NULL) {

        strcpy(cmdfile, PROCDIR);
        strcat(cmdfile, "/");
        strcat(cmdfile, dent->d_name);


        // stat stuff
        if((stat(cmdfile, &stbuf) == -1) ||
            ((stbuf.st_mode & S_IFMT) != S_IFDIR))
            continue;

        i = sizeof(PROCDIR);
        while ((c = cmdfile[i++]) != '\0')
            if(!isdigit(c))
                break;
        if(c != '\0')
            continue;


        strcat(cmdfile, "/");
        strcat(cmdfile, CMDFILE); // Cmdfile is now complete.

        if((fd = open(cmdfile,O_RDONLY)) == -1) {
            // Close dir and file, or try.
            close(fd);
            closedir(dir);
            return -3;
        } // if

        i = 0;
        while(read(fd,&c,1) == 1 &&
            c != '\n' &&
            cmd[i] != '\0' &&
            c != EOF) {
            if(c == cmd[i] || (c == WHITESPACE && isspace(cmd[i])))
                i++;
            else
                i = 0;
        }
        // We need to close this file no matter what.
        close(fd);
        // Because this is the cmd line we need to check for ourselves.
        if(cmd[i] == '\0' && atoi(dent->d_name) != getpid()) {
            closedir(dir);
            return atoi(dent->d_name);
        } // if

    } // while

    close(fd);
    closedir(dir);
    // No process found.
    return 0;
} // isrunningcmdline
