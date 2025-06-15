#include "ext2fs.h"
#include "hdd.h"
#include "pentry.h"
#include "mbr.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>

struct Ext2FSInode * Ext2FS::load_inode(unsigned int inode_number)
{
    //TODO: Ejercicio 2
    int block_size = 1024 << _superblock->log_block_size;

    unsigned int block_group_index = blockgroup_for_inode(inode_number);
    unsigned int inode_index_in_group = blockgroup_inode_index(inode_number);
    struct Ext2FSBlockGroupDescriptor* block_group_descriptor = block_group(block_group_index);

    unsigned int inodes_per_block = block_size / sizeof(struct Ext2FSInode);
    unsigned int inode_block_index = inode_index_in_group / inodes_per_block;
    unsigned int inode_index = inode_index_in_group % inodes_per_block;

    unsigned char* inode_block = (unsigned char*) malloc(block_size);
    read_block(block_group_descriptor->inode_table + inode_block_index, inode_block);

    struct Ext2FSInode* res = (struct Ext2FSInode*) malloc(sizeof(struct Ext2FSInode));
    struct Ext2FSInode* inode_array = (Ext2FSInode*) inode_block;
    std::memcpy(res, &inode_array[inode_index], sizeof(struct Ext2FSInode));

    free(inode_block);

    return res;
}

unsigned int Ext2FS::get_block_address(struct Ext2FSInode * inode, unsigned int block_number)
{
    int block_size = 1024 << _superblock->log_block_size;
    //TODO: Ejercicio 1

    // Dirección directa.
    if(block_number < 12) {
        return inode->block[block_number];
    }

    // Caso indirección simple.
    block_number -= 12;

    unsigned int entries_per_block = block_size / 4; // Tamano de cada bloque / tamano de cada entrada.

    if(block_number < entries_per_block){
        unsigned char* indirect_block = (unsigned char*) malloc(block_size);
        read_block(inode->block[12], indirect_block);

        unsigned int* block_array = (unsigned int*) indirect_block;
        unsigned int res = block_array[block_number];

        free(indirect_block);
        return res;
    }

    // Caso indirección doble.
    block_number -= entries_per_block;

    if(block_number < entries_per_block * entries_per_block){
        unsigned char* double_indirect_block = (unsigned char*) malloc(block_size);
        read_block(inode->block[13], double_indirect_block);
        unsigned int* indirect_block_array = (unsigned int*) double_indirect_block;

        unsigned int indirect_block_index = block_number / entries_per_block;
        unsigned int block_index = block_number % entries_per_block;

        unsigned char* indirect_block = (unsigned char*) malloc(block_size);
        read_block(indirect_block_array[indirect_block_index], indirect_block);
        unsigned int* block_array = (unsigned int*) indirect_block;

        unsigned int res = block_array[block_index];

        free(double_indirect_block);
        free(indirect_block);
        return res;
    }

    return -1;
}

void Ext2FS::read_block(unsigned int block_address, unsigned char * buffer)
{
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;
	for(unsigned int i = 0; i < sectors_per_block; i++)
		_hdd.read(blockaddr2sector(block_address)+i, buffer+i*SECTOR_SIZE);
}

struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
    unsigned int block_size = 1024 << _superblock->log_block_size;
    if(from == NULL)
        from = load_inode(EXT2_RDIR_INODE_NUMBER);
    //std::cerr << *from << std::endl;
    assert(INODE_ISDIR(from));

    //TODO: Ejercicio 3
    unsigned int total_read_bytes = 0;
    unsigned int i = 0;
    unsigned int block_read_bytes = 0;

    unsigned char* mega_block = (unsigned char*) malloc(block_size * 2);
    while(total_read_bytes < from->size) {
        unsigned int current_block_address = get_block_address(from, i);
        unsigned int next_block_address = get_block_address(from, i+1);
        //char* current_block = get_block_address(from, i);
        //char* next_block = get_block_address(from, i+1);
        read_block(current_block_address, mega_block);
        read_block(next_block_address, mega_block + block_size);

        while(block_read_bytes < block_size && total_read_bytes < from->size){

            Ext2FSDirEntry* dir_entry = (Ext2FSDirEntry*) (mega_block + block_read_bytes);
            //char dir_name[dir_entry.name_length];
            //&dir_name[0] = dir_entry.name;

            if(strlen(filename) == dir_entry->name_length && strncmp(dir_entry->name, filename, dir_entry->name_length) == 0){
                unsigned int inode_id = dir_entry->inode;
                free(mega_block);
                return load_inode(inode_id);
            }

            total_read_bytes += dir_entry->record_length;
            block_read_bytes += dir_entry->record_length;
        }
        i++;
        block_read_bytes -= block_size;
    }
    free(mega_block);
    return NULL;
}