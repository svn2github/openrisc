#include <stdio.h>
 
int main(int argc, char **argv)
{

	FILE  *fd;
	int c, i, j, width;
	unsigned long word;

	if(argc < 3)
		error("no input file specified");

	width = atoi(argv[1]);

	fd = fopen( argv[2], "r" );

	while (!feof(fd)) {
                j = 0;
		word = 0;
                while (j < width) {
			c = fgetc(fd);
                        if (c == EOF) {
                                c = 0;
                        }
			word = (word << 8) + c;
                        j++;
                }
		if(width == 1)	
			printf("%.2lx\n", word);
		else if(width == 2)
			printf("%.4lx\n", word);
		else
			printf("%.8lx\n", word);
        }
	return 0;
}	
