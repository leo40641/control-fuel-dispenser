/*******************************************************************************
* File Name: GSMINT.c
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

#include "GSM.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED))
    /*******************************************************************************
    * Function Name: GSM_RXISR
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
    *  GSM_rxBuffer - RAM buffer pointer for save received data.
    *  GSM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  GSM_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  GSM_rxBufferOverflow - software overflow flag. Set to one
    *     when GSM_rxBufferWrite index overtakes
    *     GSM_rxBufferRead index.
    *  GSM_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when GSM_rxBufferWrite is equal to
    *    GSM_rxBufferRead
    *  GSM_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  GSM_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(GSM_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef GSM_RXISR_ENTRY_CALLBACK
        GSM_RXISR_EntryCallback();
    #endif /* GSM_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START GSM_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = GSM_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in GSM_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (GSM_RX_STS_BREAK | 
                            GSM_RX_STS_PAR_ERROR |
                            GSM_RX_STS_STOP_ERROR | 
                            GSM_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                GSM_errorStatus |= readStatus & ( GSM_RX_STS_BREAK | 
                                                            GSM_RX_STS_PAR_ERROR | 
                                                            GSM_RX_STS_STOP_ERROR | 
                                                            GSM_RX_STS_OVERRUN);
                /* `#START GSM_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef GSM_RXISR_ERROR_CALLBACK
                GSM_RXISR_ERROR_Callback();
            #endif /* GSM_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & GSM_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = GSM_RXDATA_REG;
            #if (GSM_RXHW_ADDRESS_ENABLED)
                if(GSM_rxAddressMode == (uint8)GSM__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & GSM_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & GSM_RX_STS_ADDR_MATCH) != 0u)
                        {
                            GSM_rxAddressDetected = 1u;
                        }
                        else
                        {
                            GSM_rxAddressDetected = 0u;
                        }
                    }
                    if(GSM_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        GSM_rxBuffer[GSM_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    GSM_rxBuffer[GSM_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                GSM_rxBuffer[GSM_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (GSM_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(GSM_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        GSM_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    GSM_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(GSM_rxBufferWrite >= GSM_RX_BUFFER_SIZE)
                    {
                        GSM_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(GSM_rxBufferWrite == GSM_rxBufferRead)
                    {
                        GSM_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (GSM_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            GSM_RXSTATUS_MASK_REG  &= (uint8)~GSM_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(GSM_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (GSM_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & GSM_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START GSM_RXISR_END` */

        /* `#END` */

    #ifdef GSM_RXISR_EXIT_CALLBACK
        GSM_RXISR_ExitCallback();
    #endif /* GSM_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED)) */


#if (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED)
    /*******************************************************************************
    * Function Name: GSM_TXISR
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
    *  GSM_txBuffer - RAM buffer pointer for transmit data from.
    *  GSM_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  GSM_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(GSM_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef GSM_TXISR_ENTRY_CALLBACK
        GSM_TXISR_EntryCallback();
    #endif /* GSM_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START GSM_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((GSM_txBufferRead != GSM_txBufferWrite) &&
             ((GSM_TXSTATUS_REG & GSM_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(GSM_txBufferRead >= GSM_TX_BUFFER_SIZE)
            {
                GSM_txBufferRead = 0u;
            }

            GSM_TXDATA_REG = GSM_txBuffer[GSM_txBufferRead];

            /* Set next pointer */
            GSM_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START GSM_TXISR_END` */

        /* `#END` */

    #ifdef GSM_TXISR_EXIT_CALLBACK
        GSM_TXISR_ExitCallback();
    #endif /* GSM_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED) */


/* [] END OF FILE */
