#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include <stdint.h>
#include <ata.h>
#include <string.h>
#include <vga.h>
#include <shell.h>

#define OFFSET_BEGIN 0x10000

#define START_FS   (OFFSET_BEGIN / SECTOR_SIZE) // Here the file system start, it's like offset.
#define START_DIR  ((OFFSET_BEGIN + 0x00200) / SECTOR_SIZE) // Here the dir entries system start, it's like offset.
#define START_FAT  ((OFFSET_BEGIN + 0x01200) / SECTOR_SIZE) // Here the FAT start, it's like offset.
#define START_DATA ((OFFSET_BEGIN + 0x02200) / SECTOR_SIZE) // Here the DATA start, it's like offset.

#define SECTOR_SIZE 512
#define MAX_SECTORS 8192
#define MAX_DIR 256
#define FAT ((MAX_SECTORS / 8) * 2) // the is 8 sector per cluster, and * 2 cause we use one byte and not 2 bytes so we * by 2.

typedef struct SuperBlock // 32 byte
{
  unsigned short total_sectors;        // Total number of sectors
  unsigned short sectors_per_cluster;  // Number of sectors in each block
  unsigned short bytes_per_sector;     // Number of bytes in each sector
  unsigned short available_sectors;    // Number of sectors available
  unsigned short total_direntries;     // Total number of directory entries
  unsigned short available_direntries; // Number of available dir entries
  unsigned char fs_type;               // File system type (FA for SFAT)
  unsigned char reserved[11];          // Reserved, all set to 0
  unsigned char label[8];              // Not guaranteed to be NUL-terminated
} SuperBlock;

typedef struct DirEntry // 16 byte
{
  unsigned char name[10];   // Not guaranteed to be NUL-terminated
  unsigned short fat_entry; // First data block
  unsigned int size;        // Size of the file in bytes
} DirEntry;

void init_fs();

void create_file(char* filename);
void delete_file(char* filename);
void write_to_file(char* data, char* filename);
void output_file(char* filename);
void showAllFiles();

bool_t isCreated(char* filename);
bool_t updateSB();
bool_t updateDir(char* filename);
bool_t updateFat();

bool_t isAvaSec(int32_t sectors);
bool_t isAvaDir();
bool_t removeDir(char* filename);
bool_t removeFat(char* filename);
bool_t write_data(char* filename, char* data);
bool_t extract_file(char* filename);

void updateDirAndSec(int32_t sectors, int32_t dir);

void nextLine();

#endif /* _FILE_SYSTEM_ */