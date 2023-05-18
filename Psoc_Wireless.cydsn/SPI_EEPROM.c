/*
*********************************************************************************************************
*                                         FC_Fuel CODE
*
*                             (c) Copyright 2016; Distracom S.A
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              FC_Fuel CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5888LTI-LP097
*
* Filename      : SPI_EEPROM.c
* Version       : V1.00
* Programmer(s) : Leonardo Orjuela   
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <project.h>
#include "Variables.h"
#include "funciones.h"


/*
*********************************************************************************************************
*                              void Write_EEPROM(uint8 *buffer, uint8 size)
*********************************************************************************************************
*/
void Write_EEPROM(uint8 *buffer, uint32 dir, uint8 size)
{
	uint8 i, Status_Reg;
	uint32  bPntr;
	bPntr = dir;
    for(i=0;i<size;i++)
    {
        // Write Enable instruction before each write.
	    SPIM_WriteTxData(0x06);
        // Wait for the write to complete.
	    while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
	
	    CyDelayUs(200);
	
        // Write sequentially from the Tx_Buf
			
        // 0x02 is the Write instruction to the EEPROM.
		SPIM_WriteTxData(0x02);
    	
        while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
			
        //0x00 is th MSB of the address.
		SPIM_WriteTxData(bPntr >> 16);
		
        while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
        
        //0x00 is th MSB of the address.
		SPIM_WriteTxData(bPntr >> 8);
		
        while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));        
			
        //bPntr is the LSB of the address.			
		SPIM_WriteTxData(bPntr & 0xFF);
		
        while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
			
        //Write Tx_Buf[i] to the address with MSB 0x00 , LSB bPntr.
		SPIM_WriteTxData(buffer[i]);
		
        while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
			
	    CyDelayUs(200);
			
        //Increment the pointer to the next location.
		bPntr++;
	
	
        //Read the status register to determine if the EEPROM has completed the write internally, by 
        //scanning the Busy bit in the Status register.


    	do
	    {
            // 0x05 is the instruction to read the Status Register.
	        SPIM_WriteTxData(0x05);
	        
            while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
			
			
            // A dummy word written to the EEPROM in order to read the Status register .	
	        SPIM_WriteTxData(0x04);
            
            // Wait for the read to complete
	        while(!(SPIM_ReadStatus() & SPIM_STS_RX_FIFO_NOT_EMPTY));
            
	        CyDelayUs(200); 
	        
            Status_Reg= SPIM_ReadRxData();
            // Stay in the loop as long as the Busy bit is 1.
	
	    }while(Status_Reg & 0x01);
		
    }
}

/*
*********************************************************************************************************
*                              void Read_EEPROM(uint16 dir, uint8 size)
*********************************************************************************************************
*/

void Read_EEPROM(uint32 dir, uint8 size)
{
	uint8 i;
	uint32 bPntr;
	bPntr = dir;
 	for(i=0;i<size;i++)
	{
	    // 0x03 is the instruction to read from the EEPROM.
		SPIM_WriteTxData(0x03);
		while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
		
		// 0x00 is the MSB of the address , bPntr is the LSB.			
		SPIM_WriteTxData(bPntr >> 16);
		while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
        
		// 0x00 is the MSB of the address , bPntr is the LSB.			
		SPIM_WriteTxData(bPntr >> 8);
		while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));        
		
		SPIM_WriteTxData((bPntr & 0xFF));
		while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
		
	    // A dummy word written to the EEPROM in order to read the data .				
		SPIM_WriteTxData(0x04);
	    while(!(SPIM_ReadStatus() & SPIM_STS_RX_FIFO_NOT_EMPTY));
		
		CyDelayUs(200);
		
        // Store the read data in Rx_Buf[i].
		buffer_rxeeprom[i]= SPIM_ReadRxData();

        //Increment the Pointer.
		bPntr++;
			
	}
}


/* [] END OF FILE */
