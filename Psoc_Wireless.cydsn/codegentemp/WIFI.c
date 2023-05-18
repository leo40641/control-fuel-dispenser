/*******************************************************************************
* File Name: WIFI.c
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

#include "WIFI.h"
#if (WIFI_INTERNAL_CLOCK_USED)
    #include "WIFI_IntClock.h"
#endif /* End WIFI_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 WIFI_initVar = 0u;

#if (WIFI_TX_INTERRUPT_ENABLED && WIFI_TX_ENABLED)
    volatile uint8 WIFI_txBuffer[WIFI_TX_BUFFER_SIZE];
    volatile uint8 WIFI_txBufferRead = 0u;
    uint8 WIFI_txBufferWrite = 0u;
#endif /* (WIFI_TX_INTERRUPT_ENABLED && WIFI_TX_ENABLED) */

#if (WIFI_RX_INTERRUPT_ENABLED && (WIFI_RX_ENABLED || WIFI_HD_ENABLED))
    uint8 WIFI_errorStatus = 0u;
    volatile uint8 WIFI_rxBuffer[WIFI_RX_BUFFER_SIZE];
    volatile uint8 WIFI_rxBufferRead  = 0u;
    volatile uint8 WIFI_rxBufferWrite = 0u;
    volatile uint8 WIFI_rxBufferLoopDetect = 0u;
    volatile uint8 WIFI_rxBufferOverflow   = 0u;
    #if (WIFI_RXHW_ADDRESS_ENABLED)
        volatile uint8 WIFI_rxAddressMode = WIFI_RX_ADDRESS_MODE;
        volatile uint8 WIFI_rxAddressDetected = 0u;
    #endif /* (WIFI_RXHW_ADDRESS_ENABLED) */
#endif /* (WIFI_RX_INTERRUPT_ENABLED && (WIFI_RX_ENABLED || WIFI_HD_ENABLED)) */


/*******************************************************************************
* Function Name: WIFI_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  WIFI_Start() sets the initVar variable, calls the
*  WIFI_Init() function, and then calls the
*  WIFI_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The WIFI_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time WIFI_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the WIFI_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WIFI_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(WIFI_initVar == 0u)
    {
        WIFI_Init();
        WIFI_initVar = 1u;
    }

    WIFI_Enable();
}


/*******************************************************************************
* Function Name: WIFI_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call WIFI_Init() because
*  the WIFI_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void WIFI_Init(void) 
{
    #if(WIFI_RX_ENABLED || WIFI_HD_ENABLED)

        #if (WIFI_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(WIFI_RX_VECT_NUM, &WIFI_RXISR);
            CyIntSetPriority(WIFI_RX_VECT_NUM, WIFI_RX_PRIOR_NUM);
            WIFI_errorStatus = 0u;
        #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        #if (WIFI_RXHW_ADDRESS_ENABLED)
            WIFI_SetRxAddressMode(WIFI_RX_ADDRESS_MODE);
            WIFI_SetRxAddress1(WIFI_RX_HW_ADDRESS1);
            WIFI_SetRxAddress2(WIFI_RX_HW_ADDRESS2);
        #endif /* End WIFI_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        WIFI_RXBITCTR_PERIOD_REG = WIFI_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        WIFI_RXSTATUS_MASK_REG  = WIFI_INIT_RX_INTERRUPTS_MASK;
    #endif /* End WIFI_RX_ENABLED || WIFI_HD_ENABLED*/

    #if(WIFI_TX_ENABLED)
        #if (WIFI_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(WIFI_TX_VECT_NUM, &WIFI_TXISR);
            CyIntSetPriority(WIFI_TX_VECT_NUM, WIFI_TX_PRIOR_NUM);
        #endif /* (WIFI_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (WIFI_TXCLKGEN_DP)
            WIFI_TXBITCLKGEN_CTR_REG = WIFI_BIT_CENTER;
            WIFI_TXBITCLKTX_COMPLETE_REG = ((WIFI_NUMBER_OF_DATA_BITS +
                        WIFI_NUMBER_OF_START_BIT) * WIFI_OVER_SAMPLE_COUNT) - 1u;
        #else
            WIFI_TXBITCTR_PERIOD_REG = ((WIFI_NUMBER_OF_DATA_BITS +
                        WIFI_NUMBER_OF_START_BIT) * WIFI_OVER_SAMPLE_8) - 1u;
        #endif /* End WIFI_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (WIFI_TX_INTERRUPT_ENABLED)
            WIFI_TXSTATUS_MASK_REG = WIFI_TX_STS_FIFO_EMPTY;
        #else
            WIFI_TXSTATUS_MASK_REG = WIFI_INIT_TX_INTERRUPTS_MASK;
        #endif /*End WIFI_TX_INTERRUPT_ENABLED*/

    #endif /* End WIFI_TX_ENABLED */

    #if(WIFI_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        WIFI_WriteControlRegister( \
            (WIFI_ReadControlRegister() & (uint8)~WIFI_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(WIFI_PARITY_TYPE << WIFI_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End WIFI_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: WIFI_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call WIFI_Enable() because the WIFI_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WIFI_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void WIFI_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (WIFI_RX_ENABLED || WIFI_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        WIFI_RXBITCTR_CONTROL_REG |= WIFI_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        WIFI_RXSTATUS_ACTL_REG  |= WIFI_INT_ENABLE;

        #if (WIFI_RX_INTERRUPT_ENABLED)
            WIFI_EnableRxInt();

            #if (WIFI_RXHW_ADDRESS_ENABLED)
                WIFI_rxAddressDetected = 0u;
            #endif /* (WIFI_RXHW_ADDRESS_ENABLED) */
        #endif /* (WIFI_RX_INTERRUPT_ENABLED) */
    #endif /* (WIFI_RX_ENABLED || WIFI_HD_ENABLED) */

    #if(WIFI_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!WIFI_TXCLKGEN_DP)
            WIFI_TXBITCTR_CONTROL_REG |= WIFI_CNTR_ENABLE;
        #endif /* End WIFI_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        WIFI_TXSTATUS_ACTL_REG |= WIFI_INT_ENABLE;
        #if (WIFI_TX_INTERRUPT_ENABLED)
            WIFI_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            WIFI_EnableTxInt();
        #endif /* (WIFI_TX_INTERRUPT_ENABLED) */
     #endif /* (WIFI_TX_INTERRUPT_ENABLED) */

    #if (WIFI_INTERNAL_CLOCK_USED)
        WIFI_IntClock_Start();  /* Enable the clock */
    #endif /* (WIFI_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: WIFI_Stop
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
void WIFI_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (WIFI_RX_ENABLED || WIFI_HD_ENABLED)
        WIFI_RXBITCTR_CONTROL_REG &= (uint8) ~WIFI_CNTR_ENABLE;
    #endif /* (WIFI_RX_ENABLED || WIFI_HD_ENABLED) */

    #if (WIFI_TX_ENABLED)
        #if(!WIFI_TXCLKGEN_DP)
            WIFI_TXBITCTR_CONTROL_REG &= (uint8) ~WIFI_CNTR_ENABLE;
        #endif /* (!WIFI_TXCLKGEN_DP) */
    #endif /* (WIFI_TX_ENABLED) */

    #if (WIFI_INTERNAL_CLOCK_USED)
        WIFI_IntClock_Stop();   /* Disable the clock */
    #endif /* (WIFI_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (WIFI_RX_ENABLED || WIFI_HD_ENABLED)
        WIFI_RXSTATUS_ACTL_REG  &= (uint8) ~WIFI_INT_ENABLE;

        #if (WIFI_RX_INTERRUPT_ENABLED)
            WIFI_DisableRxInt();
        #endif /* (WIFI_RX_INTERRUPT_ENABLED) */
    #endif /* (WIFI_RX_ENABLED || WIFI_HD_ENABLED) */

    #if (WIFI_TX_ENABLED)
        WIFI_TXSTATUS_ACTL_REG &= (uint8) ~WIFI_INT_ENABLE;

        #if (WIFI_TX_INTERRUPT_ENABLED)
            WIFI_DisableTxInt();
        #endif /* (WIFI_TX_INTERRUPT_ENABLED) */
    #endif /* (WIFI_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: WIFI_ReadControlRegister
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
uint8 WIFI_ReadControlRegister(void) 
{
    #if (WIFI_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(WIFI_CONTROL_REG);
    #endif /* (WIFI_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: WIFI_WriteControlRegister
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
void  WIFI_WriteControlRegister(uint8 control) 
{
    #if (WIFI_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       WIFI_CONTROL_REG = control;
    #endif /* (WIFI_CONTROL_REG_REMOVED) */
}


#if(WIFI_RX_ENABLED || WIFI_HD_ENABLED)
    /*******************************************************************************
    * Function Name: WIFI_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      WIFI_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      WIFI_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      WIFI_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      WIFI_RX_STS_BREAK            Interrupt on break.
    *      WIFI_RX_STS_OVERRUN          Interrupt on overrun error.
    *      WIFI_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      WIFI_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void WIFI_SetRxInterruptMode(uint8 intSrc) 
    {
        WIFI_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: WIFI_ReadRxData
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
    *  WIFI_rxBuffer - RAM buffer pointer for save received data.
    *  WIFI_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  WIFI_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  WIFI_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 WIFI_ReadRxData(void) 
    {
        uint8 rxData;

    #if (WIFI_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        WIFI_DisableRxInt();

        locRxBufferRead  = WIFI_rxBufferRead;
        locRxBufferWrite = WIFI_rxBufferWrite;

        if( (WIFI_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = WIFI_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= WIFI_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            WIFI_rxBufferRead = locRxBufferRead;

            if(WIFI_rxBufferLoopDetect != 0u)
            {
                WIFI_rxBufferLoopDetect = 0u;
                #if ((WIFI_RX_INTERRUPT_ENABLED) && (WIFI_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( WIFI_HD_ENABLED )
                        if((WIFI_CONTROL_REG & WIFI_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            WIFI_RXSTATUS_MASK_REG  |= WIFI_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        WIFI_RXSTATUS_MASK_REG  |= WIFI_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end WIFI_HD_ENABLED */
                #endif /* ((WIFI_RX_INTERRUPT_ENABLED) && (WIFI_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = WIFI_RXDATA_REG;
        }

        WIFI_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = WIFI_RXDATA_REG;

    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: WIFI_ReadRxStatus
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
    *  WIFI_RX_STS_FIFO_NOTEMPTY.
    *  WIFI_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  WIFI_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   WIFI_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   WIFI_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 WIFI_ReadRxStatus(void) 
    {
        uint8 status;

        status = WIFI_RXSTATUS_REG & WIFI_RX_HW_MASK;

    #if (WIFI_RX_INTERRUPT_ENABLED)
        if(WIFI_rxBufferOverflow != 0u)
        {
            status |= WIFI_RX_STS_SOFT_BUFF_OVER;
            WIFI_rxBufferOverflow = 0u;
        }
    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: WIFI_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. WIFI_GetChar() is
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
    *  WIFI_rxBuffer - RAM buffer pointer for save received data.
    *  WIFI_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  WIFI_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  WIFI_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 WIFI_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (WIFI_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        WIFI_DisableRxInt();

        locRxBufferRead  = WIFI_rxBufferRead;
        locRxBufferWrite = WIFI_rxBufferWrite;

        if( (WIFI_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = WIFI_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= WIFI_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            WIFI_rxBufferRead = locRxBufferRead;

            if(WIFI_rxBufferLoopDetect != 0u)
            {
                WIFI_rxBufferLoopDetect = 0u;
                #if( (WIFI_RX_INTERRUPT_ENABLED) && (WIFI_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( WIFI_HD_ENABLED )
                        if((WIFI_CONTROL_REG & WIFI_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            WIFI_RXSTATUS_MASK_REG |= WIFI_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        WIFI_RXSTATUS_MASK_REG |= WIFI_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end WIFI_HD_ENABLED */
                #endif /* WIFI_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = WIFI_RXSTATUS_REG;
            if((rxStatus & WIFI_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = WIFI_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (WIFI_RX_STS_BREAK | WIFI_RX_STS_PAR_ERROR |
                                WIFI_RX_STS_STOP_ERROR | WIFI_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        WIFI_EnableRxInt();

    #else

        rxStatus =WIFI_RXSTATUS_REG;
        if((rxStatus & WIFI_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = WIFI_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (WIFI_RX_STS_BREAK | WIFI_RX_STS_PAR_ERROR |
                            WIFI_RX_STS_STOP_ERROR | WIFI_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: WIFI_GetByte
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
    uint16 WIFI_GetByte(void) 
    {
        
    #if (WIFI_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        WIFI_DisableRxInt();
        locErrorStatus = (uint16)WIFI_errorStatus;
        WIFI_errorStatus = 0u;
        WIFI_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | WIFI_ReadRxData() );
    #else
        return ( ((uint16)WIFI_ReadRxStatus() << 8u) | WIFI_ReadRxData() );
    #endif /* WIFI_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: WIFI_GetRxBufferSize
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
    *  WIFI_rxBufferWrite - used to calculate left bytes.
    *  WIFI_rxBufferRead - used to calculate left bytes.
    *  WIFI_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 WIFI_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (WIFI_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        WIFI_DisableRxInt();

        if(WIFI_rxBufferRead == WIFI_rxBufferWrite)
        {
            if(WIFI_rxBufferLoopDetect != 0u)
            {
                size = WIFI_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(WIFI_rxBufferRead < WIFI_rxBufferWrite)
        {
            size = (WIFI_rxBufferWrite - WIFI_rxBufferRead);
        }
        else
        {
            size = (WIFI_RX_BUFFER_SIZE - WIFI_rxBufferRead) + WIFI_rxBufferWrite;
        }

        WIFI_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((WIFI_RXSTATUS_REG & WIFI_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: WIFI_ClearRxBuffer
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
    *  WIFI_rxBufferWrite - cleared to zero.
    *  WIFI_rxBufferRead - cleared to zero.
    *  WIFI_rxBufferLoopDetect - cleared to zero.
    *  WIFI_rxBufferOverflow - cleared to zero.
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
    void WIFI_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        WIFI_RXDATA_AUX_CTL_REG |= (uint8)  WIFI_RX_FIFO_CLR;
        WIFI_RXDATA_AUX_CTL_REG &= (uint8) ~WIFI_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (WIFI_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        WIFI_DisableRxInt();

        WIFI_rxBufferRead = 0u;
        WIFI_rxBufferWrite = 0u;
        WIFI_rxBufferLoopDetect = 0u;
        WIFI_rxBufferOverflow = 0u;

        WIFI_EnableRxInt();

    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: WIFI_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  WIFI__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  WIFI__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  WIFI__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  WIFI__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  WIFI__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  WIFI_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  WIFI_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void WIFI_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(WIFI_RXHW_ADDRESS_ENABLED)
            #if(WIFI_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* WIFI_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = WIFI_CONTROL_REG & (uint8)~WIFI_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << WIFI_CTRL_RXADDR_MODE0_SHIFT);
                WIFI_CONTROL_REG = tmpCtrl;

                #if(WIFI_RX_INTERRUPT_ENABLED && \
                   (WIFI_RXBUFFERSIZE > WIFI_FIFO_LENGTH) )
                    WIFI_rxAddressMode = addressMode;
                    WIFI_rxAddressDetected = 0u;
                #endif /* End WIFI_RXBUFFERSIZE > WIFI_FIFO_LENGTH*/
            #endif /* End WIFI_CONTROL_REG_REMOVED */
        #else /* WIFI_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End WIFI_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: WIFI_SetRxAddress1
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
    void WIFI_SetRxAddress1(uint8 address) 
    {
        WIFI_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: WIFI_SetRxAddress2
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
    void WIFI_SetRxAddress2(uint8 address) 
    {
        WIFI_RXADDRESS2_REG = address;
    }

#endif  /* WIFI_RX_ENABLED || WIFI_HD_ENABLED*/


#if( (WIFI_TX_ENABLED) || (WIFI_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: WIFI_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   WIFI_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   WIFI_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   WIFI_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   WIFI_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void WIFI_SetTxInterruptMode(uint8 intSrc) 
    {
        WIFI_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: WIFI_WriteTxData
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
    *  WIFI_txBuffer - RAM buffer pointer for save data for transmission
    *  WIFI_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  WIFI_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  WIFI_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void WIFI_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(WIFI_initVar != 0u)
        {
        #if (WIFI_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            WIFI_DisableTxInt();

            if( (WIFI_txBufferRead == WIFI_txBufferWrite) &&
                ((WIFI_TXSTATUS_REG & WIFI_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                WIFI_TXDATA_REG = txDataByte;
            }
            else
            {
                if(WIFI_txBufferWrite >= WIFI_TX_BUFFER_SIZE)
                {
                    WIFI_txBufferWrite = 0u;
                }

                WIFI_txBuffer[WIFI_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                WIFI_txBufferWrite++;
            }

            WIFI_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            WIFI_TXDATA_REG = txDataByte;

        #endif /*(WIFI_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: WIFI_ReadTxStatus
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
    uint8 WIFI_ReadTxStatus(void) 
    {
        return(WIFI_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: WIFI_PutChar
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
    *  WIFI_txBuffer - RAM buffer pointer for save data for transmission
    *  WIFI_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  WIFI_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  WIFI_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void WIFI_PutChar(uint8 txDataByte) 
    {
    #if (WIFI_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            WIFI_DisableTxInt();
        #endif /* (WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = WIFI_txBufferWrite;
            locTxBufferRead  = WIFI_txBufferRead;

        #if ((WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            WIFI_EnableTxInt();
        #endif /* (WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(WIFI_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((WIFI_TXSTATUS_REG & WIFI_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            WIFI_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= WIFI_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            WIFI_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3))
            WIFI_DisableTxInt();
        #endif /* (WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3) */

            WIFI_txBufferWrite = locTxBufferWrite;

        #if ((WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3))
            WIFI_EnableTxInt();
        #endif /* (WIFI_TX_BUFFER_SIZE > WIFI_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (WIFI_TXSTATUS_REG & WIFI_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                WIFI_SetPendingTxInt();
            }
        }

    #else

        while((WIFI_TXSTATUS_REG & WIFI_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        WIFI_TXDATA_REG = txDataByte;

    #endif /* WIFI_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: WIFI_PutString
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
    *  WIFI_initVar - checked to identify that the component has been
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
    void WIFI_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(WIFI_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                WIFI_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: WIFI_PutArray
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
    *  WIFI_initVar - checked to identify that the component has been
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
    void WIFI_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(WIFI_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                WIFI_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: WIFI_PutCRLF
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
    *  WIFI_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void WIFI_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(WIFI_initVar != 0u)
        {
            WIFI_PutChar(txDataByte);
            WIFI_PutChar(0x0Du);
            WIFI_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: WIFI_GetTxBufferSize
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
    *  WIFI_txBufferWrite - used to calculate left space.
    *  WIFI_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 WIFI_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (WIFI_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        WIFI_DisableTxInt();

        if(WIFI_txBufferRead == WIFI_txBufferWrite)
        {
            size = 0u;
        }
        else if(WIFI_txBufferRead < WIFI_txBufferWrite)
        {
            size = (WIFI_txBufferWrite - WIFI_txBufferRead);
        }
        else
        {
            size = (WIFI_TX_BUFFER_SIZE - WIFI_txBufferRead) +
                    WIFI_txBufferWrite;
        }

        WIFI_EnableTxInt();

    #else

        size = WIFI_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & WIFI_TX_STS_FIFO_FULL) != 0u)
        {
            size = WIFI_FIFO_LENGTH;
        }
        else if((size & WIFI_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (WIFI_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: WIFI_ClearTxBuffer
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
    *  WIFI_txBufferWrite - cleared to zero.
    *  WIFI_txBufferRead - cleared to zero.
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
    void WIFI_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        WIFI_TXDATA_AUX_CTL_REG |= (uint8)  WIFI_TX_FIFO_CLR;
        WIFI_TXDATA_AUX_CTL_REG &= (uint8) ~WIFI_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (WIFI_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        WIFI_DisableTxInt();

        WIFI_txBufferRead = 0u;
        WIFI_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        WIFI_EnableTxInt();

    #endif /* (WIFI_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: WIFI_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   WIFI_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   WIFI_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   WIFI_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   WIFI_SEND_WAIT_REINIT - Performs both options: 
    *      WIFI_SEND_BREAK and WIFI_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  WIFI_initVar - checked to identify that the component has been
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
    *     When interrupt appear with WIFI_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The WIFI_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void WIFI_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(WIFI_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(WIFI_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == WIFI_SEND_BREAK) ||
                (retMode == WIFI_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                WIFI_WriteControlRegister(WIFI_ReadControlRegister() |
                                                      WIFI_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                WIFI_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = WIFI_TXSTATUS_REG;
                }
                while((tmpStat & WIFI_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == WIFI_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == WIFI_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = WIFI_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & WIFI_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == WIFI_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == WIFI_REINIT) ||
                (retMode == WIFI_SEND_WAIT_REINIT) )
            {
                WIFI_WriteControlRegister(WIFI_ReadControlRegister() &
                                              (uint8)~WIFI_CTRL_HD_SEND_BREAK);
            }

        #else /* WIFI_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == WIFI_SEND_BREAK) ||
                (retMode == WIFI_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (WIFI_PARITY_TYPE != WIFI__B_UART__NONE_REVB) || \
                                    (WIFI_PARITY_TYPE_SW != 0u) )
                    WIFI_WriteControlRegister(WIFI_ReadControlRegister() |
                                                          WIFI_CTRL_HD_SEND_BREAK);
                #endif /* End WIFI_PARITY_TYPE != WIFI__B_UART__NONE_REVB  */

                #if(WIFI_TXCLKGEN_DP)
                    txPeriod = WIFI_TXBITCLKTX_COMPLETE_REG;
                    WIFI_TXBITCLKTX_COMPLETE_REG = WIFI_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = WIFI_TXBITCTR_PERIOD_REG;
                    WIFI_TXBITCTR_PERIOD_REG = WIFI_TXBITCTR_BREAKBITS8X;
                #endif /* End WIFI_TXCLKGEN_DP */

                /* Send zeros */
                WIFI_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = WIFI_TXSTATUS_REG;
                }
                while((tmpStat & WIFI_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == WIFI_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == WIFI_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = WIFI_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & WIFI_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == WIFI_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == WIFI_REINIT) ||
                (retMode == WIFI_SEND_WAIT_REINIT) )
            {

            #if(WIFI_TXCLKGEN_DP)
                WIFI_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                WIFI_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End WIFI_TXCLKGEN_DP */

            #if( (WIFI_PARITY_TYPE != WIFI__B_UART__NONE_REVB) || \
                 (WIFI_PARITY_TYPE_SW != 0u) )
                WIFI_WriteControlRegister(WIFI_ReadControlRegister() &
                                                      (uint8) ~WIFI_CTRL_HD_SEND_BREAK);
            #endif /* End WIFI_PARITY_TYPE != NONE */
            }
        #endif    /* End WIFI_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: WIFI_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       WIFI_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       WIFI_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears WIFI_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void WIFI_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( WIFI_CONTROL_REG_REMOVED == 0u )
            WIFI_WriteControlRegister(WIFI_ReadControlRegister() |
                                                  WIFI_CTRL_MARK);
        #endif /* End WIFI_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( WIFI_CONTROL_REG_REMOVED == 0u )
            WIFI_WriteControlRegister(WIFI_ReadControlRegister() &
                                                  (uint8) ~WIFI_CTRL_MARK);
        #endif /* End WIFI_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndWIFI_TX_ENABLED */

#if(WIFI_HD_ENABLED)


    /*******************************************************************************
    * Function Name: WIFI_LoadRxConfig
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
    void WIFI_LoadRxConfig(void) 
    {
        WIFI_WriteControlRegister(WIFI_ReadControlRegister() &
                                                (uint8)~WIFI_CTRL_HD_SEND);
        WIFI_RXBITCTR_PERIOD_REG = WIFI_HD_RXBITCTR_INIT;

    #if (WIFI_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        WIFI_SetRxInterruptMode(WIFI_INIT_RX_INTERRUPTS_MASK);
    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: WIFI_LoadTxConfig
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
    void WIFI_LoadTxConfig(void) 
    {
    #if (WIFI_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        WIFI_SetRxInterruptMode(0u);
    #endif /* (WIFI_RX_INTERRUPT_ENABLED) */

        WIFI_WriteControlRegister(WIFI_ReadControlRegister() | WIFI_CTRL_HD_SEND);
        WIFI_RXBITCTR_PERIOD_REG = WIFI_HD_TXBITCTR_INIT;
    }

#endif  /* WIFI_HD_ENABLED */


/* [] END OF FILE */
