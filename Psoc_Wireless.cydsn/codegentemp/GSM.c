/*******************************************************************************
* File Name: GSM.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "GSM.h"
#if (GSM_INTERNAL_CLOCK_USED)
    #include "GSM_IntClock.h"
#endif /* End GSM_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 GSM_initVar = 0u;

#if (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED)
    volatile uint8 GSM_txBuffer[GSM_TX_BUFFER_SIZE];
    volatile uint8 GSM_txBufferRead = 0u;
    uint8 GSM_txBufferWrite = 0u;
#endif /* (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED) */

#if (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED))
    uint8 GSM_errorStatus = 0u;
    volatile uint8 GSM_rxBuffer[GSM_RX_BUFFER_SIZE];
    volatile uint8 GSM_rxBufferRead  = 0u;
    volatile uint8 GSM_rxBufferWrite = 0u;
    volatile uint8 GSM_rxBufferLoopDetect = 0u;
    volatile uint8 GSM_rxBufferOverflow   = 0u;
    #if (GSM_RXHW_ADDRESS_ENABLED)
        volatile uint8 GSM_rxAddressMode = GSM_RX_ADDRESS_MODE;
        volatile uint8 GSM_rxAddressDetected = 0u;
    #endif /* (GSM_RXHW_ADDRESS_ENABLED) */
#endif /* (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED)) */


/*******************************************************************************
* Function Name: GSM_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  GSM_Start() sets the initVar variable, calls the
*  GSM_Init() function, and then calls the
*  GSM_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The GSM_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time GSM_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the GSM_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void GSM_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(GSM_initVar == 0u)
    {
        GSM_Init();
        GSM_initVar = 1u;
    }

    GSM_Enable();
}


/*******************************************************************************
* Function Name: GSM_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call GSM_Init() because
*  the GSM_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void GSM_Init(void) 
{
    #if(GSM_RX_ENABLED || GSM_HD_ENABLED)

        #if (GSM_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(GSM_RX_VECT_NUM, &GSM_RXISR);
            CyIntSetPriority(GSM_RX_VECT_NUM, GSM_RX_PRIOR_NUM);
            GSM_errorStatus = 0u;
        #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        #if (GSM_RXHW_ADDRESS_ENABLED)
            GSM_SetRxAddressMode(GSM_RX_ADDRESS_MODE);
            GSM_SetRxAddress1(GSM_RX_HW_ADDRESS1);
            GSM_SetRxAddress2(GSM_RX_HW_ADDRESS2);
        #endif /* End GSM_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        GSM_RXBITCTR_PERIOD_REG = GSM_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        GSM_RXSTATUS_MASK_REG  = GSM_INIT_RX_INTERRUPTS_MASK;
    #endif /* End GSM_RX_ENABLED || GSM_HD_ENABLED*/

    #if(GSM_TX_ENABLED)
        #if (GSM_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(GSM_TX_VECT_NUM, &GSM_TXISR);
            CyIntSetPriority(GSM_TX_VECT_NUM, GSM_TX_PRIOR_NUM);
        #endif /* (GSM_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (GSM_TXCLKGEN_DP)
            GSM_TXBITCLKGEN_CTR_REG = GSM_BIT_CENTER;
            GSM_TXBITCLKTX_COMPLETE_REG = ((GSM_NUMBER_OF_DATA_BITS +
                        GSM_NUMBER_OF_START_BIT) * GSM_OVER_SAMPLE_COUNT) - 1u;
        #else
            GSM_TXBITCTR_PERIOD_REG = ((GSM_NUMBER_OF_DATA_BITS +
                        GSM_NUMBER_OF_START_BIT) * GSM_OVER_SAMPLE_8) - 1u;
        #endif /* End GSM_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (GSM_TX_INTERRUPT_ENABLED)
            GSM_TXSTATUS_MASK_REG = GSM_TX_STS_FIFO_EMPTY;
        #else
            GSM_TXSTATUS_MASK_REG = GSM_INIT_TX_INTERRUPTS_MASK;
        #endif /*End GSM_TX_INTERRUPT_ENABLED*/

    #endif /* End GSM_TX_ENABLED */

    #if(GSM_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        GSM_WriteControlRegister( \
            (GSM_ReadControlRegister() & (uint8)~GSM_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(GSM_PARITY_TYPE << GSM_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End GSM_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: GSM_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call GSM_Enable() because the GSM_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  GSM_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void GSM_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (GSM_RX_ENABLED || GSM_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        GSM_RXBITCTR_CONTROL_REG |= GSM_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        GSM_RXSTATUS_ACTL_REG  |= GSM_INT_ENABLE;

        #if (GSM_RX_INTERRUPT_ENABLED)
            GSM_EnableRxInt();

            #if (GSM_RXHW_ADDRESS_ENABLED)
                GSM_rxAddressDetected = 0u;
            #endif /* (GSM_RXHW_ADDRESS_ENABLED) */
        #endif /* (GSM_RX_INTERRUPT_ENABLED) */
    #endif /* (GSM_RX_ENABLED || GSM_HD_ENABLED) */

    #if(GSM_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!GSM_TXCLKGEN_DP)
            GSM_TXBITCTR_CONTROL_REG |= GSM_CNTR_ENABLE;
        #endif /* End GSM_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        GSM_TXSTATUS_ACTL_REG |= GSM_INT_ENABLE;
        #if (GSM_TX_INTERRUPT_ENABLED)
            GSM_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            GSM_EnableTxInt();
        #endif /* (GSM_TX_INTERRUPT_ENABLED) */
     #endif /* (GSM_TX_INTERRUPT_ENABLED) */

    #if (GSM_INTERNAL_CLOCK_USED)
        GSM_IntClock_Start();  /* Enable the clock */
    #endif /* (GSM_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: GSM_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void GSM_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (GSM_RX_ENABLED || GSM_HD_ENABLED)
        GSM_RXBITCTR_CONTROL_REG &= (uint8) ~GSM_CNTR_ENABLE;
    #endif /* (GSM_RX_ENABLED || GSM_HD_ENABLED) */

    #if (GSM_TX_ENABLED)
        #if(!GSM_TXCLKGEN_DP)
            GSM_TXBITCTR_CONTROL_REG &= (uint8) ~GSM_CNTR_ENABLE;
        #endif /* (!GSM_TXCLKGEN_DP) */
    #endif /* (GSM_TX_ENABLED) */

    #if (GSM_INTERNAL_CLOCK_USED)
        GSM_IntClock_Stop();   /* Disable the clock */
    #endif /* (GSM_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (GSM_RX_ENABLED || GSM_HD_ENABLED)
        GSM_RXSTATUS_ACTL_REG  &= (uint8) ~GSM_INT_ENABLE;

        #if (GSM_RX_INTERRUPT_ENABLED)
            GSM_DisableRxInt();
        #endif /* (GSM_RX_INTERRUPT_ENABLED) */
    #endif /* (GSM_RX_ENABLED || GSM_HD_ENABLED) */

    #if (GSM_TX_ENABLED)
        GSM_TXSTATUS_ACTL_REG &= (uint8) ~GSM_INT_ENABLE;

        #if (GSM_TX_INTERRUPT_ENABLED)
            GSM_DisableTxInt();
        #endif /* (GSM_TX_INTERRUPT_ENABLED) */
    #endif /* (GSM_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: GSM_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 GSM_ReadControlRegister(void) 
{
    #if (GSM_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(GSM_CONTROL_REG);
    #endif /* (GSM_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: GSM_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  GSM_WriteControlRegister(uint8 control) 
{
    #if (GSM_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       GSM_CONTROL_REG = control;
    #endif /* (GSM_CONTROL_REG_REMOVED) */
}


#if(GSM_RX_ENABLED || GSM_HD_ENABLED)
    /*******************************************************************************
    * Function Name: GSM_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      GSM_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      GSM_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      GSM_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      GSM_RX_STS_BREAK            Interrupt on break.
    *      GSM_RX_STS_OVERRUN          Interrupt on overrun error.
    *      GSM_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      GSM_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void GSM_SetRxInterruptMode(uint8 intSrc) 
    {
        GSM_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: GSM_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  GSM_rxBuffer - RAM buffer pointer for save received data.
    *  GSM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  GSM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  GSM_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 GSM_ReadRxData(void) 
    {
        uint8 rxData;

    #if (GSM_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        GSM_DisableRxInt();

        locRxBufferRead  = GSM_rxBufferRead;
        locRxBufferWrite = GSM_rxBufferWrite;

        if( (GSM_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = GSM_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= GSM_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            GSM_rxBufferRead = locRxBufferRead;

            if(GSM_rxBufferLoopDetect != 0u)
            {
                GSM_rxBufferLoopDetect = 0u;
                #if ((GSM_RX_INTERRUPT_ENABLED) && (GSM_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( GSM_HD_ENABLED )
                        if((GSM_CONTROL_REG & GSM_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            GSM_RXSTATUS_MASK_REG  |= GSM_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        GSM_RXSTATUS_MASK_REG  |= GSM_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end GSM_HD_ENABLED */
                #endif /* ((GSM_RX_INTERRUPT_ENABLED) && (GSM_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = GSM_RXDATA_REG;
        }

        GSM_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = GSM_RXDATA_REG;

    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: GSM_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  GSM_RX_STS_FIFO_NOTEMPTY.
    *  GSM_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  GSM_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   GSM_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   GSM_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 GSM_ReadRxStatus(void) 
    {
        uint8 status;

        status = GSM_RXSTATUS_REG & GSM_RX_HW_MASK;

    #if (GSM_RX_INTERRUPT_ENABLED)
        if(GSM_rxBufferOverflow != 0u)
        {
            status |= GSM_RX_STS_SOFT_BUFF_OVER;
            GSM_rxBufferOverflow = 0u;
        }
    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: GSM_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. GSM_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  GSM_rxBuffer - RAM buffer pointer for save received data.
    *  GSM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  GSM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  GSM_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 GSM_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (GSM_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        GSM_DisableRxInt();

        locRxBufferRead  = GSM_rxBufferRead;
        locRxBufferWrite = GSM_rxBufferWrite;

        if( (GSM_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = GSM_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= GSM_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            GSM_rxBufferRead = locRxBufferRead;

            if(GSM_rxBufferLoopDetect != 0u)
            {
                GSM_rxBufferLoopDetect = 0u;
                #if( (GSM_RX_INTERRUPT_ENABLED) && (GSM_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( GSM_HD_ENABLED )
                        if((GSM_CONTROL_REG & GSM_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            GSM_RXSTATUS_MASK_REG |= GSM_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        GSM_RXSTATUS_MASK_REG |= GSM_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end GSM_HD_ENABLED */
                #endif /* GSM_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = GSM_RXSTATUS_REG;
            if((rxStatus & GSM_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = GSM_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (GSM_RX_STS_BREAK | GSM_RX_STS_PAR_ERROR |
                                GSM_RX_STS_STOP_ERROR | GSM_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        GSM_EnableRxInt();

    #else

        rxStatus =GSM_RXSTATUS_REG;
        if((rxStatus & GSM_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = GSM_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (GSM_RX_STS_BREAK | GSM_RX_STS_PAR_ERROR |
                            GSM_RX_STS_STOP_ERROR | GSM_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: GSM_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 GSM_GetByte(void) 
    {
        
    #if (GSM_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        GSM_DisableRxInt();
        locErrorStatus = (uint16)GSM_errorStatus;
        GSM_errorStatus = 0u;
        GSM_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | GSM_ReadRxData() );
    #else
        return ( ((uint16)GSM_ReadRxStatus() << 8u) | GSM_ReadRxData() );
    #endif /* GSM_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: GSM_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  GSM_rxBufferWrite - used to calculate left bytes.
    *  GSM_rxBufferRead - used to calculate left bytes.
    *  GSM_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 GSM_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (GSM_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        GSM_DisableRxInt();

        if(GSM_rxBufferRead == GSM_rxBufferWrite)
        {
            if(GSM_rxBufferLoopDetect != 0u)
            {
                size = GSM_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(GSM_rxBufferRead < GSM_rxBufferWrite)
        {
            size = (GSM_rxBufferWrite - GSM_rxBufferRead);
        }
        else
        {
            size = (GSM_RX_BUFFER_SIZE - GSM_rxBufferRead) + GSM_rxBufferWrite;
        }

        GSM_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((GSM_RXSTATUS_REG & GSM_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: GSM_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_rxBufferWrite - cleared to zero.
    *  GSM_rxBufferRead - cleared to zero.
    *  GSM_rxBufferLoopDetect - cleared to zero.
    *  GSM_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void GSM_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        GSM_RXDATA_AUX_CTL_REG |= (uint8)  GSM_RX_FIFO_CLR;
        GSM_RXDATA_AUX_CTL_REG &= (uint8) ~GSM_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (GSM_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        GSM_DisableRxInt();

        GSM_rxBufferRead = 0u;
        GSM_rxBufferWrite = 0u;
        GSM_rxBufferLoopDetect = 0u;
        GSM_rxBufferOverflow = 0u;

        GSM_EnableRxInt();

    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: GSM_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  GSM__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  GSM__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  GSM__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  GSM__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  GSM__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  GSM_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void GSM_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(GSM_RXHW_ADDRESS_ENABLED)
            #if(GSM_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* GSM_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = GSM_CONTROL_REG & (uint8)~GSM_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << GSM_CTRL_RXADDR_MODE0_SHIFT);
                GSM_CONTROL_REG = tmpCtrl;

                #if(GSM_RX_INTERRUPT_ENABLED && \
                   (GSM_RXBUFFERSIZE > GSM_FIFO_LENGTH) )
                    GSM_rxAddressMode = addressMode;
                    GSM_rxAddressDetected = 0u;
                #endif /* End GSM_RXBUFFERSIZE > GSM_FIFO_LENGTH*/
            #endif /* End GSM_CONTROL_REG_REMOVED */
        #else /* GSM_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End GSM_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: GSM_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void GSM_SetRxAddress1(uint8 address) 
    {
        GSM_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: GSM_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void GSM_SetRxAddress2(uint8 address) 
    {
        GSM_RXADDRESS2_REG = address;
    }

#endif  /* GSM_RX_ENABLED || GSM_HD_ENABLED*/


#if( (GSM_TX_ENABLED) || (GSM_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: GSM_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   GSM_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   GSM_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   GSM_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   GSM_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void GSM_SetTxInterruptMode(uint8 intSrc) 
    {
        GSM_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: GSM_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  GSM_txBuffer - RAM buffer pointer for save data for transmission
    *  GSM_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  GSM_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  GSM_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void GSM_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(GSM_initVar != 0u)
        {
        #if (GSM_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            GSM_DisableTxInt();

            if( (GSM_txBufferRead == GSM_txBufferWrite) &&
                ((GSM_TXSTATUS_REG & GSM_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                GSM_TXDATA_REG = txDataByte;
            }
            else
            {
                if(GSM_txBufferWrite >= GSM_TX_BUFFER_SIZE)
                {
                    GSM_txBufferWrite = 0u;
                }

                GSM_txBuffer[GSM_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                GSM_txBufferWrite++;
            }

            GSM_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            GSM_TXDATA_REG = txDataByte;

        #endif /*(GSM_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: GSM_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 GSM_ReadTxStatus(void) 
    {
        return(GSM_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: GSM_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_txBuffer - RAM buffer pointer for save data for transmission
    *  GSM_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  GSM_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  GSM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void GSM_PutChar(uint8 txDataByte) 
    {
    #if (GSM_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            GSM_DisableTxInt();
        #endif /* (GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = GSM_txBufferWrite;
            locTxBufferRead  = GSM_txBufferRead;

        #if ((GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            GSM_EnableTxInt();
        #endif /* (GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(GSM_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((GSM_TXSTATUS_REG & GSM_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            GSM_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= GSM_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            GSM_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3))
            GSM_DisableTxInt();
        #endif /* (GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            GSM_txBufferWrite = locTxBufferWrite;

        #if ((GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3))
            GSM_EnableTxInt();
        #endif /* (GSM_TX_BUFFER_SIZE > GSM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (GSM_TXSTATUS_REG & GSM_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                GSM_SetPendingTxInt();
            }
        }

    #else

        while((GSM_TXSTATUS_REG & GSM_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        GSM_TXDATA_REG = txDataByte;

    #endif /* GSM_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: GSM_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void GSM_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(GSM_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                GSM_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: GSM_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void GSM_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(GSM_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                GSM_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: GSM_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void GSM_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(GSM_initVar != 0u)
        {
            GSM_PutChar(txDataByte);
            GSM_PutChar(0x0Du);
            GSM_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: GSM_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  GSM_txBufferWrite - used to calculate left space.
    *  GSM_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 GSM_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (GSM_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        GSM_DisableTxInt();

        if(GSM_txBufferRead == GSM_txBufferWrite)
        {
            size = 0u;
        }
        else if(GSM_txBufferRead < GSM_txBufferWrite)
        {
            size = (GSM_txBufferWrite - GSM_txBufferRead);
        }
        else
        {
            size = (GSM_TX_BUFFER_SIZE - GSM_txBufferRead) +
                    GSM_txBufferWrite;
        }

        GSM_EnableTxInt();

    #else

        size = GSM_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & GSM_TX_STS_FIFO_FULL) != 0u)
        {
            size = GSM_FIFO_LENGTH;
        }
        else if((size & GSM_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (GSM_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: GSM_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_txBufferWrite - cleared to zero.
    *  GSM_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void GSM_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        GSM_TXDATA_AUX_CTL_REG |= (uint8)  GSM_TX_FIFO_CLR;
        GSM_TXDATA_AUX_CTL_REG &= (uint8) ~GSM_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (GSM_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        GSM_DisableTxInt();

        GSM_txBufferRead = 0u;
        GSM_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        GSM_EnableTxInt();

    #endif /* (GSM_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: GSM_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   GSM_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   GSM_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   GSM_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   GSM_SEND_WAIT_REINIT - Performs both options: 
    *      GSM_SEND_BREAK and GSM_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  GSM_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with GSM_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The GSM_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void GSM_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(GSM_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(GSM_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == GSM_SEND_BREAK) ||
                (retMode == GSM_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                GSM_WriteControlRegister(GSM_ReadControlRegister() |
                                                      GSM_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                GSM_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = GSM_TXSTATUS_REG;
                }
                while((tmpStat & GSM_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == GSM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == GSM_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = GSM_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & GSM_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == GSM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == GSM_REINIT) ||
                (retMode == GSM_SEND_WAIT_REINIT) )
            {
                GSM_WriteControlRegister(GSM_ReadControlRegister() &
                                              (uint8)~GSM_CTRL_HD_SEND_BREAK);
            }

        #else /* GSM_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == GSM_SEND_BREAK) ||
                (retMode == GSM_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (GSM_PARITY_TYPE != GSM__B_UART__NONE_REVB) || \
                                    (GSM_PARITY_TYPE_SW != 0u) )
                    GSM_WriteControlRegister(GSM_ReadControlRegister() |
                                                          GSM_CTRL_HD_SEND_BREAK);
                #endif /* End GSM_PARITY_TYPE != GSM__B_UART__NONE_REVB  */

                #if(GSM_TXCLKGEN_DP)
                    txPeriod = GSM_TXBITCLKTX_COMPLETE_REG;
                    GSM_TXBITCLKTX_COMPLETE_REG = GSM_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = GSM_TXBITCTR_PERIOD_REG;
                    GSM_TXBITCTR_PERIOD_REG = GSM_TXBITCTR_BREAKBITS8X;
                #endif /* End GSM_TXCLKGEN_DP */

                /* Send zeros */
                GSM_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = GSM_TXSTATUS_REG;
                }
                while((tmpStat & GSM_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == GSM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == GSM_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = GSM_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & GSM_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == GSM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == GSM_REINIT) ||
                (retMode == GSM_SEND_WAIT_REINIT) )
            {

            #if(GSM_TXCLKGEN_DP)
                GSM_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                GSM_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End GSM_TXCLKGEN_DP */

            #if( (GSM_PARITY_TYPE != GSM__B_UART__NONE_REVB) || \
                 (GSM_PARITY_TYPE_SW != 0u) )
                GSM_WriteControlRegister(GSM_ReadControlRegister() &
                                                      (uint8) ~GSM_CTRL_HD_SEND_BREAK);
            #endif /* End GSM_PARITY_TYPE != NONE */
            }
        #endif    /* End GSM_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: GSM_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       GSM_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       GSM_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears GSM_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void GSM_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( GSM_CONTROL_REG_REMOVED == 0u )
            GSM_WriteControlRegister(GSM_ReadControlRegister() |
                                                  GSM_CTRL_MARK);
        #endif /* End GSM_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( GSM_CONTROL_REG_REMOVED == 0u )
            GSM_WriteControlRegister(GSM_ReadControlRegister() &
                                                  (uint8) ~GSM_CTRL_MARK);
        #endif /* End GSM_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndGSM_TX_ENABLED */

#if(GSM_HD_ENABLED)


    /*******************************************************************************
    * Function Name: GSM_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void GSM_LoadRxConfig(void) 
    {
        GSM_WriteControlRegister(GSM_ReadControlRegister() &
                                                (uint8)~GSM_CTRL_HD_SEND);
        GSM_RXBITCTR_PERIOD_REG = GSM_HD_RXBITCTR_INIT;

    #if (GSM_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        GSM_SetRxInterruptMode(GSM_INIT_RX_INTERRUPTS_MASK);
    #endif /* (GSM_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: GSM_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void GSM_LoadTxConfig(void) 
    {
    #if (GSM_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        GSM_SetRxInterruptMode(0u);
    #endif /* (GSM_RX_INTERRUPT_ENABLED) */

        GSM_WriteControlRegister(GSM_ReadControlRegister() | GSM_CTRL_HD_SEND);
        GSM_RXBITCTR_PERIOD_REG = GSM_HD_TXBITCTR_INIT;
    }

#endif  /* GSM_HD_ENABLED */


/* [] END OF FILE */
