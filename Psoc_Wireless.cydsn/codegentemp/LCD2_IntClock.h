/*******************************************************************************
* File Name: LCD2_IntClock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_LCD2_IntClock_H)
#define CY_CLOCK_LCD2_IntClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void LCD2_IntClock_Start(void) ;
void LCD2_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void LCD2_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void LCD2_IntClock_StandbyPower(uint8 state) ;
void LCD2_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 LCD2_IntClock_GetDividerRegister(void) ;
void LCD2_IntClock_SetModeRegister(uint8 modeBitMask) ;
void LCD2_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 LCD2_IntClock_GetModeRegister(void) ;
void LCD2_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 LCD2_IntClock_GetSourceRegister(void) ;
#if defined(LCD2_IntClock__CFG3)
void LCD2_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 LCD2_IntClock_GetPhaseRegister(void) ;
#endif /* defined(LCD2_IntClock__CFG3) */

#define LCD2_IntClock_Enable()                       LCD2_IntClock_Start()
#define LCD2_IntClock_Disable()                      LCD2_IntClock_Stop()
#define LCD2_IntClock_SetDivider(clkDivider)         LCD2_IntClock_SetDividerRegister(clkDivider, 1u)
#define LCD2_IntClock_SetDividerValue(clkDivider)    LCD2_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define LCD2_IntClock_SetMode(clkMode)               LCD2_IntClock_SetModeRegister(clkMode)
#define LCD2_IntClock_SetSource(clkSource)           LCD2_IntClock_SetSourceRegister(clkSource)
#if defined(LCD2_IntClock__CFG3)
#define LCD2_IntClock_SetPhase(clkPhase)             LCD2_IntClock_SetPhaseRegister(clkPhase)
#define LCD2_IntClock_SetPhaseValue(clkPhase)        LCD2_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(LCD2_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define LCD2_IntClock_CLKEN              (* (reg8 *) LCD2_IntClock__PM_ACT_CFG)
#define LCD2_IntClock_CLKEN_PTR          ((reg8 *) LCD2_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define LCD2_IntClock_CLKSTBY            (* (reg8 *) LCD2_IntClock__PM_STBY_CFG)
#define LCD2_IntClock_CLKSTBY_PTR        ((reg8 *) LCD2_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define LCD2_IntClock_DIV_LSB            (* (reg8 *) LCD2_IntClock__CFG0)
#define LCD2_IntClock_DIV_LSB_PTR        ((reg8 *) LCD2_IntClock__CFG0)
#define LCD2_IntClock_DIV_PTR            ((reg16 *) LCD2_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define LCD2_IntClock_DIV_MSB            (* (reg8 *) LCD2_IntClock__CFG1)
#define LCD2_IntClock_DIV_MSB_PTR        ((reg8 *) LCD2_IntClock__CFG1)

/* Mode and source configuration register */
#define LCD2_IntClock_MOD_SRC            (* (reg8 *) LCD2_IntClock__CFG2)
#define LCD2_IntClock_MOD_SRC_PTR        ((reg8 *) LCD2_IntClock__CFG2)

#if defined(LCD2_IntClock__CFG3)
/* Analog clock phase configuration register */
#define LCD2_IntClock_PHASE              (* (reg8 *) LCD2_IntClock__CFG3)
#define LCD2_IntClock_PHASE_PTR          ((reg8 *) LCD2_IntClock__CFG3)
#endif /* defined(LCD2_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define LCD2_IntClock_CLKEN_MASK         LCD2_IntClock__PM_ACT_MSK
#define LCD2_IntClock_CLKSTBY_MASK       LCD2_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define LCD2_IntClock_SRC_SEL_MSK        LCD2_IntClock__CFG2_SRC_SEL_MASK
#define LCD2_IntClock_MODE_MASK          (~(LCD2_IntClock_SRC_SEL_MSK))

#if defined(LCD2_IntClock__CFG3)
/* CFG3 phase mask */
#define LCD2_IntClock_PHASE_MASK         LCD2_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(LCD2_IntClock__CFG3) */

#endif /* CY_CLOCK_LCD2_IntClock_H */


/* [] END OF FILE */
