#pragma once

#include <stm32f4xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEVICE_ADDRESS  ((void*)0x08007F00)
#define DEVICE_SECTOR   FLASH_Sector_1
#define SWITCH_DELAY    ((uint32_t)2000000)
#define MAX_ELEMENTS    (10)
#define MEMORY          (512)
#define MAX_BUCKETS     (5)

#define GREEN           (GPIO_Pin_12)
#define ORANGE          (GPIO_Pin_13)
#define RED             (GPIO_Pin_14)
#define BLUE            (GPIO_Pin_15)

#define TEST_KEY        (0)
#define TEST_VALUE      (100)

/*
**********************************************************************************************************************
 * 
 *                              Струтктура данных в памяти
 * 
**********************************************************************************************************************
 * */

typedef struct node {
    uint32_t key;
    uint32_t value;
} Node;

typedef struct {
    uint32_t counts;
} Control_block;

typedef struct Memory_node {
    Node node;
    struct Memory_node *next;
    struct Memory_node *prev;
} Memory_node;

typedef struct {
    Control_block control_block;
    Memory_node *buckets[MAX_BUCKETS];
} Database;
/*
**********************************************************************************************************************
 * 
 *                              функции ждя работы с хранилищем в памяти STM32
 * 
**********************************************************************************************************************
 * */
void* simulate_flash(uint32_t);
void* create_database(void *);
void save_database(Database *, void *);
void add_node_flash(void*, void*, uint32_t);
void prepare_flash_memory(void);
uint32_t hasher(uint32_t );
void dealocate(Database *);
Node* find_node(Database *, void *);
/*
**********************************************************************************************************************
 * 
 *                              функции ждя работы с STM32
 * 
**********************************************************************************************************************
 * */

void init(void);

uint32_t ReadDeviceAddress(uint32_t);
void WriteDeviceAddress(void *, void *, uint32_t);
void HardFault_Handler(void);

void m_assert(const bool);
void blink_led(GPIO_TypeDef*, uint16_t);