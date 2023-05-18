/*******************************************************************************
* File Name: Timer_psoc_PM.c
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

#include "Timer_psoc.h"

static Timer_psoc_backupStruct Timer_psoc_backup;


/*******************************************************************************
* Function Name: Timer_psoc_SaveConfig
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
*  Timer_psoc_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_psoc_SaveConfig(void) 
{
    #if (!Timer_psoc_UsingFixedFunction)
        Timer_psoc_backup.TimerUdb = Timer_psoc_ReadCounter();
        Timer_psoc_backup.InterruptMaskValue = Timer_psoc_STATUS_MASK;
        #if (Timer_psoc_UsingHWCaptureCounter)
            Timer_psoc_backup.TimerCaptureCounter = Timer_psoc_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_psoc_UDB_CONTROL_REG_REMOVED)
            Timer_psoc_backup.TimerControlRegister = Timer_psoc_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_psoc_RestoreConfig
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
*  Timer_psoc_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_psoc_RestoreConfig(void) 
{   
    #if (!Timer_psoc_UsingFixedFunction)

        Timer_psoc_WriteCounter(Timer_psoc_backup.TimerUdb);
        Timer_psoc_STATUS_MASK =Timer_psoc_backup.InterruptMaskValue;
        #if (Timer_psoc_UsingHWCaptureCounter)
            Timer_psoc_SetCaptureCount(Timer_psoc_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_psoc_UDB_CONTROL_REG_REMOVED)
            Timer_psoc_WriteControlRegister(Timer_psoc_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_psoc_Sleep
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
*  Timer_psoc_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_psoc_Sleep(void) 
{
    #if(!Timer_psoc_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_psoc_CTRL_ENABLE == (Timer_psoc_CONTROL & Timer_psoc_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_psoc_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_psoc_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_psoc_Stop();
    Timer_psoc_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_psoc_Wakeup
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
*  Timer_psoc_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_psoc_Wakeup(void) 
{
    Timer_psoc_RestoreConfig();
    #if(!Timer_psoc_UDB_CONTROL_REG_REMOVED)
        if(Timer_psoc_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_psoc_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
