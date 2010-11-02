#include "common.h"
#include "support.h"
#include "spi.h"

int touch_cmd(int argc, char *argv[])
{
	unsigned long x, y, z1, z2;
	float res;

	if (argc == 0) {
		printf("usage:  -r  read coordinates\n");
		printf
		    ("        -c  read coordinates in a loop (press any key to exit)\n");
	} else if (argc == 1) {
		if (!strcmp(argv[0], "-r")) {
			spi_init(0, 1000000, 21, 0, 1, 0);
			printf("X = %.3lx\n", spi_xmit(0xd3l << 13) & 0xfff);
			printf("Y = %.3lx\n", spi_xmit(0x93l << 13) & 0xfff);
		} else if (!strcmp(argv[0], "-c")) {
			spi_init(0, 1000000, 21, 0, 1, 0);
			while (1) {
				x = spi_xmit(0xd3l << 13) & 0xfff;
				z1 = spi_xmit(0xb3l << 13) & 0xfff;
				z2 = spi_xmit(0xc3l << 13) & 0xfff;
				res = (z2 / z1) - 1;
				res = ((float)x * res) / 4096;
				if ((int)res < 20) {
					y = spi_xmit(0x93l << 13) & 0xfff;
					printf("X = %.3lx\n", x);
					printf("Y = %.3lx\n\n", y);
				}
				if (testc())
					break;
			}
		}
	} else {
		printf("usage:  -r  read coordinates\n");
		printf
		    ("        -c  read coordinates in a loop (press any key to exit)\n");
	}

	return 0;
}

void module_touch_init(void)
{
	register_command("touch", "", "touch screen utility", touch_cmd);
}
