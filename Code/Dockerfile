FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    libtool \
    pkg-config \
    nasm \
    qemu-system-x86 \
    gcc \
    gdb \
    make \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    x11-apps \
    && rm -rf /var/lib/apt/lists/*

# Download and build the i686-elf cross-compiler
RUN mkdir /usr/local/cross && cd /usr/local/cross && \
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.36.tar.gz && \
    tar -xzf binutils-2.36.tar.gz && \
    cd binutils-2.36 && \
    mkdir build && cd build && \
    ../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --disable-werror && \
    make && make install && \
    cd ../../ && \
    wget https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz && \
    tar -xzf gcc-10.2.0.tar.gz && \
    cd gcc-10.2.0 && \
    mkdir build && cd build && \
    ../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --enable-languages=c,c++ --without-headers && \
    make all-gcc && make all-target-libgcc && \
    make install-gcc && make install-target-libgcc

# Set environment variables for cross-compilation
ENV PATH="/usr/local/cross/bin:$PATH"

# Set the working directory for the bootloader project
WORKDIR /usr/src/bootloader

# Copy all files from the current directory to the Docker container
COPY . /usr/src/bootloader

# Compile and run the bootloader and kernel, with GUI enabled and 512MB of memory
CMD ["sh", "-c", "make clean && make -j$(nproc) output && qemu-system-x86_64 -drive format=raw,file=bootloader.img -m 512M -display gtk"]