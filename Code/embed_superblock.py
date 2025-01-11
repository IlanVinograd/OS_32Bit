import struct

BOOTLOADER_IMG = "bootloader.img"
OFFSET = 0x10000
SECTOR_SIZE = 512

superblock = struct.pack(
    "<HHHHHHB11s8s",
    8192,               # total_sectors
    8,                  # sectors_per_cluster
    512,                # bytes_per_sector
    8192,               # available_sectors
    256,                # total_direntries
    256,                # available_direntries
    0xFA,               # fs_type
    b'\x00' * 11,       # reserved
    b'SFAT'             # label
)

superblock += b'\x00' * (SECTOR_SIZE - len(superblock))

with open(BOOTLOADER_IMG, "r+b") as img:
    img.seek(OFFSET)
    img.write(superblock)

print(f"SuperBlock written to {BOOTLOADER_IMG} at offset {OFFSET:#X}")