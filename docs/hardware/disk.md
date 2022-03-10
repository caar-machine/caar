# Disk
The CAAR machine's disk is a simple disk device inspired by ATA and other existing devices.

!!! Note
	Albeit being inspired by ATA, the disk is very different and may work differently.

## Disk commands
Disk commands are sent through a special structure inspired by x86's DAP.

Here is its format:

```c
struct DiskCommand
{
	uint8_t type; // 0 = read, 1 = write, 2 = identify
	uint8_t sectors; // Number of sectors to read/write
	uint32_t offset; // Offset
	uint32_t address; // Where to store the result
};
```

## Reading
The `READ` command will read `sectors` sectors at `offset` and write them to `address`.

## Writing
!!! Warning
	Writing is still **work-in-progress**, I'm still figuring out how to do commands properly.
