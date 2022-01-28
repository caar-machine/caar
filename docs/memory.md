# The CAAR memory layout
There are two types of memory in the CAAR computer:
- video ram
- ram

Note that the computer has 256mb of ram by default.

# Layout

```
0x000000...0x001000 -> CPU-based allocations (for cons cells)
0x001000...0xff8000 -> regular memory (~256 mebibytes)
0xff8000...0xffe000 -> framebuffer (~24 kb)
0xffe000...0x1000000 -> stack (8 kb)
```

## Framebuffer
The framebuffer is 24 bpp

