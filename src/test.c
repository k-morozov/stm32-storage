#include "init.h"

void start_test(void);
void end_test(void);

void test1(void *, bool);
void test2(Database *, bool);
void test3(void *, bool);
void test4(Database *, bool);

void start_test(void)
{
    blink_led(GPIOD, GREEN | BLUE);
}
void end_test(void)
{
    blink_led(GPIOD, BLUE);
}



void test1(void *address, bool check_blink)
{
    if (check_blink) start_test();
    Control_block *control_block = address;
    if (check_blink) m_assert(control_block->counts == MAX_ELEMENTS);
    void *node = address + sizeof(Control_block);
    for(uint32_t i=0; i<control_block->counts; ++i) 
    {
        if (check_blink) m_assert( ((Node*)node)->key==i+TEST_KEY &&  ((Node*)node)->value==i+TEST_VALUE);
        node += sizeof(Node);
    }
    if (check_blink) end_test();
}
// тестить только при Bucket = 0
void test2(Database *database, bool check_blink)
{
    if (check_blink) start_test();

    Control_block *control_block = &database->control_block;
    if (check_blink) m_assert(control_block->counts == MAX_ELEMENTS);
    Memory_node *memory_node = database->buckets[0];
    for(uint32_t i=0; i<control_block->counts; ++i) 
    {
        if (check_blink) m_assert(memory_node->node.key == i+TEST_KEY);// && memory_node->node.value == i+TEST_VALUE);
        memory_node = memory_node->next;
    }

    if (check_blink) end_test();
}
// тестить только при Bucket = 0
void test3(void *address, bool check_blink)
{
    if (check_blink) start_test();

    Control_block *control = address;
    if (check_blink) m_assert(control->counts == MAX_ELEMENTS);
    void *node = (void *)address + sizeof(Control_block);

    for(uint32_t i = 0; i<control->counts; ++i) 
    {
        if (check_blink) 
        {
            m_assert( ((Node *)node)->key == i+TEST_KEY && ((Node *)node)->value == i+TEST_VALUE);
        }
        node += sizeof(Node);
    }
    if (check_blink) end_test();
}

// test find node
void test4(Database *database, bool check_blink) 
{
    if (check_blink) start_test();

    uint32_t key = 7;
    Node *result = find_node(database, &key);
    if (check_blink) m_assert(result->value == 107);

    key = 3;
    result = find_node(database, &key);
    if (check_blink) m_assert(result->value == 103);

    key = 10000000;
    result = find_node(database, &key);
    if (check_blink) m_assert(result == NULL);

    key = 4;
    result = find_node(database, &key);
    if (check_blink) m_assert(result->value == 104);

    result->value = 1;
    result = find_node(database, &key);
    if (check_blink) m_assert(result->value == 1);

    if (check_blink) end_test();
}