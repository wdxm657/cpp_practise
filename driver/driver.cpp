#include "driver.h"


Driver::Driver()
:net(0)
{
    
}

int Driver::open_pci_driver()
{
	int fd;
	fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
	if(fd < 0)
	{
		perror("open fail\n");
		return -1;
	}
	return fd;
}