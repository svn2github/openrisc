#include "support.h"
#include "board.h"
#include "spi.h"


#define SPI_XMIT()                              \
        REG32(SPI_BASE + SPI_CTRL) |= SPI_CTRL_GO;         \
        while(REG32(SPI_BASE + SPI_CTRL) & SPI_CTRL_BSY)

void spi_init (int slave, int fq, int bit_nb, int lsb, int tx_pol, int rx_pol)
{
        int ctrl = 0;

        /* Set devider register to obtain desired serial clock frequency */
        REG32(SPI_BASE + SPI_DEVIDER) =  IN_CLK/(fq*2) - 1;

        /* Set control register */
        ctrl = bit_nb << 3;
        ctrl |= lsb ? SPI_CTRL_LSB : 0; 
        ctrl |= tx_pol ? SPI_CTRL_TX_NEGEDGE : 0; 
        ctrl |= rx_pol ? SPI_CTRL_RX_NEGEDGE : 0; 
        REG32(SPI_BASE + SPI_CTRL) = ctrl;

        /* Activate desired slave device */
        REG32(SPI_BASE + SPI_SS) = 1 << slave;
}
        
unsigned long spi_xmit (unsigned long val)
{
        REG32(SPI_BASE + SPI_TX) = val;
        SPI_XMIT();
        return REG32(SPI_BASE + SPI_RX);
}
