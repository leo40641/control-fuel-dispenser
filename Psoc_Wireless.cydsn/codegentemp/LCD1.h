/*******************************************************************************
* File Name: LCD1.h
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


#if !defined(CY_UART_LCD1_H)
#define CY_UART_LCD1_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define LCD1_RX_ENABLED                     (1u)
#define LCD1_TX_ENABLED                     (1u)
#define LCD1_HD_ENABLED                     (0u)
#define LCD1_RX_INTERRUPT_ENABLED           (1u)
#define LCD1_TX_INTERRUPT_ENABLED           (0u)
#define LCD1_INTERNAL_CLOCK_USED            (1u)
#define LCD1_RXHW_ADDRESS_ENABLED           (0u)
#define LCD1_OVER_SAMPLE_COUNT              (8u)
#define LCD1_PARITY_TYPE                    (0u)
#define LCD1_PARITY_TYPE_SW                 (0u)
#define LCD1_BREAK_DETECT                   (0u)
#define LCD1_BREAK_BITS_TX                  (13u)
#define LCD1_BREAK_BITS_RX                  (13u)
#define LCD1_TXCLKGEN_DP                    (1u)
#define LCD1_USE23POLLING                   (1u)
#define LCD1_FLOW_CONTROL                   (0u)
#define LCD1_CLK_FREQ                       (0u)
#define LCD1_TX_BUFFER_SIZE                 (4u)
#define LCD1_RX_BUFFER_SIZE                 (50u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define LCD1_CONTROL_REG_REMOVED            (0u)
#else
    #define LCD1_CONTROL_REG_REMOVED            (1u)
#endif /* End LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct LCD1_backupStruct_
{
    uint8 enableState;

    #if(LCD1_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End LCD1_CONTROL_REG_REMOVED */

} LCD1_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void LCD1_Start(void) ;
void LCD1_Stop(void) ;
uint8 LCD1_ReadControlRegister(void) ;
void LCD1_WriteControlRegister(uint8 control) ;

void LCD1_Init(void) ;
void LCD1_Enable(void) ;
void LCD1_SaveConfig(void) ;
void LCD1_RestoreConfig(void) ;
void LCD1_Sleep(void) ;
void LCD1_Wakeup(void) ;

/* Only if RX is enabled */
#if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )

    #if (LCD1_RX_INTERRUPT_ENABLED)
        #define LCD1_EnableRxInt()  CyIntEnable (LCD1_RX_VECT_NUM)
        #define LCD1_DisableRxInt() CyIntDisable(LCD1_RX_VECT_NUM)
        CY_ISR_PROTO(LCD1_RXISR);
    #endif /* LCD1_RX_INTERRUPT_ENABLED */

    void LCD1_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void LCD1_SetRxAddress1(uint8 address) ;
    void LCD1_SetRxAddress2(uint8 address) ;

    void  LCD1_SetRxInterruptMode(uint8 intSrc) ;
    uint8 LCD1_ReadRxData(void) ;
    uint8 LCD1_ReadRxStatus(void) ;
    uint8 LCD1_GetChar(void) ;
    uint16 LCD1_GetByte(void) ;
    uint8 LCD1_GetRxBufferSize(void)
                                                            ;
    void LCD1_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define LCD1_GetRxInterruptSource   LCD1_ReadRxStatus

#endif /* End (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */

/* Only if TX is enabled */
#if(LCD1_TX_ENABLED || LCD1_HD_ENABLED)

    #if(LCD1_TX_INTERRUPT_ENABLED)
        #define LCD1_EnableTxInt()  CyIntEnable (LCD1_TX_VECT_NUM)
        #define LCD1_DisableTxInt() CyIntDisable(LCD1_TX_VECT_NUM)
        #define LCD1_SetPendingTxInt() CyIntSetPending(LCD1_TX_VECT_NUM)
        #define LCD1_ClearPendingTxInt() CyIntClearPending(LCD1_TX_VECT_NUM)
        CY_ISR_PROTO(LCD1_TXISR);
    #endif /* LCD1_TX_INTERRUPT_ENABLED */

    void LCD1_SetTxInterruptMode(uint8 intSrc) ;
    void LCD1_WriteTxData(uint8 txDataByte) ;
    uint8 LCD1_ReadTxStatus(void) ;
    void LCD1_PutChar(uint8 txDataByte) ;
    void LCD1_PutString(const char8 string[]) ;
    void LCD1_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void LCD1_PutCRLF(uint8 txDataByte) ;
    void LCD1_ClearTxBuffer(void) ;
    void LCD1_SetTxAddressMode(uint8 addressMode) ;
    void LCD1_SendBreak(uint8 retMode) ;
    uint8 LCD1_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define LCD1_PutStringConst         LCD1_PutString
    #define LCD1_PutArrayConst          LCD1_PutArray
    #define LCD1_GetTxInterruptSource   LCD1_ReadTxStatus

#endif /* End LCD1_TX_ENABLED || LCD1_HD_ENABLED */

#if(LCD1_HD_ENABLED)
    void LCD1_LoadRxConfig(void) ;
    void LCD1_LoadTxConfig(void) ;
#endif /* End LCD1_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LCD1) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    LCD1_CyBtldrCommStart(void) CYSMALL ;
    void    LCD1_CyBtldrCommStop(void) CYSMALL ;
    void    LCD1_CyBtldrCommReset(void) CYSMALL ;
    cystatus LCD1_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus LCD1_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LCD1)
        #define CyBtldrCommStart    LCD1_CyBtldrCommStart
        #define CyBtldrCommStop     LCD1_CyBtldrCommStop
        #define CyBtldrCommReset    LCD1_CyBtldrCommReset
        #define CyBtldrCommWrite    LCD1_CyBtldrCommWrite
        #define CyBtldrCommRead     LCD1_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LCD1) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define LCD1_BYTE2BYTE_TIME_OUT (25u)
    #define LCD1_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define LCD1_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define LCD1_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define LCD1_SET_SPACE      (0x00u)
#define LCD1_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (LCD1_TX_ENABLED) || (LCD1_HD_ENABLED) )
    #if(LCD1_TX_INTERRUPT_ENABLED)
        #define LCD1_TX_VECT_NUM            (uint8)LCD1_TXInternalInterrupt__INTC_NUMBER
        #define LCD1_TX_PRIOR_NUM           (uint8)LCD1_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* LCD1_TX_INTERRUPT_ENABLED */

    #define LCD1_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define LCD1_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define LCD1_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(LCD1_TX_ENABLED)
        #define LCD1_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (LCD1_HD_ENABLED) */
        #define LCD1_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (LCD1_TX_ENABLED) */

    #define LCD1_TX_STS_COMPLETE            (uint8)(0x01u << LCD1_TX_STS_COMPLETE_SHIFT)
    #define LCD1_TX_STS_FIFO_EMPTY          (uint8)(0x01u << LCD1_TX_STS_FIFO_EMPTY_SHIFT)
    #define LCD1_TX_STS_FIFO_FULL           (uint8)(0x01u << LCD1_TX_STS_FIFO_FULL_SHIFT)
    #define LCD1_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << LCD1_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (LCD1_TX_ENABLED) || (LCD1_HD_ENABLED)*/

#if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )
    #if(LCD1_RX_INTERRUPT_ENABLED)
        #define LCD1_RX_VECT_NUM            (uint8)LCD1_RXInternalInterrupt__INTC_NUMBER
        #define LCD1_RX_PRIOR_NUM           (uint8)LCD1_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* LCD1_RX_INTERRUPT_ENABLED */
    #define LCD1_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define LCD1_RX_STS_BREAK_SHIFT             (0x01u)
    #define LCD1_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define LCD1_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define LCD1_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define LCD1_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define LCD1_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define LCD1_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define LCD1_RX_STS_MRKSPC           (uint8)(0x01u << LCD1_RX_STS_MRKSPC_SHIFT)
    #define LCD1_RX_STS_BREAK            (uint8)(0x01u << LCD1_RX_STS_BREAK_SHIFT)
    #define LCD1_RX_STS_PAR_ERROR        (uint8)(0x01u << LCD1_RX_STS_PAR_ERROR_SHIFT)
    #define LCD1_RX_STS_STOP_ERROR       (uint8)(0x01u << LCD1_RX_STS_STOP_ERROR_SHIFT)
    #define LCD1_RX_STS_OVERRUN          (uint8)(0x01u << LCD1_RX_STS_OVERRUN_SHIFT)
    #define LCD1_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << LCD1_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define LCD1_RX_STS_ADDR_MATCH       (uint8)(0x01u << LCD1_RX_STS_ADDR_MATCH_SHIFT)
    #define LCD1_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << LCD1_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define LCD1_RX_HW_MASK                     (0x7Fu)
#endif /* End (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */

/* Control Register definitions */
#define LCD1_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define LCD1_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define LCD1_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define LCD1_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define LCD1_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define LCD1_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define LCD1_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define LCD1_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define LCD1_CTRL_HD_SEND               (uint8)(0x01u << LCD1_CTRL_HD_SEND_SHIFT)
#define LCD1_CTRL_HD_SEND_BREAK         (uint8)(0x01u << LCD1_CTRL_HD_SEND_BREAK_SHIFT)
#define LCD1_CTRL_MARK                  (uint8)(0x01u << LCD1_CTRL_MARK_SHIFT)
#define LCD1_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << LCD1_CTRL_PARITY_TYPE0_SHIFT)
#define LCD1_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << LCD1_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define LCD1_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define LCD1_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define LCD1_SEND_BREAK                         (0x00u)
#define LCD1_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define LCD1_REINIT                             (0x02u)
#define LCD1_SEND_WAIT_REINIT                   (0x03u)

#define LCD1_OVER_SAMPLE_8                      (8u)
#define LCD1_OVER_SAMPLE_16                     (16u)

#define LCD1_BIT_CENTER                         (LCD1_OVER_SAMPLE_COUNT - 2u)

#define LCD1_FIFO_LENGTH                        (4u)
#define LCD1_NUMBER_OF_START_BIT                (1u)
#define LCD1_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define LCD1_TXBITCTR_BREAKBITS8X   ((LCD1_BREAK_BITS_TX * LCD1_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define LCD1_TXBITCTR_BREAKBITS ((LCD1_BREAK_BITS_TX * LCD1_OVER_SAMPLE_COUNT) - 1u)

#define LCD1_HALF_BIT_COUNT   \
                            (((LCD1_OVER_SAMPLE_COUNT / 2u) + (LCD1_USE23POLLING * 1u)) - 2u)
#if (LCD1_OVER_SAMPLE_COUNT == LCD1_OVER_SAMPLE_8)
    #define LCD1_HD_TXBITCTR_INIT   (((LCD1_BREAK_BITS_TX + \
                            LCD1_NUMBER_OF_START_BIT) * LCD1_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define LCD1_RXBITCTR_INIT  ((((LCD1_BREAK_BITS_RX + LCD1_NUMBER_OF_START_BIT) \
                            * LCD1_OVER_SAMPLE_COUNT) + LCD1_HALF_BIT_COUNT) - 1u)

#else /* LCD1_OVER_SAMPLE_COUNT == LCD1_OVER_SAMPLE_16 */
    #define LCD1_HD_TXBITCTR_INIT   ((8u * LCD1_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define LCD1_RXBITCTR_INIT      (((7u * LCD1_OVER_SAMPLE_COUNT) - 1u) + \
                                                      LCD1_HALF_BIT_COUNT)
#endif /* End LCD1_OVER_SAMPLE_COUNT */

#define LCD1_HD_RXBITCTR_INIT                   LCD1_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 LCD1_initVar;
#if (LCD1_TX_INTERRUPT_ENABLED && LCD1_TX_ENABLED)
    extern volatile uint8 LCD1_txBuffer[LCD1_TX_BUFFER_SIZE];
    extern volatile uint8 LCD1_txBufferRead;
    extern uint8 LCD1_txBufferWrite;
#endif /* (LCD1_TX_INTERRUPT_ENABLED && LCD1_TX_ENABLED) */
#if (LCD1_RX_INTERRUPT_ENABLED && (LCD1_RX_ENABLED || LCD1_HD_ENABLED))
    extern uint8 LCD1_errorStatus;
    extern volatile uint8 LCD1_rxBuffer[LCD1_RX_BUFFER_SIZE];
    extern volatile uint8 LCD1_rxBufferRead;
    extern volatile uint8 LCD1_rxBufferWrite;
    extern volatile uint8 LCD1_rxBufferLoopDetect;
    extern volatile uint8 LCD1_rxBufferOverflow;
    #if (LCD1_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 LCD1_rxAddressMode;
        extern volatile uint8 LCD1_rxAddressDetected;
    #endif /* (LCD1_RXHW_ADDRESS_ENABLED) */
#endif /* (LCD1_RX_INTERRUPT_ENABLED && (LCD1_RX_ENABLED || LCD1_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define LCD1__B_UART__AM_SW_BYTE_BYTE 1
#define LCD1__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define LCD1__B_UART__AM_HW_BYTE_BY_BYTE 3
#define LCD1__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define LCD1__B_UART__AM_NONE 0

#define LCD1__B_UART__NONE_REVB 0
#define LCD1__B_UART__EVEN_REVB 1
#define LCD1__B_UART__ODD_REVB 2
#define LCD1__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define LCD1_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define LCD1_NUMBER_OF_STOP_BITS    (1u)

#if (LCD1_RXHW_ADDRESS_ENABLED)
    #define LCD1_RX_ADDRESS_MODE    (0u)
    #define LCD1_RX_HW_ADDRESS1     (0u)
    #define LCD1_RX_HW_ADDRESS2     (0u)
#endif /* (LCD1_RXHW_ADDRESS_ENABLED) */

#define LCD1_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << LCD1_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << LCD1_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << LCD1_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << LCD1_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << LCD1_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << LCD1_RX_STS_BREAK_SHIFT) \
                                        | (0 << LCD1_RX_STS_OVERRUN_SHIFT))

#define LCD1_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << LCD1_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << LCD1_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << LCD1_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << LCD1_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define LCD1_CONTROL_REG \
                            (* (reg8 *) LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define LCD1_CONTROL_PTR \
                            (  (reg8 *) LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(LCD1_TX_ENABLED)
    #define LCD1_TXDATA_REG          (* (reg8 *) LCD1_BUART_sTX_TxShifter_u0__F0_REG)
    #define LCD1_TXDATA_PTR          (  (reg8 *) LCD1_BUART_sTX_TxShifter_u0__F0_REG)
    #define LCD1_TXDATA_AUX_CTL_REG  (* (reg8 *) LCD1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define LCD1_TXDATA_AUX_CTL_PTR  (  (reg8 *) LCD1_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define LCD1_TXSTATUS_REG        (* (reg8 *) LCD1_BUART_sTX_TxSts__STATUS_REG)
    #define LCD1_TXSTATUS_PTR        (  (reg8 *) LCD1_BUART_sTX_TxSts__STATUS_REG)
    #define LCD1_TXSTATUS_MASK_REG   (* (reg8 *) LCD1_BUART_sTX_TxSts__MASK_REG)
    #define LCD1_TXSTATUS_MASK_PTR   (  (reg8 *) LCD1_BUART_sTX_TxSts__MASK_REG)
    #define LCD1_TXSTATUS_ACTL_REG   (* (reg8 *) LCD1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define LCD1_TXSTATUS_ACTL_PTR   (  (reg8 *) LCD1_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(LCD1_TXCLKGEN_DP)
        #define LCD1_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define LCD1_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define LCD1_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define LCD1_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define LCD1_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define LCD1_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define LCD1_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define LCD1_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define LCD1_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define LCD1_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) LCD1_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* LCD1_TXCLKGEN_DP */

#endif /* End LCD1_TX_ENABLED */

#if(LCD1_HD_ENABLED)

    #define LCD1_TXDATA_REG             (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__F1_REG )
    #define LCD1_TXDATA_PTR             (  (reg8 *) LCD1_BUART_sRX_RxShifter_u0__F1_REG )
    #define LCD1_TXDATA_AUX_CTL_REG     (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define LCD1_TXDATA_AUX_CTL_PTR     (  (reg8 *) LCD1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define LCD1_TXSTATUS_REG           (* (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_REG )
    #define LCD1_TXSTATUS_PTR           (  (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_REG )
    #define LCD1_TXSTATUS_MASK_REG      (* (reg8 *) LCD1_BUART_sRX_RxSts__MASK_REG )
    #define LCD1_TXSTATUS_MASK_PTR      (  (reg8 *) LCD1_BUART_sRX_RxSts__MASK_REG )
    #define LCD1_TXSTATUS_ACTL_REG      (* (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define LCD1_TXSTATUS_ACTL_PTR      (  (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End LCD1_HD_ENABLED */

#if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )
    #define LCD1_RXDATA_REG             (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__F0_REG )
    #define LCD1_RXDATA_PTR             (  (reg8 *) LCD1_BUART_sRX_RxShifter_u0__F0_REG )
    #define LCD1_RXADDRESS1_REG         (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__D0_REG )
    #define LCD1_RXADDRESS1_PTR         (  (reg8 *) LCD1_BUART_sRX_RxShifter_u0__D0_REG )
    #define LCD1_RXADDRESS2_REG         (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__D1_REG )
    #define LCD1_RXADDRESS2_PTR         (  (reg8 *) LCD1_BUART_sRX_RxShifter_u0__D1_REG )
    #define LCD1_RXDATA_AUX_CTL_REG     (* (reg8 *) LCD1_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define LCD1_RXBITCTR_PERIOD_REG    (* (reg8 *) LCD1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define LCD1_RXBITCTR_PERIOD_PTR    (  (reg8 *) LCD1_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define LCD1_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) LCD1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define LCD1_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) LCD1_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define LCD1_RXBITCTR_COUNTER_REG   (* (reg8 *) LCD1_BUART_sRX_RxBitCounter__COUNT_REG )
    #define LCD1_RXBITCTR_COUNTER_PTR   (  (reg8 *) LCD1_BUART_sRX_RxBitCounter__COUNT_REG )

    #define LCD1_RXSTATUS_REG           (* (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_REG )
    #define LCD1_RXSTATUS_PTR           (  (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_REG )
    #define LCD1_RXSTATUS_MASK_REG      (* (reg8 *) LCD1_BUART_sRX_RxSts__MASK_REG )
    #define LCD1_RXSTATUS_MASK_PTR      (  (reg8 *) LCD1_BUART_sRX_RxSts__MASK_REG )
    #define LCD1_RXSTATUS_ACTL_REG      (* (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define LCD1_RXSTATUS_ACTL_PTR      (  (reg8 *) LCD1_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */

#if(LCD1_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define LCD1_INTCLOCK_CLKEN_REG     (* (reg8 *) LCD1_IntClock__PM_ACT_CFG)
    #define LCD1_INTCLOCK_CLKEN_PTR     (  (reg8 *) LCD1_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define LCD1_INTCLOCK_CLKEN_MASK    LCD1_IntClock__PM_ACT_MSK
#endif /* End LCD1_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(LCD1_TX_ENABLED)
    #define LCD1_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End LCD1_TX_ENABLED */

#if(LCD1_HD_ENABLED)
    #define LCD1_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End LCD1_HD_ENABLED */

#if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )
    #define LCD1_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define LCD1_WAIT_1_MS      LCD1_BL_CHK_DELAY_MS   

#define LCD1_TXBUFFERSIZE   LCD1_TX_BUFFER_SIZE
#define LCD1_RXBUFFERSIZE   LCD1_RX_BUFFER_SIZE

#if (LCD1_RXHW_ADDRESS_ENABLED)
    #define LCD1_RXADDRESSMODE  LCD1_RX_ADDRESS_MODE
    #define LCD1_RXHWADDRESS1   LCD1_RX_HW_ADDRESS1
    #define LCD1_RXHWADDRESS2   LCD1_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define LCD1_RXAddressMode  LCD1_RXADDRESSMODE
#endif /* (LCD1_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define LCD1_initvar                    LCD1_initVar

#define LCD1_RX_Enabled                 LCD1_RX_ENABLED
#define LCD1_TX_Enabled                 LCD1_TX_ENABLED
#define LCD1_HD_Enabled                 LCD1_HD_ENABLED
#define LCD1_RX_IntInterruptEnabled     LCD1_RX_INTERRUPT_ENABLED
#define LCD1_TX_IntInterruptEnabled     LCD1_TX_INTERRUPT_ENABLED
#define LCD1_InternalClockUsed          LCD1_INTERNAL_CLOCK_USED
#define LCD1_RXHW_Address_Enabled       LCD1_RXHW_ADDRESS_ENABLED
#define LCD1_OverSampleCount            LCD1_OVER_SAMPLE_COUNT
#define LCD1_ParityType                 LCD1_PARITY_TYPE

#if( LCD1_TX_ENABLED && (LCD1_TXBUFFERSIZE > LCD1_FIFO_LENGTH))
    #define LCD1_TXBUFFER               LCD1_txBuffer
    #define LCD1_TXBUFFERREAD           LCD1_txBufferRead
    #define LCD1_TXBUFFERWRITE          LCD1_txBufferWrite
#endif /* End LCD1_TX_ENABLED */
#if( ( LCD1_RX_ENABLED || LCD1_HD_ENABLED ) && \
     (LCD1_RXBUFFERSIZE > LCD1_FIFO_LENGTH) )
    #define LCD1_RXBUFFER               LCD1_rxBuffer
    #define LCD1_RXBUFFERREAD           LCD1_rxBufferRead
    #define LCD1_RXBUFFERWRITE          LCD1_rxBufferWrite
    #define LCD1_RXBUFFERLOOPDETECT     LCD1_rxBufferLoopDetect
    #define LCD1_RXBUFFER_OVERFLOW      LCD1_rxBufferOverflow
#endif /* End LCD1_RX_ENABLED */

#ifdef LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define LCD1_CONTROL                LCD1_CONTROL_REG
#endif /* End LCD1_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(LCD1_TX_ENABLED)
    #define LCD1_TXDATA                 LCD1_TXDATA_REG
    #define LCD1_TXSTATUS               LCD1_TXSTATUS_REG
    #define LCD1_TXSTATUS_MASK          LCD1_TXSTATUS_MASK_REG
    #define LCD1_TXSTATUS_ACTL          LCD1_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(LCD1_TXCLKGEN_DP)
        #define LCD1_TXBITCLKGEN_CTR        LCD1_TXBITCLKGEN_CTR_REG
        #define LCD1_TXBITCLKTX_COMPLETE    LCD1_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define LCD1_TXBITCTR_PERIOD        LCD1_TXBITCTR_PERIOD_REG
        #define LCD1_TXBITCTR_CONTROL       LCD1_TXBITCTR_CONTROL_REG
        #define LCD1_TXBITCTR_COUNTER       LCD1_TXBITCTR_COUNTER_REG
    #endif /* LCD1_TXCLKGEN_DP */
#endif /* End LCD1_TX_ENABLED */

#if(LCD1_HD_ENABLED)
    #define LCD1_TXDATA                 LCD1_TXDATA_REG
    #define LCD1_TXSTATUS               LCD1_TXSTATUS_REG
    #define LCD1_TXSTATUS_MASK          LCD1_TXSTATUS_MASK_REG
    #define LCD1_TXSTATUS_ACTL          LCD1_TXSTATUS_ACTL_REG
#endif /* End LCD1_HD_ENABLED */

#if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )
    #define LCD1_RXDATA                 LCD1_RXDATA_REG
    #define LCD1_RXADDRESS1             LCD1_RXADDRESS1_REG
    #define LCD1_RXADDRESS2             LCD1_RXADDRESS2_REG
    #define LCD1_RXBITCTR_PERIOD        LCD1_RXBITCTR_PERIOD_REG
    #define LCD1_RXBITCTR_CONTROL       LCD1_RXBITCTR_CONTROL_REG
    #define LCD1_RXBITCTR_COUNTER       LCD1_RXBITCTR_COUNTER_REG
    #define LCD1_RXSTATUS               LCD1_RXSTATUS_REG
    #define LCD1_RXSTATUS_MASK          LCD1_RXSTATUS_MASK_REG
    #define LCD1_RXSTATUS_ACTL          LCD1_RXSTATUS_ACTL_REG
#endif /* End  (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */

#if(LCD1_INTERNAL_CLOCK_USED)
    #define LCD1_INTCLOCK_CLKEN         LCD1_INTCLOCK_CLKEN_REG
#endif /* End LCD1_INTERNAL_CLOCK_USED */

#define LCD1_WAIT_FOR_COMLETE_REINIT    LCD1_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_LCD1_H */


/* [] END OF FILE */
