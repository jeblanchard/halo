#include "kernel/memory/virtual/virtual_mem_mgr.h"

physical_address get_pt_base_addr(page_dir_entry* entry) {
    physical_address raw_pt_addr = *entry >> 12;
    return raw_pt_addr;
}
