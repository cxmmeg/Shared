/****************************************************************************/
/*! \file main.c
 *
 *  \brief Main function module for ASB15
 *
 *  $Version: $ 0.1
 *  $Date:    $ 20.08.2012
 *  $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *       This module contains the "main" function of the slave software,
 *       which is called after startup of the hardware. The main task
 *       of this module is, to specify all function modules required
 *       by this firmware and to call the base module start routine.
 *       All the rest is done by the base module in combination with
 *       the selected function modules.
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai R&D Center.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "Global.h"
#include "Basemodule.h"
#include "fmTemperature.h"
#include "fmPressure.h"
#include "fmDigiOutput.h"
#include "fmDigiInput.h"
#include "fmAnaOutput.h"
#include "fmAnaInput.h"
#include "ModuleIDs.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define DEFAULT_NODE_INDEX      0       //!< Node index (if no DIP switch)

#define VOLTAGE_LIMIT_WARNING   20000   //!< Supply voltage warning limit [mV]
#define VOLTAGE_LIMIT_FAILURE   16000   //!< Supply voltage error limit [mV]
#define CURRENT_LIMIT_WARNING   2000    //!< Supply current warning limit [mA]
#define CURRENT_LIMIT_FAILURE   3000    //!< Supply current error limit [mA]

//****************************************************************************/
//! Board options of the base module
//****************************************************************************/

static const UInt32 BASEMODULE_OPTIONS = (
        OPTION_IDENTIFICATION  |        // Enable identification phase 
        OPTION_NODE_INDEX_DIP  |        // Enable node index DIP switch
        OPTION_STATUS_LED      |        // Enable onboard status/error LEDs
        OPTION_LIFECYCLE_DATA  |        // Enable life cycle data collection
        OPTION_PROCESS_DATA    |        // Enable process data support
        OPTION_TRIGGER_EVENTS  |        // Enable trigger event support
        OPTION_TASK_STATISTICS |        // Enable statistics collection
        OPTION_VOLTAGE_MONITOR |        // Enable supply voltage monitor
        OPTION_CURRENT_MONITOR |        // Enable supply current monitor
        OPTION_INFO_MESSAGES            // Enable info messages on startup
);

//****************************************************************************/
//! Board options list for all modules
//****************************************************************************/

static const UInt32 TestOptionList[] = {
    // Base modules board options
    BASEMODULE_MODULE_ID, 6, 
        BASEMODULE_OPTIONS,             // Base module option bits
        DEFAULT_NODE_INDEX,             // Node index (if no DIP switch)
        VOLTAGE_LIMIT_WARNING,          // Voltage monitor warning level
        VOLTAGE_LIMIT_FAILURE,          // Voltage monitor failure level
        CURRENT_LIMIT_WARNING,          // Current monitor warning level
        CURRENT_LIMIT_FAILURE,          // Current monitor failure level
        
    // Function modules board options
    //MODULE_ID_TEMPERATURE, 4, 0x12012, 0x12012, 0x12012, 0x12012
#ifndef ASB_FCT    
    MODULE_ID_TEMPERATURE, 3, 0x11041011, 0x01041011, 0x01041011,
    
#ifdef ASB15_VER_A
    MODULE_ID_DIGITAL_OUT, 6, 1, 1, 1, 1, 1, 1,
#endif

#ifdef ASB15_VER_B
    MODULE_ID_DIGITAL_OUT, 4, 1, 1, 1, 1,
#endif 

    MODULE_ID_PRESSURE, 1, 0x2111
#else
    MODULE_ID_DIGITAL_OUT, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif
    

};

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t InitTestBootloaderInfoBlock (void);
static Error_t InitTestBoardOptionBlock (void);
static Error_t InitTestBoardInfoBlock (void);


/*****************************************************************************/
/*!
 *  \brief   Function module descriptor table
 *
 *      ModuleInitTable must contain a function pointer to the initialization
 *      function of each function module used. Beside this information, the
 *      number of instances (incarnations) and the unique module identifier
 *      of each module must be specified. If the number of instances is zero,
 *      the board options are checked for the number of module instances.
 *
 ****************************************************************************/

static const bmModuleParameters_t ModuleInitTable[] = {
#ifndef ASB_FCT
    { MODULE_ID_PRESSURE,    1, pressInitializeModule },
    { MODULE_ID_TEMPERATURE, 3, tempInitializeModule },
#ifdef ASB15_VER_A
    { MODULE_ID_DIGITAL_OUT, 6, doInitializeModule },
    { MODULE_ID_DIGITAL_IN,  1, diInitializeModule }
#endif
#ifdef ASB15_VER_B
    { MODULE_ID_DIGITAL_OUT, 4, doInitializeModule },
    { MODULE_ID_DIGITAL_IN,  4, diInitializeModule }
#endif
#else
    { MODULE_ID_DIGITAL_OUT, 11, doInitializeModule },
    { MODULE_ID_DIGITAL_IN,   4, diInitializeModule },
    { MODULE_ID_ANALOG_IN,    9, aiInitializeModule } 
#endif
};
static const int NumberOfModules = ELEMENTS(ModuleInitTable);


/*****************************************************************************/
/*!
 *  \brief   Define board option list
 *
 *      Defines a debug board option list. Normally, the board options
 *      are programmed during manufacturing external to this firmware.
 *      This is for debug only
 *
 *  \return  Should never return
 *
 ****************************************************************************/

static Error_t InitTestBoardOptionBlock (void) {

    UInt16 Elements = ELEMENTS(TestOptionList);
    UInt16 i;

    UInt32 *BoardOptions = calloc (Elements+3, sizeof(UInt32));

    if (BoardOptions != NULL) {

        BoardOptions[0] = INFOBLOCK_SIGNATURE;
        BoardOptions[1] = Elements + 2;

        for (i=0; i < Elements; i++) {
            BoardOptions[2+i] = TestOptionList[i];
        }
        BoardOptions[2+i] =
            bmCalculateCrc (BoardOptions, sizeof(UInt32) * (Elements+2));

        halSetAddress (ADDRESS_BOARD_OPTION_BLOCK, BoardOptions);
    }
    return (NO_ERROR);
}


//****************************************************************************/

static Error_t InitTestBoardInfoBlock (void) {

    static bmBoardInfoBlock_t InfoBlock;

    InfoBlock.Signature       = INFOBLOCK_SIGNATURE;
    InfoBlock.NodeType        = 15;
    InfoBlock.NodeClass       = 0;

    strcpy (InfoBlock.BoardName, "ASB15");
    strcpy (InfoBlock.SerialNum, "SN:0307.12345.AB/9-4");


    InfoBlock.VersionMajor    = 0;
    InfoBlock.VersionMinor    = 1;

    InfoBlock.ProductionYear  = 10;
    InfoBlock.ProductionMonth = 7;
    InfoBlock.ProductionDay   = 25;

    InfoBlock.EndTestYear     = 10;
    InfoBlock.EndTestMonth    = 7;
    InfoBlock.EndTestDay      = 1;
    InfoBlock.EndTestResult   = 1;

    InfoBlock.Checksum = bmCalculateCrc (
        &InfoBlock, sizeof(bmBoardInfoBlock_t) - sizeof(UInt32));

    halSetAddress (ADDRESS_BOARD_HARDWARE_INFO, &InfoBlock);
    return (NO_ERROR);
}

//****************************************************************************/

static Error_t InitTestBootloaderInfoBlock (void) {

    static bmBootInfoBlock_t BootBlock;

    BootBlock.Signature       = INFOBLOCK_SIGNATURE;
    BootBlock.VersionMajor    = 1;
    BootBlock.VersionMinor    = 1;
    BootBlock.CreationYear    = 10;
    BootBlock.CreationMonth   = 7;
    BootBlock.CreationDay     = 12;

    BootBlock.Checksum = bmCalculateCrc (
        &BootBlock, sizeof(bmBootInfoBlock_t) - sizeof(UInt32));

    halSetAddress (ADDRESS_BOOTLOADER_INFO, &BootBlock);
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Firmware's main function
 *
 *      This is the main function, which is called by the startup code after
 *      reset of the hardware. Its main purpose is to start the base module,
 *      which is some kind of operating system for the function modules.
 *      The base module does all the initialization of itself, the hardware
 *      abstraction layer, and the function modules. The initialization
 *      function pointers in the ModuleInitTable are used to achieve this.
 *      After initialization the base module calls the task functions of
 *      the function modules in a regular interval.
 *
 *  \return  Should never return
 *
 ****************************************************************************/

int main (int argc, char **argv) {

    #ifdef DEBUG
    volatile Int32 i;
    #endif
    Error_t Status;

    // Wait for JTAG to synchronize
    #ifdef DEBUG
    for (i=0; i < 300000; i++);
    #endif

    // this is for debugging only (tables are later defined externally)
    InitTestBoardInfoBlock();
    InitTestBootloaderInfoBlock();
    InitTestBoardOptionBlock();

    // this function call should never return
    Status = bmStartBaseModule (ModuleInitTable, NumberOfModules);

    if (Status < 0) {
        dbgPrintError (BASEMODULE_CHANNEL, Status, ON, 0);
    }
    dbgPrint ("Firmware stopped");
}

//****************************************************************************/
