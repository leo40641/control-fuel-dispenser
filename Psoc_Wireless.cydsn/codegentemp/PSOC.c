/*******************************************************************************
* File Name: PSOC.c
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

#include "PSOC.h"
#if (PSOC_INTERNAL_CLOCK_USED)
    #include "PSOC_IntClock.h"
#endif /* End PSOC_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 PSOC_initVar = 0u;

#if (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED)
    volatile uint8 PSOC_txBuffer[PSOC_TX_BUFFER_SIZE];
    volatile uint8 PSOC_txBufferRead = 0u;
    uint8 PSOC_txBufferWrite = 0u;
#endif /* (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED) */

#if (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED))
    uint8 PSOC_errorStatus = 0u;
    volatile uint8 PSOC_rxBuffer[PSOC_RX_BUFFER_SIZE];
    volatile uint16 PSOC_rxBufferRead  = 0u;
    volatile uint16 PSOC_rxBufferWrite = 0u;
    volatile uint8 PSOC_rxBufferLoopDetect = 0u;
    volatile uint8 PSOC_rxBufferOverflow   = 0u;
    #if (PSOC_RXHW_ADDRESS_ENABLED)
        volatile uint8 PSOC_rxAddressMode = PSOC_RX_ADDRESS_MODE;
        volatile uint8 PSOC_rxAddressDetected = 0u;
    #endif /* (PSOC_RXHW_ADDRESS_ENABLED) */
#endif /* (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED)) */


/*******************************************************************************
* Function Name: PSOC_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  PSOC_Start() sets the initVar variable, calls the
*  PSOC_Init() function, and then calls the
*  PSOC_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The PSOC_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time PSOC_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the PSOC_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PSOC_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(PSOC_initVar == 0u)
    {
        PSOC_Init();
        PSOC_initVar = 1u;
    }

    PSOC_Enable();
}


/*******************************************************************************
* Function Name: PSOC_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call PSOC_Init() because
*  the PSOC_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PSOC_Init(void) 
{
    #if(PSOC_RX_ENABLED || PSOC_HD_ENABLED)

        #if (PSOC_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(PSOC_RX_VECT_NUM, &PSOC_RXISR);
            CyIntSetPriority(PSOC_RX_VECT_NUM, PSOC_RX_PRIOR_NUM);
            PSOC_errorStatus = 0u;
        #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        #if (PSOC_RXHW_ADDRESS_ENABLED)
            PSOC_SetRxAddressMode(PSOC_RX_ADDRESS_MODE);
            PSOC_SetRxAddress1(PSOC_RX_HW_ADDRESS1);
            PSOC_SetRxAddress2(PSOC_RX_HW_ADDRESS2);
        #endif /* End PSOC_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        PSOC_RXBITCTR_PERIOD_REG = PSOC_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        PSOC_RXSTATUS_MASK_REG  = PSOC_INIT_RX_INTERRUPTS_MASK;
    #endif /* End PSOC_RX_ENABLED || PSOC_HD_ENABLED*/

    #if(PSOC_TX_ENABLED)
        #if (PSOC_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(PSOC_TX_VECT_NUM, &PSOC_TXISR);
            CyIntSetPriority(PSOC_TX_VECT_NUM, PSOC_TX_PRIOR_NUM);
        #endif /* (PSOC_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (PSOC_TXCLKGEN_DP)
            PSOC_TXBITCLKGEN_CTR_REG = PSOC_BIT_CENTER;
            PSOC_TXBITCLKTX_COMPLETE_REG = ((PSOC_NUMBER_OF_DATA_BITS +
                        PSOC_NUMBER_OF_START_BIT) * PSOC_OVER_SAMPLE_COUNT) - 1u;
        #else
            PSOC_TXBITCTR_PERIOD_REG = ((PSOC_NUMBER_OF_DATA_BITS +
                        PSOC_NUMBER_OF_START_BIT) * PSOC_OVER_SAMPLE_8) - 1u;
        #endif /* End PSOC_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (PSOC_TX_INTERRUPT_ENABLED)
            PSOC_TXSTATUS_MASK_REG = PSOC_TX_STS_FIFO_EMPTY;
        #else
            PSOC_TXSTATUS_MASK_REG = PSOC_INIT_TX_INTERRUPTS_MASK;
        #endif /*End PSOC_TX_INTERRUPT_ENABLED*/

    #endif /* End PSOC_TX_ENABLED */

    #if(PSOC_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        PSOC_WriteControlRegister( \
            (PSOC_ReadControlRegister() & (uint8)~PSOC_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(PSOC_PARITY_TYPE << PSOC_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End PSOC_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: PSOC_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call PSOC_Enable() because the PSOC_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PSOC_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void PSOC_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (PSOC_RX_ENABLED || PSOC_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        PSOC_RXBITCTR_CONTROL_REG |= PSOC_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        PSOC_RXSTATUS_ACTL_REG  |= PSOC_INT_ENABLE;

        #if (PSOC_RX_INTERRUPT_ENABLED)
            PSOC_EnableRxInt();

            #if (PSOC_RXHW_ADDRESS_ENABLED)
                PSOC_rxAddressDetected = 0u;
            #endif /* (PSOC_RXHW_ADDRESS_ENABLED) */
        #endif /* (PSOC_RX_INTERRUPT_ENABLED) */
    #endif /* (PSOC_RX_ENABLED || PSOC_HD_ENABLED) */

    #if(PSOC_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!PSOC_TXCLKGEN_DP)
            PSOC_TXBITCTR_CONTROL_REG |= PSOC_CNTR_ENABLE;
        #endif /* End PSOC_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        PSOC_TXSTATUS_ACTL_REG |= PSOC_INT_ENABLE;
        #if (PSOC_TX_INTERRUPT_ENABLED)
            PSOC_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            PSOC_EnableTxInt();
        #endif /* (PSOC_TX_INTERRUPT_ENABLED) */
     #endif /* (PSOC_TX_INTERRUPT_ENABLED) */

    #if (PSOC_INTERNAL_CLOCK_USED)
        PSOC_IntClock_Start();  /* Enable the clock */
    #endif /* (PSOC_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: PSOC_Stop
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
void PSOC_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (PSOC_RX_ENABLED || PSOC_HD_ENABLED)
        PSOC_RXBITCTR_CONTROL_REG &= (uint8) ~PSOC_CNTR_ENABLE;
    #endif /* (PSOC_RX_ENABLED || PSOC_HD_ENABLED) */

    #if (PSOC_TX_ENABLED)
        #if(!PSOC_TXCLKGEN_DP)
            PSOC_TXBITCTR_CONTROL_REG &= (uint8) ~PSOC_CNTR_ENABLE;
        #endif /* (!PSOC_TXCLKGEN_DP) */
    #endif /* (PSOC_TX_ENABLED) */

    #if (PSOC_INTERNAL_CLOCK_USED)
        PSOC_IntClock_Stop();   /* Disable the clock */
    #endif /* (PSOC_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (PSOC_RX_ENABLED || PSOC_HD_ENABLED)
        PSOC_RXSTATUS_ACTL_REG  &= (uint8) ~PSOC_INT_ENABLE;

        #if (PSOC_RX_INTERRUPT_ENABLED)
            PSOC_DisableRxInt();
        #endif /* (PSOC_RX_INTERRUPT_ENABLED) */
    #endif /* (PSOC_RX_ENABLED || PSOC_HD_ENABLED) */

    #if (PSOC_TX_ENABLED)
        PSOC_TXSTATUS_ACTL_REG &= (uint8) ~PSOC_INT_ENABLE;

        #if (PSOC_TX_INTERRUPT_ENABLED)
            PSOC_DisableTxInt();
        #endif /* (PSOC_TX_INTERRUPT_ENABLED) */
    #endif /* (PSOC_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: PSOC_ReadControlRegister
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
uint8 PSOC_ReadControlRegister(void) 
{
    #if (PSOC_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(PSOC_CONTROL_REG);
    #endif /* (PSOC_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: PSOC_WriteControlRegister
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
void  PSOC_WriteControlRegister(uint8 control) 
{
    #if (PSOC_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       PSOC_CONTROL_REG = control;
    #endif /* (PSOC_CONTROL_REG_REMOVED) */
}


#if(PSOC_RX_ENABLED || PSOC_HD_ENABLED)
    /*******************************************************************************
    * Function Name: PSOC_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      PSOC_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      PSOC_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      PSOC_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      PSOC_RX_STS_BREAK            Interrupt on break.
    *      PSOC_RX_STS_OVERRUN          Interrupt on overrun error.
    *      PSOC_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      PSOC_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void PSOC_SetRxInterruptMode(uint8 intSrc) 
    {
        PSOC_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: PSOC_ReadRxData
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
    *  PSOC_rxBuffer - RAM buffer pointer for save received data.
    *  PSOC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  PSOC_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  PSOC_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 PSOC_ReadRxData(void) 
    {
        uint8 rxData;

    #if (PSOC_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        PSOC_DisableRxInt();

        locRxBufferRead  = PSOC_rxBufferRead;
        locRxBufferWrite = PSOC_rxBufferWrite;

        if( (PSOC_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = PSOC_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= PSOC_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            PSOC_rxBufferRead = locRxBufferRead;

            if(PSOC_rxBufferLoopDetect != 0u)
            {
                PSOC_rxBufferLoopDetect = 0u;
                #if ((PSOC_RX_INTERRUPT_ENABLED) && (PSOC_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( PSOC_HD_ENABLED )
                        if((PSOC_CONTROL_REG & PSOC_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            PSOC_RXSTATUS_MASK_REG  |= PSOC_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        PSOC_RXSTATUS_MASK_REG  |= PSOC_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end PSOC_HD_ENABLED */
                #endif /* ((PSOC_RX_INTERRUPT_ENABLED) && (PSOC_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = PSOC_RXDATA_REG;
        }

        PSOC_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = PSOC_RXDATA_REG;

    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: PSOC_ReadRxStatus
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
    *  PSOC_RX_STS_FIFO_NOTEMPTY.
    *  PSOC_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  PSOC_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   PSOC_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   PSOC_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 PSOC_ReadRxStatus(void) 
    {
        uint8 status;

        status = PSOC_RXSTATUS_REG & PSOC_RX_HW_MASK;

    #if (PSOC_RX_INTERRUPT_ENABLED)
        if(PSOC_rxBufferOverflow != 0u)
        {
            status |= PSOC_RX_STS_SOFT_BUFF_OVER;
            PSOC_rxBufferOverflow = 0u;
        }
    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: PSOC_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. PSOC_GetChar() is
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
    *  PSOC_rxBuffer - RAM buffer pointer for save received data.
    *  PSOC_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  PSOC_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  PSOC_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 PSOC_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (PSOC_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        PSOC_DisableRxInt();

        locRxBufferRead  = PSOC_rxBufferRead;
        locRxBufferWrite = PSOC_rxBufferWrite;

        if( (PSOC_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = PSOC_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= PSOC_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            PSOC_rxBufferRead = locRxBufferRead;

            if(PSOC_rxBufferLoopDetect != 0u)
            {
                PSOC_rxBufferLoopDetect = 0u;
                #if( (PSOC_RX_INTERRUPT_ENABLED) && (PSOC_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( PSOC_HD_ENABLED )
                        if((PSOC_CONTROL_REG & PSOC_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            PSOC_RXSTATUS_MASK_REG |= PSOC_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        PSOC_RXSTATUS_MASK_REG |= PSOC_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end PSOC_HD_ENABLED */
                #endif /* PSOC_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = PSOC_RXSTATUS_REG;
            if((rxStatus & PSOC_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = PSOC_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (PSOC_RX_STS_BREAK | PSOC_RX_STS_PAR_ERROR |
                                PSOC_RX_STS_STOP_ERROR | PSOC_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        PSOC_EnableRxInt();

    #else

        rxStatus =PSOC_RXSTATUS_REG;
        if((rxStatus & PSOC_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = PSOC_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (PSOC_RX_STS_BREAK | PSOC_RX_STS_PAR_ERROR |
                            PSOC_RX_STS_STOP_ERROR | PSOC_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: PSOC_GetByte
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
    uint16 PSOC_GetByte(void) 
    {
        
    #if (PSOC_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        PSOC_DisableRxInt();
        locErrorStatus = (uint16)PSOC_errorStatus;
        PSOC_errorStatus = 0u;
        PSOC_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | PSOC_ReadRxData() );
    #else
        return ( ((uint16)PSOC_ReadRxStatus() << 8u) | PSOC_ReadRxData() );
    #endif /* PSOC_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: PSOC_GetRxBufferSize
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
    *  uint16: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  PSOC_rxBufferWrite - used to calculate left bytes.
    *  PSOC_rxBufferRead - used to calculate left bytes.
    *  PSOC_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 PSOC_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (PSOC_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        PSOC_DisableRxInt();

        if(PSOC_rxBufferRead == PSOC_rxBufferWrite)
        {
            if(PSOC_rxBufferLoopDetect != 0u)
            {
                size = PSOC_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(PSOC_rxBufferRead < PSOC_rxBufferWrite)
        {
            size = (PSOC_rxBufferWrite - PSOC_rxBufferRead);
        }
        else
        {
            size = (PSOC_RX_BUFFER_SIZE - PSOC_rxBufferRead) + PSOC_rxBufferWrite;
        }

        PSOC_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((PSOC_RXSTATUS_REG & PSOC_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: PSOC_ClearRxBuffer
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
    *  PSOC_rxBufferWrite - cleared to zero.
    *  PSOC_rxBufferRead - cleared to zero.
    *  PSOC_rxBufferLoopDetect - cleared to zero.
    *  PSOC_rxBufferOverflow - cleared to zero.
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
    void PSOC_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        PSOC_RXDATA_AUX_CTL_REG |= (uint8)  PSOC_RX_FIFO_CLR;
        PSOC_RXDATA_AUX_CTL_REG &= (uint8) ~PSOC_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (PSOC_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PSOC_DisableRxInt();

        PSOC_rxBufferRead = 0u;
        PSOC_rxBufferWrite = 0u;
        PSOC_rxBufferLoopDetect = 0u;
        PSOC_rxBufferOverflow = 0u;

        PSOC_EnableRxInt();

    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: PSOC_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  PSOC__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  PSOC__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  PSOC__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  PSOC__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  PSOC__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  PSOC_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  PSOC_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void PSOC_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(PSOC_RXHW_ADDRESS_ENABLED)
            #if(PSOC_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* PSOC_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = PSOC_CONTROL_REG & (uint8)~PSOC_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << PSOC_CTRL_RXADDR_MODE0_SHIFT);
                PSOC_CONTROL_REG = tmpCtrl;

                #if(PSOC_RX_INTERRUPT_ENABLED && \
                   (PSOC_RXBUFFERSIZE > PSOC_FIFO_LENGTH) )
                    PSOC_rxAddressMode = addressMode;
                    PSOC_rxAddressDetected = 0u;
                #endif /* End PSOC_RXBUFFERSIZE > PSOC_FIFO_LENGTH*/
            #endif /* End PSOC_CONTROL_REG_REMOVED */
        #else /* PSOC_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End PSOC_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: PSOC_SetRxAddress1
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
    void PSOC_SetRxAddress1(uint8 address) 
    {
        PSOC_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: PSOC_SetRxAddress2
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
    void PSOC_SetRxAddress2(uint8 address) 
    {
        PSOC_RXADDRESS2_REG = address;
    }

#endif  /* PSOC_RX_ENABLED || PSOC_HD_ENABLED*/


#if( (PSOC_TX_ENABLED) || (PSOC_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: PSOC_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   PSOC_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   PSOC_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   PSOC_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   PSOC_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void PSOC_SetTxInterruptMode(uint8 intSrc) 
    {
        PSOC_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: PSOC_WriteTxData
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
    *  PSOC_txBuffer - RAM buffer pointer for save data for transmission
    *  PSOC_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  PSOC_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  PSOC_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void PSOC_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(PSOC_initVar != 0u)
        {
        #if (PSOC_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            PSOC_DisableTxInt();

            if( (PSOC_txBufferRead == PSOC_txBufferWrite) &&
                ((PSOC_TXSTATUS_REG & PSOC_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                PSOC_TXDATA_REG = txDataByte;
            }
            else
            {
                if(PSOC_txBufferWrite >= PSOC_TX_BUFFER_SIZE)
                {
                    PSOC_txBufferWrite = 0u;
                }

                PSOC_txBuffer[PSOC_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                PSOC_txBufferWrite++;
            }

            PSOC_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            PSOC_TXDATA_REG = txDataByte;

        #endif /*(PSOC_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: PSOC_ReadTxStatus
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
    uint8 PSOC_ReadTxStatus(void) 
    {
        return(PSOC_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: PSOC_PutChar
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
    *  PSOC_txBuffer - RAM buffer pointer for save data for transmission
    *  PSOC_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  PSOC_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  PSOC_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void PSOC_PutChar(uint8 txDataByte) 
    {
    #if (PSOC_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            PSOC_DisableTxInt();
        #endif /* (PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = PSOC_txBufferWrite;
            locTxBufferRead  = PSOC_txBufferRead;

        #if ((PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            PSOC_EnableTxInt();
        #endif /* (PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(PSOC_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((PSOC_TXSTATUS_REG & PSOC_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            PSOC_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= PSOC_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            PSOC_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3))
            PSOC_DisableTxInt();
        #endif /* (PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            PSOC_txBufferWrite = locTxBufferWrite;

        #if ((PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3))
            PSOC_EnableTxInt();
        #endif /* (PSOC_TX_BUFFER_SIZE > PSOC_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (PSOC_TXSTATUS_REG & PSOC_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                PSOC_SetPendingTxInt();
            }
        }

    #else

        while((PSOC_TXSTATUS_REG & PSOC_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        PSOC_TXDATA_REG = txDataByte;

    #endif /* PSOC_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: PSOC_PutString
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
    *  PSOC_initVar - checked to identify that the component has been
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
    void PSOC_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(PSOC_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                PSOC_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: PSOC_PutArray
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
    *  PSOC_initVar - checked to identify that the component has been
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
    void PSOC_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(PSOC_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                PSOC_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: PSOC_PutCRLF
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
    *  PSOC_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void PSOC_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(PSOC_initVar != 0u)
        {
            PSOC_PutChar(txDataByte);
            PSOC_PutChar(0x0Du);
            PSOC_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: PSOC_GetTxBufferSize
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
    *  PSOC_txBufferWrite - used to calculate left space.
    *  PSOC_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 PSOC_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (PSOC_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PSOC_DisableTxInt();

        if(PSOC_txBufferRead == PSOC_txBufferWrite)
        {
            size = 0u;
        }
        else if(PSOC_txBufferRead < PSOC_txBufferWrite)
        {
            size = (PSOC_txBufferWrite - PSOC_txBufferRead);
        }
        else
        {
            size = (PSOC_TX_BUFFER_SIZE - PSOC_txBufferRead) +
                    PSOC_txBufferWrite;
        }

        PSOC_EnableTxInt();

    #else

        size = PSOC_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & PSOC_TX_STS_FIFO_FULL) != 0u)
        {
            size = PSOC_FIFO_LENGTH;
        }
        else if((size & PSOC_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (PSOC_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: PSOC_ClearTxBuffer
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
    *  PSOC_txBufferWrite - cleared to zero.
    *  PSOC_txBufferRead - cleared to zero.
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
    void PSOC_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        PSOC_TXDATA_AUX_CTL_REG |= (uint8)  PSOC_TX_FIFO_CLR;
        PSOC_TXDATA_AUX_CTL_REG &= (uint8) ~PSOC_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (PSOC_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        PSOC_DisableTxInt();

        PSOC_txBufferRead = 0u;
        PSOC_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        PSOC_EnableTxInt();

    #endif /* (PSOC_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: PSOC_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   PSOC_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   PSOC_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   PSOC_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   PSOC_SEND_WAIT_REINIT - Performs both options: 
    *      PSOC_SEND_BREAK and PSOC_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  PSOC_initVar - checked to identify that the component has been
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
    *     When interrupt appear with PSOC_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The PSOC_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void PSOC_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(PSOC_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(PSOC_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == PSOC_SEND_BREAK) ||
                (retMode == PSOC_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                PSOC_WriteControlRegister(PSOC_ReadControlRegister() |
                                                      PSOC_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                PSOC_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = PSOC_TXSTATUS_REG;
                }
                while((tmpStat & PSOC_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == PSOC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PSOC_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = PSOC_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & PSOC_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == PSOC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PSOC_REINIT) ||
                (retMode == PSOC_SEND_WAIT_REINIT) )
            {
                PSOC_WriteControlRegister(PSOC_ReadControlRegister() &
                                              (uint8)~PSOC_CTRL_HD_SEND_BREAK);
            }

        #else /* PSOC_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == PSOC_SEND_BREAK) ||
                (retMode == PSOC_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (PSOC_PARITY_TYPE != PSOC__B_UART__NONE_REVB) || \
                                    (PSOC_PARITY_TYPE_SW != 0u) )
                    PSOC_WriteControlRegister(PSOC_ReadControlRegister() |
                                                          PSOC_CTRL_HD_SEND_BREAK);
                #endif /* End PSOC_PARITY_TYPE != PSOC__B_UART__NONE_REVB  */

                #if(PSOC_TXCLKGEN_DP)
                    txPeriod = PSOC_TXBITCLKTX_COMPLETE_REG;
                    PSOC_TXBITCLKTX_COMPLETE_REG = PSOC_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = PSOC_TXBITCTR_PERIOD_REG;
                    PSOC_TXBITCTR_PERIOD_REG = PSOC_TXBITCTR_BREAKBITS8X;
                #endif /* End PSOC_TXCLKGEN_DP */

                /* Send zeros */
                PSOC_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = PSOC_TXSTATUS_REG;
                }
                while((tmpStat & PSOC_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == PSOC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PSOC_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = PSOC_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & PSOC_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == PSOC_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == PSOC_REINIT) ||
                (retMode == PSOC_SEND_WAIT_REINIT) )
            {

            #if(PSOC_TXCLKGEN_DP)
                PSOC_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                PSOC_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End PSOC_TXCLKGEN_DP */

            #if( (PSOC_PARITY_TYPE != PSOC__B_UART__NONE_REVB) || \
                 (PSOC_PARITY_TYPE_SW != 0u) )
                PSOC_WriteControlRegister(PSOC_ReadControlRegister() &
                                                      (uint8) ~PSOC_CTRL_HD_SEND_BREAK);
            #endif /* End PSOC_PARITY_TYPE != NONE */
            }
        #endif    /* End PSOC_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: PSOC_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       PSOC_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       PSOC_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears PSOC_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void PSOC_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( PSOC_CONTROL_REG_REMOVED == 0u )
            PSOC_WriteControlRegister(PSOC_ReadControlRegister() |
                                                  PSOC_CTRL_MARK);
        #endif /* End PSOC_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( PSOC_CONTROL_REG_REMOVED == 0u )
            PSOC_WriteControlRegister(PSOC_ReadControlRegister() &
                                                  (uint8) ~PSOC_CTRL_MARK);
        #endif /* End PSOC_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndPSOC_TX_ENABLED */

#if(PSOC_HD_ENABLED)


    /*******************************************************************************
    * Function Name: PSOC_LoadRxConfig
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
    void PSOC_LoadRxConfig(void) 
    {
        PSOC_WriteControlRegister(PSOC_ReadControlRegister() &
                                                (uint8)~PSOC_CTRL_HD_SEND);
        PSOC_RXBITCTR_PERIOD_REG = PSOC_HD_RXBITCTR_INIT;

    #if (PSOC_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        PSOC_SetRxInterruptMode(PSOC_INIT_RX_INTERRUPTS_MASK);
    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: PSOC_LoadTxConfig
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
    void PSOC_LoadTxConfig(void) 
    {
    #if (PSOC_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        PSOC_SetRxInterruptMode(0u);
    #endif /* (PSOC_RX_INTERRUPT_ENABLED) */

        PSOC_WriteControlRegister(PSOC_ReadControlRegister() | PSOC_CTRL_HD_SEND);
        PSOC_RXBITCTR_PERIOD_REG = PSOC_HD_TXBITCTR_INIT;
    }

#endif  /* PSOC_HD_ENABLED */


/* [] END OF FILE */
