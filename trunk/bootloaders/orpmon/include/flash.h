#ifndef _FLASH_H
#define _FLASH_H

#ifdef FLASH_BASE_ADDR

#ifdef FLASH_ORG_16_2
/* INC_ADDR = for how many bytes address should be incremented */
#define INC_ADDR             4
#define reg_write(a,b)       (REG32(a) = b)
#define reg_read(a)          (REG32(a))
#define check_error_bit(a,b) ((a & (b << 16)) || (a & b))
#define fl_wait_busy(a)      (!(a & (FL_SR_WSM_READY << 16)) || !(a & FL_SR_WSM_READY))

#elif FLASH_ORG_16_1
#define INC_ADDR             2
#define reg_write(a,b)       (REG16(a) = (unsigned short)b)
#define reg_read(a)          (REG16(a))
#define check_error_bit(a,b) (a & b)
#define fl_wait_busy(a)      (!(a & FL_SR_WSM_READY))
#else
#error Flash organization is not set! Check board.h.
#endif

#define FL_SR_WSM_READY   0x80
#define FL_SR_ERASE_ERR   0x20
#define FL_SR_PROG_ERR    0x40
#define FL_SR_PROG_LV     0x08
#define FL_SR_LOCK        0x02

int fl_init (void);
int fl_unlock_one_block (unsigned long addr);
int fl_unlock_blocks (void);
int fl_word_program (unsigned long addr, unsigned long val);
int fl_block_erase (unsigned long addr);

/* erase = 1 (whole chip), erase = 2 (required only) */
int fl_program (unsigned long src_addr, unsigned long dst_addr, unsigned long len, int erase, int verify);

/*
 * Next definitions and functions are here, because we need there flash 
 * functions in RAM (when we are running orpmon from Flash). They are
 * copied to RAM and fl_ext_program, fl_ext_erase and fl_ext_unlock are
 * pointing to them.
 */
typedef int(*t_fl_ext_program)(unsigned long, unsigned long);
typedef int(*t_fl_erase)(unsigned long);

typedef void(*t_uart_putc)(unsigned char);

t_fl_ext_program fl_ext_program;
t_fl_erase fl_ext_erase, fl_ext_unlock;

#endif

#endif /* _FLASH_H */
