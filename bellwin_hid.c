#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "hidapi.h"

static void print_help(FILE *out)
{
	fprintf(out, "Usage: bellwin_ctl [OPTION...] \n\n");
	fprintf(out, "  -l, --list\t\t List available bellwin USB devices\n");
	fprintf(out, "  -h, --help\t\t Display this help and exit\n");
	fprintf(out, "  -v, --version\t\t Output version information and exit\n");

}
static void print_version(void)
{
	printf("Bellwin USB power control v0.1\n");
}


static int bellwin_list_devices(void)
{
	struct hid_device_info *devs, *cur_dev;

	if (hid_init())
		return EXIT_FAILURE;

	devs = hid_enumerate(0x04d8, 0xfedc);
	cur_dev = devs;
	if (!cur_dev)
		printf("No Bellwin USB devices found.\n");
	else while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	int c;

	while (1) {

		static struct option long_options[] = {
			{"list", no_argument, 0, 'l'},
			{"version", no_argument, 0, 'v'},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0, 0}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "vhl", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
		case 'h':
			print_help(stdout);
			exit(EXIT_SUCCESS);
		case 'l':
			return bellwin_list_devices();
			break;
		case 0:
		case '?':
		default:
			print_help(stderr);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}

