FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    libtool \
    pkg-config \
    nasm \
    qemu-system-x86 \
    make \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/nasm-2.16.03.tar.gz && \
    tar xzf nasm-2.16.03.tar.gz && \
    cd nasm-2.16.03 && \
    ./configure && \
    make && \
    make install

WORKDIR /usr/src/bootloader

COPY . /usr/src/bootloader

CMD ["sh", "-c", "make clean && make output && qemu-system-x86_64 -drive format=raw,file=bootloader.img -nographic -serial mon:stdio"]