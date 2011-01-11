#include <stdio.h>
 
#define SMARK "S214"
#define SADDR 0x000000
#define INIT_ADDR 0x100000
#define SCHKSUM 0xff

int main(int argc, char **argv)
{

	FILE  *fd;
	int c, i, j;
	unsigned long addr = INIT_ADDR;
        unsigned char chksum;

	if(argc < 2)
		error("no input file specified");

	fd = fopen( argv[1], "r" );

	while (!feof(fd)) {
                j = 0;
                chksum = SCHKSUM;
                printf("%s%.6lx", SMARK, addr);
                while (j < 16) {
			c = fgetc(fd);
                        if (c == EOF) {
                                c = 0;
                        }
                        printf("%.2lx", c);
                        chksum -= c;
                        j++;
                }

                chksum -= addr & 0xff;
                chksum -= (addr >> 8) & 0xff;
                chksum -= (addr >> 16) & 0xff;
                chksum -= 0x14;
                printf("%.2lx\r\n", chksum);
                addr += 16;
        }
	return 0;
}	
