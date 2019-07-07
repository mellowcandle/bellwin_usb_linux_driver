#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

int main(int argc, char **argv)
{
	struct hid_device_info *devs, *cur_dev;

	if (hid_init())
		return EXIT_FAILURE;

	devs = hid_enumerate(0x04d8, 0xfedc);
	cur_dev = devs;
	while (cur_dev) {
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

