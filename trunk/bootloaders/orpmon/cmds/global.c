#include "common.h"
#include "support.h"

global_struct global;

int src_addr_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.src_addr = strtoul(argv[0], 0, 0);
		return 0;
	} else
		return -1;
}

int dst_addr_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.dst_addr = strtoul(argv[0], 0, 0);
		return 0;
	} else
		return -1;
}

int length_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.length = strtoul(argv[0], 0, 0);
		return 0;
	} else
		return -1;
}

int ip_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.ip = parse_ip(argv[0]);
		return 0;
	} else
		return -1;
}

int srv_ip_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.srv_ip = parse_ip(argv[0]);
		return 0;
	} else
		return -1;
}

int erase_method_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		int a = strtoul(argv[0], 0, 0);
		if (a < 0 || a > 2)
			return -1;
		global.erase_method = a;
		return 0;
	} else
		return -1;
}

int start_addr_cmd(int argc, char *argv[])
{
	if (argc == 1) {
		global.start_addr = strtoul(argv[0], 0, 0);
		return 0;
	} else
		return -1;
}


int globals_cmd(int argc, char *argv[])
{
#if HELP_ENABLED
	const char *erase_method_desc[] =
	    { "do not erase", "fully", "as needed" };
	if (argc)
		return -1;
	printf("src_addr = %08lx\n", global.src_addr);
	printf("dst_addr = %08lx\n", global.dst_addr);
	printf("start_addr = %08lx\n", global.start_addr);
	printf("length = %08lx\n", global.length);
	printf("ip = %08lx\n", global.ip);
	printf("srv_ip = %08lx\n", global.srv_ip);
	printf("erase_method = %i (%s)\n", (int)global.erase_method,
	       erase_method_desc[global.erase_method]);
#endif /* HELP_ENABLED */
	return 0;

}



void module_global_init(void)
{
	register_command("src_addr", "<value>",
			 "sets global parameter source address", src_addr_cmd);
	register_command("dst_addr", "<value>",
			 "sets global parameter destination address",
			 dst_addr_cmd);
	register_command("start_addr", "<value>", "sets start address",
			 start_addr_cmd);
	register_command("length", "<value>", "sets global parameter length",
			 length_cmd);
	register_command("ip", "<value> <value> <value>",
			 "sets global parameter ip address", ip_cmd);
	register_command("srv_ip", "<value> <value> <value>",
			 "sets global parameter server ip address", srv_ip_cmd);
	register_command("erase_method", "<value> <value> <value>",
			 "sets flash erase method (0 = do not erase, 1 = fully, 2 = as needed)",
			 erase_method_cmd);
	if (HELP_ENABLED)
		register_command("globals", "", "show globals", globals_cmd);
}
