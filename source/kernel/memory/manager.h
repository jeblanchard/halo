typedef enum halloc_stat {
    HALLOC_SUCCESS = 0, 
    NOT_ENOUGH_MEM = 1
} halloc_stat;

typedef struct halloc_resp {
    halloc_stat status;
    void* buffer;
} halloc_resp;

halloc_resp halloc(unsigned int num_bytes);
