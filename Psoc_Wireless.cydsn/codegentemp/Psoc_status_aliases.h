/*******************************************************************************
* File Name: Psoc_status.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Psoc_status_ALIASES_H) /* Pins Psoc_status_ALIASES_H */
#define CY_PINS_Psoc_status_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Psoc_status_0			(Psoc_status__0__PC)
#define Psoc_status_0_INTR	((uint16)((uint16)0x0001u << Psoc_status__0__SHIFT))

#define Psoc_status_1			(Psoc_status__1__PC)
#define Psoc_status_1_INTR	((uint16)((uint16)0x0001u << Psoc_status__1__SHIFT))

#define Psoc_status_INTR_ALL	 ((uint16)(Psoc_status_0_INTR| Psoc_status_1_INTR))

#endif /* End Pins Psoc_status_ALIASES_H */


/* [] END OF FILE */
