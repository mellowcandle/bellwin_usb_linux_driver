#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hidapi.h"

#define BELLWIN_VENDOR	0x04d8
#define BELLWIN_PRODUCT	0xfedc
#define BIT(x) (1 << (x))

#define POWER_SWITCH_COUNT 5

#define OP_GET_STATUS 0
#define OP_SET_POWER 1

static void print_help(FILE *out)
{
	fprintf(out, "Usage: bellwin_ctl [OPTIONS] [<outlet1>=<value1> <outlet2>=<value2>] ...\n\n");

	fprintf(out, "  -l, --list\t\t List available bellwin USB devices\n");
	fprintf(out, "  -h, --help\t\t Display this help and exit\n");
	fprintf(out, "  -v, --version\t\t Output version information and exit\n");
	fprintf(out, "  -D, --device\t\t <dev path> Open device by device node (IE. /dev/hidraw3/)\n");
	fprintf(out, "  -S, --serial\t\t <serial> Open device by serial number\n");

}
static void print_version(void)
{
	printf("Bellwin USB power control v0.1\n");
}

static bool verbose = false;

static int bellwin_list_devices(void)
{
	struct hid_device_info *devs, *cur_dev;

	if (hid_init())
		return EXIT_FAILURE;

	devs = hid_enumerate(BELLWIN_VENDOR, BELLWIN_PRODUCT);
	cur_dev = devs;
	if (!cur_dev)
		printf("No Bellwin USB devices found.\n");
	else while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
		       cur_dev->vendor_id, cur_dev->product_id, cur_dev->path,
		       cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}

	hid_free_enumeration(devs);
	hid_exit();

	return EXIT_SUCCESS;
}

static int send_command(hid_device *handle, const char *cmd, size_t len)
{
	unsigned char buf[0x40];
	int ret;
	int i;

	memset(buf, 0x5A, 0x40);

	if (len > 0x40) {
		printf("Command is too long\n");
		exit(EXIT_FAILURE);
	}

	memcpy(buf, cmd, len);

	if (verbose) {
		printf("Sending to device:\n");
		for (i = 0; i < 0x40; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

	ret = hid_write(handle, buf, 0x40);
	if (ret < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
	}

	return 0;
}

static void prepare_cmd(char *cmd, int idx, bool on)
{
	char cmd_template[7] = { 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (idx < 1 || idx > POWER_SWITCH_COUNT) {
		fprintf(stderr, "Index out of bounds");
		exit(EXIT_FAILURE);
	}

	cmd_template[5] = idx - 1;
	cmd_template[6] = !!on;

	memcpy(cmd, cmd_template, 7);
}

static int get_device_status(hid_device *handle)
{
	const char cmd1[7] = { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char buf[256];
	int ret;
	int timeout = 5;
	send_command(handle, cmd1, 7);

	/* Wait for response */
	ret = 0;
	while (ret == 0 && timeout--) {
		ret = hid_read(handle, buf, sizeof(buf));
		if (ret == 0)
			printf("waiting...\n");
		if (ret < 0)
			printf("Unable to read()\n");

		usleep(500*1000);
	}

	if (!timeout) {
		fprintf("Timeout occurred while waiting for device reply\n");
		return 1;
	}
	for (int i = 1; i < (POWER_SWITCH_COUNT + 1); i++)
		printf("Power switch %d: %s\n", i,
		       (buf[5] & BIT(i-1)) ? "ON" : "OFF");

	return 0;
}

hid_device *device_open_path(const char *path)
{
	hid_device *handle = NULL;
	handle = hid_open_path(path);
	if (!handle) {
		perror("Unable to open device");
	}

	return handle;
}

hid_device *device_open_serial(const char *serial)
{
	hid_device *handle = NULL;
	struct hid_device_info *devs;

	if (!serial) {
		devs = hid_enumerate(BELLWIN_VENDOR, BELLWIN_PRODUCT);
		if (!devs)
			return NULL;
		else if (devs->next) {
			fprintf(stderr,
				"More than one bellwin device found, please use --serial or --device option\n");
			hid_free_enumeration(devs);
			return NULL;
		}
	}

	handle = hid_open(BELLWIN_VENDOR, BELLWIN_PRODUCT, serial);
	if (!handle)
		perror("Unable to open device");

	return handle;
}

#if 0
static int test(bool verbose)
{
	unsigned char buf[256];
	int ret;
	int i;
	char new_cmd[7];
	const char cmd1[7] = { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	//const char cmd2[7] = { 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	if (hid_init())
		return -1;

	handle = hid_open(BELLWIN_VENDOR, BELLWIN_PRODUCT, NULL);
	if (!handle) {
		printf("unable to open device\n");
		return 1;
	}
	//
	// Set the hid_read() function to be non-blocking.

	send_command(handle, cmd1, 7, verbose);

	/* Wait for response */
	ret = 0;
	while (ret == 0) {
		ret = hid_read(handle, buf, sizeof(buf));
		if (ret == 0)
			printf("waiting...\n");
		if (ret < 0)
			printf("Unable to read()\n");

		usleep(500*1000);
	}

	for (i = 0; i < ret; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");

	prepare_cmd(new_cmd, 1, false);
	send_command(handle, new_cmd, 7, verbose);


#if 0

	send_command(handle, cmd2, 7, verbose);

	usleep(100);

	send_command(handle, cmd1, 7, verbose);

	/* Wait for response */
	ret = 0;
	while (ret == 0) {
		ret = hid_read(handle, buf, sizeof(buf));
		if (ret == 0)
			printf("waiting...\n");
		if (ret < 0)
			printf("Unable to read()\n");

		usleep(500*1000);
	}

	for (i = 0; i < ret; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");
#endif
	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

	return 0;
}
#endif
int main(int argc, char **argv)
{
	int c;
	int ret;
	char *serial = NULL;
	char *path = NULL;
	hid_device *handle = NULL;

	int operation = OP_GET_STATUS;

	while (1) {

		static struct option long_options[] = {
			{"list", no_argument, 0, 'l'},
			{"version", no_argument, 0, 'v'},
			{"verbose", no_argument, 0, 'V'},
			{"help", no_argument, 0, 'h'},
			{"status", no_argument, 0, 's'},
			{"serial", required_argument, 0, 'S'},
			{"device", required_argument, 0, 'D'},
			{0, 0, 0, 0}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "VvhlsS:D:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
		case 'V':
			verbose = true;
			break;
		case 'h':
			print_help(stdout);
			exit(EXIT_SUCCESS);
		case 'l':
			return bellwin_list_devices();
		case 's':
			operation = OP_GET_STATUS;
			break;
		case 'S':
			serial = optstring;
			break;
		case 'D':
			device = optstring;
			break;
		case 0:
		case '?':
		default:
			print_help(stderr);
			exit(EXIT_FAILURE);
		}
	}

	if (hid_init()) {
		fprintf(stderr, "Failed initializing HID subsystem\n");
		exit(EXIT_FAILURE);
	}

	if (path)
		handle = device_open_path(path);
	else
		handle = device_open_serial(serial);

	if (!handle) {
		fprintf(stderr, "Couldn't open HID device\n");
		print_help(stderr);
		exit(EXIT_FAILURE);
	}

	hid_set_nonblocking(handle, 1);
	//print_help(stdout);
//	test(verbose);
	switch (operation) {
	case OP_GET_STATUS:
		ret = get_device_status(handle);
		break;
	}

	hid_close(handle);
	hid_exit();
	if (!ret)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

