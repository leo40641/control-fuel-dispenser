/*******************************************************************************
* File Name: LCD2_PM.c
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

#include "LCD2.h"


/***************************************
* Local data allocation
***************************************/

static LCD2_BACKUP_STRUCT  LCD2_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: LCD2_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the LCD2_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD2_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD2_SaveConfig(void)
{
    #if(LCD2_CONTROL_REG_REMOVED == 0u)
        LCD2_backup.cr = LCD2_CONTROL_REG;
    #endif /* End LCD2_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LCD2_RestoreConfig
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
*  LCD2_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling LCD2_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void LCD2_RestoreConfig(void)
{
    #if(LCD2_CONTROL_REG_REMOVED == 0u)
        LCD2_CONTROL_REG = LCD2_backup.cr;
    #endif /* End LCD2_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LCD2_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The LCD2_Sleep() API saves the current component state. Then it
*  calls the LCD2_Stop() function and calls 
*  LCD2_SaveConfig() to save the hardware configuration.
*  Call the LCD2_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD2_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD2_Sleep(void)
{
    #if(LCD2_RX_ENABLED || LCD2_HD_ENABLED)
        if((LCD2_RXSTATUS_ACTL_REG  & LCD2_INT_ENABLE) != 0u)
        {
            LCD2_backup.enableState = 1u;
        }
        else
        {
            LCD2_backup.enableState = 0u;
        }
    #else
        if((LCD2_TXSTATUS_ACTL_REG  & LCD2_INT_ENABLE) !=0u)
        {
            LCD2_backup.enableState = 1u;
        }
        else
        {
            LCD2_backup.enableState = 0u;
        }
    #endif /* End LCD2_RX_ENABLED || LCD2_HD_ENABLED*/

    LCD2_Stop();
    LCD2_SaveConfig();
}


/*******************************************************************************
* Function Name: LCD2_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  LCD2_Sleep() was called. The LCD2_Wakeup() function
*  calls the LCD2_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  LCD2_Sleep() function was called, the LCD2_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD2_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD2_Wakeup(void)
{
    LCD2_RestoreConfig();
    #if( (LCD2_RX_ENABLED) || (LCD2_HD_ENABLED) )
        LCD2_ClearRxBuffer();
    #endif /* End (LCD2_RX_ENABLED) || (LCD2_HD_ENABLED) */
    #if(LCD2_TX_ENABLED || LCD2_HD_ENABLED)
        LCD2_ClearTxBuffer();
    #endif /* End LCD2_TX_ENABLED || LCD2_HD_ENABLED */

    if(LCD2_backup.enableState != 0u)
    {
        LCD2_Enable();
    }
}


/* [] END OF FILE */
