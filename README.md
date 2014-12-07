Don't Use This
==============

When I wrote this there was only slightly more of a reason than now, but now
there are loads of ways to search the process list that don't require
installing a stupid program, so use one of those instead of this.


Is Running?
===========

This is a little program I wrote a long time ago to find a process that was
running on a Unix machine by name that would include the command line
arguments in the search.  At the time this system had no way to do that
except by going through the proc directory (and even now this is nicer
to use in some cases than the built in tools).  In addition I've needed to
include this functionality in C programs before and would like to not
make a system call to an external program so this has been useful since.


Including in a C Program
========================

If you want to include it in a C program just do:

    make isrunning.o

and then include `isrunning.h` in the source and compile with `isrunning.o`.

You can then call `int isrunning( char* )` and `int isrunningcmdline( char* )`
directly.


Running the Stand Alone Executable
==================================

Make it with:

    make isrunning

and then copy the executable `isrunning` to somewhere in your `$PATH`.

Running it with a first argument of `-c` you will run `isrunningcmdline`
instead of `isrunning`.

Then the next argument is searched for and if found that is the return value.
It is also printed of stdout.  Then the next one, if given, is printed (but
not returned, obviously), then the next, etc.  So usage is:

    isrunning [-c] "proc_name1 [arg1 [arg2] [...]] "proc_name2 [arg1 [arg2] [...]]"

A return value of `0` means that `proc_name1` was not found, and a negative
return value means an error occurred.  On some systems the return value will
never be above `255`, so the return value may not be trustworthy.


isrunning
=========

This function takes in a string which is searched for within the name of
a process.  That is, in *just* the name, not in the whole command line,
which means arguments are not included and if the process changed it's name
(by writing to the memory space that `argv` was copied from) it will *not*
use that.  Specifically it will use the first line of the
file `/proc/{pid}/status`.

For instance, if the command line is:

     myprog arg1 arg2 arg3

and then `isrunning("arg")` is called it would return `0` for not running or
no process found.  Whereas `isrunning("rog")` would return the pid of the first
process called `myprog` found.

Returns a negative value to indicate an error, see `isrunning.c` for details.


isrunningcmdline
================

This function takes in a string which is searched for within the entire
command line of the process.  This includes the name and the arguments,
but if the process changed it's name (by writing to the memory space that
`argv` was copied from) it will search that instead of what was used to spawn
it.  Specifically it will use the file `/proc/{pid}/cmdline`.

Most(all) Linux distros combine all null space in the command line and
replace it with a white space delimiter.  On my system this is the null char.

For instance if:

     myprog arg1 arg2

was running, `isrunningcmdline("arg")` would return the pid of `myprog`, (unless
another process was found first, and it hadn't changed it's name).
This is useful if you have many of the same program running with different
arguments and want to find a particular one.
 
Returns a negative value to indicate an error, see `isrunning.c` for details.
