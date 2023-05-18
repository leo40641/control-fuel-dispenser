/*******************************************************************************
* File Name: LCD1INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCD1.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (LCD1_RX_INTERRUPT_ENABLED && (LCD1_RX_ENABLED || LCD1_HD_ENABLED))
    /*******************************************************************************
    * Function Name: LCD1_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD1_rxBuffer - RAM buffer pointer for save received data.
    *  LCD1_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  LCD1_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  LCD1_rxBufferOverflow - software overflow flag. Set to one
    *     when LCD1_rxBufferWrite index overtakes
    *     LCD1_rxBufferRead index.
    *  LCD1_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when LCD1_rxBufferWrite is equal to
    *    LCD1_rxBufferRead
    *  LCD1_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  LCD1_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD1_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD1_RXISR_ENTRY_CALLBACK
        LCD1_RXISR_EntryCallback();
    #endif /* LCD1_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD1_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = LCD1_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in LCD1_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (LCD1_RX_STS_BREAK | 
                            LCD1_RX_STS_PAR_ERROR |
                            LCD1_RX_STS_STOP_ERROR | 
                            LCD1_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                LCD1_errorStatus |= readStatus & ( LCD1_RX_STS_BREAK | 
                                                            LCD1_RX_STS_PAR_ERROR | 
                                                            LCD1_RX_STS_STOP_ERROR | 
                                                            LCD1_RX_STS_OVERRUN);
                /* `#START LCD1_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef LCD1_RXISR_ERROR_CALLBACK
                LCD1_RXISR_ERROR_Callback();
            #endif /* LCD1_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & LCD1_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = LCD1_RXDATA_REG;
            #if (LCD1_RXHW_ADDRESS_ENABLED)
                if(LCD1_rxAddressMode == (uint8)LCD1__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & LCD1_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & LCD1_RX_STS_ADDR_MATCH) != 0u)
                        {
                            LCD1_rxAddressDetected = 1u;
                        }
                        else
                        {
                            LCD1_rxAddressDetected = 0u;
                        }
                    }
                    if(LCD1_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        LCD1_rxBuffer[LCD1_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    LCD1_rxBuffer[LCD1_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                LCD1_rxBuffer[LCD1_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (LCD1_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(LCD1_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        LCD1_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    LCD1_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(LCD1_rxBufferWrite >= LCD1_RX_BUFFER_SIZE)
                    {
                        LCD1_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(LCD1_rxBufferWrite == LCD1_rxBufferRead)
                    {
                        LCD1_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (LCD1_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            LCD1_RXSTATUS_MASK_REG  &= (uint8)~LCD1_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(LCD1_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (LCD1_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & LCD1_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START LCD1_RXISR_END` */

        /* `#END` */

    #ifdef LCD1_RXISR_EXIT_CALLBACK
        LCD1_RXISR_ExitCallback();
    #endif /* LCD1_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (LCD1_RX_INTERRUPT_ENABLED && (LCD1_RX_ENABLED || LCD1_HD_ENABLED)) */


#if (LCD1_TX_INTERRUPT_ENABLED && LCD1_TX_ENABLED)
    /*******************************************************************************
    * Function Name: LCD1_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  LCD1_txBuffer - RAM buffer pointer for transmit data from.
    *  LCD1_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  LCD1_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(LCD1_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef LCD1_TXISR_ENTRY_CALLBACK
        LCD1_TXISR_EntryCallback();
    #endif /* LCD1_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START LCD1_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((LCD1_txBufferRead != LCD1_txBufferWrite) &&
             ((LCD1_TXSTATUS_REG & LCD1_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(LCD1_txBufferRead >= LCD1_TX_BUFFER_SIZE)
            {
                LCD1_txBufferRead = 0u;
            }

            LCD1_TXDATA_REG = LCD1_txBuffer[LCD1_txBufferRead];

            /* Set next pointer */
            LCD1_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START LCD1_TXISR_END` */

        /* `#END` */

    #ifdef LCD1_TXISR_EXIT_CALLBACK
        LCD1_TXISR_ExitCallback();
    #endif /* LCD1_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (LCD1_TX_INTERRUPT_ENABLED && LCD1_TX_ENABLED) */


/* [] END OF FILE */
