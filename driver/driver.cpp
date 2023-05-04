#include "driver.h"

Driver::Driver()
	: fd(0)
{
	open_pci_driver();
}

Driver::~Driver()
{
	if (fd)
	{
		close(fd);
	}
}

void Driver::open_pci_driver()
{
	fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
	if (fd < 0)
	{
		cout << "PCIe Device Open Fail !!!" << endl;
		perror("open fail\n");
	}
	else
	{
		cout << "PCIe Device Open Success !!!" << endl;
	}
}