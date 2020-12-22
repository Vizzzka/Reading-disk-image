#include "fat/fat.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <fstream>

void print_formatted(int space) {
    std::cout << std::left << std::setw(space);
}

bool is_fat32(const fat_boot_sector_t& boot_sector) {
    return boot_sector.dir_entries_count == 0;
}

std::string time_to_string(unsigned short time) {
    char result[128];
    int second = time & ((1 << 5) - 1);
    int minute = (time >> 5) & ((1 << 6) - 1);
    int hour = time >> 11;
    sprintf(result, "%d:%d:%d", hour, minute, second);
    return result;
}

std::string date_to_string(unsigned short date) {
    char result[128];
    int day = date & ((1 << 5) - 1);
    int month = (date >> 5) & ((1 << 4) - 1);
    int year = 1980 + (date >> 9);
    sprintf(result, "%d-%d-%d", day, month, year);
    return result;
}

int get_data_area_offset(const fat_boot_sector_t& boot_sector) {
    int root_directory_offset = get_root_dir_offset(boot_sector);
    return root_directory_offset + DIRECTORY_ENTRY_SIZE * boot_sector.dir_entries_count;
}

int get_root_dir_offset(const fat_boot_sector_t& boot_sector) {
    unsigned int fats_size = is_fat32(boot_sector) ?
                             boot_sector.sectors_per_table_fat32 : boot_sector.sectors_per_table;
    int reserved_area = boot_sector.reserved_sectors_count;
    int fat_area = fats_size * (int)boot_sector.tables_count;
    return (reserved_area + fat_area) * boot_sector.bytes_per_sector;
}

void print_boot_sector_info(const fat_boot_sector_t& boot_sector) {
    print_formatted();
    std::cout << "Bytes per sector: " << boot_sector.bytes_per_sector << "\n";
    print_formatted();
    std::cout << "Sectors per cluster: " << (int)boot_sector.sectors_per_cluster << "\n";
    print_formatted();
    std::cout << "Sectors of reserved area: " << boot_sector.reserved_sectors_count << "\n";
    print_formatted();
    std::cout << "Number of FATs: " << (int)boot_sector.tables_count << "\n";
    print_formatted();
    std::cout << "Dir entries count of the root dir: " << boot_sector.dir_entries_count << "\n";
    print_formatted();
    std::cout << "Sectors in FAT: " << (is_fat32(boot_sector) ?
    boot_sector.sectors_per_table_fat32 : boot_sector.sectors_per_table) << "\n";
}

void print_dir_entry_info(const fat_boot_sector_t& boot_sector,
                          const fat_dir_entry& dir_entry) {
    std::string filename(dir_entry.filename, dir_entry.filename + 8);
    std::string ext(dir_entry.extension,dir_entry.extension + 3);
    filename.erase(std::remove (filename.begin(), filename.end(), ' '), filename.end());
    if (dir_entry.attr == std::byte(16)) {    // if is directory
        print_formatted();
        std::cout << "Directory name: " << "/" << filename << "\n";
    }
    else {
        print_formatted();
        std::cout << "Filename: " << filename << "." << ext << "\n";
        print_formatted();
        std::cout << "Size: " << dir_entry.file_size << "\n";
    }

    if (is_fat32(boot_sector)) {
        print_formatted();
        std::cout << "Time of creation: " << time_to_string(dir_entry.create_time) << "\n";
        print_formatted();
        std::cout << "Date of creation: " << date_to_string(dir_entry.create_date) << "\n";
    }
    print_formatted();
    std::cout << "Time of last modification: " << time_to_string(dir_entry.last_mod_time) << "\n";
    print_formatted();
    std::cout << "Date of last modification: " << date_to_string(dir_entry.last_mod_date) << "\n";

    // Attributes
    std::cout << ((dir_entry.attr == std::byte(1)) ? "Readonly file\n" : "");
    std::cout << ((dir_entry.attr == std::byte(2)) ? "Hidden file\n" : "");
    std::cout << ((dir_entry.attr == std::byte(4)) ? "System file\n" : "");
    std::cout << ((dir_entry.attr == std::byte(8)) ? "Volume label\n" : "");
    std::cout << ((dir_entry.attr == std::byte(15)) ? "Long filename\n" : "");
    std::cout << ((dir_entry.attr == std::byte(32)) ? "Archived file\n" : "");

    int first_cluster, first_sector;
    if (is_fat32(boot_sector)) {
        first_cluster = dir_entry.first_cluster_msb << 16 | dir_entry.first_cluster_lsb;
    }
    else {
        first_cluster = dir_entry.first_cluster_lsb;
    }
    first_sector = (first_cluster - FIRST_CLUSTER_DATA_REGION) * (int)boot_sector.sectors_per_cluster;

    print_formatted();
    std::cout << "Number of first cluster: " << first_cluster << "\n";
    print_formatted();
    std::cout << "Number of first sector: " << first_sector << "\n";
}


void print_files(std::ifstream& input_stream, int first_cluster_offset,
                 int data_area_offset, fat_boot_sector_t boot_sector) {
    std::vector<int> first_cluster_of_dirs;
    char directory_entry_buff[DIRECTORY_ENTRY_SIZE];
    bool flag = true;
    input_stream.seekg(first_cluster_offset, std::ios::beg);
    while (flag) {
        input_stream.read(directory_entry_buff, DIRECTORY_ENTRY_SIZE);
        auto *directory_entry = reinterpret_cast<fat_dir_entry *>(directory_entry_buff);
        unsigned char first_ch = directory_entry->filename[0];
        if (first_ch == 0x00) {                         // 0x00 -- No later entry
            flag = false;
        }
        if (first_ch == 0xe5 || first_ch == 0x00 || first_ch == '.'     // 0xe5 -- File is deleted
        || directory_entry->attr == std::byte(15)) {
            continue;
        }
        print_dir_entry_info(boot_sector, *directory_entry);
        std::cout << "\n";

        if (directory_entry->attr == std::byte(16)) {         // 0x2e -- Directory
            if (is_fat32(boot_sector)) {
                int first_cluster = directory_entry->first_cluster_lsb |
                                    (directory_entry->first_cluster_msb << 16);
                first_cluster_of_dirs.push_back(first_cluster);
            }
            else {
                first_cluster_of_dirs.push_back(directory_entry->first_cluster_lsb);
            }

        }
    }

    for (const auto& first_cluster : first_cluster_of_dirs) {
        int new_first_cluster_offset = data_area_offset + (first_cluster - FIRST_CLUSTER_DATA_REGION)
                * (int)boot_sector.sectors_per_cluster * boot_sector.bytes_per_sector;
        print_files(input_stream, new_first_cluster_offset, data_area_offset, boot_sector);
    }

}