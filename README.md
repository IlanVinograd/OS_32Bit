# Overview Version 0.7
**OS_32Bit** is a 32-bit operating system designed to showcase technical capabilities and serve as a platform for learning and collaboration. It is entirely open-source, inviting contributions from the developer community.

![Images/BG test.gif](https://github.com/IlanVinograd/OS_32Bit/blob/main/Images/GIF_TEST.gif)

## Core Components:
- Kernel Entry
- Virtual Memory
- Multitasking (Round Robin)
- Task Switching
- Interrupt Handling (IDT)
- Global Descriptor Table (GDT)
- Task State Segment (TSS)
- Utility Libraries
- Input/Output
- Memory Detection
- Time and Date
- Test and Debug

# Installation
This section provides the necessary steps for installing the operating system, including system requirements, installation guides, and booting instructions.

- **Do Fork + Clone Repository**:
  
  ```bash
  gh repo fork    https://github.com/IlanVinograd/OS_32Bi  t.git --clone=true gh repo star IlanVinograd/OS_32Bit
  ```
- **System Requirements**: A basic development environment with Docker installed.
- **For GUI QEMU**: Required install -> [VcXsrv](https://sourceforge.net/projects/vcxsrv/), When you start VcXsrv, choose "Multiple Windows" and ensure that "Disable access control" is checked.
- **Installation Guide**: Follow the steps below to build and run the OS using Docker.
- **Booting Instructions**: Run the bootloader through Docker and QEMU.

### Build the Docker Image

To build the Docker image, use the following command:

```
cd Code
```

```
docker build -t os_32bit .
```
```
cd ..
```

### Run the Project
- Run the project using either CMD or PowerShell with the following commands.

  **CMD**
  ```
  docker run --rm -e DISPLAY=host.docker.internal:0.0 -v %cd%\Code:/usr/src/bootloader os_32bit
  ```

  **OR**
  
  **Powershell**
  ```
  docker run --rm -e DISPLAY=host.docker.internal:0.0 -v "$(Get-Location)/Code:/usr/src/bootloader" os_32bit
  ```

### Rebuilding the File System (IF IT IS BROKEN OR TO RESET FS)
- If you need to reset the file system (rebuild the bootloader image and embed the file system), use the following commands:

**CMD**
```
docker run --rm -e DISPLAY=host.docker.internal:0.0 -v %cd%\Code:/usr/src/bootloader os_32bit sh -c "make clean_all && make output && make embed_superblock && qemu-system-x86_64 -drive format=raw,file=bootloader.img -m 512M -display gtk"
```

**Poweershell**
```
docker run --rm -e DISPLAY=host.docker.internal:0.0 -v "$(Get-Location)/Code:/usr/src/bootloader" os_32bit sh -c "make clean_all && make output && make embed_superblock && qemu-system-x86_64 -drive format=raw,file=bootloader.img -m 512M -display gtk"
```

# Documentation
Comprehensive documentation is provided to help you understand the core concepts and contribute to the project effectively.

- [CONTRIBUTING.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/CONTRIBUTING.md)
- [TECHNICAL_GUIDE.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Docs/Technical_Guide.md)
- [Resources.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Resources.md)
