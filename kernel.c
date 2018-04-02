#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
 
/* Note the use of the volatile keyword to prevent the compiler from eliminating dead stores. */
volatile uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(void)
{
	for( size_t i = 0; i < (VGA_HEIGHT); i++)
	{	for( size_t j = 0; j < VGA_WIDTH; j++)
		{
			terminal_buffer[i*VGA_WIDTH + j] = terminal_buffer[(i+1)* (VGA_WIDTH) + j];
		}
	}
}

void terminal_putchar(char c) 
{	
	if(c == '\n')
	{
		terminal_column = 0; ++terminal_row;	return;
	}
	
	if (++terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0; ++terminal_row;
	}
	if ((terminal_row) == (VGA_HEIGHT-1) && (terminal_column != 0))
	{
		terminal_scroll(); 
		--terminal_row;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);	
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
 
	terminal_writestring("Loading Kernel Main...                Done\n");

	terminal_setcolor(vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));
	terminal_writestring("Hello there! \n");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
	
	terminal_writestring("Line 1 \n Line 2 \n Line 3 \n Line 4 \n Line 5 \n Line 6 \n Line 7 \n Line 8 \n Line 9 \n Line 10 \n Line 11 \n Line 12 \n Line 13 \n Line 14 \n Line 15 \n Line 16 \n Line 17 \n Line 18 \n Line 19 \n Line 20 \n Line 21 \n Line 22 \n Line 23 \n Line 24 \n Line 25 \n Line 26 \n Line 27 \n Line 28 \n Line 29 \n Line 30 \n Line 31 \n Line 32 \n Line 33 \n Line 34 \n Line 35 \n Line 36 \n Line 37 \n Line 38 \n ine 39 \n Line 40 \n Line 41 \n Line 42 \n Line 43 \n Line 44 \n Line 45 \n Line 46 \n Line 47 \n Line 48 \n Line 49 \n Line 50 \n Line 51 \n Line 52 \n Line 53 \n Line 54 \n  ine 55 \n Line 56 \n  Line 57 \n Line 58 \n Line 59 \n Line 60 \n Line 61 \n Line 62 \n Line 63 \n Line 64 \n Line 65 \n Line 66 \n Line 67 \n Line 68 \n Line 69 \n Line 70 \n Line 71 \n Line 72 \n Line 73 \n Line 74 \n Line 75 \n Line 76 \n Line 77 \n Line 78 \n ine 79 \n Line 80 \n Line 81 \n Line 82 \n Line 83 \n Line 84 \n Line 85 \n Line 86 \n Line 87 \n Line 88 \n Line 89 \n Line 90 \n Line 91 \n Line 92 \n Line 93 \n Line 94 \n Line 95 \n Line 96 \n Line 97 \n Line 98 \n Line 99 \n Line 100 \n Line 101 \n Line 102 \n Line 103 \n Line 104 \n Line 105 \n Line 106 \n Line 107 \n Line 108 \n");
	
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	
	
}
