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
 
 
// Global Variables
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;

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
 

/* Cursor related functions */
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
} 
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3E0) & 0xE0) | cursor_end);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
/* Cursor related functions END*/


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

/* Copy Next line to current line when scrolling */
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
	/* Procedure for newlines */
	if(c == '\n')
	{
		terminal_column = 0; ++terminal_row;	return;
	}
	
	/* Procedure for wrapping text to next line*/
	if (++terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0; ++terminal_row;
	}
	/* Scroll down and reset the terminal_row to previous line */
	if ((terminal_row) == (VGA_HEIGHT-1) && (terminal_column != 0))
	{
		terminal_scroll(); 
		--terminal_row;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);	
	update_cursor(terminal_column, terminal_row);
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
 
	terminal_writestring("Loading Kernel Main...                    Done\n");
	terminal_writestring("Creating TTY...               \n");
	
	/* Enable a cursor at correct location*/
	enable_cursor(12,15);
	update_cursor(55,22);
	
	/* Test lines */
	terminal_setcolor(vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
	terminal_writestring("Test_Line 1 \nTest_Line 2 \nTest_Line 3 \nTest_Line 4 \nTest_Line 50000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000000 \nTest_Line 6 \nTest_Line 7 \nTest_Line 8 \nTest_Line 9 \nTest_Line 10 \nTest_Line 11 \nTest_Line 12 \nTest_Line 13 \nTest_Line 14 \nTest_Line 15 \nTest_Line 16 \nTest_Line 17 \n");
//	terminal_writestring("\nTest_Line 18 \nTest_Line 19 \nTest_Line 20 \nTest_Line 21 \nTest_Line 22 \nTest_Line 23 \nTest_Line 24 \nTest_Line 25 \nTest_Line 26 \nTest_Line 27 \nTest_Line 28 \nTest_Line 29 \nTest_Line 30 \nTest_Line 31 \nTest_Line 32 \nTest_Line 33 \nTest_Line 34 \nTest_Line 35 \nTest_Line 36 \nTest_Line 37 \nTest_Line 38 \nine 39 \nTest_Line 40 \nTest_Line 41 \nTest_Line 42 \nTest_Line 43 \nTest_Line 44 \nTest_Line 45 \nTest_Line 46 \nTest_Line 47 \nTest_Line 48 \nTest_Line 49 \nTest_Line 50 \nTest_Line 51 \nTest_Line 52 \nTest_Line 53 \nTest_Line 54 \n ine 55 \nTest_Line 56 \n Test_Line 57 \nTest_Line 58 \nTest_Line 59 \nTest_Line 60 \nTest_Line 61 \nTest_Line 62 \nTest_Line 63 \nTest_Line 64 \nTest_Line 65 \nTest_Line 66 \nTest_Line 67 \nTest_Line 68 \nTest_Line 69 \nTest_Line 70 \nTest_Line 71 \nTest_Line 72 \nTest_Line 73 \nTest_Line 74 \nTest_Line 75 \nTest_Line 76 \nTest_Line 77 \nTest_Line 78 \nine 79 \nTest_Line 80 \nTest_Line 81 \nTest_Line 82 \nTest_Line 83 \nTest_Line 84 \nTest_Line 85 \nTest_Line 86 \nTest_Line 87 \nTest_Line 88 \nTest_Line 89 \nTest_Line 90 \nTest_Line 91 \nTest_Line 92 \nTest_Line 93 \nTest_Line 94 \nTest_Line 95 \nTest_Line 96 \nTest_Line 97 \nTest_Line 98 \nTest_Line 99 \nTest_Line 100 \nTest_Line 101 \nTest_Line 102 \nTest_Line 103 \nTest_Line 104 \nTest_Line 105 \nTest_Line 106 \nTest_Line 107 \nTest_Line 108 \n");
	
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	terminal_writestring("                                          Done\n");
	
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
	terminal_writestring("Hello there! \n");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	
}
