# Overview Version 0.4
**OS_32Bit** is an open-source operating system built on a 32-bit architecture, providing essential functionalities for educational and practical purposes. It encourages collaboration from the developer community.

![Images/BG test.gif](https://github.com/IlanVinograd/OS_32Bit/blob/main/Images/BG%20test.gif)

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

- **Clone Repository**:
  
  ```
  git clone https://github.com/IlanVinograd/OS_32Bit.git
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
## Cleaning Up Containers
- You can stop and remove all running and stopped containers using the commands below.
- **NOTE:**
  Always clean the containers to free up resources!

  **CMD**
  ```
  for /f "tokens=*" %i in ('docker ps -q') do docker stop %i
  for /f "tokens=*" %i in ('docker ps -a -q') do docker rm %i
  ```

  **OR**

  **Powershell**
  ```
  docker stop $(docker ps -q) ; docker rm $(docker ps -a -q)
  ```

## Important Note: Save and Run

- After making changes to your code, **always save your changes** before testing.
- To test the updated code, **you must run the `docker run` command** again to start a new container with the latest changes.


  **CMD**
  ```
  docker run --rm -e DISPLAY=host.docker.internal:0.0 -v %cd%\Code:/usr/src/bootloader os_32bit
  ```

  **OR**

  **Powershell**
  ```
  docker run --rm -e DISPLAY=host.docker.internal:0.0 -v "$(Get-Location)/Code:/usr/src/bootloader" os_32bit
  ```

# Documentation
Comprehensive documentation is provided to help you understand the core concepts and contribute to the project effectively.

- [CONTRIBUTING.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/CONTRIBUTING.md)
- [TECHNICAL_GUIDE.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Docs/Technical_Guide.md)
- [Resources.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Resources.md)
