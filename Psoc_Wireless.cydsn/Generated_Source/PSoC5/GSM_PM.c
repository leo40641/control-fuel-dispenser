/*******************************************************************************
* File Name: GSM_PM.c
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

#include "GSM.h"


/***************************************
* Local data allocation
***************************************/

static GSM_BACKUP_STRUCT  GSM_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: GSM_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the GSM_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  GSM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void GSM_SaveConfig(void)
{
    #if(GSM_CONTROL_REG_REMOVED == 0u)
        GSM_backup.cr = GSM_CONTROL_REG;
    #endif /* End GSM_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: GSM_RestoreConfig
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
*  GSM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling GSM_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void GSM_RestoreConfig(void)
{
    #if(GSM_CONTROL_REG_REMOVED == 0u)
        GSM_CONTROL_REG = GSM_backup.cr;
    #endif /* End GSM_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: GSM_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The GSM_Sleep() API saves the current component state. Then it
*  calls the GSM_Stop() function and calls 
*  GSM_SaveConfig() to save the hardware configuration.
*  Call the GSM_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  GSM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void GSM_Sleep(void)
{
    #if(GSM_RX_ENABLED || GSM_HD_ENABLED)
        if((GSM_RXSTATUS_ACTL_REG  & GSM_INT_ENABLE) != 0u)
        {
            GSM_backup.enableState = 1u;
        }
        else
        {
            GSM_backup.enableState = 0u;
        }
    #else
        if((GSM_TXSTATUS_ACTL_REG  & GSM_INT_ENABLE) !=0u)
        {
            GSM_backup.enableState = 1u;
        }
        else
        {
            GSM_backup.enableState = 0u;
        }
    #endif /* End GSM_RX_ENABLED || GSM_HD_ENABLED*/

    GSM_Stop();
    GSM_SaveConfig();
}


/*******************************************************************************
* Function Name: GSM_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  GSM_Sleep() was called. The GSM_Wakeup() function
*  calls the GSM_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  GSM_Sleep() function was called, the GSM_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  GSM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void GSM_Wakeup(void)
{
    GSM_RestoreConfig();
    #if( (GSM_RX_ENABLED) || (GSM_HD_ENABLED) )
        GSM_ClearRxBuffer();
    #endif /* End (GSM_RX_ENABLED) || (GSM_HD_ENABLED) */
    #if(GSM_TX_ENABLED || GSM_HD_ENABLED)
        GSM_ClearTxBuffer();
    #endif /* End GSM_TX_ENABLED || GSM_HD_ENABLED */

    if(GSM_backup.enableState != 0u)
    {
        GSM_Enable();
    }
}


/* [] END OF FILE */
