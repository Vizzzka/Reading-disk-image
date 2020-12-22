//
// Created by vizka on 22.12.20.
//

#ifndef ADDER_EXT_H
#define ADDER_EXT_H

#include <iostream>

#pragma pack(push, 1) // Пакувати поля щільно, без padding
struct ext2_superblock_t {
    unsigned int        inode_count;
    unsigned int        block_count;
    unsigned int        reserved;
    unsigned int        unallocated_blocks;
    unsigned int        unallocated_inodes;
    unsigned int        superblock_block_number;
    unsigned int        block_size;
    unsigned int        fragment_size;
    unsigned int        block_count_in_group;
    unsigned int        fragment_count_in_group;
    unsigned int        inode_count_in_group;
    unsigned int        last_mount_time;
    unsigned int        last_written_time;
    unsigned short      mount_count_before_check;
    unsigned short      allowed_mount_count;
    unsigned short      ext2_signature;
    unsigned short      fs_state;
    unsigned short      error_behaviour;
    unsigned short      minor_portion;
    unsigned int        last_check_time;
    unsigned int        check_interval;
    unsigned int        os_id;
    unsigned int        major_portion;
    unsigned short      user_id;
    unsigned short      group_id;
    unsigned int        first_inode;
    unsigned short      inode_size;
};

struct ext2_block_group_descriptor_table_t {
    unsigned int        block_usage_bitmap_block_address;
    unsigned int        inode_usage_bitmap_block_address;
    unsigned int        inode_table_start_block;
    unsigned short      unallocated_blocks_count;
    unsigned short      unallocated_inodes_count;
    unsigned short      directories_count;
};

struct ext2_inode_t {
    unsigned short      i_mode;         /* File type and access rights */
    unsigned short      i_uid;          /* Low 16 bits of Owner Uid */
    unsigned int        i_size;         /* Size in bytes */
    unsigned int        i_atime;        /* Access time */
    unsigned int        i_ctime;        /* Creation time */
    unsigned int        i_mtime;        /* Modification time */
    unsigned int        i_dtime;        /* Deletion Time */
    unsigned short      i_gid;          /* Low 16 bits of Group Id */
    unsigned short      i_links_count;  /* Links count */
    unsigned int        i_blocks;       /* Blocks count */
    unsigned int        i_flags;        /* File flags */
    unsigned int        i_os_specific_value;
    unsigned int        i_block_pointer_0;
    unsigned int        i_block_pointer_1;
    unsigned int        i_block_pointer_2;
    unsigned int        i_block_pointer_3;
    unsigned int        i_block_pointer_4;
    unsigned int        i_block_pointer_5;
    unsigned int        i_block_pointer_6;
    unsigned int        i_block_pointer_7;
    unsigned int        i_block_pointer_8;
    unsigned int        i_block_pointer_9;
    unsigned int        i_block_pointer_10;
    unsigned int        i_block_pointer_11;
};

struct ext2_directory_t {
    unsigned int        d_inode;
    unsigned short      d_entry_size;
    std::byte       d_name_length;
    std::byte       d_type;
};
#pragma pack(pop)


#endif //ADDER_EXT_H
