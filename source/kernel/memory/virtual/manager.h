typedef enum alloc_page_resp {
    SUCCESS = 0,
    FAILURE = 1
} alloc_page_resp;

typedef unsigned int page_table_entry;

alloc_page_resp alloc_page(page_table_entry* entry);
