#if KBD_ENABLED

/*
 * keyboard controller registers
 */
#define KBD_STATUS_REG      (unsigned int) KBD_BASE_ADD+0x4
#define KBD_CNTL_REG        (unsigned int) KBD_BASE_ADD+0x4
#define KBD_DATA_REG	    (unsigned int) KBD_BASE_ADD+0x0
/*
 * controller commands
 */
#define KBD_READ_MODE	    (unsigned int) 0x20
#define KBD_WRITE_MODE	    (unsigned int) 0x60
#define KBD_SELF_TEST	    (unsigned int) 0xAA
#define KBD_SELF_TEST2	    (unsigned int) 0xAB
#define KBD_CNTL_ENABLE	    (unsigned int) 0xAE
/*
 * keyboard commands
 */
#define KBD_ENABLE	    (unsigned int) 0xF4
#define KBD_DISABLE	    (unsigned int) 0xF5
#define KBD_RESET	    (unsigned int) 0xFF
/*
 * keyboard replies
 */
#define KBD_ACK		    (unsigned int) 0xFA
#define KBD_POR		    (unsigned int) 0xAA
/*
 * status register bits
 */
#define KBD_OBF		    (unsigned int) 0x01
#define KBD_IBF		    (unsigned int) 0x02
#define KBD_GTO		    (unsigned int) 0x40
#define KBD_PERR	    (unsigned int) 0x80
/*
 * keyboard controller mode register bits
 */
#define KBD_EKI		    (unsigned int) 0x01
#define KBD_SYS		    (unsigned int) 0x04
#define KBD_DMS		    (unsigned int) 0x20
#define KBD_KCC		    (unsigned int) 0x40

#define TIMEOUT_CONST	500000

extern volatile int kbd_tail;
extern volatile int kbd_head;
extern volatile int kbd_buf[KBDBUF_SIZE];

extern int kbd_init(void);

#endif /* KBD_ENABLED */
