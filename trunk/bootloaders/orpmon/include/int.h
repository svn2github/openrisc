
/* Number of interrupt handlers */
#define MAX_INT_HANDLERS	32

/* Handler entry */
struct ihnd {
	void 	(*handler)(void);
};

/* Add interrupt handler */ 
int int_add(unsigned long vect, void (* handler)(void));

/* Initialize routine */
int int_init(void);

/* Disable interrupt */ 
int int_disable(unsigned long vect);

/* Enable interrupt */ 
int int_enable(unsigned long vect);

/* Error vector handler */
void int_error(int vect);
