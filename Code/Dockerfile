FROM ubuntu:latest

#  Install dependencies
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

# Install and build i686-elf cross-compiler with parallel build
RUN mkdir /usr/local/cross && cd /usr/local/cross && \
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.36.tar.gz && \
    tar -xzf binutils-2.36.tar.gz && \
    cd binutils-2.36 && \
    mkdir build && cd build && \
    ../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --disable-werror && \
    make -j$(nproc) && make install -j$(nproc) && \
    cd ../../ && \
    wget https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz && \
    tar -xzf gcc-10.2.0.tar.gz && \
    cd gcc-10.2.0 && \
    mkdir build && cd build && \
    ../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --enable-languages=c,c++ --without-headers && \
    make all-gcc -j$(nproc) && make all-target-libgcc -j$(nproc) && \
    make install-gcc -j$(nproc) && make install-target-libgcc -j$(nproc)

# Set environment variable for cross-compilation
ENV PATH="/usr/local/cross/bin:$PATH"

# Set working directory for the bootloader project
WORKDIR /usr/src/bootloader

# Copy all files from the current directory to the container
COPY . /usr/src/bootloader

# Build the project, clean previous builds, and run with QEMU
CMD ["sh", "-c", "make clean && make -j$(nproc) output && qemu-system-x86_64 -drive format=raw,file=bootloader.img -m 512M -display gtk"]