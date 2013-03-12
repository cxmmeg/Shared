/****************************************************************************/
/*! \file Global/Include/GlobalDefines.h
 *
 *  \brief Some global definitions.
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

#ifndef GLOBAL_GLOBALDEFINES_H
#define GLOBAL_GLOBALDEFINES_H

#include <QtGlobal>
#include <QString>
#include <QHash>
#include <Global/Include/TranslatableString.h>

/****************************************************************************/
/**
 * \brief In this namespace live globally used classes and defines.
 */
/****************************************************************************/
namespace Global {

typedef quint16 gSourceType;            ///< typedef for HeartBeat source: source type.
typedef quint16 gSubComponentType;      ///< typedef for logging source: subcomponent type.
typedef quint64 tRefType;               ///< typedef for tRefType.

const QString NO_NUMERIC_DATA = "";     ///< Self explaining


/****************************************************************************/
/**
 * \brief Enum containing all known return codes from the application.
 */
/****************************************************************************/
enum {
    RETCODE_OK                  = 0,    ///< Everything OK.
    RETCODE_TIME_OFFSET         = 1,    ///< Reading or writing to time offset file failed.
    RETCODE_CONNECT_FAILED      = 2,    ///< A connect between a signal and a slot failed.
    RETCODE_MASTERTHREAD_INIT   = 3,    ///< Master thread initialization failed.
    RETCODE_MASTERTHREAD_WAIT   = 4     ///< Waiting for master thread failed.
};

/****************************************************************************/
/**
 * \brief Enum containing all known event Types.
 */
/****************************************************************************/
enum EventType {
    EVTTYPE_UNDEFINED,      ///< Undefined. Used for initialisation.
    EVTTYPE_DEBUG,           ///< Debug message. Should be used only for debugging purposes.
    EVTTYPE_INFO,           ///< Info.
    EVTTYPE_WARNING,        ///< Warning.
    EVTTYPE_ERROR,          ///< "Normal" error.
    EVTTYPE_FATAL_ERROR,    ///< Fatal error.
    EVTTYPE_SYS_ERROR,      ///< Hardware system error
    EVTTYPE_SYS_WARNING,    ///< Hardware system warning
    EVTTYPE_SYS_HINT        ///< Hardware system hint
};

enum EventLogLevel {
    LOGLEVEL_NONE,
    LOGLEVEL_LOW,
    LOGLEVEL_MEDIUM,
    LOGLEVEL_HIGH,
    LOGLEVEL_UNEXPECTED
};

/****************************************************************************/
/**
 * \brief Enum containing all known action Types.
 */
/****************************************************************************/

enum ActionType {
    ACNTYPE_SHUTDOWN,   ///< Proceed for System Shutdown
    ACNTYPE_STOP,       ///< Proceed for System stop (System is Paused)
    ACNTYPE_ERROR,      ///< Action for Normal Error Message
    ACNTYPE_INFO,       ///< Send Info if required, log info
    ACNTYPE_RETRY,      ///< Send a Retry Msg, needs acknowledgement
    ACNTYPE_REBOOT,     ///< Proceed for System Reboot
    ACNTYPE_WARNING,    ///< Action for Normal Warning Message
    ACNTYPE_BUSY,       ///< TBD after discussion
    ACNTYPE_IDLE,       ///< TBD
    ACNTYPE_NONE,        ///< No action, just log
    ACNTYPE_REMOVEALLRACKS,
    ACNTYPE_REMOVERACK,
    ACNTYPE_UNEXPECTED,        /// < Unexpected text in action column, raise an error
    ACNTYPE_INIT_ROTARYVALVE,
    ACNTYPE_BOTTLECHECK_I,           ///< Please check the reagent bottle first and then click Recovery!
    ACNTYPE_BUILD_PRESSURE,          ///< Please check the Retort Lid first and then click Recovery!
    ACNTYPE_BUILD_VACUUM,            ///< Please check the Retort Lid first and then click Recovery!
    ACNTYPE_MAINTENANCE_AIRSYSTEM    ///< Please confirm there is no Tissue & Reagent in Retort and Cleaning-Xylene Bottle is not empty!
};

enum EventSourceType {
    EVENTSOURCE_MAIN,
    EVENTSOURCE_DEVICECOMMANDPROCESSOR,
    EVENTSOURCE_SCHEDULER,
    EVENTSOURCE_SCHEDULER_MAIN,
    EVENTSOURCE_EXPORT,
    EVENTSOURCE_IMPORTEXPORT,
    EVENTSOURCE_BLG,
    EVENTSOURCE_EVENTHANDLER,
    EVENTSOURCE_SEPIA,
    EVENTSOURCE_NONE,
    EVENTSOURCE_DATALOGGER,
    EVENTSOURCE_NOTEXIST
};

/****************************************************************************/
/**
 * \brief Enum containing all alarm position Types.
 */
/****************************************************************************/
enum AlarmPosType {
    ALARMPOS_NONE,          ///< No alarm
    ALARMPOS_DEVICE,        ///< only device alarm .
    ALARMPOS_LOCAL,         ///< alarm includes device and local
    ALARMPOS_REMOTE         ///< alarm includes device, local and remote site
};

/****************************************************************************/
/**
 * \brief Enum containing all log authority Types.
 */
/****************************************************************************/
enum LogAuthorityType {
    LOGAUTHTYPE_NO_SHOW,     ///< Not Show the log item in the Event View
    LOGAUTHTYPE_USER,        ///< only general user can see the log item.
    LOGAUTHTYPE_ADMIN,       ///< genaral user and administrator can see the log item
    LOGAUTHTYPE_SERVICE      ///< all user can see the log item
};

/****************************************************************************/
/**
 * \brief Enum containing all Response Types.
 */
/****************************************************************************/
enum ResponseType {
    RESTYPE_RS_NO_ACTION,
    RESTYPE_RS_RESET,    ///< sequence/protocols-Pump/Valves Reset,Release P/V, Register clear-up;
    RESTYPE_RS_STOPLATER,///< sequence/protocols-Stop after the running/current protocol finished, Stop=Pump/Valves Reset,Release P/V;
    RESTYPE_RS_STOPATONCE,///< sequence/protocols-Stop at once, Stop=Pump/Valves Reset,Release P/V; Remember current status of protocol
    RESTYPE_RS_PAUSE,     ///< sequence/protocols-Paused, keep status; Remember current status of protocol;
    RESTYPE_RS_DRAINATONCE,///< Drain at once if overflow
    RESTYPE_RS_DRAINATONCE_T,///< Drain at once for certain time if overflow
    RESTYPE_RS_TISSUE_PROTECT,///< Change the tissue to Formalin or some safety reagent when Err happened(Such as.: Draining Xylene, Sucking Formalin -- TBD)
    RESTYPE_RS_CHECK_BLOCKAGE,///< Bulid Pressure to attempt to recovery from Blockage automaticly
    RESTYPE_RS_AIRSYSTEM_CLEANING ///< AirSystem(Pump/Valve/Tube) do a series of action to get recovery from potential blockage in Airsystem(Just air no Xylene this)
};

/****************************************************************************/
/**
 * \brief Enum containing all ResponseRecovery Types.
 */
/****************************************************************************/
enum ResponseRecoveryType {
    RESRECTYPE_ONLY_RECOVERY = 0,
    RESRECTYPE_RESPONSE_RECOVERY,
    RESRECTYPE_RES_RESULT_RECOVERY,
    RESRECTYPE_ONLY_RESPONSE,
    RESRECTYPE_NO_ACTION,
};

/****************************************************************************/
/**
 * \brief Enum describing if an event "turned on" or "turned off".
 */
/****************************************************************************/
enum EventStatus {
    EVTSTAT_OFF = 0,        ///< Event "turned off". For example: now there is glass in cover slipper.
    EVTSTAT_ON  = 1         ///< Event "turned on". For example: no glass in cover slipper.
};

/****************************************************************************/
/**
 * \brief Enum containing all known GUI message Types.
 */
/****************************************************************************/
enum GUIMessageType {
    GUIMSGTYPE_UNDEFINED,   ///< Undefined. Used for initialisation.
    GUIMSGTYPE_ERROR,       ///< Error.
    GUIMSGTYPE_WARNING,     ///< Warning.
    GUIMSGTYPE_INFO         ///< Info.
};

/****************************************************************************/
/**
 * \brief Enum for temperature formats.
 */
/****************************************************************************/
enum TemperatureFormat {
    TEMP_FORMAT_UNDEFINED,  ///< Undefined. Used for initialization.
    TEMP_FORMAT_CELSIUS,    ///< Celsius.
    TEMP_FORMAT_FAHRENHEIT  ///< Fahrenheit.
};

/****************************************************************************/
/**
 * \brief Enum for on / off state.
 */
/****************************************************************************/
enum OnOffState {
    ONOFFSTATE_UNDEFINED,   ///< Undefined. Used for initialization.
    ONOFFSTATE_ON,          ///< On.
    ONOFFSTATE_OFF          ///< Off.
};

/****************************************************************************/
/**
 * \brief Enum for date format.
 */
/****************************************************************************/
enum DateFormat {
    DATE_UNDEFINED,     ///< Undefined. Used for initialization.
    DATE_INTERNATIONAL, ///< "DD.MM.YYYY" with leading zeroes.
    DATE_ISO,           ///< "YYYY-MM-DD" with leading zeroes.
    DATE_US             ///< "MM/DD/YYYY" with leading zeroes.
};

/****************************************************************************/
/**
 * \brief Enum for time format.
 */
/****************************************************************************/
enum TimeFormat {
    TIME_UNDEFINED, ///< Undefined. Used for initialization.
    TIME_24,        ///< 17:01:42
    TIME_12         ///< "05:01:42 p.m."
};

/****************************************************************************/
/**
 * \brief Enum for oven start mode.
 */
/****************************************************************************/
enum OvenStartMode {
    OVENSTART_UNDEFINED,        ///< Undefined. Used for initialization.
    OVENSTART_AFTER_STARTUP,    ///< After startup.
    OVENSTART_BEFORE_PROGRAM    ///< Before program start.
};

/****************************************************************************/
/**
 * \brief Enum for date time format.
 */
/****************************************************************************/
enum DateTimeFormat {
    DATE_TIME_UNDEFINED, ///< Undefined. Used for initialization.
    DATE_INTERNATIONAL_TIME_24, ///< "DD.MM.YYYY" with leading zeroes. ///< 17:01:42
    DATE_INTERNATIONAL_12, ///< "DD.MM.YYYY" with leading zeroes. ///< 05:01:42 p.m.
    DATE_ISO_TIME_24, ///< "YYYY-MM-DD" with leading zeroes ///< 17:01:42
    DATE_ISO_12, ///< "YYYY-MM-DD" with leading zeroes ///< 05:01:42 p.m.
    DATE_US_TIME_24, ///< "MM/DD/YYYY" with leading zeroes ///< 17:01:42
    DATE_US_12, ///< "MM/DD/YYYY" with leading zeroes ///< 05:01:42 p.m.
};

/****************************************************************************/
/**
 * \brief Enum for Water type.
 */
/****************************************************************************/
enum WaterType {
    WATER_TYPE_UNDEFINED, ///< Undefined. Used for initialization.
    WATER_TYPE_TAP, ///< Tap water
    WATER_TYPE_DISTILLED, ///< Distilled water
};

/****************************************************************************/
/**
 * \brief Enum for Water type.
 */
/****************************************************************************/
enum LogLevel {
    LOG_ENABLED, ///< Enable logging
    LOG_DISABLED,///< Disable logging
    LOG_DEBUG,  ///< Debugging
    LOG_CONSOLE, ///< Dump log messages to console
};

/****************************************************************************/
/**
 * \brief Enum for Water type.
 */
/****************************************************************************/
enum HeatingState{
    OFF,                    ///< Heating is off
    TEMPERATURE_IN_RANGE,   ///< Self explaining
    TEMPERATURE_OUT_OF_RANGE ///< Self explaining
};

/****************************************************************************/
/**
 * \brief Enum for Hood cover state
 */
/****************************************************************************/
enum CoverState {
    OPEN,   ///< Cover opened
    CLOSED  ///< Cover closed
};

/****************************************************************************/
/**
 * \brief Enum for execution state
 */
/****************************************************************************/
enum ExecutionState {
    START,  ///< Start execution
    STOP   ///< Stop execution
};

enum AlarmType {
    ALARM_NONE,
    ALARM_WARNING,
    ALARM_ERROR
};

/****************************************************************************/
/**
 * \brief Enum for GUI button type in Msg Box
 */
/****************************************************************************/
enum GuiButtonType {
    NO_BUTTON,
    OK,             //!< Msg Box with Ok
    YES_NO,         //!< Msg Box with Yes and No
    CONTINUE_STOP,  //!< Msg Box with Continue and Stop
    OK_CANCEL,       //!< Msg Box with Ok and Cancel
    RECOVERYLATER_RECOVERYNOW //!< Msg Box with Recovery Later and Recovery Now
};

/****************************************************************************/
/**
 * \brief Enum for GUI user roles
 */
/****************************************************************************/
enum GuiUserLevel{
    OPERATOR = 0, //!< Operator (normal user)
    ADMIN,    //!< Admin user
    SERVICE   //!< Service user
};

/****************************************************************************/
/**
 * \brief Enum for GUI user roles
 */
/****************************************************************************/
enum RMSOptions_t {
    RMS_OFF,
    RMS_CASSETTES,
    RMS_CYCLES,
    RMS_DAYS
};

/****************************************************************************/
/**
 * \brief Enum for using Alternate EventStrings
 */
/****************************************************************************/
enum AlternateEventStringUsage {
    NOT_APPLICABLE,
    GUI_MSG_BOX,
    USER_RESPONSE,
    LOGGING,
    GUI_MSG_BOX_AND_USER_RESPONSE,
    GUI_MSG_BOX_AND_LOGGING,
    GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE
};
} // end namespace Global

#endif // GLOBAL_GLOBALDEFINES_H
