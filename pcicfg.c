/*
 * Copyright (C) 2009 Greg Kroah-Hartman <gregkh@suse.de>
 * Copyright (C) 2009 Novell Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#define PCI_SYSFS	"/sys/bus/pci/devices/"
#define PCI_DEV		"0000:00:02.0"
#define DEFAULT_OFFSET	0xf4
#define DEFAULT_VALUE	0xff

static void display_help(char *program)
{
	fprintf(stdout, "%s - read or write a specific pci config value\n",
		program);
	fprintf(stdout, "Usage:\n");
	fprintf(stdout, "  %s [options]\n", program);
	fprintf(stdout, "options are:\n");
	fprintf(stdout, "  --action action\n");
	fprintf(stdout, "    ('read' or 'write', default is 'read')\n");
	fprintf(stdout, "  --device DEVICE\n");
	fprintf(stdout, "    default is "PCI_DEV"\n");
	fprintf(stdout, "  --offset OFFSET\n");
	fprintf(stdout, "    default is 0x%x\n", DEFAULT_OFFSET);
	fprintf(stdout, "  --value VALUE\n");
	fprintf(stdout, "    default is 0x%x\n", DEFAULT_VALUE);
	fprintf(stdout, "  --debug\n");
	fprintf(stdout, "  --help\n");
}


enum action {
	ACTION_READ,
	ACTION_WRITE,
};

int main(int argc, char *argv[], char *envp[])
{
	static const struct option options[] = {
		{ "action", 1, NULL, 'A' },
		{ "device", 1, NULL, 'D' },
		{ "offset", 1, NULL, 'O' },
		{ "value", 1, NULL, 'V' },
		{ "debug", 0, NULL, 'd' },
		{ "help", 0, NULL, 'h' },
		{ }
	};
	char pci_device[256];
	int option;
	int file;
	int flags;
	int result;
	int debug = 0;
	unsigned int offset = DEFAULT_OFFSET;
	unsigned int value = DEFAULT_VALUE;
	enum action action = ACTION_READ;
	char *program = argv[0];

	snprintf(pci_device, sizeof(pci_device), "%s%s/config", PCI_SYSFS, PCI_DEV);

	while (1) {
		option = getopt_long_only(argc, argv, "dhA:D:O:V:", options, NULL);
		if (option == -1)
			break;
		switch (option) {
		case 'A':
			if (strcasecmp(optarg, "read") == 0)
				action = ACTION_READ;
			else if (strcasecmp(optarg, "write") == 0)
				action = ACTION_WRITE;
			break;
		case 'D':
			snprintf(pci_device, sizeof(pci_device), "%s%s/config",
				 PCI_SYSFS, optarg);
			break;
		case 'O':
			result = sscanf(optarg, "%x", &offset);
		case 'V':
			result = sscanf(optarg, "%x", &value);
			break;
		case 'd':
			debug = 1;
			break;
		case 'h':
		default:
			display_help(program);
			return 0;
		}
	}

	if (debug)
		fprintf(stdout, "value = %x, offset = %x, pci_device = %s\n",
			value, offset, pci_device);

	switch (action) {
	case ACTION_WRITE:
		flags = O_WRONLY;
		break;
	case ACTION_READ:
		flags = O_RDONLY;
		break;
	}

	file = open(pci_device, flags);
	if (file == -1) {
		fprintf(stderr, "Could not open the pci device %s\n", pci_device);
		return -1;
	}

	lseek(file, offset, SEEK_SET);
	switch (action) {
	case ACTION_WRITE:
		write(file, &value, 0x01);
		break;
	case ACTION_READ:
		read(file, &value, 0x01);
		fprintf(stdout, "%x\n", value);
		break;
	}

	close(file);

	return 0;
}
