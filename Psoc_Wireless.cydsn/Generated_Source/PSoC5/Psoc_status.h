/*******************************************************************************
* File Name: Psoc_status.h  
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

#if !defined(CY_PINS_Psoc_status_H) /* Pins Psoc_status_H */
#define CY_PINS_Psoc_status_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Psoc_status_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Psoc_status__PORT == 15 && ((Psoc_status__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Psoc_status_Write(uint8 value);
void    Psoc_status_SetDriveMode(uint8 mode);
uint8   Psoc_status_ReadDataReg(void);
uint8   Psoc_status_Read(void);
void    Psoc_status_SetInterruptMode(uint16 position, uint16 mode);
uint8   Psoc_status_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Psoc_status_SetDriveMode() function.
     *  @{
     */
        #define Psoc_status_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Psoc_status_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Psoc_status_DM_RES_UP          PIN_DM_RES_UP
        #define Psoc_status_DM_RES_DWN         PIN_DM_RES_DWN
        #define Psoc_status_DM_OD_LO           PIN_DM_OD_LO
        #define Psoc_status_DM_OD_HI           PIN_DM_OD_HI
        #define Psoc_status_DM_STRONG          PIN_DM_STRONG
        #define Psoc_status_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Psoc_status_MASK               Psoc_status__MASK
#define Psoc_status_SHIFT              Psoc_status__SHIFT
#define Psoc_status_WIDTH              2u

/* Interrupt constants */
#if defined(Psoc_status__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Psoc_status_SetInterruptMode() function.
     *  @{
     */
        #define Psoc_status_INTR_NONE      (uint16)(0x0000u)
        #define Psoc_status_INTR_RISING    (uint16)(0x0001u)
        #define Psoc_status_INTR_FALLING   (uint16)(0x0002u)
        #define Psoc_status_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Psoc_status_INTR_MASK      (0x01u) 
#endif /* (Psoc_status__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Psoc_status_PS                     (* (reg8 *) Psoc_status__PS)
/* Data Register */
#define Psoc_status_DR                     (* (reg8 *) Psoc_status__DR)
/* Port Number */
#define Psoc_status_PRT_NUM                (* (reg8 *) Psoc_status__PRT) 
/* Connect to Analog Globals */                                                  
#define Psoc_status_AG                     (* (reg8 *) Psoc_status__AG)                       
/* Analog MUX bux enable */
#define Psoc_status_AMUX                   (* (reg8 *) Psoc_status__AMUX) 
/* Bidirectional Enable */                                                        
#define Psoc_status_BIE                    (* (reg8 *) Psoc_status__BIE)
/* Bit-mask for Aliased Register Access */
#define Psoc_status_BIT_MASK               (* (reg8 *) Psoc_status__BIT_MASK)
/* Bypass Enable */
#define Psoc_status_BYP                    (* (reg8 *) Psoc_status__BYP)
/* Port wide control signals */                                                   
#define Psoc_status_CTL                    (* (reg8 *) Psoc_status__CTL)
/* Drive Modes */
#define Psoc_status_DM0                    (* (reg8 *) Psoc_status__DM0) 
#define Psoc_status_DM1                    (* (reg8 *) Psoc_status__DM1)
#define Psoc_status_DM2                    (* (reg8 *) Psoc_status__DM2) 
/* Input Buffer Disable Override */
#define Psoc_status_INP_DIS                (* (reg8 *) Psoc_status__INP_DIS)
/* LCD Common or Segment Drive */
#define Psoc_status_LCD_COM_SEG            (* (reg8 *) Psoc_status__LCD_COM_SEG)
/* Enable Segment LCD */
#define Psoc_status_LCD_EN                 (* (reg8 *) Psoc_status__LCD_EN)
/* Slew Rate Control */
#define Psoc_status_SLW                    (* (reg8 *) Psoc_status__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Psoc_status_PRTDSI__CAPS_SEL       (* (reg8 *) Psoc_status__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Psoc_status_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Psoc_status__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Psoc_status_PRTDSI__OE_SEL0        (* (reg8 *) Psoc_status__PRTDSI__OE_SEL0) 
#define Psoc_status_PRTDSI__OE_SEL1        (* (reg8 *) Psoc_status__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Psoc_status_PRTDSI__OUT_SEL0       (* (reg8 *) Psoc_status__PRTDSI__OUT_SEL0) 
#define Psoc_status_PRTDSI__OUT_SEL1       (* (reg8 *) Psoc_status__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Psoc_status_PRTDSI__SYNC_OUT       (* (reg8 *) Psoc_status__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Psoc_status__SIO_CFG)
    #define Psoc_status_SIO_HYST_EN        (* (reg8 *) Psoc_status__SIO_HYST_EN)
    #define Psoc_status_SIO_REG_HIFREQ     (* (reg8 *) Psoc_status__SIO_REG_HIFREQ)
    #define Psoc_status_SIO_CFG            (* (reg8 *) Psoc_status__SIO_CFG)
    #define Psoc_status_SIO_DIFF           (* (reg8 *) Psoc_status__SIO_DIFF)
#endif /* (Psoc_status__SIO_CFG) */

/* Interrupt Registers */
#if defined(Psoc_status__INTSTAT)
    #define Psoc_status_INTSTAT            (* (reg8 *) Psoc_status__INTSTAT)
    #define Psoc_status_SNAP               (* (reg8 *) Psoc_status__SNAP)
    
	#define Psoc_status_0_INTTYPE_REG 		(* (reg8 *) Psoc_status__0__INTTYPE)
	#define Psoc_status_1_INTTYPE_REG 		(* (reg8 *) Psoc_status__1__INTTYPE)
#endif /* (Psoc_status__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Psoc_status_H */


/* [] END OF FILE */
