/*******************************************************************************
* File Name: WIFIINT.c
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

#include "WIFI.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (WIFI_RX_INTERRUPT_ENABLED && (WIFI_RX_ENABLED || WIFI_HD_ENABLED))
    /*******************************************************************************
    * Function Name: WIFI_RXISR
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
    *  WIFI_rxBuffer - RAM buffer pointer for save received data.
    *  WIFI_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  WIFI_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  WIFI_rxBufferOverflow - software overflow flag. Set to one
    *     when WIFI_rxBufferWrite index overtakes
    *     WIFI_rxBufferRead index.
    *  WIFI_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when WIFI_rxBufferWrite is equal to
    *    WIFI_rxBufferRead
    *  WIFI_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  WIFI_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(WIFI_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef WIFI_RXISR_ENTRY_CALLBACK
        WIFI_RXISR_EntryCallback();
    #endif /* WIFI_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START WIFI_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = WIFI_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in WIFI_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (WIFI_RX_STS_BREAK | 
                            WIFI_RX_STS_PAR_ERROR |
                            WIFI_RX_STS_STOP_ERROR | 
                            WIFI_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                WIFI_errorStatus |= readStatus & ( WIFI_RX_STS_BREAK | 
                                                            WIFI_RX_STS_PAR_ERROR | 
                                                            WIFI_RX_STS_STOP_ERROR | 
                                                            WIFI_RX_STS_OVERRUN);
                /* `#START WIFI_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef WIFI_RXISR_ERROR_CALLBACK
                WIFI_RXISR_ERROR_Callback();
            #endif /* WIFI_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & WIFI_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = WIFI_RXDATA_REG;
            #if (WIFI_RXHW_ADDRESS_ENABLED)
                if(WIFI_rxAddressMode == (uint8)WIFI__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & WIFI_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & WIFI_RX_STS_ADDR_MATCH) != 0u)
                        {
                            WIFI_rxAddressDetected = 1u;
                        }
                        else
                        {
                            WIFI_rxAddressDetected = 0u;
                        }
                    }
                    if(WIFI_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        WIFI_rxBuffer[WIFI_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    WIFI_rxBuffer[WIFI_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                WIFI_rxBuffer[WIFI_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (WIFI_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(WIFI_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        WIFI_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    WIFI_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(WIFI_rxBufferWrite >= WIFI_RX_BUFFER_SIZE)
                    {
                        WIFI_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(WIFI_rxBufferWrite == WIFI_rxBufferRead)
                    {
                        WIFI_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (WIFI_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            WIFI_RXSTATUS_MASK_REG  &= (uint8)~WIFI_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(WIFI_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (WIFI_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & WIFI_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START WIFI_RXISR_END` */

        /* `#END` */

    #ifdef WIFI_RXISR_EXIT_CALLBACK
        WIFI_RXISR_ExitCallback();
    #endif /* WIFI_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (WIFI_RX_INTERRUPT_ENABLED && (WIFI_RX_ENABLED || WIFI_HD_ENABLED)) */


#if (WIFI_TX_INTERRUPT_ENABLED && WIFI_TX_ENABLED)
    /*******************************************************************************
    * Function Name: WIFI_TXISR
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
    *  WIFI_txBuffer - RAM buffer pointer for transmit data from.
    *  WIFI_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  WIFI_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(WIFI_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef WIFI_TXISR_ENTRY_CALLBACK
        WIFI_TXISR_EntryCallback();
    #endif /* WIFI_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START WIFI_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((WIFI_txBufferRead != WIFI_txBufferWrite) &&
             ((WIFI_TXSTATUS_REG & WIFI_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(WIFI_txBufferRead >= WIFI_TX_BUFFER_SIZE)
            {
                WIFI_txBufferRead = 0u;
            }

            WIFI_TXDATA_REG = WIFI_txBuffer[WIFI_txBufferRead];

            /* Set next pointer */
            WIFI_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START WIFI_TXISR_END` */

        /* `#END` */

    #ifdef WIFI_TXISR_EXIT_CALLBACK
        WIFI_TXISR_ExitCallback();
    #endif /* WIFI_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (WIFI_TX_INTERRUPT_ENABLED && WIFI_TX_ENABLED) */


/* [] END OF FILE */
