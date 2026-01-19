# SimpleOS

![Version](https://img.shields.io/badge/alpha-0.03-blue)
![Raspberry Pi](https://img.shields.io/badge/Platform-Raspberry%20Pi-red?logo=raspberry-pi)
![Bare Metal](https://img.shields.io/badge/Type-Bare%20Metal%20OS-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

**SimpleOS** is a from-scratch bare-metal operating system for the Raspberry Pi family, built using Assembly, C, and (eventually) Rust. The goal is to explore low-level systems programming while creating a minimal, efficient kernel that runs directly on the hardware — no Linux, no firmware blobs, just the metal and myself.

The repository is structured to support **both 32-bit and 64-bit architectures** in a single codebase where possible:

```text
SimpleOS/
├── 32bit/          # Raspberry Pi 1, Zero, 2 (ARMv7/AArch32)
├── 64bit/          # Raspberry Pi 3, 4, 5 (AArch64)
├── common/         # Shared code (planned)
└── README.md

The user-facing API and high-level design will be virtually identical across
both branches, with architecture-specific differences isolated to boot code,
UART drivers, and low-level initialisation.
```

## Development Status

We are actively developing the **64-bit version** for Raspberry Pi 3, 4 & 5. The kernel currently:
- Boots in AArch64 EL1
- Initialises the PL011 UART for early console output
- Provides a simple echoing terminal
- Parks secondary cores safely
- Builds with a clean, freestanding toolchain

We are actively developing the **32-bit version** for Raspberry Pi 1, Zero & 2b. The kernel currently:
- Boots in AArch32 EL1
- Initialises the PL011 UART for early console output
- Initializes memory stack and tests memory functions
- Mini-shell with interactive user input. very basic with much more to come
- Parks secondary cores safely
- Builds with a clean, freestanding toolchain


## Roadmap

### Phase 1 – Core Boot & Console (Current Focus)
- [x] Multi-core safe boot (core 0 runs, others parked)
- [x] Reliable early UART console (PL011 for QEMU reliability, conditional GPIO mux for real hardware)
- [x] Basic `puts` / `putc` / `getc` with echo loop
- [x] stdio and stdlib building for more diverse terminal output.
- [x] Robust exception vector table (synchronous, IRQ, FIQ, SError)
- [x] Refined debugging and IO with printf
- [x] Hex terminal colors
- [x] System timer interrupts

### Phase 2 – Memory & Scheduling
- [ ] Physical memory manager (frame allocator)
- [x] Virtual memory with page tables
- [ ] Simple round-robin scheduler
- [ ] Heap allocation (custom allocator)

### Phase 3 – Multi-Core & Concurrency
- [ ] Wake secondary cores
- [ ] Per-core stacks and data
- [ ] Spinlocks and basic synchronisation

### Phase 4 – Rust Integration
- [ ] `no_std` Rust crate for higher-level kernel code
- [ ] Mixed C/Rust build system
- [ ] Safe abstractions over hardware (drivers in Rust)

### Phase 5 – Drivers & Features
- [ ] Framebuffer/graphics output
- [ ] GPIO & basic input
- [ ] SD card / filesystem
- [ ] USB & keyboard input

### Long-Term
- Advanced shell
- User-mode processes
- Networking

## Prerequisites

To build and test SimpleOS, you will need:

- [**Toolchain**:](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  - For 64-bit: `aarch64-none-elf-gcc` (ARM GNU Toolchain, Apple Silicon build recommended on macOS)
  - For 32-bit: `arm-none-eabi-gcc`
- **QEMU**: `qemu-system-aarch64` for 64bit or `qemu-system-arm` for 32bit (QEMU version 8.0 or newer recommended)
- **Make**: I provide an included makefile which simplifies the kenel compilation process if you wish to compile from source.
- macOS/Linux recommended (Windows via WSL works)

## On macOS with Homebrew:
```bash
brew install qemu
```

Then download and install the ARM GNU Toolchain from developer.arm.com
#### Building & Running

Inside the 64bit directory
```bash
make clean && make && make run
```
You should see the debug output for the kernel initialization then be met with a brief interactive shell menu with basic functionality.

To quit QEMU: Ctrl+A then X (or Ctrl+C in some configurations).
### Real Hardware

Build with GPIO pin muxing enabled:
```bash
make real
```
Copy kernel8.img to an SD card (replace the stock one on a fresh Raspberry Pi OS image, or use a minimal boot setup).
On Pi 4/5, disable Bluetooth if using PL011 on GPIO 14/15:
Add to `config.txt`:
```text
dtoverlay=disable-bt
```
Boot the Pi — output appears on the serial console (3.3V TTL USB adapter on GPIO 14/15).

## Contributing
Feel free to open issues, suggest features, or submit pull requests. This is an educational project, so clean, well-commented code and explanations are highly valued.

### License
MIT License — see LICENSE for details.