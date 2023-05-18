/*******************************************************************************
* File Name: ss0.h  
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

#if !defined(CY_PINS_ss0_H) /* Pins ss0_H */
#define CY_PINS_ss0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ss0_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ss0__PORT == 15 && ((ss0__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ss0_Write(uint8 value);
void    ss0_SetDriveMode(uint8 mode);
uint8   ss0_ReadDataReg(void);
uint8   ss0_Read(void);
void    ss0_SetInterruptMode(uint16 position, uint16 mode);
uint8   ss0_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ss0_SetDriveMode() function.
     *  @{
     */
        #define ss0_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ss0_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ss0_DM_RES_UP          PIN_DM_RES_UP
        #define ss0_DM_RES_DWN         PIN_DM_RES_DWN
        #define ss0_DM_OD_LO           PIN_DM_OD_LO
        #define ss0_DM_OD_HI           PIN_DM_OD_HI
        #define ss0_DM_STRONG          PIN_DM_STRONG
        #define ss0_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ss0_MASK               ss0__MASK
#define ss0_SHIFT              ss0__SHIFT
#define ss0_WIDTH              1u

/* Interrupt constants */
#if defined(ss0__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ss0_SetInterruptMode() function.
     *  @{
     */
        #define ss0_INTR_NONE      (uint16)(0x0000u)
        #define ss0_INTR_RISING    (uint16)(0x0001u)
        #define ss0_INTR_FALLING   (uint16)(0x0002u)
        #define ss0_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ss0_INTR_MASK      (0x01u) 
#endif /* (ss0__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ss0_PS                     (* (reg8 *) ss0__PS)
/* Data Register */
#define ss0_DR                     (* (reg8 *) ss0__DR)
/* Port Number */
#define ss0_PRT_NUM                (* (reg8 *) ss0__PRT) 
/* Connect to Analog Globals */                                                  
#define ss0_AG                     (* (reg8 *) ss0__AG)                       
/* Analog MUX bux enable */
#define ss0_AMUX                   (* (reg8 *) ss0__AMUX) 
/* Bidirectional Enable */                                                        
#define ss0_BIE                    (* (reg8 *) ss0__BIE)
/* Bit-mask for Aliased Register Access */
#define ss0_BIT_MASK               (* (reg8 *) ss0__BIT_MASK)
/* Bypass Enable */
#define ss0_BYP                    (* (reg8 *) ss0__BYP)
/* Port wide control signals */                                                   
#define ss0_CTL                    (* (reg8 *) ss0__CTL)
/* Drive Modes */
#define ss0_DM0                    (* (reg8 *) ss0__DM0) 
#define ss0_DM1                    (* (reg8 *) ss0__DM1)
#define ss0_DM2                    (* (reg8 *) ss0__DM2) 
/* Input Buffer Disable Override */
#define ss0_INP_DIS                (* (reg8 *) ss0__INP_DIS)
/* LCD Common or Segment Drive */
#define ss0_LCD_COM_SEG            (* (reg8 *) ss0__LCD_COM_SEG)
/* Enable Segment LCD */
#define ss0_LCD_EN                 (* (reg8 *) ss0__LCD_EN)
/* Slew Rate Control */
#define ss0_SLW                    (* (reg8 *) ss0__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ss0_PRTDSI__CAPS_SEL       (* (reg8 *) ss0__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ss0_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ss0__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ss0_PRTDSI__OE_SEL0        (* (reg8 *) ss0__PRTDSI__OE_SEL0) 
#define ss0_PRTDSI__OE_SEL1        (* (reg8 *) ss0__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ss0_PRTDSI__OUT_SEL0       (* (reg8 *) ss0__PRTDSI__OUT_SEL0) 
#define ss0_PRTDSI__OUT_SEL1       (* (reg8 *) ss0__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ss0_PRTDSI__SYNC_OUT       (* (reg8 *) ss0__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ss0__SIO_CFG)
    #define ss0_SIO_HYST_EN        (* (reg8 *) ss0__SIO_HYST_EN)
    #define ss0_SIO_REG_HIFREQ     (* (reg8 *) ss0__SIO_REG_HIFREQ)
    #define ss0_SIO_CFG            (* (reg8 *) ss0__SIO_CFG)
    #define ss0_SIO_DIFF           (* (reg8 *) ss0__SIO_DIFF)
#endif /* (ss0__SIO_CFG) */

/* Interrupt Registers */
#if defined(ss0__INTSTAT)
    #define ss0_INTSTAT            (* (reg8 *) ss0__INTSTAT)
    #define ss0_SNAP               (* (reg8 *) ss0__SNAP)
    
	#define ss0_0_INTTYPE_REG 		(* (reg8 *) ss0__0__INTTYPE)
#endif /* (ss0__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ss0_H */


/* [] END OF FILE */
