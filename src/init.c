#include "init.h"
#include <assert.h>
#include <string.h>

/*
**********************************************************************************************************************
 * 
 *                                      функции ждя работы с хранилищем
 * 
**********************************************************************************************************************
 * */

void* simulate_flash(uint32_t bytes) 
{
    void *start = malloc(bytes);
    if (start==NULL) {
        exit(EXIT_FAILURE);
    }
    Control_block *control_block = start;
    control_block->counts = MAX_ELEMENTS;
    void *start_node = start + sizeof(Control_block);

    for(uint32_t i=0; i<MAX_ELEMENTS; ++i) {
        Node *cur_node  = start_node + i*sizeof(Node);
        cur_node->key = i;
        cur_node->value = (i+TEST_VALUE);  
    }
    return start;
}

void* create_database(void *flash_memory) 
{
    void *start_memory = malloc(MEMORY);
    if (start_memory==NULL) 
    {
        exit(-1);
    }
    void *start_flash = flash_memory;

    // обработка контрольного блока
    Control_block *flash_control_block = start_flash;
    Database *database = start_memory;
    database->control_block = *flash_control_block;

    // обнуляем цепочки
    for(uint32_t i=0; i<MAX_BUCKETS; ++i)
    {
        database->buckets[i] = NULL;
    }

    // добавление элемента в цепочку
    void *start_flash_node = start_flash + sizeof(Control_block);
    for(uint32_t i=0; i< database->control_block.counts; ++i) 
    {
        Node *cur_flash_node = start_flash_node + i*sizeof(Node);
        
        Memory_node **all_buckets = database->buckets;
        
        Memory_node *new_node = malloc(sizeof(Memory_node));
        // check malloc
        new_node->node = *cur_flash_node;
        //m_assert(new_node->node.key == (i+11));
        new_node->next = NULL;
        new_node->prev = NULL;

        uint32_t hash = hasher(new_node->node.key);
        
        Memory_node *cur_bucket = all_buckets[hash];
        
        if (cur_bucket==NULL) 
        {
            all_buckets[hash] = new_node;
        }
        else {
            while (cur_bucket->next!=NULL) 
            {
                cur_bucket = cur_bucket->next;
            }
            cur_bucket->next = new_node;
            new_node->prev = cur_bucket;
        }
    }
    return start_memory;
}

void save_database(Database *database, void *address)
{
    FLASH_Unlock();

    if (FLASH_EraseSector(DEVICE_SECTOR, VoltageRange_3) != FLASH_COMPLETE) 
    {
        blink_led(GPIOD, ORANGE);
    }
    
    void *flash_control_block = address;
    void *memory_control_block = &database->control_block;
    
    add_node_flash(flash_control_block, memory_control_block, sizeof(Control_block));

    void *flash_node = (void *)address + sizeof(Control_block);

    for(uint32_t i=0; i<MAX_BUCKETS; ++i) 
    {
        Memory_node *bucket = database->buckets[i];

        while (bucket!=NULL)
        {
            void *memory_node = &bucket->node;

            add_node_flash(flash_node, memory_node, sizeof(Node));
            
            bucket = bucket->next;
            flash_node += sizeof(Node);
        }
    }

    FLASH_Lock();
}

void add_node_flash(void* flash_node, void* memory_node, uint32_t count_bytes) 
{
    for(uint32_t i = 0; i<count_bytes; ++i) 
    {
        uint8_t val = *(uint8_t *)(memory_node + sizeof(uint8_t)*i);
        if (FLASH_ProgramByte((uint32_t)flash_node, val) != FLASH_COMPLETE) 
        {
            blink_led(GPIOD, ORANGE);
        }
        flash_node += sizeof(uint8_t);
    }
}

void prepare_flash_memory(void) 
{
    init();
    void* start = simulate_flash(MEMORY);
    WriteDeviceAddress(DEVICE_ADDRESS, start, MEMORY);
    free(start);
}

uint32_t hasher(uint32_t key) 
{
    const uint32_t k = MAX_BUCKETS;
    return key % k;
}

Node* find_node(Database *database, void *key) {
    const uint32_t *p_key = key;
    uint32_t hash = hasher(*(uint32_t *)key);

    Memory_node *bucket = database->buckets[hash];
    while (bucket!=NULL) 
    {
        if (bucket->node.key == *(uint32_t *)key)
        {
            return &bucket->node;
        }
        bucket = bucket->next;
    }

    return 0;
}
void dealocate(Database *database) {
    Memory_node **memory_node = database->buckets;
    for(uint32_t i=0; i<MAX_BUCKETS; ++i)
    {
        Memory_node *node = memory_node[i];
        while (node!=NULL)
        {
            Memory_node *next_node = node->next;
            free(node);
            node = next_node;
        }
    }
    free(database);
}
/*
**********************************************************************************************************************
 * 
 *                              функции ждя работы с STM32
 * 
**********************************************************************************************************************
 * */

void init() 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Medium_Speed ;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}
void m_assert(const bool flag) 
{
    if (flag) {
        blink_led(GPIOD, GREEN);
    }
    else {
        blink_led(GPIOD, RED);
    }
}

void blink_led(GPIO_TypeDef* port, uint16_t pins) 
{
    GPIO_SetBits(port, pins);
    for(uint32_t i=0; i<SWITCH_DELAY ; ++i) ;
    GPIO_ResetBits(port, pins);
    for(uint32_t i=0; i<SWITCH_DELAY ; ++i) ;
}
void WriteDeviceAddress(void * address, void *value, uint32_t count_bytes)
{
    FLASH_Unlock();

    if (FLASH_EraseSector(DEVICE_SECTOR, VoltageRange_3) != FLASH_COMPLETE) 
    {
        blink_led(GPIOD, ORANGE);
    }
    
    void* cur_addr = (void *)address;
    
    for(uint32_t i=0; i<count_bytes; ++i) 
    {
        uint8_t val = *(uint8_t *)(value + sizeof(uint8_t)*i);
        if (FLASH_ProgramByte((uint32_t)cur_addr, val) != FLASH_COMPLETE) 
        {
            blink_led(GPIOD, ORANGE);
        }
        /**
         * @todo почему assert не работает
         * */
        //m_assert(ReadDeviceAddress(cur_addr) == val);
        cur_addr += sizeof(uint8_t);
    }
    
    FLASH_Lock();
}
uint32_t ReadDeviceAddress(uint32_t address)
{
    return (*(volatile uint32_t*) address);
}

