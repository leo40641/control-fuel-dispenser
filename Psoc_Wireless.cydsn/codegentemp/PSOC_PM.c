/*******************************************************************************
* File Name: PSOC_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PSOC.h"


/***************************************
* Local data allocation
***************************************/

static PSOC_BACKUP_STRUCT  PSOC_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: PSOC_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the PSOC_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PSOC_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PSOC_SaveConfig(void)
{
    #if(PSOC_CONTROL_REG_REMOVED == 0u)
        PSOC_backup.cr = PSOC_CONTROL_REG;
    #endif /* End PSOC_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: PSOC_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PSOC_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling PSOC_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void PSOC_RestoreConfig(void)
{
    #if(PSOC_CONTROL_REG_REMOVED == 0u)
        PSOC_CONTROL_REG = PSOC_backup.cr;
    #endif /* End PSOC_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: PSOC_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The PSOC_Sleep() API saves the current component state. Then it
*  calls the PSOC_Stop() function and calls 
*  PSOC_SaveConfig() to save the hardware configuration.
*  Call the PSOC_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PSOC_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PSOC_Sleep(void)
{
    #if(PSOC_RX_ENABLED || PSOC_HD_ENABLED)
        if((PSOC_RXSTATUS_ACTL_REG  & PSOC_INT_ENABLE) != 0u)
        {
            PSOC_backup.enableState = 1u;
        }
        else
        {
            PSOC_backup.enableState = 0u;
        }
    #else
        if((PSOC_TXSTATUS_ACTL_REG  & PSOC_INT_ENABLE) !=0u)
        {
            PSOC_backup.enableState = 1u;
        }
        else
        {
            PSOC_backup.enableState = 0u;
        }
    #endif /* End PSOC_RX_ENABLED || PSOC_HD_ENABLED*/

    PSOC_Stop();
    PSOC_SaveConfig();
}


/*******************************************************************************
* Function Name: PSOC_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  PSOC_Sleep() was called. The PSOC_Wakeup() function
*  calls the PSOC_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  PSOC_Sleep() function was called, the PSOC_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PSOC_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PSOC_Wakeup(void)
{
    PSOC_RestoreConfig();
    #if( (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) )
        PSOC_ClearRxBuffer();
    #endif /* End (PSOC_RX_ENABLED) || (PSOC_HD_ENABLED) */
    #if(PSOC_TX_ENABLED || PSOC_HD_ENABLED)
        PSOC_ClearTxBuffer();
    #endif /* End PSOC_TX_ENABLED || PSOC_HD_ENABLED */

    if(PSOC_backup.enableState != 0u)
    {
        PSOC_Enable();
    }
}


/* [] END OF FILE */
