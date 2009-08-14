#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>


#define PCI_DEV	"0000:00:02.0"
#define OFFSET	0xf4


int main(int argc, const char *argv[])
{
	int file;
	char *pci_device = "/sys/bus/pci/devices/" PCI_DEV;
	void *map;
	unsigned char *config;
	unsigned char value;

	file = open("/sys/bus/pci/devices/"PCI_DEV"/config", O_RDONLY);
	if (file == -1) {
		fprintf(stderr, "Could not open the pci device %s\n", pci_device);
		return -1;
	}

	lseek(file, OFFSET, SEEK_SET);
	read(file, &value, 0x01);
	fprintf(stdout, "value at offset 0x%x is 0x%x\n", OFFSET, value);

	close(file);

	return 0;
}
