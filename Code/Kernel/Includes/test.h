#ifndef _TEST_H_
#define _TEST_H_

#include <stdint.h>
#include <vga.h>
#include <bitmap.h>
#include <paging.h>
#include <memory_manager.h>

void delay(uint32_t count);
void test_bitmap();
void test_isr0();
void test_isr6();
void test_isr13();
void test_isr14();
void test_malloc_fragmentation_large_scale();

#endif