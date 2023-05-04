#ifndef DRIVER_H
#define DRIVER_H

#include "config_gui.h"

class Driver {
public:
    Driver();
    
    int open_pci_driver();
private:
    int net;
};

#endif // DRIVER_H