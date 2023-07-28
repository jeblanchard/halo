// A struct representing what the boot sector provides
// the kernel upon entry.
#pragma pack(push, 1)
struct multiboot2_info {
    unsigned int total_size;
    unsigned int reserved;

    unsigned int mem_info_type;
    unsigned int mem_info_size;
    unsigned int num_kb_in_mem;

    unsigned int mem_map_info_type;
    unsigned int mem_map_info_size;
    unsigned int entry_size;
    unsigned int entry_version;
    unsigned int mem_map_base_addr;
    unsigned int mem_map_num_entries;

    unsigned int terminating_tag_type;
    unsigned int terminating_tag_size;
};
#pragma pack(pop)
