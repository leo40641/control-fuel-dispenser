/*******************************************************************************
* File Name: PSOCINT.c
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

#include "PSOC.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED))
    /*******************************************************************************
    * Function Name: PSOC_RXISR
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
    *  PSOC_rxBuffer - RAM buffer pointer for save received data.
    *  PSOC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  PSOC_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  PSOC_rxBufferOverflow - software overflow flag. Set to one
    *     when PSOC_rxBufferWrite index overtakes
    *     PSOC_rxBufferRead index.
    *  PSOC_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when PSOC_rxBufferWrite is equal to
    *    PSOC_rxBufferRead
    *  PSOC_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  PSOC_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(PSOC_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef PSOC_RXISR_ENTRY_CALLBACK
        PSOC_RXISR_EntryCallback();
    #endif /* PSOC_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START PSOC_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = PSOC_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in PSOC_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (PSOC_RX_STS_BREAK | 
                            PSOC_RX_STS_PAR_ERROR |
                            PSOC_RX_STS_STOP_ERROR | 
                            PSOC_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                PSOC_errorStatus |= readStatus & ( PSOC_RX_STS_BREAK | 
                                                            PSOC_RX_STS_PAR_ERROR | 
                                                            PSOC_RX_STS_STOP_ERROR | 
                                                            PSOC_RX_STS_OVERRUN);
                /* `#START PSOC_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef PSOC_RXISR_ERROR_CALLBACK
                PSOC_RXISR_ERROR_Callback();
            #endif /* PSOC_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & PSOC_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = PSOC_RXDATA_REG;
            #if (PSOC_RXHW_ADDRESS_ENABLED)
                if(PSOC_rxAddressMode == (uint8)PSOC__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & PSOC_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & PSOC_RX_STS_ADDR_MATCH) != 0u)
                        {
                            PSOC_rxAddressDetected = 1u;
                        }
                        else
                        {
                            PSOC_rxAddressDetected = 0u;
                        }
                    }
                    if(PSOC_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        PSOC_rxBuffer[PSOC_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    PSOC_rxBuffer[PSOC_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                PSOC_rxBuffer[PSOC_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (PSOC_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(PSOC_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        PSOC_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    PSOC_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(PSOC_rxBufferWrite >= PSOC_RX_BUFFER_SIZE)
                    {
                        PSOC_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(PSOC_rxBufferWrite == PSOC_rxBufferRead)
                    {
                        PSOC_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (PSOC_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            PSOC_RXSTATUS_MASK_REG  &= (uint8)~PSOC_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(PSOC_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (PSOC_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & PSOC_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START PSOC_RXISR_END` */

        /* `#END` */

    #ifdef PSOC_RXISR_EXIT_CALLBACK
        PSOC_RXISR_ExitCallback();
    #endif /* PSOC_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED)) */


#if (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED)
    /*******************************************************************************
    * Function Name: PSOC_TXISR
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
    *  PSOC_txBuffer - RAM buffer pointer for transmit data from.
    *  PSOC_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  PSOC_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(PSOC_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef PSOC_TXISR_ENTRY_CALLBACK
        PSOC_TXISR_EntryCallback();
    #endif /* PSOC_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START PSOC_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((PSOC_txBufferRead != PSOC_txBufferWrite) &&
             ((PSOC_TXSTATUS_REG & PSOC_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(PSOC_txBufferRead >= PSOC_TX_BUFFER_SIZE)
            {
                PSOC_txBufferRead = 0u;
            }

            PSOC_TXDATA_REG = PSOC_txBuffer[PSOC_txBufferRead];

            /* Set next pointer */
            PSOC_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START PSOC_TXISR_END` */

        /* `#END` */

    #ifdef PSOC_TXISR_EXIT_CALLBACK
        PSOC_TXISR_ExitCallback();
    #endif /* PSOC_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED) */


/* [] END OF FILE */
