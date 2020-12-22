#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "fat/fat.hpp"
#include <ext/ext.h>


int main(int argc, char *argv[]) {
    std::string image_file_path, fs_type;
    if (argc == 1) {
        std::cerr << "The image file was not specified\n";
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        fs_type = "vfats";      // default fs type
    }

    if (argc == 3) {
       fs_type = argv[2];
    }

    if (argc > 3) {
        std::cerr << "Too many arguments\n";
        return EXIT_FAILURE;
    }

    image_file_path = argv[1];

    if (fs_type == "vfats") {
        char boot_sector_buff[BOOT_SECTOR_SIZE];
        std::ifstream input_stream(image_file_path, std::ios::binary | std::ios::in);
        if (!input_stream) {
            std::cerr << "Error while creating stream from file " << image_file_path << "\n";
            return EXIT_FAILURE;
        }
        if (!input_stream.read(boot_sector_buff, BOOT_SECTOR_SIZE)) {
            std::cerr << "Error while reading image. Only " << input_stream.gcount() << " characters could be read\n";
            return EXIT_FAILURE;
        }

        auto *boot_sector = reinterpret_cast<fat_boot_sector_t *>(boot_sector_buff);    // get boot sector
        print_boot_sector_info(*boot_sector);

        char signature[2] = {boot_sector_buff[510], boot_sector_buff[511]}; // get signature value
        unsigned short signature_value = *reinterpret_cast<unsigned short *>(signature);
        if (signature_value != FAT_SIGNATURE) {
            std::cerr << "Wrong signature value for FAT: " << signature_value << "\n";
            return EXIT_FAILURE;
        }

        print_formatted();
        std::cout << "Signature value: " << signature_value << "\n";
        std::cout << "------------------------------------------------\n";

        print_files(input_stream, get_root_dir_offset(*boot_sector),
                    get_data_area_offset(*boot_sector), *boot_sector);
    }
    else if (fs_type == "ext2") {
        std::ifstream input_stream(image_file_path, std::ios::binary | std::ios::in);
        if (!input_stream) {
            std::cerr << "Error while creating stream from file " << image_file_path << "\n";
            return EXIT_FAILURE;
        }

    }
    else {
        std::cerr << "Unknown fs\n";
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
