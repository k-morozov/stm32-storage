/**
*******************************************************************************************************************
 * @file    main.c
 * @author  Morozov C.
 * @date    14/01/2020
 * @brief   Основной фаил проекта "Hash". Реализация хранилища "ключ"-"значения" на flash для STM32 Discovery
 * @todo    работа с платой
 * @todo    любые типы ключ-значения
*******************************************************************************************************************
 */

#include <stm32f4xx.h>
#include "init.h"

#define DEBUG
#undef DEBUG

void test1(void *, bool);
void test2(Database *, bool);
void test3(void *, bool);

int main(void) 
{
    prepare_flash_memory();
    #ifdef DEBUG
        test1(DEVICE_ADDRESS, false);
    #endif // DEBUG
    
    Database *database = create_database(DEVICE_ADDRESS);
    #ifdef DEBUG
        test2(database, false);
    #endif // DEBUG
    
    save_database(database, DEVICE_ADDRESS);
    

     #ifdef DEBUG
            test3(DEVICE_ADDRESS, true);
    #endif // DEBUG
    
    while (1) 
    {
       test4(database, true);
       
    }
    dealocate(database);
    return EXIT_SUCCESS;
}