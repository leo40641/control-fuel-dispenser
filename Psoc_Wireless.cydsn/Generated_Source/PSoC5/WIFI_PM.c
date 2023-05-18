/*******************************************************************************
* File Name: WIFI_PM.c
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

#include "WIFI.h"


/***************************************
* Local data allocation
***************************************/

static WIFI_BACKUP_STRUCT  WIFI_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: WIFI_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the WIFI_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WIFI_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WIFI_SaveConfig(void)
{
    #if(WIFI_CONTROL_REG_REMOVED == 0u)
        WIFI_backup.cr = WIFI_CONTROL_REG;
    #endif /* End WIFI_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: WIFI_RestoreConfig
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
*  WIFI_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling WIFI_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void WIFI_RestoreConfig(void)
{
    #if(WIFI_CONTROL_REG_REMOVED == 0u)
        WIFI_CONTROL_REG = WIFI_backup.cr;
    #endif /* End WIFI_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: WIFI_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The WIFI_Sleep() API saves the current component state. Then it
*  calls the WIFI_Stop() function and calls 
*  WIFI_SaveConfig() to save the hardware configuration.
*  Call the WIFI_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WIFI_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WIFI_Sleep(void)
{
    #if(WIFI_RX_ENABLED || WIFI_HD_ENABLED)
        if((WIFI_RXSTATUS_ACTL_REG  & WIFI_INT_ENABLE) != 0u)
        {
            WIFI_backup.enableState = 1u;
        }
        else
        {
            WIFI_backup.enableState = 0u;
        }
    #else
        if((WIFI_TXSTATUS_ACTL_REG  & WIFI_INT_ENABLE) !=0u)
        {
            WIFI_backup.enableState = 1u;
        }
        else
        {
            WIFI_backup.enableState = 0u;
        }
    #endif /* End WIFI_RX_ENABLED || WIFI_HD_ENABLED*/

    WIFI_Stop();
    WIFI_SaveConfig();
}


/*******************************************************************************
* Function Name: WIFI_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  WIFI_Sleep() was called. The WIFI_Wakeup() function
*  calls the WIFI_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  WIFI_Sleep() function was called, the WIFI_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  WIFI_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void WIFI_Wakeup(void)
{
    WIFI_RestoreConfig();
    #if( (WIFI_RX_ENABLED) || (WIFI_HD_ENABLED) )
        WIFI_ClearRxBuffer();
    #endif /* End (WIFI_RX_ENABLED) || (WIFI_HD_ENABLED) */
    #if(WIFI_TX_ENABLED || WIFI_HD_ENABLED)
        WIFI_ClearTxBuffer();
    #endif /* End WIFI_TX_ENABLED || WIFI_HD_ENABLED */

    if(WIFI_backup.enableState != 0u)
    {
        WIFI_Enable();
    }
}


/* [] END OF FILE */
