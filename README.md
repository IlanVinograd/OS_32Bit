# Overview Version 0.3
**OS_32Bit** is a 32-bit operating system designed to showcase technical capabilities and serve as a platform for learning and collaboration. It is entirely open-source, inviting contributions from the developer community.

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
docker build -t os_32bit .
```

### Run the Project
- You can run the project using either Command Prompt or PowerShell.

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

# Technical Details
**OS_32Bit** is designed around a 32-bit architecture, supporting essential functionalities for educational and practical purposes.

- *Architecture Overview*
- *File System Details*

# Documentation
Comprehensive documentation is provided to help you understand the core concepts and contribute to the project effectively.

- [WorkFlow.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/WorkFlow.md)
- [TECHNICAL_GUIDE.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Docs/Technical_Guide.md)
- [Resources.md](https://github.com/IlanVinograd/OS_32Bit/blob/main/Resources.md)
