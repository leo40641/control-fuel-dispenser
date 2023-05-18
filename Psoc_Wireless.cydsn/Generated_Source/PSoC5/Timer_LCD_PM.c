/*******************************************************************************
* File Name: Timer_LCD_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_LCD.h"

static Timer_LCD_backupStruct Timer_LCD_backup;


/*******************************************************************************
* Function Name: Timer_LCD_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_LCD_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_LCD_SaveConfig(void) 
{
    #if (!Timer_LCD_UsingFixedFunction)
        Timer_LCD_backup.TimerUdb = Timer_LCD_ReadCounter();
        Timer_LCD_backup.InterruptMaskValue = Timer_LCD_STATUS_MASK;
        #if (Timer_LCD_UsingHWCaptureCounter)
            Timer_LCD_backup.TimerCaptureCounter = Timer_LCD_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_LCD_UDB_CONTROL_REG_REMOVED)
            Timer_LCD_backup.TimerControlRegister = Timer_LCD_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_LCD_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_LCD_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_LCD_RestoreConfig(void) 
{   
    #if (!Timer_LCD_UsingFixedFunction)

        Timer_LCD_WriteCounter(Timer_LCD_backup.TimerUdb);
        Timer_LCD_STATUS_MASK =Timer_LCD_backup.InterruptMaskValue;
        #if (Timer_LCD_UsingHWCaptureCounter)
            Timer_LCD_SetCaptureCount(Timer_LCD_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_LCD_UDB_CONTROL_REG_REMOVED)
            Timer_LCD_WriteControlRegister(Timer_LCD_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_LCD_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_LCD_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_LCD_Sleep(void) 
{
    #if(!Timer_LCD_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_LCD_CTRL_ENABLE == (Timer_LCD_CONTROL & Timer_LCD_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_LCD_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_LCD_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_LCD_Stop();
    Timer_LCD_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_LCD_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_LCD_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_LCD_Wakeup(void) 
{
    Timer_LCD_RestoreConfig();
    #if(!Timer_LCD_UDB_CONTROL_REG_REMOVED)
        if(Timer_LCD_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_LCD_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
