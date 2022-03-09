# Bus
The device bus is the bus where every device is connected, similar to PCI.

!!! Note 
	Whilst similar to PCI, the bus is **much** simpler; Note that even though it is simpler, it may not be hardware accurate.

## Structure
To get the address of the bus, one needs to read from the io port #1 using the `in` instruction.

The bus has the following structure (pseudocode):
```c
struct BusDevice
{
    uint32_t type;
    uint32_t addr;
};

struct Bus
{
    uint32_t device_count;
    BusDevice devices[32];
};
```

So, to get the disk address, you could do this (again pseudocode):

```c
Bus *bus = (Bus*)io_read(1);

for(int i = 0; i < bus->device_count; i++)
{
    if(bus->devices[i].type == DISK)
    {
        disk_addr = bus->devices[i].addr;
        break;
    }
}
```

## Devices
The CAAR computer features simple devices.

### Types

Here are the different device types:

| Number | Description  |
| ------ | ------------ |
| 0      | Empty.       |
| 1      | Disk device. |
| 2      | GPU.         |

### Commands

Commands are performed by writing to a device's address.
Commands for devices are specified in their own documentation page in the HARDWARE section.
