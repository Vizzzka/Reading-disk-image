#ifndef ADDER_FAT_HPP
#define ADDER_FAT_HPP

#include <iostream>

const size_t BOOT_SECTOR_SIZE = 512;
const size_t DIRECTORY_ENTRY_SIZE = 32;
const size_t FIRST_CLUSTER_DATA_REGION = 2;
const int FAT_SIGNATURE = 43605;

#pragma pack(push, 1) // Пакувати поля щільно, без padding
struct fat_boot_sector_t { // alignas(16)
    unsigned char bootjmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    std::byte sectors_per_cluster;
    unsigned short reserved_sectors_count;
    std::byte tables_count;
    unsigned short dir_entries_count;
    unsigned short total_sectors_count;
    std::byte media_type;
    unsigned short sectors_per_table;  // 0 for FAT32
    unsigned short sectors_per_track;
    unsigned short heads_count;
    unsigned int hidden_sectors_count;
    unsigned int total_sectors_count_fat32;
    unsigned int sectors_per_table_fat32;      // valid only for FAT32
};

struct fat_dir_entry {
    unsigned char filename[8];
    unsigned char extension[3];
    std::byte attr;
    std::byte reserved;
    std::byte milli_second_create_time;     // reserved for FAT16/FAT12
    unsigned short create_time;             // reserved for FAT16/FAT12
    unsigned short create_date;             // reserved for FAT16/FAT12
    unsigned short last_access_date;        // reserved for FAT16/FAT12
    unsigned short first_cluster_msb;       // reserved for FAT16/FAT12
    unsigned short last_mod_time;
    unsigned short last_mod_date;
    unsigned short first_cluster_lsb;
    unsigned int file_size;
};

#pragma pack(pop)   // Відновити пакування по замовчуванню
// Обов'язкове, щоб не зламати ABI мови,
// її роботу із іншими структурами.

void print_formatted(int space=50);

bool is_fat32(const fat_boot_sector_t& boot_sector);

std::string time_to_string(unsigned short time);
std::string date_to_string(unsigned short date);

int get_data_area_offset(const fat_boot_sector_t& boot_sector);

int get_root_dir_offset(const fat_boot_sector_t& boot_sector);

void print_boot_sector_info(const fat_boot_sector_t& boot_sector);

void print_dir_entry_info(const fat_boot_sector_t& boot_sector,
                          const fat_dir_entry& dir_entry);

void print_files(std::ifstream& input_stream, int first_cluster_offset,
                 int data_area_offset, fat_boot_sector_t boot_sector);
#endif //ADDER_FAT_HPP