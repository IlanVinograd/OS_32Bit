#ifndef _TEST_H_
#define _TEST_H_

#include <stdint.h>
#include <vga.h>
#include <bitmap.h>
#include <paging.h>

void test_bitmap();
void test_isr0();
void test_isr6();
void test_isr13();
void test_isr14();

void test_alloc_medium();
void test_alloc_hard();
void test_alloc_even_hard();

#endif