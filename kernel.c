#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INPUT_BUFFER 128

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

static const char scancode_ascii[] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,   '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0,  ' '
};

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

void disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void enable_cursor(uint8_t start, uint8_t end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}


void move_cursor(size_t row, size_t col) {
    uint16_t pos = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}


void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void clear_screen(void) {
  terminal_row = 0;
  terminal_column = 0;
  for(size_t height = 0; height != VGA_HEIGHT; height++) {
    for(size_t width = 0; width != VGA_WIDTH; width++) {
      terminal_putentryat(' ', terminal_color, width, height);
    }
  }

}

uint16_t make_vgaentry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}


void terminal_scroll() {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y-1)*VGA_WIDTH + x] = terminal_buffer[y*VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT-1)*VGA_WIDTH + x] = make_vgaentry(' ', terminal_color);
    }

    terminal_row--;
}


void terminal_putchar(char c) {
  if(terminal_row == VGA_HEIGHT) {
    terminal_scroll();
  }

  if(c == '\n') {
    terminal_row++;
    terminal_column = 0;
    move_cursor(terminal_row, terminal_column);
    return;
  }
  if(c == '\r') {
    terminal_column = 0;
    move_cursor(terminal_row, terminal_column);
    return;
  }
  if(c == '\b') {
    if(!(terminal_column == 0) && !(terminal_row == 0)) {
      terminal_column--;
      move_cursor(terminal_row, terminal_column);
      terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    }
    return;
  }
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
  move_cursor(terminal_row, terminal_column);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

char map(uint8_t scancode) {
    switch (scancode) {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        case 0x20: return 'd';
        case 0x12: return 'e';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x17: return 'i';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x32: return 'm';
        case 0x31: return 'n';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x10: return 'q';
        case 0x13: return 'r';
        case 0x1F: return 's';
        case 0x14: return 't';
        case 0x16: return 'u';
        case 0x2F: return 'v';
        case 0x11: return 'w';
        case 0x2D: return 'x';
        case 0x15: return 'y';
        case 0x2C: return 'z';

        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';

        case 0x1C: return '\n';   
        case 0x0E: return '\b';  
        case 0x39: return ' ';  

        default: return 0;
    }
}

// REMIND ME TO TURN THIS INTO A STRING.H

int cmpstr(char *one, char *two) {
  if(strlen(one) == strlen(two)) {
    for(int i = 0; i!=strlen(one); i++) {
      if(one[i] != two[i]) {
        return 1;
      }
    }
  } else { return 1; }

  return 0;
}
 
// non-skidded part:

void handle_command(char *command) {
  terminal_putchar('\n');
  if(!(cmpstr(command, "help"))) {
    terminal_writestring("help - show this message\nclear - clear the screen\n");
  } else if(!(cmpstr(command, "clear"))) {
    clear_screen();
  } else {
    terminal_writestring("Command not found: ");
    terminal_writestring(command);
    terminal_putchar('\n');
  }
}

void kernel_main(void) {
	terminal_initialize();

  enable_cursor(13, 15);

	terminal_writestring("          SpoolyOS\n");
  terminal_setcolor(VGA_COLOR_MAGENTA);
  terminal_writestring("\nCommand functionality exists. Type ");
  terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
  terminal_writestring("help");
  terminal_setcolor(VGA_COLOR_MAGENTA);
  terminal_writestring(" for more information.\n\n");

  terminal_setcolor(VGA_COLOR_WHITE);
  terminal_writestring("\r\n[spoolyOS] > ");
  terminal_setcolor(VGA_COLOR_WHITE);

  // this should be input working not non-skidded

  uint8_t last_scancode = 0;
  char input_buf[INPUT_BUFFER];
  size_t input_len = 0;

  while (1) {
      uint8_t scancode = inb(0x60);
      if (scancode == last_scancode) continue;

      last_scancode = scancode;

      if (scancode & 0x80) continue;

      char input = map(scancode);
      if (input == 0) continue;

      if (input == '\n') {
          input_buf[input_len] = '\0';
          if (input_len > 0) {
              handle_command(input_buf);
          }
          input_len = 0;
          terminal_writestring("\r\n[spoolyOS] > ");
          continue;
      }

      if (input == '\b') {
          if (input_len > 0) {
              input_len--;
              terminal_putchar('\b');
          }
          continue;
      }

      if (input_len < INPUT_BUFFER - 1) {
          input_buf[input_len++] = input;
          terminal_putchar(input);
      }
  }

}

