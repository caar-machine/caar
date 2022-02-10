# CAAR GPU
The CAAR GPU is a simple way to draw graphics.

## Framebuffer
To get a framebuffer, read from the gpu's bus address.
<br/>
The framebuffer is 32bpp, that is 4 bytes per pixel in the 0xRRGGBB00 format

## Getting the framebuffer
To get the framebuffer, you need to first find a GPU on the device bus, with type 2.

Pseudocode:
```c
BusDevice gpu;

for(int i = 0; i < bus.device_count; i++)
{
  if(bus.devices[i].type == BUS_GPU) // 2
  {
    gpu = bus.devices[i];
    break;
  }
}

uint32_t *fb = (uint32_t*)read_addr(gpu.address);
fb[0] = 0xffffff00; // 0xRRGGBB00, white pixel
```
