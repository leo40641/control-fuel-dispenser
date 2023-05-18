/*******************************************************************************
* File Name: LCD1_PM.c
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

#include "LCD1.h"


/***************************************
* Local data allocation
***************************************/

static LCD1_BACKUP_STRUCT  LCD1_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: LCD1_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the LCD1_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD1_SaveConfig(void)
{
    #if(LCD1_CONTROL_REG_REMOVED == 0u)
        LCD1_backup.cr = LCD1_CONTROL_REG;
    #endif /* End LCD1_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LCD1_RestoreConfig
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
*  LCD1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling LCD1_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void LCD1_RestoreConfig(void)
{
    #if(LCD1_CONTROL_REG_REMOVED == 0u)
        LCD1_CONTROL_REG = LCD1_backup.cr;
    #endif /* End LCD1_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: LCD1_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The LCD1_Sleep() API saves the current component state. Then it
*  calls the LCD1_Stop() function and calls 
*  LCD1_SaveConfig() to save the hardware configuration.
*  Call the LCD1_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD1_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD1_Sleep(void)
{
    #if(LCD1_RX_ENABLED || LCD1_HD_ENABLED)
        if((LCD1_RXSTATUS_ACTL_REG  & LCD1_INT_ENABLE) != 0u)
        {
            LCD1_backup.enableState = 1u;
        }
        else
        {
            LCD1_backup.enableState = 0u;
        }
    #else
        if((LCD1_TXSTATUS_ACTL_REG  & LCD1_INT_ENABLE) !=0u)
        {
            LCD1_backup.enableState = 1u;
        }
        else
        {
            LCD1_backup.enableState = 0u;
        }
    #endif /* End LCD1_RX_ENABLED || LCD1_HD_ENABLED*/

    LCD1_Stop();
    LCD1_SaveConfig();
}


/*******************************************************************************
* Function Name: LCD1_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  LCD1_Sleep() was called. The LCD1_Wakeup() function
*  calls the LCD1_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  LCD1_Sleep() function was called, the LCD1_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LCD1_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD1_Wakeup(void)
{
    LCD1_RestoreConfig();
    #if( (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) )
        LCD1_ClearRxBuffer();
    #endif /* End (LCD1_RX_ENABLED) || (LCD1_HD_ENABLED) */
    #if(LCD1_TX_ENABLED || LCD1_HD_ENABLED)
        LCD1_ClearTxBuffer();
    #endif /* End LCD1_TX_ENABLED || LCD1_HD_ENABLED */

    if(LCD1_backup.enableState != 0u)
    {
        LCD1_Enable();
    }
}


/* [] END OF FILE */
