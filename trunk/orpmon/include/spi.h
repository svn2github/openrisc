extern void spi_init (int slave, int fq, int bit_nb, int lsb, int tx_pol, int rx_pol);
extern unsigned long spi_xmit (unsigned long val);

/* SPI register offsets */
#define SPI_RX          0x00
#define SPI_TX          0x00
#define SPI_CTRL        0x04
#define SPI_DEVIDER     0x08
#define SPI_SS          0x0c

/* SPI control register bits */
#define SPI_CTRL_IE             0x00000200
#define SPI_CTRL_LSB            0x00000100
#define SPI_CTRL_TX_NEGEDGE     0x00000004
#define SPI_CTRL_RX_NEGEDGE     0x00000002
#define SPI_CTRL_GO             0x00000001
#define SPI_CTRL_BSY            0x00000001
