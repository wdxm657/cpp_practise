#ifndef DRIVER_H
#define DRIVER_H

#include "config_gui.h"
#include "dma.h"
#include "iostream"

using namespace std;

class Driver
{
public:
    Driver();
    ~Driver();
    DMA dma;

private:
    int fd;
    void open_pci_driver();
};

#endif // DRIVER_H