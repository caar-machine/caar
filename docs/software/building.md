# Getting Started

!!! Note
	This process works best on a UNIX-like system, preferably GNU/Linux. If you're on Windows, try using WSL2.

CAAR is a multiple language project, using C, C++ and CAAR assembly.
Since managing multiple subprojects is hard to do with a traditional build system such as GNU Make, we opted for meson.

## Dependencies
* `ninja`
* `meson`
* `gcc`
* `sdl2`

## Getting the source code
Run the following command in your terminal to get the source code:

```
git clone https://github.com/caar-machine/caar --recursive
```

## Building

You can run this one-liner to build the project:

```
cd caar && meson build && ninja -C build && ninja -C build install
```

## Running
To run the project, you can use the executables in the `bin` directory created by meson.
Here is a sample command which would run the emulator with the bios, loading a disk `disk.img`:

```
bin/caar-emu -r bin/bios.bin -d disk.img
```
