/*******************************************************************************
* File Name: PSOC.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_PSOC_H)
#define CY_UART_PSOC_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define PSOC_RX_ENABLED                     (1u)
#define PSOC_TX_ENABLED                     (1u)
#define PSOC_HD_ENABLED                     (0u)
#define PSOC_RX_INTERRUPT_ENABLED           (1u)
#define PSOC_TX_INTERRUPT_ENABLED           (0u)
#define PSOC_INTERNAL_CLOCK_USED            (1u)
#define PSOC_RXHW_ADDRESS_ENABLED           (0u)
#define PSOC_OVER_SAMPLE_COUNT              (8u)
#define PSOC_PARITY_TYPE                    (0u)
#define PSOC_PARITY_TYPE_SW                 (0u)
#define PSOC_BREAK_DETECT                   (0u)
#define PSOC_BREAK_BITS_TX                  (13u)
#define PSOC_BREAK_BITS_RX                  (13u)
#define PSOC_TXCLKGEN_DP                    (1u)
#define PSOC_USE23POLLING                   (1u)
#define PSOC_FLOW_CONTROL                   (0u)
#define PSOC_CLK_FREQ                       (0u)
#define PSOC_TX_BUFFER_SIZE                 (4u)
#define PSOC_RX_BUFFER_SIZE                 (1000u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define PSOC_CONTROL_REG_REMOVED            (0u)
#else
    #define PSOC_CONTROL_REG_REMOVED            (1u)
#endif /* End PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct PSOC_backupStruct_
{
    uint8 enableState;

    #if(PSOC_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End PSOC_CONTROL_REG_REMOVED */

} PSOC_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void PSOC_Start(void) ;
void PSOC_Stop(void) ;
uint8 PSOC_ReadControlRegister(void) ;
void PSOC_WriteControlRegister(uint8 control) ;

void PSOC_Init(void) ;
void PSOC_Enable(void) ;
void PSOC_SaveConfig(void) ;
void PSOC_RestoreConfig(void) ;
void PSOC_Sleep(void) ;
void PSOC_Wakeup(void) ;

/* Only if RX is enabled */
#if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )

    #if (PSOC_RX_INTERRUPT_ENABLED)
        #define PSOC_EnableRxInt()  CyIntEnable (PSOC_RX_VECT_NUM)
        #define PSOC_DisableRxInt() CyIntDisable(PSOC_RX_VECT_NUM)
        CY_ISR_PROTO(PSOC_RXISR);
    #endif /* PSOC_RX_INTERRUPT_ENABLED */

    void PSOC_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void PSOC_SetRxAddress1(uint8 address) ;
    void PSOC_SetRxAddress2(uint8 address) ;

    void  PSOC_SetRxInterruptMode(uint8 intSrc) ;
    uint8 PSOC_ReadRxData(void) ;
    uint8 PSOC_ReadRxStatus(void) ;
    uint8 PSOC_GetChar(void) ;
    uint16 PSOC_GetByte(void) ;
    uint16 PSOC_GetRxBufferSize(void)
                                                            ;
    void PSOC_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define PSOC_GetRxInterruptSource   PSOC_ReadRxStatus

#endif /* End (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */

/* Only if TX is enabled */
#if(PSOC_TX_ENABLED || PSOC_HD_ENABLED)

    #if(PSOC_TX_INTERRUPT_ENABLED)
        #define PSOC_EnableTxInt()  CyIntEnable (PSOC_TX_VECT_NUM)
        #define PSOC_DisableTxInt() CyIntDisable(PSOC_TX_VECT_NUM)
        #define PSOC_SetPendingTxInt() CyIntSetPending(PSOC_TX_VECT_NUM)
        #define PSOC_ClearPendingTxInt() CyIntClearPending(PSOC_TX_VECT_NUM)
        CY_ISR_PROTO(PSOC_TXISR);
    #endif /* PSOC_TX_INTERRUPT_ENABLED */

    void PSOC_SetTxInterruptMode(uint8 intSrc) ;
    void PSOC_WriteTxData(uint8 txDataByte) ;
    uint8 PSOC_ReadTxStatus(void) ;
    void PSOC_PutChar(uint8 txDataByte) ;
    void PSOC_PutString(const char8 string[]) ;
    void PSOC_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void PSOC_PutCRLF(uint8 txDataByte) ;
    void PSOC_ClearTxBuffer(void) ;
    void PSOC_SetTxAddressMode(uint8 addressMode) ;
    void PSOC_SendBreak(uint8 retMode) ;
    uint8 PSOC_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define PSOC_PutStringConst         PSOC_PutString
    #define PSOC_PutArrayConst          PSOC_PutArray
    #define PSOC_GetTxInterruptSource   PSOC_ReadTxStatus

#endif /* End PSOC_TX_ENABLED || PSOC_HD_ENABLED */

#if(PSOC_HD_ENABLED)
    void PSOC_LoadRxConfig(void) ;
    void PSOC_LoadTxConfig(void) ;
#endif /* End PSOC_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PSOC) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    PSOC_CyBtldrCommStart(void) CYSMALL ;
    void    PSOC_CyBtldrCommStop(void) CYSMALL ;
    void    PSOC_CyBtldrCommReset(void) CYSMALL ;
    cystatus PSOC_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus PSOC_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PSOC)
        #define CyBtldrCommStart    PSOC_CyBtldrCommStart
        #define CyBtldrCommStop     PSOC_CyBtldrCommStop
        #define CyBtldrCommReset    PSOC_CyBtldrCommReset
        #define CyBtldrCommWrite    PSOC_CyBtldrCommWrite
        #define CyBtldrCommRead     PSOC_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_PSOC) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define PSOC_BYTE2BYTE_TIME_OUT (25u)
    #define PSOC_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define PSOC_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define PSOC_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define PSOC_SET_SPACE      (0x00u)
#define PSOC_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (PSOC_TX_ENABLED) || (PSOC_HD_ENABLED) )
    #if(PSOC_TX_INTERRUPT_ENABLED)
        #define PSOC_TX_VECT_NUM            (uint8)PSOC_TXInternalInterrupt__INTC_NUMBER
        #define PSOC_TX_PRIOR_NUM           (uint8)PSOC_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* PSOC_TX_INTERRUPT_ENABLED */

    #define PSOC_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define PSOC_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define PSOC_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(PSOC_TX_ENABLED)
        #define PSOC_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (PSOC_HD_ENABLED) */
        #define PSOC_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (PSOC_TX_ENABLED) */

    #define PSOC_TX_STS_COMPLETE            (uint8)(0x01u << PSOC_TX_STS_COMPLETE_SHIFT)
    #define PSOC_TX_STS_FIFO_EMPTY          (uint8)(0x01u << PSOC_TX_STS_FIFO_EMPTY_SHIFT)
    #define PSOC_TX_STS_FIFO_FULL           (uint8)(0x01u << PSOC_TX_STS_FIFO_FULL_SHIFT)
    #define PSOC_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << PSOC_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (PSOC_TX_ENABLED) || (PSOC_HD_ENABLED)*/

#if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )
    #if(PSOC_RX_INTERRUPT_ENABLED)
        #define PSOC_RX_VECT_NUM            (uint8)PSOC_RXInternalInterrupt__INTC_NUMBER
        #define PSOC_RX_PRIOR_NUM           (uint8)PSOC_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* PSOC_RX_INTERRUPT_ENABLED */
    #define PSOC_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define PSOC_RX_STS_BREAK_SHIFT             (0x01u)
    #define PSOC_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define PSOC_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define PSOC_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define PSOC_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define PSOC_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define PSOC_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define PSOC_RX_STS_MRKSPC           (uint8)(0x01u << PSOC_RX_STS_MRKSPC_SHIFT)
    #define PSOC_RX_STS_BREAK            (uint8)(0x01u << PSOC_RX_STS_BREAK_SHIFT)
    #define PSOC_RX_STS_PAR_ERROR        (uint8)(0x01u << PSOC_RX_STS_PAR_ERROR_SHIFT)
    #define PSOC_RX_STS_STOP_ERROR       (uint8)(0x01u << PSOC_RX_STS_STOP_ERROR_SHIFT)
    #define PSOC_RX_STS_OVERRUN          (uint8)(0x01u << PSOC_RX_STS_OVERRUN_SHIFT)
    #define PSOC_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << PSOC_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define PSOC_RX_STS_ADDR_MATCH       (uint8)(0x01u << PSOC_RX_STS_ADDR_MATCH_SHIFT)
    #define PSOC_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << PSOC_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define PSOC_RX_HW_MASK                     (0x7Fu)
#endif /* End (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */

/* Control Register definitions */
#define PSOC_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define PSOC_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define PSOC_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define PSOC_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define PSOC_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define PSOC_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define PSOC_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define PSOC_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define PSOC_CTRL_HD_SEND               (uint8)(0x01u << PSOC_CTRL_HD_SEND_SHIFT)
#define PSOC_CTRL_HD_SEND_BREAK         (uint8)(0x01u << PSOC_CTRL_HD_SEND_BREAK_SHIFT)
#define PSOC_CTRL_MARK                  (uint8)(0x01u << PSOC_CTRL_MARK_SHIFT)
#define PSOC_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << PSOC_CTRL_PARITY_TYPE0_SHIFT)
#define PSOC_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << PSOC_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define PSOC_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define PSOC_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define PSOC_SEND_BREAK                         (0x00u)
#define PSOC_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define PSOC_REINIT                             (0x02u)
#define PSOC_SEND_WAIT_REINIT                   (0x03u)

#define PSOC_OVER_SAMPLE_8                      (8u)
#define PSOC_OVER_SAMPLE_16                     (16u)

#define PSOC_BIT_CENTER                         (PSOC_OVER_SAMPLE_COUNT - 2u)

#define PSOC_FIFO_LENGTH                        (4u)
#define PSOC_NUMBER_OF_START_BIT                (1u)
#define PSOC_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define PSOC_TXBITCTR_BREAKBITS8X   ((PSOC_BREAK_BITS_TX * PSOC_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define PSOC_TXBITCTR_BREAKBITS ((PSOC_BREAK_BITS_TX * PSOC_OVER_SAMPLE_COUNT) - 1u)

#define PSOC_HALF_BIT_COUNT   \
                            (((PSOC_OVER_SAMPLE_COUNT / 2u) + (PSOC_USE23POLLING * 1u)) - 2u)
#if (PSOC_OVER_SAMPLE_COUNT == PSOC_OVER_SAMPLE_8)
    #define PSOC_HD_TXBITCTR_INIT   (((PSOC_BREAK_BITS_TX + \
                            PSOC_NUMBER_OF_START_BIT) * PSOC_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define PSOC_RXBITCTR_INIT  ((((PSOC_BREAK_BITS_RX + PSOC_NUMBER_OF_START_BIT) \
                            * PSOC_OVER_SAMPLE_COUNT) + PSOC_HALF_BIT_COUNT) - 1u)

#else /* PSOC_OVER_SAMPLE_COUNT == PSOC_OVER_SAMPLE_16 */
    #define PSOC_HD_TXBITCTR_INIT   ((8u * PSOC_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define PSOC_RXBITCTR_INIT      (((7u * PSOC_OVER_SAMPLE_COUNT) - 1u) + \
                                                      PSOC_HALF_BIT_COUNT)
#endif /* End PSOC_OVER_SAMPLE_COUNT */

#define PSOC_HD_RXBITCTR_INIT                   PSOC_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 PSOC_initVar;
#if (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED)
    extern volatile uint8 PSOC_txBuffer[PSOC_TX_BUFFER_SIZE];
    extern volatile uint8 PSOC_txBufferRead;
    extern uint8 PSOC_txBufferWrite;
#endif /* (PSOC_TX_INTERRUPT_ENABLED && PSOC_TX_ENABLED) */
#if (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED))
    extern uint8 PSOC_errorStatus;
    extern volatile uint8 PSOC_rxBuffer[PSOC_RX_BUFFER_SIZE];
    extern volatile uint16 PSOC_rxBufferRead;
    extern volatile uint16 PSOC_rxBufferWrite;
    extern volatile uint8 PSOC_rxBufferLoopDetect;
    extern volatile uint8 PSOC_rxBufferOverflow;
    #if (PSOC_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 PSOC_rxAddressMode;
        extern volatile uint8 PSOC_rxAddressDetected;
    #endif /* (PSOC_RXHW_ADDRESS_ENABLED) */
#endif /* (PSOC_RX_INTERRUPT_ENABLED && (PSOC_RX_ENABLED || PSOC_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define PSOC__B_UART__AM_SW_BYTE_BYTE 1
#define PSOC__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define PSOC__B_UART__AM_HW_BYTE_BY_BYTE 3
#define PSOC__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define PSOC__B_UART__AM_NONE 0

#define PSOC__B_UART__NONE_REVB 0
#define PSOC__B_UART__EVEN_REVB 1
#define PSOC__B_UART__ODD_REVB 2
#define PSOC__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define PSOC_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define PSOC_NUMBER_OF_STOP_BITS    (1u)

#if (PSOC_RXHW_ADDRESS_ENABLED)
    #define PSOC_RX_ADDRESS_MODE    (0u)
    #define PSOC_RX_HW_ADDRESS1     (0u)
    #define PSOC_RX_HW_ADDRESS2     (0u)
#endif /* (PSOC_RXHW_ADDRESS_ENABLED) */

#define PSOC_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << PSOC_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << PSOC_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << PSOC_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << PSOC_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << PSOC_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << PSOC_RX_STS_BREAK_SHIFT) \
                                        | (0 << PSOC_RX_STS_OVERRUN_SHIFT))

#define PSOC_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << PSOC_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << PSOC_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << PSOC_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << PSOC_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define PSOC_CONTROL_REG \
                            (* (reg8 *) PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define PSOC_CONTROL_PTR \
                            (  (reg8 *) PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(PSOC_TX_ENABLED)
    #define PSOC_TXDATA_REG          (* (reg8 *) PSOC_BUART_sTX_TxShifter_u0__F0_REG)
    #define PSOC_TXDATA_PTR          (  (reg8 *) PSOC_BUART_sTX_TxShifter_u0__F0_REG)
    #define PSOC_TXDATA_AUX_CTL_REG  (* (reg8 *) PSOC_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define PSOC_TXDATA_AUX_CTL_PTR  (  (reg8 *) PSOC_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define PSOC_TXSTATUS_REG        (* (reg8 *) PSOC_BUART_sTX_TxSts__STATUS_REG)
    #define PSOC_TXSTATUS_PTR        (  (reg8 *) PSOC_BUART_sTX_TxSts__STATUS_REG)
    #define PSOC_TXSTATUS_MASK_REG   (* (reg8 *) PSOC_BUART_sTX_TxSts__MASK_REG)
    #define PSOC_TXSTATUS_MASK_PTR   (  (reg8 *) PSOC_BUART_sTX_TxSts__MASK_REG)
    #define PSOC_TXSTATUS_ACTL_REG   (* (reg8 *) PSOC_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define PSOC_TXSTATUS_ACTL_PTR   (  (reg8 *) PSOC_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(PSOC_TXCLKGEN_DP)
        #define PSOC_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define PSOC_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define PSOC_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define PSOC_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define PSOC_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define PSOC_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define PSOC_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define PSOC_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define PSOC_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define PSOC_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) PSOC_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* PSOC_TXCLKGEN_DP */

#endif /* End PSOC_TX_ENABLED */

#if(PSOC_HD_ENABLED)

    #define PSOC_TXDATA_REG             (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__F1_REG )
    #define PSOC_TXDATA_PTR             (  (reg8 *) PSOC_BUART_sRX_RxShifter_u0__F1_REG )
    #define PSOC_TXDATA_AUX_CTL_REG     (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define PSOC_TXDATA_AUX_CTL_PTR     (  (reg8 *) PSOC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define PSOC_TXSTATUS_REG           (* (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_REG )
    #define PSOC_TXSTATUS_PTR           (  (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_REG )
    #define PSOC_TXSTATUS_MASK_REG      (* (reg8 *) PSOC_BUART_sRX_RxSts__MASK_REG )
    #define PSOC_TXSTATUS_MASK_PTR      (  (reg8 *) PSOC_BUART_sRX_RxSts__MASK_REG )
    #define PSOC_TXSTATUS_ACTL_REG      (* (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define PSOC_TXSTATUS_ACTL_PTR      (  (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End PSOC_HD_ENABLED */

#if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )
    #define PSOC_RXDATA_REG             (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__F0_REG )
    #define PSOC_RXDATA_PTR             (  (reg8 *) PSOC_BUART_sRX_RxShifter_u0__F0_REG )
    #define PSOC_RXADDRESS1_REG         (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__D0_REG )
    #define PSOC_RXADDRESS1_PTR         (  (reg8 *) PSOC_BUART_sRX_RxShifter_u0__D0_REG )
    #define PSOC_RXADDRESS2_REG         (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__D1_REG )
    #define PSOC_RXADDRESS2_PTR         (  (reg8 *) PSOC_BUART_sRX_RxShifter_u0__D1_REG )
    #define PSOC_RXDATA_AUX_CTL_REG     (* (reg8 *) PSOC_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define PSOC_RXBITCTR_PERIOD_REG    (* (reg8 *) PSOC_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define PSOC_RXBITCTR_PERIOD_PTR    (  (reg8 *) PSOC_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define PSOC_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) PSOC_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define PSOC_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) PSOC_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define PSOC_RXBITCTR_COUNTER_REG   (* (reg8 *) PSOC_BUART_sRX_RxBitCounter__COUNT_REG )
    #define PSOC_RXBITCTR_COUNTER_PTR   (  (reg8 *) PSOC_BUART_sRX_RxBitCounter__COUNT_REG )

    #define PSOC_RXSTATUS_REG           (* (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_REG )
    #define PSOC_RXSTATUS_PTR           (  (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_REG )
    #define PSOC_RXSTATUS_MASK_REG      (* (reg8 *) PSOC_BUART_sRX_RxSts__MASK_REG )
    #define PSOC_RXSTATUS_MASK_PTR      (  (reg8 *) PSOC_BUART_sRX_RxSts__MASK_REG )
    #define PSOC_RXSTATUS_ACTL_REG      (* (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define PSOC_RXSTATUS_ACTL_PTR      (  (reg8 *) PSOC_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */

#if(PSOC_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define PSOC_INTCLOCK_CLKEN_REG     (* (reg8 *) PSOC_IntClock__PM_ACT_CFG)
    #define PSOC_INTCLOCK_CLKEN_PTR     (  (reg8 *) PSOC_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define PSOC_INTCLOCK_CLKEN_MASK    PSOC_IntClock__PM_ACT_MSK
#endif /* End PSOC_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(PSOC_TX_ENABLED)
    #define PSOC_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End PSOC_TX_ENABLED */

#if(PSOC_HD_ENABLED)
    #define PSOC_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End PSOC_HD_ENABLED */

#if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )
    #define PSOC_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define PSOC_WAIT_1_MS      PSOC_BL_CHK_DELAY_MS   

#define PSOC_TXBUFFERSIZE   PSOC_TX_BUFFER_SIZE
#define PSOC_RXBUFFERSIZE   PSOC_RX_BUFFER_SIZE

#if (PSOC_RXHW_ADDRESS_ENABLED)
    #define PSOC_RXADDRESSMODE  PSOC_RX_ADDRESS_MODE
    #define PSOC_RXHWADDRESS1   PSOC_RX_HW_ADDRESS1
    #define PSOC_RXHWADDRESS2   PSOC_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define PSOC_RXAddressMode  PSOC_RXADDRESSMODE
#endif /* (PSOC_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define PSOC_initvar                    PSOC_initVar

#define PSOC_RX_Enabled                 PSOC_RX_ENABLED
#define PSOC_TX_Enabled                 PSOC_TX_ENABLED
#define PSOC_HD_Enabled                 PSOC_HD_ENABLED
#define PSOC_RX_IntInterruptEnabled     PSOC_RX_INTERRUPT_ENABLED
#define PSOC_TX_IntInterruptEnabled     PSOC_TX_INTERRUPT_ENABLED
#define PSOC_InternalClockUsed          PSOC_INTERNAL_CLOCK_USED
#define PSOC_RXHW_Address_Enabled       PSOC_RXHW_ADDRESS_ENABLED
#define PSOC_OverSampleCount            PSOC_OVER_SAMPLE_COUNT
#define PSOC_ParityType                 PSOC_PARITY_TYPE

#if( PSOC_TX_ENABLED && (PSOC_TXBUFFERSIZE > PSOC_FIFO_LENGTH))
    #define PSOC_TXBUFFER               PSOC_txBuffer
    #define PSOC_TXBUFFERREAD           PSOC_txBufferRead
    #define PSOC_TXBUFFERWRITE          PSOC_txBufferWrite
#endif /* End PSOC_TX_ENABLED */
#if( ( PSOC_RX_ENABLED || PSOC_HD_ENABLED ) && \
     (PSOC_RXBUFFERSIZE > PSOC_FIFO_LENGTH) )
    #define PSOC_RXBUFFER               PSOC_rxBuffer
    #define PSOC_RXBUFFERREAD           PSOC_rxBufferRead
    #define PSOC_RXBUFFERWRITE          PSOC_rxBufferWrite
    #define PSOC_RXBUFFERLOOPDETECT     PSOC_rxBufferLoopDetect
    #define PSOC_RXBUFFER_OVERFLOW      PSOC_rxBufferOverflow
#endif /* End PSOC_RX_ENABLED */

#ifdef PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define PSOC_CONTROL                PSOC_CONTROL_REG
#endif /* End PSOC_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(PSOC_TX_ENABLED)
    #define PSOC_TXDATA                 PSOC_TXDATA_REG
    #define PSOC_TXSTATUS               PSOC_TXSTATUS_REG
    #define PSOC_TXSTATUS_MASK          PSOC_TXSTATUS_MASK_REG
    #define PSOC_TXSTATUS_ACTL          PSOC_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(PSOC_TXCLKGEN_DP)
        #define PSOC_TXBITCLKGEN_CTR        PSOC_TXBITCLKGEN_CTR_REG
        #define PSOC_TXBITCLKTX_COMPLETE    PSOC_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define PSOC_TXBITCTR_PERIOD        PSOC_TXBITCTR_PERIOD_REG
        #define PSOC_TXBITCTR_CONTROL       PSOC_TXBITCTR_CONTROL_REG
        #define PSOC_TXBITCTR_COUNTER       PSOC_TXBITCTR_COUNTER_REG
    #endif /* PSOC_TXCLKGEN_DP */
#endif /* End PSOC_TX_ENABLED */

#if(PSOC_HD_ENABLED)
    #define PSOC_TXDATA                 PSOC_TXDATA_REG
    #define PSOC_TXSTATUS               PSOC_TXSTATUS_REG
    #define PSOC_TXSTATUS_MASK          PSOC_TXSTATUS_MASK_REG
    #define PSOC_TXSTATUS_ACTL          PSOC_TXSTATUS_ACTL_REG
#endif /* End PSOC_HD_ENABLED */

#if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )
    #define PSOC_RXDATA                 PSOC_RXDATA_REG
    #define PSOC_RXADDRESS1             PSOC_RXADDRESS1_REG
    #define PSOC_RXADDRESS2             PSOC_RXADDRESS2_REG
    #define PSOC_RXBITCTR_PERIOD        PSOC_RXBITCTR_PERIOD_REG
    #define PSOC_RXBITCTR_CONTROL       PSOC_RXBITCTR_CONTROL_REG
    #define PSOC_RXBITCTR_COUNTER       PSOC_RXBITCTR_COUNTER_REG
    #define PSOC_RXSTATUS               PSOC_RXSTATUS_REG
    #define PSOC_RXSTATUS_MASK          PSOC_RXSTATUS_MASK_REG
    #define PSOC_RXSTATUS_ACTL          PSOC_RXSTATUS_ACTL_REG
#endif /* End  (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */

#if(PSOC_INTERNAL_CLOCK_USED)
    #define PSOC_INTCLOCK_CLKEN         PSOC_INTCLOCK_CLKEN_REG
#endif /* End PSOC_INTERNAL_CLOCK_USED */

#define PSOC_WAIT_FOR_COMLETE_REINIT    PSOC_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_PSOC_H */


/* [] END OF FILE */
