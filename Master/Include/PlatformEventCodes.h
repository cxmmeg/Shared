/****************************************************************************/
/*! \file PlatformEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef PLATFORMEVENTCODES_H
#define PLATFORMEVENTCODES_H

#include <QtGlobal>



// Note : Add event groups for your platform components here
/// Event group for platform
const quint32 EVENT_GROUP_PLATFORM                          = 0x01;

/// Event group for global stuff
const quint32 EVENT_GROUP_PLATFORM_GLOBAL                   = ((EVENT_GROUP_PLATFORM << 8) + 0x01) << 16;
/// Event group for thread stuff
const quint32 EVENT_GROUP_PLATFORM_THREADS                  = ((EVENT_GROUP_PLATFORM << 8) + 0x02) << 16;
/// Event group for data logging
const quint32 EVENT_GROUP_PLATFORM_DATALOGGING              = ((EVENT_GROUP_PLATFORM << 8) + 0x03) << 16;
/// Event group for event handler.
const quint32 EVENT_GROUP_PLATFORM_EVENTHANDLER             = ((EVENT_GROUP_PLATFORM << 8) + 0x04) << 16;
/// Event group for network client
const quint32 EVENT_GROUP_PLATFORM_NETWORKCLIENT            = ((EVENT_GROUP_PLATFORM << 8) + 0x05) << 16;
/// Event group for network server
const quint32 EVENT_GROUP_PLATFORM_NETWORKSERVER            = ((EVENT_GROUP_PLATFORM << 8) + 0x06) << 16;
/// Event group for data manager.
const quint32 EVENT_GROUP_PLATFORM_DATAMANAGER              = ((EVENT_GROUP_PLATFORM << 8) + 0x07) << 16;
/// Event group for device command processor
const quint32 EVENT_GROUP_PLATFORM_DEVICECOMMANDPROCESSOR   = ((EVENT_GROUP_PLATFORM << 8) + 0x08) << 16;
/// Event group for state machines
const quint32 EVENT_GROUP_PLATFORM_STATEMACHINES            = ((EVENT_GROUP_PLATFORM << 8) + 0x09) << 16;
/// Event group for XML parser.
const quint32 EVENT_GROUP_PLATFORM_XML_PARSER               = ((EVENT_GROUP_PLATFORM << 8) + 0x0A) << 16;
/// Event group for external process control
const quint32 EVENT_GROUP_PLATFORM_EXTERNALPROCESSCONTROL   = ((EVENT_GROUP_PLATFORM << 8) + 0x0B) << 16;
/// Event group for device control layer
const quint32 EVENT_GROUP_PLATFORM_DEVICECONTROL            = ((EVENT_GROUP_PLATFORM << 8) + 0x0C) << 16;
/// Event group for Export
const quint32 EVENT_GROUP_PLATFORM_EXPORT                   = ((EVENT_GROUP_PLATFORM << 8) + 0x0D) << 16;
/// Event group for Password manager
const quint32 EVENT_GROUP_PLATFORM_PASSWORDMANAGER          = ((EVENT_GROUP_PLATFORM << 8) + 0x0E) << 16;
/// Event group for Usersettings Data container
const quint32 EVENT_GROUP_PLATFORM_DM_USERSETTINGS          = ((EVENT_GROUP_PLATFORM << 8) + 0x0F) << 16;
/// Event group for GPIO manager
const quint32 EVENT_GROUP_PLATFORM_GPIO_MANAGER             = ((EVENT_GROUP_PLATFORM << 8) + 0x10) << 16;
/// Event group for generic events
const quint32 EVENT_GROUP_PLATFORM_GENERIC_EVENTS           = ((EVENT_GROUP_PLATFORM << 8) + 0x11) << 16;
/// Event group for device settings data container
const quint32 EVENT_GROUP_PLATFORM_DM_DEVICECONFIG          = ((EVENT_GROUP_PLATFORM << 8) + 0x12) << 16;
/// Event group for Slave Module Hardware Abstraction Layer
const quint32 EVENT_GROUP_PLATFORM_SLAVE_HAL                = ((EVENT_GROUP_PLATFORM << 8) + 0x13) << 16;
/// Event group for Slave Module Base Module
const quint32 EVENT_GROUP_PLATFORM_SLAVE_BASEMODULE         = ((EVENT_GROUP_PLATFORM << 8) + 0x14) << 16;
/// Event group for Slave Module Analog Input
const quint32 EVENT_GROUP_PLATFORM_SLAVE_ANALOGINPUT        = ((EVENT_GROUP_PLATFORM << 8) + 0x15) << 16;
/// Event group for Slave Module Analog Output
const quint32 EVENT_GROUP_PLATFORM_SLAVE_ANALOGOUTPUT       = ((EVENT_GROUP_PLATFORM << 8) + 0x16) << 16;
/// Event group for Slave Module Digital Input
const quint32 EVENT_GROUP_PLATFORM_SLAVE_DIGITALINPUT       = ((EVENT_GROUP_PLATFORM << 8) + 0x17) << 16;
/// Event group for Slave Module Digital Output
const quint32 EVENT_GROUP_PLATFORM_SLAVE_DIGITALOUTPUT      = ((EVENT_GROUP_PLATFORM << 8) + 0x18) << 16;
/// Event group for Slave Module Joystick
const quint32 EVENT_GROUP_PLATFORM_SLAVE_JOYSTICK           = ((EVENT_GROUP_PLATFORM << 8) + 0x19) << 16;
/// Event group for Slave Module RFID ISO 11785
const quint32 EVENT_GROUP_PLATFORM_SLAVE_RFIDISO11785       = ((EVENT_GROUP_PLATFORM << 8) + 0x1A) << 16;
/// Event group for Slave Module RFID ISO 15693
const quint32 EVENT_GROUP_PLATFORM_SLAVE_RFIDISO15693       = ((EVENT_GROUP_PLATFORM << 8) + 0x1B) << 16;
/// Event group for Slave Module Stepper Motor
const quint32 EVENT_GROUP_PLATFORM_SLAVE_STEPPERMOTOR       = ((EVENT_GROUP_PLATFORM << 8) + 0x1C) << 16;
/// Event group for Slave Module Temperature Control
const quint32 EVENT_GROUP_PLATFORM_SLAVE_TEMPERATURECONTROL = ((EVENT_GROUP_PLATFORM << 8) + 0x1D) << 16;
/// Event group for Slave Module UART
const quint32 EVENT_GROUP_PLATFORM_SLAVE_UART               = ((EVENT_GROUP_PLATFORM << 8) + 0x1E) << 16;

/// Event group for rack data container
const quint32 EVENT_GROUP_PLATFORM_DM_RACK                  = ((EVENT_GROUP_PLATFORM << 8) + 0x1F) << 16;

/// Event group for SW_Version data container
const quint32 EVENT_GROUP_PLATFORM_DM_SWVERSION             = ((EVENT_GROUP_PLATFORM << 8) + 0x20) << 16;

/// Event group for SW_Version data container
const quint32 EVENT_GROUP_PLATFORM_DM_EXPORT_CONFIG         = ((EVENT_GROUP_PLATFORM << 8) + 0x21) << 16;

/// Event group for SW update manager
const quint32 EVENT_GROUP_PLATFORM_SW_UPDATE                = ((EVENT_GROUP_PLATFORM << 8) + 0x22) << 16;

/// Event group for RCConfiguration data container
const quint32 EVENT_GROUP_PLATFORM_DM_RC_CONFIG             = ((EVENT_GROUP_PLATFORM << 8) + 0x23) << 16;

/// Event group for ParameterSets data container
const quint32 EVENT_GROUP_PLATFORM_DM_PARAM_SETS            = ((EVENT_GROUP_PLATFORM << 8) + 0x24) << 16;

/// Event group for Stations data container
const quint32 EVENT_GROUP_PLATFORM_DM_STATIONS              = ((EVENT_GROUP_PLATFORM << 8) + 0x25) << 16;

/// Event group for Reagent data container
const quint32 EVENT_GROUP_PLATFORM_DM_REAGENTS              = ((EVENT_GROUP_PLATFORM << 8) + 0x26) << 16;

/// Event group for RemoteCare
const quint32 EVENT_GROUP_PLATFORM_REMOTECARE               = ((EVENT_GROUP_PLATFORM << 8) + 0x27) << 16;

/// Event group for network components
const quint32 EVENT_GROUP_PLATFORM_NETWORKCOMPONENT         = ((EVENT_GROUP_PLATFORM << 8) + 0x28) << 16;

/// Event group for RemoteCare manager
const quint32 EVENT_GROUP_PLATFORM_RC_MANAGER               = ((EVENT_GROUP_PLATFORM << 8) + 0x29) << 16;

/// Event group for GUI manager
const quint32 EVENT_GROUP_PLATFORM_GUI_MANAGER               = ((EVENT_GROUP_PLATFORM << 8) + 0x2A) << 16;

/// Events shared between different applications
const quint32 EVENT_GROUP_SHARED_EVENTS                     = ((EVENT_GROUP_PLATFORM << 8) + 0x2B) << 16;

// Generic events
/// Raised when basic system init is complete
const quint32 EVENT_SOFTSWITCH_MONITOR_START                = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x001;
/// Raised when basic system init is complete
const quint32 EVENT_SOFTSWITCH_PRESSED_FIRST_TIME           = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x002;
/// Raised when basic system init is complete
const quint32 EVENT_SYSTEM_INIT_COMPLETE                    = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x003;
/// Errors -> log errors for testing/debugging
const quint32 EVENT_ERROR                                   = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x004;
/// Warning -> log warning for testing/debugging
const quint32 EVENT_WARNING                                 = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x005;
/// Info ->log info for testing/debugging
const quint32 EVENT_INFO                                    = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x006;
const quint32 EVENT_PLEASE_TURNOFF_HARDSWITCH               = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x007;  ///< Please turn off the switch on the back of machine to shutdown the machine.
const quint32 EVENT_INSTALLATION_FINISHED                   = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x008;  ///< Installation is finished, please unplug usb disk, then reboot the machine.
const quint32 EVENT_SWU_ROLBACKING                          = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x009;  ///< SW Update Rollback running ... Don't touch anything.
const quint32 EVENT_SWU_RUNNING                             = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x00A;  ///< SW Update running ... Don't touch anything.
const quint32 EVENT_CALL_FOR_SERVICE                        = EVENT_GROUP_PLATFORM_GENERIC_EVENTS + 0x00B;  ///< Unexpected error. Please call Service.


#endif // namespace PLATFORMEVENTCODES_H
