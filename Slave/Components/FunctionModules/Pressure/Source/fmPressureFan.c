/****************************************************************************/
/*! \file fmPressureFan.c
 * 
 *  \brief Functions determining the speed of a ventilation fan.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  This file's only task the determination of speed of a ventilation fan.
 *  The unit is rotations per minute.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <stdlib.h>
#include "Global.h"
#include "bmError.h"
#include "fmPressureFan.h"
#include "halLayer.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

static const Device_t PRESS_FAN_HAL_TIMER = HAL_PRESS_FANSPEED; //!< The timer unit for the first instance

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Contains all variables needed for fan speed measurments */
typedef struct {
    Handle_t Handle;            //!< Handle to access a hardware timer (HAL)
    Handle_t *HandleControl;    //!< Handle to access digital output ports (HAL)
    UInt8 CaptureNumber;        //!< Number of active capture channels
    UInt32 *CountOld;           //!< Last counter value from the capture register
    UInt32 *CountDiff;          //!< The difference between two capture events
    Bool *TopSpeed;             //!< Indicates if the fan is at top speed or not
    UInt32 TimeMask;            //!< The size of the timer register
} PressureFan_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static PressureFan_t *PressFanTable = NULL; //!< data table for all instances
static UInt16 PressFanInstances = 0; //!< number of module instances

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static void pressFanInterrupt (UInt32 Channel, UInt32 IntrFlags);


/*****************************************************************************/
/*! 
 *  \brief   Initializes the data structures for all instances 
 *
 *      This method allocates the data table that holds all information
 *      required for the computation of the speed of the ventilation fans. It
 *      initializes the table for all instances. It must not be called more
 *      than once. 
 * 
 *  \iparam  Instances = Number of instances 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressFanInit (UInt16 Instances)
{
    if (NULL == PressFanTable) {
        PressFanTable = calloc (Instances, sizeof(PressureFan_t));
        if (NULL == PressFanTable) {
            return (E_MEMORY_FULL);
        }
        PressFanInstances = Instances;
        return (NO_ERROR);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*! 
 *  \brief   Opens the timer capture channels
 *
 *      The function initializes a capture unit that is part of the hardware
 *      abstraction layer. The capture unit measures the time between two
 *      strobes generated by the sensor of the ventilation fans.
 * 
 *  \iparam  Instance = Instance number 
 *  \oparam  Fans = Total number of fans for this instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressFanOpen (UInt16 Instance, UInt8 Fans)
{
    UInt8 i;
    Error_t Error;
    TimerMode_t TimerMode;
    PressureFan_t *PressFan = &PressFanTable[Instance];
    
    TimerMode.Direction = TIM_MODE_COUNT_UP;
    TimerMode.OneShot = TIM_MODE_INTERVAL;
    TimerMode.ClkSource = TIM_MODE_INTERNAL;
    TimerMode.ClkMode = 0;

    if (Fans == 0) {
        return (NO_ERROR);
    }

    if (Instance >= PressFanInstances) {
        return E_PARAMETER_OUT_OF_RANGE;
    }
    
    // Allocate fan control handles
    PressFan->HandleControl = calloc (Fans, sizeof(Handle_t));
    if (NULL == PressFanTable) {
        return (E_MEMORY_FULL);
    }
    // Open fan control switches
    for (i = 0; i < Fans; i++) {
        PressFan->HandleControl[i] = halPortOpen (HAL_PRESS_FANCONTROL + i, HAL_OPEN_WRITE);
        if (PressFan->HandleControl[i] < 0) {
            return (PressFan->HandleControl[i]);
        }
    }

    // Open fan speed capture units
    PressFan->Handle = halTimerOpen (PRESS_FAN_HAL_TIMER + Instance, Instance, pressFanInterrupt);
    if (PressFan->Handle < 0) {
        return (PressFan->Handle);
    }
    Error = halTimerStatus (PressFan->Handle, TIM_STAT_CLOCKRATE);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halTimerSetup (PressFan->Handle, &TimerMode, Error / 100000);
    if (Error < NO_ERROR) {
        return (Error);
    }
    // Request the size of the capture registers
    Error = halTimerStatus (PressFan->Handle, TIM_STAT_MAXCOUNT);
    if (Error < NO_ERROR) {
        return (Error);
    }
    PressFan->TimeMask = Error;

    Error = halTimerStatus (PressFan->Handle, TIM_STAT_UNITS);
    if (Error < 0) {
        return (Error);
    }
    else if (Error < Fans) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    
    Error = halTimerControl (PressFan->Handle, TIM_CTRL_START);
    if (Error < 0) {
        return (Error);
    }
    
    // Allocation of measurement variables
    PressFan->CaptureNumber = Fans;
    PressFanTable->CountOld = calloc (PressFan->CaptureNumber, sizeof(UInt32));
    if (NULL == PressFanTable->CountOld) {
        return (E_MEMORY_FULL);
    }
    PressFanTable->CountDiff = calloc (PressFan->CaptureNumber, sizeof(UInt32));
    if (NULL == PressFanTable->CountDiff) {
        return (E_MEMORY_FULL);
    }
    PressFanTable->TopSpeed = calloc (PressFan->CaptureNumber, sizeof(Bool));
    if (NULL == PressFanTable->TopSpeed) {
        return (E_MEMORY_FULL);
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Activates or deactivates the fans
 *
 *      This method starts and stopps all ventialtion fans assigned to an
 *      instance of this function module.
 * 
 *  \iparam  Instance = Instance number 
 *  \iparam  Activate = Switch on (true) or off (false)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressFanControl (UInt16 Instance, Bool Activate)
{
    UInt8 i;
    Error_t Status;

    if (Instance >= PressFanInstances) {
        return E_PARAMETER_OUT_OF_RANGE;
    }

    for (i = 0; i < PressFanTable[Instance].CaptureNumber; i++) {
        PressFanTable[Instance].CountOld[i] = 0;
        PressFanTable[Instance].CountDiff[i] = 0;
        PressFanTable[Instance].TopSpeed[i] = FALSE;
        
        Status = halPortWrite (PressFanTable[Instance].HandleControl[i], Activate);
        if (Status < NO_ERROR) {
            return (Status);
        }
        
        if (Activate == TRUE) {
            Status = halCapComControl (PressFanTable[Instance].Handle, i, TIM_INTR_ENABLE);
        }
        else {
            Status = halCapComControl (PressFanTable[Instance].Handle, i, TIM_INTR_DISABLE);
        }
        if (Status < NO_ERROR) {
            return (Status);
        }
    }
    
    return (Status);
}


/*****************************************************************************/
/*! 
 *  \brief   Measures the speed of a ventilation fan
 *
 *      This function converts the information fetched from a single capture
 *      channel into rotations per minute and returns the value.
 * 
 *  \iparam  Instance = Instance number 
 *  \iparam  Fan = Number of the ventilation fan
 *  \oparam  Speed = Fan speed in rotations per minute
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t pressFanSpeed (UInt16 Instance, UInt8 Fan, UInt16 *Speed)
{
    UInt32 Count;
    Error_t Status = PressFanTable[Instance].TopSpeed[Fan];

    if (Instance >= PressFanInstances) {
        return E_PARAMETER_OUT_OF_RANGE;
    }
    if (Fan >= PressFanTable[Instance].CaptureNumber) {
        return E_PARAMETER_OUT_OF_RANGE;
    }

    Count = PressFanTable[Instance].CountDiff[Fan];
    
    if (Count != 0) {
        *Speed = 3000000 / Count;
    }
    else {
        *Speed = 0;
    }
    
    PressFanTable[Instance].TopSpeed[Fan] = TRUE;
    return (Status);
}


/*****************************************************************************/
/*! 
 *  \brief   Interrupt handler reading capture channels
 *
 *      This interrupt handler reads all capture registers of the timer unit
 *      and computes the counter difference since the last event. These values
 *      are stored in the corresponding data struture of the function module
 *      instance.
 * 
 *  \iparam  Channel = user infomation
 *  \iparam  IntrFlags = interrupt flags
 *
 ****************************************************************************/

static void pressFanInterrupt (UInt32 Channel, UInt32 IntrFlags)
{
    UInt8 i;
    UInt32 CountNew;
    Error_t Error;
    PressureFan_t *PressFan = &PressFanTable[Channel];
    
    // Looking for capture events
    for (i = 0; i < PressFan->CaptureNumber; i++) {
        // Get the captured counter value
        Error = halCapComRead (PressFan->Handle, i, &CountNew);
        if (Error < 0) {
            return;
        }
        else if (Error == 1) {
            PressFan->CountDiff[i] = PressFan->TimeMask & (CountNew - PressFan->CountOld[i]);
            PressFan->CountOld[i] = CountNew;
        }
    }
    
    return;
}

//****************************************************************************/
