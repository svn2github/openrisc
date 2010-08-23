#include "uart.h"
#include "board.h"
#include "support.h"

int spin_cursor_enabled = 0;


void enable_spincursor(void)
{
  spin_cursor_enabled = 1;
}


void disable_spincursor(void)
{
  spin_cursor_enabled = 0;

}

void spincursor(void)
{
  static int last_spin_char = 0;
  static int last_spin_ticks = 0;
  
  if (!spin_cursor_enabled)
    return;
  
  if ((get_timer(0) - last_spin_ticks) < (TICKS_PER_US*2000))
    return;
  
  // Put a backspace
  uart_putc(0x8);
  if (last_spin_char == 0)
    uart_putc("/");
  else if (last_spin_char == 1)
    uart_putc("-");
  else if (last_spin_char == 2)
    uart_putc("\\");
  else if (last_spin_char == 3)
    uart_putc("|");
  else if (last_spin_char == 4)
    uart_putc("/");
  else if (last_spin_char == 5)
    uart_putc("-");
  else if (last_spin_char == 6)
    uart_putc("\\");
  else if (last_spin_char == 7)
    {
      uart_putc("|");
      last_spin_char=-1;
    }
  
  last_spin_char++;
  last_spin_ticks = get_timer(0);
}
