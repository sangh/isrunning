/*
 * This file is the wrapper for isrunning.[hc]; it produces the stand alone
 * executable (instead of isrunning being compiled into something else).
 * All it does it parse command line arguments and return the return value of
 * the isrunning function called (there are two) in isrunning.c.
 *
 * For information on what isrunning is, and how to use it look in README.md.
 */
#include <stdlib.h>
#include <stdio.h>
#include "isrunning.h"

int main(int argc, char**argv) {
	int ret;
	int i, op;
	int (*isrun)(char*);

	if(argc < 2) {
		fprintf(stderr,
            "Usage:\n"
			"isrunning [-c] \"proc_name1 [arg1 [arg2] [...]]\" "
			"\"proc_name2 [arg1 [arg2] [...]]\" ...\n\n"
			"Returns and prints to stdout the pid of the first "
			"process found matching\n"
			"proc_name1, or 0 if no process was found or a "
			"negative number if there\n"
			"was an error; see the readme for more information.\n"
			"In the case of multiple arguments only the value from "
			"proc_name1 is returned,\n"
			"they're all, however, printed to stdout separated by "
			"spaces.\n"
			"The option \"-c\" causes isrunningcmdline to be used "
			"instead of isrunning (which will search the arguments as well).\n"
			"On some systems the return value will never be above"
			"255, so this value may not be trustworthy.\n");
		exit(-11);
	} // if

	// Ideally you should be able to put the -c in any argument.
	isrun = isrunning;
	for(i=1; i<argc; i++)
		if(argv[i][0] == '-')
			if(argv[i][1] != 'c' || argv[i][2] != '\0') {
				fprintf(stderr,
				"isrunning: Unknown argument: %s\n", argv[i]);
				exit(-12);
			} else {
				isrun = isrunningcmdline;
				op = i;
				break;
			} // if


	// Find and print the first one.
	if(op == 1)
		i = 2;
	else
		i = 1;
	printf("%d", (ret = isrun(argv[i])));

	// Print the rest.
	for(i++; i < argc; i++)
		if(i != op)
			printf(" %d", isrun(argv[i]));

	printf("\n");

	return ret;
} // main
