#include <stdio.h>

#define NUM_INP 3
#define firstStr  "#define"

int
main(int argc, char **argv)
{
    int i;
    FILE *fd;
    char inpStr[NUM_INP][128];
    int inpInt;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s fileName\n",argv[0]);
      	exit(1);
    }
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
	fprintf(stderr, "Unable to open file %s\n", argv[1]);
	return (-1);
    }

    while ((i = fscanf(fd, "%s%s%s", inpStr[0], inpStr[1], inpStr[2])) != EOF) {
	if (i != NUM_INP || strcmp (inpStr[0], firstStr))
	    continue;
	inpInt = atoi (inpStr[2]);
	fprintf (stdout, "{%d, %s, \042%s\042, \042\042, \042\042}\n", 
	 inpInt, inpStr[1], inpStr[1]); 
    }
}


