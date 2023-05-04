#ifndef DRIVER_H
#define DRIVER_H

#include "config_gui.h"
#include "iostream"

using namespace std;

class Driver
{
public:
    Driver();
    ~Driver();


private:
    int fd;
    void open_pci_driver();
};

#endif // DRIVER_H