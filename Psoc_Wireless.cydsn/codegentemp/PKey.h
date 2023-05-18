/*******************************************************************************
* File Name: PKey.h  
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

#if !defined(CY_PINS_PKey_H) /* Pins PKey_H */
#define CY_PINS_PKey_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "PKey_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 PKey__PORT == 15 && ((PKey__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    PKey_Write(uint8 value);
void    PKey_SetDriveMode(uint8 mode);
uint8   PKey_ReadDataReg(void);
uint8   PKey_Read(void);
void    PKey_SetInterruptMode(uint16 position, uint16 mode);
uint8   PKey_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the PKey_SetDriveMode() function.
     *  @{
     */
        #define PKey_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define PKey_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define PKey_DM_RES_UP          PIN_DM_RES_UP
        #define PKey_DM_RES_DWN         PIN_DM_RES_DWN
        #define PKey_DM_OD_LO           PIN_DM_OD_LO
        #define PKey_DM_OD_HI           PIN_DM_OD_HI
        #define PKey_DM_STRONG          PIN_DM_STRONG
        #define PKey_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define PKey_MASK               PKey__MASK
#define PKey_SHIFT              PKey__SHIFT
#define PKey_WIDTH              1u

/* Interrupt constants */
#if defined(PKey__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in PKey_SetInterruptMode() function.
     *  @{
     */
        #define PKey_INTR_NONE      (uint16)(0x0000u)
        #define PKey_INTR_RISING    (uint16)(0x0001u)
        #define PKey_INTR_FALLING   (uint16)(0x0002u)
        #define PKey_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define PKey_INTR_MASK      (0x01u) 
#endif /* (PKey__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define PKey_PS                     (* (reg8 *) PKey__PS)
/* Data Register */
#define PKey_DR                     (* (reg8 *) PKey__DR)
/* Port Number */
#define PKey_PRT_NUM                (* (reg8 *) PKey__PRT) 
/* Connect to Analog Globals */                                                  
#define PKey_AG                     (* (reg8 *) PKey__AG)                       
/* Analog MUX bux enable */
#define PKey_AMUX                   (* (reg8 *) PKey__AMUX) 
/* Bidirectional Enable */                                                        
#define PKey_BIE                    (* (reg8 *) PKey__BIE)
/* Bit-mask for Aliased Register Access */
#define PKey_BIT_MASK               (* (reg8 *) PKey__BIT_MASK)
/* Bypass Enable */
#define PKey_BYP                    (* (reg8 *) PKey__BYP)
/* Port wide control signals */                                                   
#define PKey_CTL                    (* (reg8 *) PKey__CTL)
/* Drive Modes */
#define PKey_DM0                    (* (reg8 *) PKey__DM0) 
#define PKey_DM1                    (* (reg8 *) PKey__DM1)
#define PKey_DM2                    (* (reg8 *) PKey__DM2) 
/* Input Buffer Disable Override */
#define PKey_INP_DIS                (* (reg8 *) PKey__INP_DIS)
/* LCD Common or Segment Drive */
#define PKey_LCD_COM_SEG            (* (reg8 *) PKey__LCD_COM_SEG)
/* Enable Segment LCD */
#define PKey_LCD_EN                 (* (reg8 *) PKey__LCD_EN)
/* Slew Rate Control */
#define PKey_SLW                    (* (reg8 *) PKey__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define PKey_PRTDSI__CAPS_SEL       (* (reg8 *) PKey__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define PKey_PRTDSI__DBL_SYNC_IN    (* (reg8 *) PKey__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define PKey_PRTDSI__OE_SEL0        (* (reg8 *) PKey__PRTDSI__OE_SEL0) 
#define PKey_PRTDSI__OE_SEL1        (* (reg8 *) PKey__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define PKey_PRTDSI__OUT_SEL0       (* (reg8 *) PKey__PRTDSI__OUT_SEL0) 
#define PKey_PRTDSI__OUT_SEL1       (* (reg8 *) PKey__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define PKey_PRTDSI__SYNC_OUT       (* (reg8 *) PKey__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(PKey__SIO_CFG)
    #define PKey_SIO_HYST_EN        (* (reg8 *) PKey__SIO_HYST_EN)
    #define PKey_SIO_REG_HIFREQ     (* (reg8 *) PKey__SIO_REG_HIFREQ)
    #define PKey_SIO_CFG            (* (reg8 *) PKey__SIO_CFG)
    #define PKey_SIO_DIFF           (* (reg8 *) PKey__SIO_DIFF)
#endif /* (PKey__SIO_CFG) */

/* Interrupt Registers */
#if defined(PKey__INTSTAT)
    #define PKey_INTSTAT            (* (reg8 *) PKey__INTSTAT)
    #define PKey_SNAP               (* (reg8 *) PKey__SNAP)
    
	#define PKey_0_INTTYPE_REG 		(* (reg8 *) PKey__0__INTTYPE)
#endif /* (PKey__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_PKey_H */


/* [] END OF FILE */
