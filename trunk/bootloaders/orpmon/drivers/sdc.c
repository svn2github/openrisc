#include "sdc.h"
volatile sd_card dev;

void reset_card()
{
    SD_REG(SD_ARG)   = 0;
    SD_REG(SD_COMMAND) = 0;
    return;
}

unsigned char sd_wait_rsp()
{  
  volatile unsigned long r1, r2;
 
 //Polling for timeout and command complete
 while (1 ) 
 {
  r1= SD_REG(SD_ERROR_INT_STATUS);
  r2= SD_REG(SD_NORMAL_INT_STATUS);
  
   if (( r1 & CMD_TIMEOUT ) == CMD_TIMEOUT) 
	  return 0;
   else if ((r2  & CMD_COMPLETE ) == CMD_COMPLETE) 
	  return 1;
   
 }
  //Later Exception restart module
  return 0;
 
}

int setup_bd_transfer(boolean direction, int block_addr, volatile unsigned char *buff)
{
     int offset;
     int block_addr_sd;
   //debug("Read mmc_read_block %d to addr %d \n", block_number, buff_addr);
	// debug("read  %d to addr %d \n", block_number, buff_addr);
    if (dev.phys_spec_2_0 && dev.cid_reg !=66848)
       block_addr_sd=block_addr;
    else
        block_addr_sd=(block_addr<<9);
    
    if (direction)
    {
        SD_REG(BD_TX)  = buff;
        SD_REG(BD_TX)  = block_addr_sd;	
        
        return TRUE;        
    }
    else
    {
        SD_REG(BD_RX)  = buff;
        SD_REG(BD_RX)  = block_addr_sd;
        return TRUE;     	      
    }
    
}

int finnish_bd_transfer()
{
    volatile  unsigned long rtn_reg=0;
    
    rtn_reg= SD_REG(BD_ISR);
    while ( rtn_reg==0 ){
		rtn_reg= SD_REG(BD_ISR) ;
	}
        SD_REG(BD_ISR) =0;		
    
    if ( rtn_reg & 0x1)
    {
        DBGA("\n Data transfer succesful\n");
        return TRUE;
    }
    else
    {   
        DBGA("Data transfer failed, rtn %x\n",rtn_reg);
        return FALSE;
    }    
}



int sd_setup_transfer (sd_card sd_card_0)
{
    DBGA("Set up transfer\n");
	
	 //Put in transfer state 
    SD_REG(SD_COMMAND) = CMD7 | CICE | CRCE | RSP_48;  
    SD_REG(SD_ARG)=sd_card_0.rca | 0xf0f0;
    if (!sd_wait_rsp()){
            DBGA("Go send failed TO:/!\n");
            return FALSE;
    }
    else if (   SD_REG(SD_RESP1) == (CARD_STATUS_STB  |  READY_FOR_DATA ) )
        DBGA("Ready to transfer data!\n");
    else{
        DBGA("Card not ready for data %x \n",  SD_REG(SD_RESP1) );    
        return FALSE;
    }
    //Set block size
        
    if (!setBLockLength())
         return FALSE;

    //Set Bus width to 4, CMD55 followed by ACMD 6
     SD_REG(SD_COMMAND) = CMD55|RSP_48; 
     SD_REG(SD_ARG) =sd_card_0.rca | 0xf0f0;
    if (!sd_wait_rsp())
    {
            DBGA("CMD55 send failed :/!\n");
            return FALSE;
    }
            
     
     SD_REG(SD_COMMAND) = ACMD6 | CICE | CRCE | RSP_48;  
     SD_REG(SD_ARG)=0x2;
    if (!sd_wait_rsp())
    {
            DBGA("ACMD6 send failed :/!\n");
            return FALSE;
    }
            
                
    DBGA("Card Status reg ACMD6: 0x%x \n", SD_REG(SD_RESP1)  );		 	
    DBGA("FREE BD TX/RX: 0x%x \n", SD_REG(BD_STATUS)  ) ; 
    DBGA("CARD in Transfer state\n");
	return TRUE;	 
		  
}

int setBLockLength(void){
    
	SD_REG(SD_COMMAND) = CMD16 | CICE | CRCE | RSP_48;  
		SD_REG(SD_ARG)=512;
		if (!sd_wait_rsp()){
			 	DBGA("Set block size failed :/!\n");	
                return FALSE;}
	 	DBGA("Set block size to 512 Succes, resp 0x%x \n", SD_REG(SD_RESP1));
    	return TRUE;
}
int memCardInit(void)
{
	
    volatile unsigned long rtn_reg=0;
    volatile  unsigned long rtn_reg1=0;
    
    
    SD_REG(SD_TIMEOUT)=0x28FF;	 
    SD_REG(SD_SOFTWARE_RST)=1; 
    
    SD_REG(SD_CLOCK_D)  =0;
    SD_REG(SD_SOFTWARE_RST)=0; 
	
    reset_card();     	
    sd_wait_rsp();
       DBGA("sd reset \n");
    
    SD_REG(SD_COMMAND) = ( CMD8 | CICE | CRCE | RSP_48);
    SD_REG(SD_ARG) = VHS|CHECK_PATTERN;
    
    dev.phys_spec_2_0 = sd_wait_rsp();
    reset_card; 
    sd_wait_rsp();
     if (dev.phys_spec_2_0)   
     { 
	
	rtn_reg=0;
	while ((rtn_reg & BUSY) != BUSY)
	{
	    SD_REG(SD_COMMAND) = CMD55|RSP_48; 
	    SD_REG(SD_ARG) =0;
	    if (!sd_wait_rsp())
		return FALSE;
	
	    SD_REG(SD_COMMAND) =ACMD41 | RSP_48;
	    SD_REG(SD_ARG)   = 0x40000000 | 0xFF8000;
	    if (!sd_wait_rsp())
		return FALSE;
	    
	    rtn_reg= SD_REG(SD_RESP1);	    
	}
	dev.Voltage_window=rtn_reg&VOLTAGE_MASK;	
	dev.HCS_s = 0;
		
    }
    else
    {
	DBGA("SDC 1.xx card \n");
	SD_REG(SD_ARG)   =0x0000;
	SD_REG(SD_COMMAND) =0x0000;
	while (REG32(SDC_CONTROLLER_BASE+SD_STATUS)& 1) {}		
	
	rtn_reg=0;
	
	while ((rtn_reg & BUSY) != BUSY)
	{
	    SD_REG(SD_COMMAND) = CMD55|RSP_48; 
	    SD_REG(SD_ARG) = 0;
	    if (!sd_wait_rsp())
	    {
		rtn_reg= SD_REG(SD_RESP1) ;
		DBGA("FAIL rtn CMD55 %x\n", rtn_reg);
		   return FALSE;
	    }
	    SD_REG(SD_COMMAND) =ACMD41 | RSP_48;
	    SD_REG(SD_ARG) = 0;
	    if (!sd_wait_rsp())
	    {
		rtn_reg= SD_REG(SD_RESP1) ;
		DBGA("FAIL rtn ACMD41 %x\n", rtn_reg);
		   return FALSE;
	    }
	    rtn_reg= SD_REG(SD_RESP1) ;
	    DBGA("rtn ACMD41 %x\n", rtn_reg);
	}
	dev.Voltage_window=rtn_reg&VOLTAGE_MASK;	
	dev.HCS_s = 0;

    }   
    DBGA("get cid \n");		
    SD_REG(SD_COMMAND) =CMD2 | RSP_146;
    SD_REG(SD_ARG) =0;
    if (!sd_wait_rsp())
	     return FALSE;

    DBGA("get rca \n");
    SD_REG(SD_COMMAND) = CMD3 | CICE | CRCE | RSP_48;  
    SD_REG(SD_ARG) = 0;
    if (!sd_wait_rsp()){
	DBGA("rca failed \n");
        return FALSE;
    }
    rtn_reg = SD_REG(SD_RESP1);
    dev.rca = ((rtn_reg&RCA_RCA_MASK));
   
   SD_REG(SD_COMMAND) = CMD9 | RSP_146;  
                        SD_REG(SD_ARG)=0;
                        if (!sd_wait_rsp())
                              DBGA("NO CID! \n");
                        dev.cid_reg = SD_REG(SD_RESP1);
   
    dev.Active=1;    
    return sd_setup_transfer(dev);
   
   
}