/*******************************************************************************
* File Name: GSM.h
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


#if !defined(CY_UART_GSM_H)
#define CY_UART_GSM_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define GSM_RX_ENABLED                     (1u)
#define GSM_TX_ENABLED                     (1u)
#define GSM_HD_ENABLED                     (0u)
#define GSM_RX_INTERRUPT_ENABLED           (0u)
#define GSM_TX_INTERRUPT_ENABLED           (0u)
#define GSM_INTERNAL_CLOCK_USED            (1u)
#define GSM_RXHW_ADDRESS_ENABLED           (0u)
#define GSM_OVER_SAMPLE_COUNT              (8u)
#define GSM_PARITY_TYPE                    (0u)
#define GSM_PARITY_TYPE_SW                 (0u)
#define GSM_BREAK_DETECT                   (0u)
#define GSM_BREAK_BITS_TX                  (13u)
#define GSM_BREAK_BITS_RX                  (13u)
#define GSM_TXCLKGEN_DP                    (1u)
#define GSM_USE23POLLING                   (1u)
#define GSM_FLOW_CONTROL                   (0u)
#define GSM_CLK_FREQ                       (0u)
#define GSM_TX_BUFFER_SIZE                 (4u)
#define GSM_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define GSM_CONTROL_REG_REMOVED            (0u)
#else
    #define GSM_CONTROL_REG_REMOVED            (1u)
#endif /* End GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct GSM_backupStruct_
{
    uint8 enableState;

    #if(GSM_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End GSM_CONTROL_REG_REMOVED */

} GSM_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void GSM_Start(void) ;
void GSM_Stop(void) ;
uint8 GSM_ReadControlRegister(void) ;
void GSM_WriteControlRegister(uint8 control) ;

void GSM_Init(void) ;
void GSM_Enable(void) ;
void GSM_SaveConfig(void) ;
void GSM_RestoreConfig(void) ;
void GSM_Sleep(void) ;
void GSM_Wakeup(void) ;

/* Only if RX is enabled */
#if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )

    #if (GSM_RX_INTERRUPT_ENABLED)
        #define GSM_EnableRxInt()  CyIntEnable (GSM_RX_VECT_NUM)
        #define GSM_DisableRxInt() CyIntDisable(GSM_RX_VECT_NUM)
        CY_ISR_PROTO(GSM_RXISR);
    #endif /* GSM_RX_INTERRUPT_ENABLED */

    void GSM_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void GSM_SetRxAddress1(uint8 address) ;
    void GSM_SetRxAddress2(uint8 address) ;

    void  GSM_SetRxInterruptMode(uint8 intSrc) ;
    uint8 GSM_ReadRxData(void) ;
    uint8 GSM_ReadRxStatus(void) ;
    uint8 GSM_GetChar(void) ;
    uint16 GSM_GetByte(void) ;
    uint8 GSM_GetRxBufferSize(void)
                                                            ;
    void GSM_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define GSM_GetRxInterruptSource   GSM_ReadRxStatus

#endif /* End (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */

/* Only if TX is enabled */
#if(GSM_TX_ENABLED || GSM_HD_ENABLED)

    #if(GSM_TX_INTERRUPT_ENABLED)
        #define GSM_EnableTxInt()  CyIntEnable (GSM_TX_VECT_NUM)
        #define GSM_DisableTxInt() CyIntDisable(GSM_TX_VECT_NUM)
        #define GSM_SetPendingTxInt() CyIntSetPending(GSM_TX_VECT_NUM)
        #define GSM_ClearPendingTxInt() CyIntClearPending(GSM_TX_VECT_NUM)
        CY_ISR_PROTO(GSM_TXISR);
    #endif /* GSM_TX_INTERRUPT_ENABLED */

    void GSM_SetTxInterruptMode(uint8 intSrc) ;
    void GSM_WriteTxData(uint8 txDataByte) ;
    uint8 GSM_ReadTxStatus(void) ;
    void GSM_PutChar(uint8 txDataByte) ;
    void GSM_PutString(const char8 string[]) ;
    void GSM_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void GSM_PutCRLF(uint8 txDataByte) ;
    void GSM_ClearTxBuffer(void) ;
    void GSM_SetTxAddressMode(uint8 addressMode) ;
    void GSM_SendBreak(uint8 retMode) ;
    uint8 GSM_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define GSM_PutStringConst         GSM_PutString
    #define GSM_PutArrayConst          GSM_PutArray
    #define GSM_GetTxInterruptSource   GSM_ReadTxStatus

#endif /* End GSM_TX_ENABLED || GSM_HD_ENABLED */

#if(GSM_HD_ENABLED)
    void GSM_LoadRxConfig(void) ;
    void GSM_LoadTxConfig(void) ;
#endif /* End GSM_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_GSM) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    GSM_CyBtldrCommStart(void) CYSMALL ;
    void    GSM_CyBtldrCommStop(void) CYSMALL ;
    void    GSM_CyBtldrCommReset(void) CYSMALL ;
    cystatus GSM_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus GSM_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_GSM)
        #define CyBtldrCommStart    GSM_CyBtldrCommStart
        #define CyBtldrCommStop     GSM_CyBtldrCommStop
        #define CyBtldrCommReset    GSM_CyBtldrCommReset
        #define CyBtldrCommWrite    GSM_CyBtldrCommWrite
        #define CyBtldrCommRead     GSM_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_GSM) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define GSM_BYTE2BYTE_TIME_OUT (25u)
    #define GSM_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define GSM_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define GSM_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define GSM_SET_SPACE      (0x00u)
#define GSM_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (GSM_TX_ENABLED) || (GSM_HD_ENABLED) )
    #if(GSM_TX_INTERRUPT_ENABLED)
        #define GSM_TX_VECT_NUM            (uint8)GSM_TXInternalInterrupt__INTC_NUMBER
        #define GSM_TX_PRIOR_NUM           (uint8)GSM_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* GSM_TX_INTERRUPT_ENABLED */

    #define GSM_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define GSM_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define GSM_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(GSM_TX_ENABLED)
        #define GSM_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (GSM_HD_ENABLED) */
        #define GSM_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (GSM_TX_ENABLED) */

    #define GSM_TX_STS_COMPLETE            (uint8)(0x01u << GSM_TX_STS_COMPLETE_SHIFT)
    #define GSM_TX_STS_FIFO_EMPTY          (uint8)(0x01u << GSM_TX_STS_FIFO_EMPTY_SHIFT)
    #define GSM_TX_STS_FIFO_FULL           (uint8)(0x01u << GSM_TX_STS_FIFO_FULL_SHIFT)
    #define GSM_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << GSM_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (GSM_TX_ENABLED) || (GSM_HD_ENABLED)*/

#if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )
    #if(GSM_RX_INTERRUPT_ENABLED)
        #define GSM_RX_VECT_NUM            (uint8)GSM_RXInternalInterrupt__INTC_NUMBER
        #define GSM_RX_PRIOR_NUM           (uint8)GSM_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* GSM_RX_INTERRUPT_ENABLED */
    #define GSM_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define GSM_RX_STS_BREAK_SHIFT             (0x01u)
    #define GSM_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define GSM_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define GSM_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define GSM_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define GSM_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define GSM_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define GSM_RX_STS_MRKSPC           (uint8)(0x01u << GSM_RX_STS_MRKSPC_SHIFT)
    #define GSM_RX_STS_BREAK            (uint8)(0x01u << GSM_RX_STS_BREAK_SHIFT)
    #define GSM_RX_STS_PAR_ERROR        (uint8)(0x01u << GSM_RX_STS_PAR_ERROR_SHIFT)
    #define GSM_RX_STS_STOP_ERROR       (uint8)(0x01u << GSM_RX_STS_STOP_ERROR_SHIFT)
    #define GSM_RX_STS_OVERRUN          (uint8)(0x01u << GSM_RX_STS_OVERRUN_SHIFT)
    #define GSM_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << GSM_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define GSM_RX_STS_ADDR_MATCH       (uint8)(0x01u << GSM_RX_STS_ADDR_MATCH_SHIFT)
    #define GSM_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << GSM_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define GSM_RX_HW_MASK                     (0x7Fu)
#endif /* End (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */

/* Control Register definitions */
#define GSM_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define GSM_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define GSM_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define GSM_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define GSM_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define GSM_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define GSM_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define GSM_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define GSM_CTRL_HD_SEND               (uint8)(0x01u << GSM_CTRL_HD_SEND_SHIFT)
#define GSM_CTRL_HD_SEND_BREAK         (uint8)(0x01u << GSM_CTRL_HD_SEND_BREAK_SHIFT)
#define GSM_CTRL_MARK                  (uint8)(0x01u << GSM_CTRL_MARK_SHIFT)
#define GSM_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << GSM_CTRL_PARITY_TYPE0_SHIFT)
#define GSM_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << GSM_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define GSM_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define GSM_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define GSM_SEND_BREAK                         (0x00u)
#define GSM_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define GSM_REINIT                             (0x02u)
#define GSM_SEND_WAIT_REINIT                   (0x03u)

#define GSM_OVER_SAMPLE_8                      (8u)
#define GSM_OVER_SAMPLE_16                     (16u)

#define GSM_BIT_CENTER                         (GSM_OVER_SAMPLE_COUNT - 2u)

#define GSM_FIFO_LENGTH                        (4u)
#define GSM_NUMBER_OF_START_BIT                (1u)
#define GSM_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define GSM_TXBITCTR_BREAKBITS8X   ((GSM_BREAK_BITS_TX * GSM_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define GSM_TXBITCTR_BREAKBITS ((GSM_BREAK_BITS_TX * GSM_OVER_SAMPLE_COUNT) - 1u)

#define GSM_HALF_BIT_COUNT   \
                            (((GSM_OVER_SAMPLE_COUNT / 2u) + (GSM_USE23POLLING * 1u)) - 2u)
#if (GSM_OVER_SAMPLE_COUNT == GSM_OVER_SAMPLE_8)
    #define GSM_HD_TXBITCTR_INIT   (((GSM_BREAK_BITS_TX + \
                            GSM_NUMBER_OF_START_BIT) * GSM_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define GSM_RXBITCTR_INIT  ((((GSM_BREAK_BITS_RX + GSM_NUMBER_OF_START_BIT) \
                            * GSM_OVER_SAMPLE_COUNT) + GSM_HALF_BIT_COUNT) - 1u)

#else /* GSM_OVER_SAMPLE_COUNT == GSM_OVER_SAMPLE_16 */
    #define GSM_HD_TXBITCTR_INIT   ((8u * GSM_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define GSM_RXBITCTR_INIT      (((7u * GSM_OVER_SAMPLE_COUNT) - 1u) + \
                                                      GSM_HALF_BIT_COUNT)
#endif /* End GSM_OVER_SAMPLE_COUNT */

#define GSM_HD_RXBITCTR_INIT                   GSM_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 GSM_initVar;
#if (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED)
    extern volatile uint8 GSM_txBuffer[GSM_TX_BUFFER_SIZE];
    extern volatile uint8 GSM_txBufferRead;
    extern uint8 GSM_txBufferWrite;
#endif /* (GSM_TX_INTERRUPT_ENABLED && GSM_TX_ENABLED) */
#if (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED))
    extern uint8 GSM_errorStatus;
    extern volatile uint8 GSM_rxBuffer[GSM_RX_BUFFER_SIZE];
    extern volatile uint8 GSM_rxBufferRead;
    extern volatile uint8 GSM_rxBufferWrite;
    extern volatile uint8 GSM_rxBufferLoopDetect;
    extern volatile uint8 GSM_rxBufferOverflow;
    #if (GSM_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 GSM_rxAddressMode;
        extern volatile uint8 GSM_rxAddressDetected;
    #endif /* (GSM_RXHW_ADDRESS_ENABLED) */
#endif /* (GSM_RX_INTERRUPT_ENABLED && (GSM_RX_ENABLED || GSM_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define GSM__B_UART__AM_SW_BYTE_BYTE 1
#define GSM__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define GSM__B_UART__AM_HW_BYTE_BY_BYTE 3
#define GSM__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define GSM__B_UART__AM_NONE 0

#define GSM__B_UART__NONE_REVB 0
#define GSM__B_UART__EVEN_REVB 1
#define GSM__B_UART__ODD_REVB 2
#define GSM__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define GSM_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define GSM_NUMBER_OF_STOP_BITS    (1u)

#if (GSM_RXHW_ADDRESS_ENABLED)
    #define GSM_RX_ADDRESS_MODE    (0u)
    #define GSM_RX_HW_ADDRESS1     (0u)
    #define GSM_RX_HW_ADDRESS2     (0u)
#endif /* (GSM_RXHW_ADDRESS_ENABLED) */

#define GSM_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << GSM_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << GSM_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << GSM_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << GSM_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << GSM_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << GSM_RX_STS_BREAK_SHIFT) \
                                        | (0 << GSM_RX_STS_OVERRUN_SHIFT))

#define GSM_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << GSM_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << GSM_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << GSM_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << GSM_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define GSM_CONTROL_REG \
                            (* (reg8 *) GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define GSM_CONTROL_PTR \
                            (  (reg8 *) GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(GSM_TX_ENABLED)
    #define GSM_TXDATA_REG          (* (reg8 *) GSM_BUART_sTX_TxShifter_u0__F0_REG)
    #define GSM_TXDATA_PTR          (  (reg8 *) GSM_BUART_sTX_TxShifter_u0__F0_REG)
    #define GSM_TXDATA_AUX_CTL_REG  (* (reg8 *) GSM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define GSM_TXDATA_AUX_CTL_PTR  (  (reg8 *) GSM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define GSM_TXSTATUS_REG        (* (reg8 *) GSM_BUART_sTX_TxSts__STATUS_REG)
    #define GSM_TXSTATUS_PTR        (  (reg8 *) GSM_BUART_sTX_TxSts__STATUS_REG)
    #define GSM_TXSTATUS_MASK_REG   (* (reg8 *) GSM_BUART_sTX_TxSts__MASK_REG)
    #define GSM_TXSTATUS_MASK_PTR   (  (reg8 *) GSM_BUART_sTX_TxSts__MASK_REG)
    #define GSM_TXSTATUS_ACTL_REG   (* (reg8 *) GSM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define GSM_TXSTATUS_ACTL_PTR   (  (reg8 *) GSM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(GSM_TXCLKGEN_DP)
        #define GSM_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define GSM_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define GSM_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define GSM_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define GSM_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define GSM_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define GSM_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define GSM_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define GSM_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define GSM_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) GSM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* GSM_TXCLKGEN_DP */

#endif /* End GSM_TX_ENABLED */

#if(GSM_HD_ENABLED)

    #define GSM_TXDATA_REG             (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__F1_REG )
    #define GSM_TXDATA_PTR             (  (reg8 *) GSM_BUART_sRX_RxShifter_u0__F1_REG )
    #define GSM_TXDATA_AUX_CTL_REG     (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define GSM_TXDATA_AUX_CTL_PTR     (  (reg8 *) GSM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define GSM_TXSTATUS_REG           (* (reg8 *) GSM_BUART_sRX_RxSts__STATUS_REG )
    #define GSM_TXSTATUS_PTR           (  (reg8 *) GSM_BUART_sRX_RxSts__STATUS_REG )
    #define GSM_TXSTATUS_MASK_REG      (* (reg8 *) GSM_BUART_sRX_RxSts__MASK_REG )
    #define GSM_TXSTATUS_MASK_PTR      (  (reg8 *) GSM_BUART_sRX_RxSts__MASK_REG )
    #define GSM_TXSTATUS_ACTL_REG      (* (reg8 *) GSM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define GSM_TXSTATUS_ACTL_PTR      (  (reg8 *) GSM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End GSM_HD_ENABLED */

#if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )
    #define GSM_RXDATA_REG             (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__F0_REG )
    #define GSM_RXDATA_PTR             (  (reg8 *) GSM_BUART_sRX_RxShifter_u0__F0_REG )
    #define GSM_RXADDRESS1_REG         (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__D0_REG )
    #define GSM_RXADDRESS1_PTR         (  (reg8 *) GSM_BUART_sRX_RxShifter_u0__D0_REG )
    #define GSM_RXADDRESS2_REG         (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__D1_REG )
    #define GSM_RXADDRESS2_PTR         (  (reg8 *) GSM_BUART_sRX_RxShifter_u0__D1_REG )
    #define GSM_RXDATA_AUX_CTL_REG     (* (reg8 *) GSM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define GSM_RXBITCTR_PERIOD_REG    (* (reg8 *) GSM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define GSM_RXBITCTR_PERIOD_PTR    (  (reg8 *) GSM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define GSM_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) GSM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define GSM_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) GSM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define GSM_RXBITCTR_COUNTER_REG   (* (reg8 *) GSM_BUART_sRX_RxBitCounter__COUNT_REG )
    #define GSM_RXBITCTR_COUNTER_PTR   (  (reg8 *) GSM_BUART_sRX_RxBitCounter__COUNT_REG )

    #define GSM_RXSTATUS_REG           (* (reg8 *) GSM_BUART_sRX_RxSts__STATUS_REG )
    #define GSM_RXSTATUS_PTR           (  (reg8 *) GSM_BUART_sRX_RxSts__STATUS_REG )
    #define GSM_RXSTATUS_MASK_REG      (* (reg8 *) GSM_BUART_sRX_RxSts__MASK_REG )
    #define GSM_RXSTATUS_MASK_PTR      (  (reg8 *) GSM_BUART_sRX_RxSts__MASK_REG )
    #define GSM_RXSTATUS_ACTL_REG      (* (reg8 *) GSM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define GSM_RXSTATUS_ACTL_PTR      (  (reg8 *) GSM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */

#if(GSM_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define GSM_INTCLOCK_CLKEN_REG     (* (reg8 *) GSM_IntClock__PM_ACT_CFG)
    #define GSM_INTCLOCK_CLKEN_PTR     (  (reg8 *) GSM_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define GSM_INTCLOCK_CLKEN_MASK    GSM_IntClock__PM_ACT_MSK
#endif /* End GSM_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(GSM_TX_ENABLED)
    #define GSM_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End GSM_TX_ENABLED */

#if(GSM_HD_ENABLED)
    #define GSM_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End GSM_HD_ENABLED */

#if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )
    #define GSM_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define GSM_WAIT_1_MS      GSM_BL_CHK_DELAY_MS   

#define GSM_TXBUFFERSIZE   GSM_TX_BUFFER_SIZE
#define GSM_RXBUFFERSIZE   GSM_RX_BUFFER_SIZE

#if (GSM_RXHW_ADDRESS_ENABLED)
    #define GSM_RXADDRESSMODE  GSM_RX_ADDRESS_MODE
    #define GSM_RXHWADDRESS1   GSM_RX_HW_ADDRESS1
    #define GSM_RXHWADDRESS2   GSM_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define GSM_RXAddressMode  GSM_RXADDRESSMODE
#endif /* (GSM_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define GSM_initvar                    GSM_initVar

#define GSM_RX_Enabled                 GSM_RX_ENABLED
#define GSM_TX_Enabled                 GSM_TX_ENABLED
#define GSM_HD_Enabled                 GSM_HD_ENABLED
#define GSM_RX_IntInterruptEnabled     GSM_RX_INTERRUPT_ENABLED
#define GSM_TX_IntInterruptEnabled     GSM_TX_INTERRUPT_ENABLED
#define GSM_InternalClockUsed          GSM_INTERNAL_CLOCK_USED
#define GSM_RXHW_Address_Enabled       GSM_RXHW_ADDRESS_ENABLED
#define GSM_OverSampleCount            GSM_OVER_SAMPLE_COUNT
#define GSM_ParityType                 GSM_PARITY_TYPE

#if( GSM_TX_ENABLED && (GSM_TXBUFFERSIZE > GSM_FIFO_LENGTH))
    #define GSM_TXBUFFER               GSM_txBuffer
    #define GSM_TXBUFFERREAD           GSM_txBufferRead
    #define GSM_TXBUFFERWRITE          GSM_txBufferWrite
#endif /* End GSM_TX_ENABLED */
#if( ( GSM_RX_ENABLED || GSM_HD_ENABLED ) && \
     (GSM_RXBUFFERSIZE > GSM_FIFO_LENGTH) )
    #define GSM_RXBUFFER               GSM_rxBuffer
    #define GSM_RXBUFFERREAD           GSM_rxBufferRead
    #define GSM_RXBUFFERWRITE          GSM_rxBufferWrite
    #define GSM_RXBUFFERLOOPDETECT     GSM_rxBufferLoopDetect
    #define GSM_RXBUFFER_OVERFLOW      GSM_rxBufferOverflow
#endif /* End GSM_RX_ENABLED */

#ifdef GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define GSM_CONTROL                GSM_CONTROL_REG
#endif /* End GSM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(GSM_TX_ENABLED)
    #define GSM_TXDATA                 GSM_TXDATA_REG
    #define GSM_TXSTATUS               GSM_TXSTATUS_REG
    #define GSM_TXSTATUS_MASK          GSM_TXSTATUS_MASK_REG
    #define GSM_TXSTATUS_ACTL          GSM_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(GSM_TXCLKGEN_DP)
        #define GSM_TXBITCLKGEN_CTR        GSM_TXBITCLKGEN_CTR_REG
        #define GSM_TXBITCLKTX_COMPLETE    GSM_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define GSM_TXBITCTR_PERIOD        GSM_TXBITCTR_PERIOD_REG
        #define GSM_TXBITCTR_CONTROL       GSM_TXBITCTR_CONTROL_REG
        #define GSM_TXBITCTR_COUNTER       GSM_TXBITCTR_COUNTER_REG
    #endif /* GSM_TXCLKGEN_DP */
#endif /* End GSM_TX_ENABLED */

#if(GSM_HD_ENABLED)
    #define GSM_TXDATA                 GSM_TXDATA_REG
    #define GSM_TXSTATUS               GSM_TXSTATUS_REG
    #define GSM_TXSTATUS_MASK          GSM_TXSTATUS_MASK_REG
    #define GSM_TXSTATUS_ACTL          GSM_TXSTATUS_ACTL_REG
#endif /* End GSM_HD_ENABLED */

#if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )
    #define GSM_RXDATA                 GSM_RXDATA_REG
    #define GSM_RXADDRESS1             GSM_RXADDRESS1_REG
    #define GSM_RXADDRESS2             GSM_RXADDRESS2_REG
    #define GSM_RXBITCTR_PERIOD        GSM_RXBITCTR_PERIOD_REG
    #define GSM_RXBITCTR_CONTROL       GSM_RXBITCTR_CONTROL_REG
    #define GSM_RXBITCTR_COUNTER       GSM_RXBITCTR_COUNTER_REG
    #define GSM_RXSTATUS               GSM_RXSTATUS_REG
    #define GSM_RXSTATUS_MASK          GSM_RXSTATUS_MASK_REG
    #define GSM_RXSTATUS_ACTL          GSM_RXSTATUS_ACTL_REG
#endif /* End  (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */

#if(GSM_INTERNAL_CLOCK_USED)
    #define GSM_INTCLOCK_CLKEN         GSM_INTCLOCK_CLKEN_REG
#endif /* End GSM_INTERNAL_CLOCK_USED */

#define GSM_WAIT_FOR_COMLETE_REINIT    GSM_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_GSM_H */


/* [] END OF FILE */
