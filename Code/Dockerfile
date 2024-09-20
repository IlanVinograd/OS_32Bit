FROM ubuntu:latest

# Install necessary packages including dependencies for building i686-elf cross-compiler
RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    libtool \
    pkg-config \
    nasm \
    qemu-system-x86 \
    make \
    gcc \
    binutils \
    libgmp3-dev \
    libmpfr-dev \
    libisl-dev \
    libmpc-dev \
    flex \
    bison \
    && rm -rf /var/lib/apt/lists/*

# Download and build i686-elf cross-compiler (binutils)
RUN mkdir -p /usr/src/cross && \
    cd /usr/src/cross && \
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.36.1.tar.gz && \
    tar xf binutils-2.36.1.tar.gz && \
    mkdir binutils-build && cd binutils-build && \
    ../binutils-2.36.1/configure --target=i686-elf --prefix=/usr/local/cross --with-sysroot --disable-nls --disable-werror && \
    make && make install

# Download and build i686-elf cross-compiler (GCC)
RUN cd /usr/src/cross && \
    wget https://ftp.gnu.org/gnu/gcc/gcc-10.3.0/gcc-10.3.0.tar.gz && \
    tar xf gcc-10.3.0.tar.gz && \
    mkdir gcc-build && cd gcc-build && \
    ../gcc-10.3.0/configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --enable-languages=c,c++ --without-headers && \
    make all-gcc && make all-target-libgcc && make install-gcc && make install-target-libgcc

# Set PATH for cross-compiler
ENV PATH="/usr/local/cross/bin:${PATH}"

# Download, extract, and install NASM
RUN wget https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/nasm-2.16.03.tar.gz && \
    tar xzf nasm-2.16.03.tar.gz && \
    cd nasm-2.16.03 && \
    ./configure && \
    make && \
    make install

# Set the working directory
WORKDIR /usr/src/bootloader

# Copy the project files into the container
COPY . /usr/src/bootloader

# Build the bootloader and run it in QEMU
CMD ["sh", "-c", "make clean && make output && qemu-system-x86_64 -drive format=raw,file=bootloader.img -nographic -serial mon:stdio"]
