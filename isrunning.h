/*
 * This is the header for isrunning, you need to inculde this header and then
 * compile with the isrunning.o file.
 * 
 * isrunning.c contains two functions:
 *     int isrunning(char*)
 * and
 *     int isrunningcmdline(char*)
 * they both do approximately the same thing except that the second looks at the
 * cmdline (which includes arguments) rather than the status file (which does not).
 * 
 * They return the pid (if found), 0 if not, and a negative number otherwise.
 * 
 * Look in isrunning.c and README.md for more information.
 */
int isrunning(char* );
int isrunningcmdline(char* );
