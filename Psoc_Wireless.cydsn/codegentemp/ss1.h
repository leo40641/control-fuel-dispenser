/*******************************************************************************
* File Name: ss1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ss1_H) /* Pins ss1_H */
#define CY_PINS_ss1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ss1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ss1__PORT == 15 && ((ss1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ss1_Write(uint8 value);
void    ss1_SetDriveMode(uint8 mode);
uint8   ss1_ReadDataReg(void);
uint8   ss1_Read(void);
void    ss1_SetInterruptMode(uint16 position, uint16 mode);
uint8   ss1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ss1_SetDriveMode() function.
     *  @{
     */
        #define ss1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ss1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ss1_DM_RES_UP          PIN_DM_RES_UP
        #define ss1_DM_RES_DWN         PIN_DM_RES_DWN
        #define ss1_DM_OD_LO           PIN_DM_OD_LO
        #define ss1_DM_OD_HI           PIN_DM_OD_HI
        #define ss1_DM_STRONG          PIN_DM_STRONG
        #define ss1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ss1_MASK               ss1__MASK
#define ss1_SHIFT              ss1__SHIFT
#define ss1_WIDTH              1u

/* Interrupt constants */
#if defined(ss1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ss1_SetInterruptMode() function.
     *  @{
     */
        #define ss1_INTR_NONE      (uint16)(0x0000u)
        #define ss1_INTR_RISING    (uint16)(0x0001u)
        #define ss1_INTR_FALLING   (uint16)(0x0002u)
        #define ss1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ss1_INTR_MASK      (0x01u) 
#endif /* (ss1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ss1_PS                     (* (reg8 *) ss1__PS)
/* Data Register */
#define ss1_DR                     (* (reg8 *) ss1__DR)
/* Port Number */
#define ss1_PRT_NUM                (* (reg8 *) ss1__PRT) 
/* Connect to Analog Globals */                                                  
#define ss1_AG                     (* (reg8 *) ss1__AG)                       
/* Analog MUX bux enable */
#define ss1_AMUX                   (* (reg8 *) ss1__AMUX) 
/* Bidirectional Enable */                                                        
#define ss1_BIE                    (* (reg8 *) ss1__BIE)
/* Bit-mask for Aliased Register Access */
#define ss1_BIT_MASK               (* (reg8 *) ss1__BIT_MASK)
/* Bypass Enable */
#define ss1_BYP                    (* (reg8 *) ss1__BYP)
/* Port wide control signals */                                                   
#define ss1_CTL                    (* (reg8 *) ss1__CTL)
/* Drive Modes */
#define ss1_DM0                    (* (reg8 *) ss1__DM0) 
#define ss1_DM1                    (* (reg8 *) ss1__DM1)
#define ss1_DM2                    (* (reg8 *) ss1__DM2) 
/* Input Buffer Disable Override */
#define ss1_INP_DIS                (* (reg8 *) ss1__INP_DIS)
/* LCD Common or Segment Drive */
#define ss1_LCD_COM_SEG            (* (reg8 *) ss1__LCD_COM_SEG)
/* Enable Segment LCD */
#define ss1_LCD_EN                 (* (reg8 *) ss1__LCD_EN)
/* Slew Rate Control */
#define ss1_SLW                    (* (reg8 *) ss1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ss1_PRTDSI__CAPS_SEL       (* (reg8 *) ss1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ss1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ss1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ss1_PRTDSI__OE_SEL0        (* (reg8 *) ss1__PRTDSI__OE_SEL0) 
#define ss1_PRTDSI__OE_SEL1        (* (reg8 *) ss1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ss1_PRTDSI__OUT_SEL0       (* (reg8 *) ss1__PRTDSI__OUT_SEL0) 
#define ss1_PRTDSI__OUT_SEL1       (* (reg8 *) ss1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ss1_PRTDSI__SYNC_OUT       (* (reg8 *) ss1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ss1__SIO_CFG)
    #define ss1_SIO_HYST_EN        (* (reg8 *) ss1__SIO_HYST_EN)
    #define ss1_SIO_REG_HIFREQ     (* (reg8 *) ss1__SIO_REG_HIFREQ)
    #define ss1_SIO_CFG            (* (reg8 *) ss1__SIO_CFG)
    #define ss1_SIO_DIFF           (* (reg8 *) ss1__SIO_DIFF)
#endif /* (ss1__SIO_CFG) */

/* Interrupt Registers */
#if defined(ss1__INTSTAT)
    #define ss1_INTSTAT            (* (reg8 *) ss1__INTSTAT)
    #define ss1_SNAP               (* (reg8 *) ss1__SNAP)
    
	#define ss1_0_INTTYPE_REG 		(* (reg8 *) ss1__0__INTTYPE)
#endif /* (ss1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ss1_H */


/* [] END OF FILE */
