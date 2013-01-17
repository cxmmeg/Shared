/****************************************************************************/
/*! \file TemperatureControl.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CTemperatureControl
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

#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

#define CANTEMP_READ_REQ_TIMEOUT    500 //!< timeout req. actual temperature
#define CANTEMP_WRITE_REQ_TIMEOUT   500 //!< timeout set temperature

#define TEMP_CTRL_ACTIVE_BITPOS 0   //!< bit position state
#define TEMP_CTRL_TUNINGBITPOS 1    //!< bit position tuning
#define TEMP_CTRL_PHASE_BITPOS 2    //!< bit position phase
#define TEMP_CTRL_VOLTAGE_BITPOS 3  //!< bit position voltage

/****************************************************************************/
/*!
 *  \brief  Constructor for the CANTemperatureControl
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to CANNode, where this module is assigned to
 */
/****************************************************************************/
CTemperatureControl::CTemperatureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                                         CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_TEMPERATURE_CTL, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDError(0), m_unCanIDErrorReq(0),
    m_unCanIDTemperatureSet(0), m_unCanIDFanWatchdogSet(0), m_unCanIDCurrentWatchdogSet(0),
    m_unCanIDPIDParamSet(0), m_unCanIDHeaterTimeSet(0),
    m_unCanIDTemperatureReq(0), m_unCanIDTemperature(0),
    m_unCanIDPIDParamReq(0), m_unCanIDPIDParam(0),
    m_unCanIDHeaterTimeReq(0), m_unCanIDHeaterTime(0),
    m_unCanIDServiceSensorReq(0), m_unCanIDServiceSensor(0),
    m_unCanIDServiceFanReq(0), m_unCanIDServiceFan(0),
    m_unCanIDHardwareReq(0), m_unCanIDHardware(0),
    m_aktionTimespan(0)
{
    // main state
    m_mainState = FM_MAIN_STATE_BOOTUP;
    // configuration state
    m_subStateConfig = FM_TEMP_SUB_STATE_CONFIG_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CANTemperatureControl
 */
/****************************************************************************/
CTemperatureControl::~CTemperatureControl()
{
}

/****************************************************************************/
/*!
 *  \brief  Initialize this function module
 *
 *  The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *  Receiveable CAN-message are registered to the communication layer
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::Initialize()
{
    ReturnCode_t RetVal;

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module initialized: " << GetName().toStdString();
        }
    }
    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the module's CAN message IDs
 *
 *  The CAN-IDs are read from the CAN-Message configuration class.
 *  The CAN-ID is composed by the message key, the function
 *  module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t  CTemperatureControl::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bChannel;
    const quint8 ModuleID = MODULE_ID_TEMPERATURE;

    if (m_pCANObjectConfig == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    bChannel = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDTemperatureSet     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlTemperatureSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDFanWatchdogSet     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlFanWatchdogSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDCurrentWatchdogSet = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlCurrentWatchdogSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParamSet        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlPIDParamSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDHeaterTimeSet      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlHeaterTimeSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDTemperatureReq     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlTemperatureReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDTemperature        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlTemperature", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParamReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlPIDParamReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParam           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlPIDParam", bChannel, m_pParent->GetNodeID());
    m_unCanIDHeaterTimeReq      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlHeaterTimeReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDHeaterTime         = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlHeaterTime", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceSensorReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlServiceSensorReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceSensor      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlServiceSensor", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceFanReq      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlServiceFanReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceFan         = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlServiceFan", bChannel, m_pParent->GetNodeID());
    m_unCanIDHardwareReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlHardwareReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDHardware           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlHardware", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiAutoTune       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlNotiAutoTune", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiInRange        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlNotiInRange", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiOutOfRange     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "TempCtrlNotiOutOfRange", bChannel, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo          : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning       : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError         : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError    : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   TemperatureSet     : 0x" << std::hex << m_unCanIDTemperatureSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   FanWatchdogSet     : 0x" << std::hex << m_unCanIDFanWatchdogSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   CurrentWatchdogSet : 0x" << std::hex << m_unCanIDCurrentWatchdogSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParamSet        : 0x" << std::hex << m_unCanIDPIDParamSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   HeaterTimeSet      : 0x" << std::hex << m_unCanIDHeaterTimeSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   TemperatureReq     : 0x" << std::hex << m_unCanIDTemperatureReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   Temperature        : 0x" << std::hex << m_unCanIDTemperature;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParamReq        : 0x" << std::hex << m_unCanIDPIDParamReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParam           : 0x" << std::hex << m_unCanIDPIDParam;
    FILE_LOG_L(laINIT, llDEBUG) << "   HeaterTimeReq      : 0x" << std::hex << m_unCanIDHeaterTimeReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   HeaterTime         : 0x" << std::hex << m_unCanIDHeaterTime;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceSensorReq   : 0x" << std::hex << m_unCanIDServiceSensorReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceSensor      : 0x" << std::hex << m_unCanIDServiceSensor;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceFanReq      : 0x" << std::hex << m_unCanIDServiceFanReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceFan         : 0x" << std::hex << m_unCanIDServiceFan;
    FILE_LOG_L(laINIT, llDEBUG) << "   HardwareReq        : 0x" << std::hex << m_unCanIDHardwareReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   Hardware           : 0x" << std::hex << m_unCanIDHardware;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiAutoTune       : 0x" << std::hex << m_unCanIDNotiAutoTune;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiInRange        : 0x" << std::hex << m_unCanIDNotiInRange;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiOutOfRange     : 0x" << std::hex << m_unCanIDNotiOutOfRange;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive CAN-messages to communication layer
 *
 *  Each receiveable CAN-message must be registered to the communication layer.
 *  This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *  instance after receiption of the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::RegisterCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDTemperature, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDPIDParam, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHeaterTime, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDServiceSensor, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDServiceFan, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHardware, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDNotiInRange, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDNotiOutOfRange, this);
    }
    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine
 *
 *  Depending on the active main state, the appropriate task function will be called.
 */
/****************************************************************************/
void CTemperatureControl::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(FM_MAIN_STATE_CONFIRMED == m_mainState)
    {
        //fall through
        m_subStateConfig = FM_TEMP_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        if(FM_TEMP_SUB_STATE_CONFIG_START == m_subStateConfig)
        {
            m_subStateConfig = FM_TEMP_SUB_STATE_CONFIG_SEND;
            StartTimeDelay();
        }
        else if(FM_TEMP_SUB_STATE_CONFIG_SEND == m_subStateConfig)
        {
            ReturnCode_t RetVal;
            CANFctModuleTempCtrl* pCANObjConfTemp = (CANFctModuleTempCtrl*) m_pCANObjectConfig;
            if (pCANObjConfTemp == NULL) {
                m_mainState = FM_MAIN_STATE_ERROR;
                return;
            }

            RetVal = SendCANMsgFanWatchdogSet();
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_mainState = FM_MAIN_STATE_ERROR;
                return;
            }
            RetVal = SendCANMsgCurrentWatchdogSet();
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_mainState = FM_MAIN_STATE_ERROR;
                return;
            }
            for(qint32 i = 0; i < pCANObjConfTemp->listPidControllers.size(); i++) {
                RetVal = SendCANMsgPidParametersSet(i);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                    FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                    m_mainState = FM_MAIN_STATE_ERROR;
                    return;
                }
            }

            m_subStateConfig = FM_TEMP_SUB_STATE_CONFIG_FINISHED;
            m_TaskID    = MODULE_TASKID_FREE;
            m_mainState = FM_MAIN_STATE_IDLE;
            FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine in idle main state
 *
 *      Depending on the active task ID, the appropriate task function will
 *      be called.
 */
/****************************************************************************/
void CTemperatureControl::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the task to execute the module commands
 *
 *  The function is called from HandleIdleState() if m_taskID == FM_TEMP_TASKID_COMMAND_HDL.
 *  The function loops thru the m_ModuleCommand array and controls the commands stored there
 *
 *   - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the temp. ctrl. function module
 *     on slave side by sending the corresponding CAN-messge
 *
 *   - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout
 *
 *  Usually, a motor command will be confirmed and closed by receiving the expected CAN-message (e.g. act. temperature)
 *  This is done at the HandleCANMessage... function
 */
/****************************************************************************/
void CTemperatureControl::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;

    QMutableListIterator<ModuleCommand_t *> Iterator(m_ModuleCommand);
    while(Iterator.hasNext())
    {
        ModuleCommand_t *p_ModuleCommand = Iterator.next();
        bool RemoveCommand = false;

        if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ)
        {
            // forward the motor command to the motor function module on slave side by sending
            // the corresponding CAN-message
            p_ModuleCommand->State = MODULE_CMD_STATE_REQ_SEND;
            p_ModuleCommand->ReqSendTime.Trigger();

            if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTTEMP)
            {
                //send the act temperature request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDTemperature CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send temp. request";
                RetVal = SendCANMsgServiceSensorRequest(p_ModuleCommand->Index);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActTemperature(GetModuleHandle(), RetVal, p_ModuleCommand->Index, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_SET_OPMODE)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl set operation mode";

                //send the temperature ctrl operating mode to the slave
                RetVal = SendCANMsgSetTemperature();

                RemoveCommand = true;
                //because there is no slave acknowledge, we send our own acknowledge
                emit ReportSetOperatingModeAckn(GetModuleHandle(), RetVal, m_TempCtrlOpMode);
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTOPMODE)
            {
                //send the act operation mode request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDTemperature CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send temp. request";
                RetVal = SendCANMsgTemperatureRequest();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActOperatingMode(GetModuleHandle(), RetVal, TEMPCTRL_OPMODE_UNDEF);
                }
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_SET_STATUS)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl set status";

                //send the temperature ctrl status to the slave
                RetVal = SendCANMsgSetTemperature();

                RemoveCommand = true;
                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportSetStatusAckn(GetModuleHandle(), RetVal, m_TempCtrlState, m_Temperature);
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTSTATUS)
            {
                //send the act temperature ctrl. status request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDTemperature CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send temp. request";
                RetVal = SendCANMsgTemperatureRequest();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActStatus(GetModuleHandle(), RetVal, TEMPCTRL_STATUS_UNDEF, TEMPCTRL_VOLTAGE_UNDEF);
                }
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_RESET_OPTIME)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl reset heater operating time";

                //send the reset command to the slave
                RetVal = SendCANMsgHeaterTimeSet(p_ModuleCommand->Index);

                RemoveCommand = true;
                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportResetHeaterOperatingTime(GetModuleHandle(), RetVal, p_ModuleCommand->Index);
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_OPTIME)
            {
                //send the act temperature ctrl. get operating time request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDHeaterTime CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send get operating time";
                RetVal = SendCANMsgHeaterTimeReq(p_ModuleCommand->Index);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportHeaterOperatingTime(GetModuleHandle(), RetVal, p_ModuleCommand->Index, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_FANSPEED)
            {
                //send the act temperature ctrl. get fan speed request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDFanSpeed CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send temp. get fan speed";
                RetVal = SendCANMsgServiceFanReq(p_ModuleCommand->Index);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportFanSpeed(GetModuleHandle(), RetVal, p_ModuleCommand->Index, 0);
                }
            }
            else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_HARDWARE)
            {
                //send the act temperature ctrl. get fan speed request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDFanSpeed CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANTemperatureControl send temp. get fan speed";
                RetVal = SendCANMsgHardwareReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    p_ModuleCommand->Timeout = CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportHardwareStatus(GetModuleHandle(), RetVal, 0, 0, 0, 0, 0);
                }
            }

            // Check for success
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                RemoveCommand = true;
            }
        }
        else if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ_SEND)
        {
            // Check active module commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTTEMP)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Act temp. request timeout error.";
                    emit ReportActTemperature(GetModuleHandle(), DCL_ERR_TIMEOUT, p_ModuleCommand->Index, 0);
                }
                else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTOPMODE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Act op. mode request timeout error.";
                    emit ReportActOperatingMode(GetModuleHandle(), DCL_ERR_TIMEOUT, TEMPCTRL_OPMODE_UNDEF);
                }
                else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_ACTSTATUS)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Act status request timeout error.";
                    emit ReportActStatus(GetModuleHandle(), DCL_ERR_TIMEOUT, TEMPCTRL_STATUS_UNDEF, TEMPCTRL_VOLTAGE_UNDEF);
                }
                else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_OPTIME)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Operating time request timeout error.";
                    emit ReportHeaterOperatingTime(GetModuleHandle(), DCL_ERR_TIMEOUT, p_ModuleCommand->Index, 0);
                }
                else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_FANSPEED)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Fan speed request timeout error.";
                    emit ReportFanSpeed(GetModuleHandle(), DCL_ERR_TIMEOUT, p_ModuleCommand->Index, 0);
                }
                else if(p_ModuleCommand->Type == FM_TEMP_CMD_TYPE_REQ_HARDWARE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl '" << GetKey().toStdString() <<
                                                  "': Hardware information request timeout error.";
                    emit ReportHardwareStatus(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0, 0, 0, 0);
                }
            }
        }

        if (RemoveCommand == true)
        {
            delete p_ModuleCommand;
            Iterator.remove();
        }
    }

    if(m_ModuleCommand.isEmpty())
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a CAN message
 *
 *  The function is called from communication layer if a CAN message, which
 *  was registered to this class instance, was received.
 *  The message will be forwarded to the specialized function.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return void
 */
/****************************************************************************/
void CTemperatureControl::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG1) << "   CTemperatureControl::HandleCanMessage 0x" << std::hex << pCANframe->can_id;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgEvent(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            m_TempCtrlState = TEMPCTRL_STATUS_OFF;
            emit ReportEvent(BuildEventCode(m_lastEventGroup, m_lastEventCode), m_lastEventData, m_lastEventTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDTemperature)
    {
        HandleCANMsgTemperature(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDServiceSensor)
    {
        HandleCANMsgServiceSensor(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDHeaterTime)
    {
        HandleCANMsgHeaterTime(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDServiceFan)
    {
        HandleCANMsgServiceFan(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDHardware)
    {
        HandleCANMsgHardware(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDNotiInRange)
    {
        HandleCANMsgNotiRange(pCANframe, true);
    }
    else if(pCANframe->can_id == m_unCanIDNotiOutOfRange)
    {
        HandleCANMsgNotiRange(pCANframe, false);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceSensor' response CAN message
 *
 *      The message contains information to a sensor's temperature.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgServiceSensor(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_ACTTEMP);
    }

    if(pCANframe->can_dlc == 3)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        qreal ActTemperature;

        ActTemperature = (qreal)GetCANMsgDataU16(pCANframe, 1) / 100;

        FILE_LOG_L(laFCT, llDEBUG) << " CANTemperatureControl Temperature received: " << ActTemperature;
        emit ReportActTemperature(GetModuleHandle(), hdlInfo, pCANframe->data[0], ActTemperature);
    }
    else
    {
        emit ReportActTemperature(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the 'Temperature' response CAN message
 *
 *      The message contains information to
 *       - temperature control status and operation mode
 *       - desired temperature
 *       - tolerance
 *       - sampling rate
 *       - auto tuning duration
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgTemperature(can_frame* pCANframe)
{
    ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    TempCtrlStatus_t TempCtrlStatus = TEMPCTRL_STATUS_OFF;
    TempCtrlOperatingMode_t TempCtrlOpMode = TEMPCTRL_OPMODE_FULL;
    TempCtrlMainsVoltage_t TempCtrlVoltage = TEMPCTRL_VOLTAGE_220V;

    if(pCANframe->can_dlc == 7)
    {
        //determine the temperature control status
        if(pCANframe->data[0] & (1 << TEMP_CTRL_ACTIVE_BITPOS))
        {
            TempCtrlStatus = TEMPCTRL_STATUS_ON;
        }

        //determine the temperature control operating mode
        if(pCANframe->data[0] & (1 << TEMP_CTRL_PHASE_BITPOS))
        {
            TempCtrlOpMode = TEMPCTRL_OPMODE_HOLD;
        }

        //determine the temperature control mains voltage
        if(pCANframe->data[0] & (1 << TEMP_CTRL_VOLTAGE_BITPOS))
        {
            TempCtrlVoltage = TEMPCTRL_VOLTAGE_110V;
        }

        FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::HandleCANMsgTemperature: "
                                   << (qint32)TempCtrlStatus << ", "
                                   << (qint32)TempCtrlOpMode << ", "
                                   << (qint32)TempCtrlVoltage;
    }
    else
    {
        hdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    // Forward the status information, if still requested
    if(ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_ACTSTATUS))
    {
        emit ReportActStatus(GetModuleHandle(), hdlInfo, TempCtrlStatus, TempCtrlVoltage);
    }

    // Forward the operating mode, if still requested
    if(ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_ACTOPMODE))
    {
        emit ReportActOperatingMode(GetModuleHandle(), hdlInfo, TempCtrlOpMode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'HeaterTime' response CAN message
 *
 *      The message contains information to a operating time counter of a
 *      heating element.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgHeaterTime(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_OPTIME);
    }

    if(pCANframe->can_dlc == 5)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint32 OperatingTime;

        OperatingTime = GetCANMsgDataU32(pCANframe, 1);

        FILE_LOG_L(laFCT, llDEBUG) << " CANTemperatureControl operating time received: " << OperatingTime;
        emit ReportHeaterOperatingTime(GetModuleHandle(), hdlInfo, pCANframe->data[0], OperatingTime);
    }
    else
    {
        emit ReportHeaterOperatingTime(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceFan' response CAN message
 *
 *      The message contains information to a ventilation fan's speed.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgServiceFan(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_FANSPEED);
    }

    if(pCANframe->can_dlc == 3)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint16 FanSpeed;

        FanSpeed = GetCANMsgDataU16(pCANframe, 1);

        FILE_LOG_L(laFCT, llDEBUG) << " CANTemperatureControl fan speed received: " << FanSpeed;
        emit ReportFanSpeed(GetModuleHandle(), hdlInfo, pCANframe->data[0], FanSpeed);
    }
    else
    {
        emit ReportFanSpeed(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceFan' response CAN message
 *
 *      The message contains information to a boards's hardware setup.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgHardware(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_TEMP_CMD_TYPE_REQ_HARDWARE);
    }

    if(pCANframe->can_dlc == 6)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint16 Current;

        Current = GetCANMsgDataU16(pCANframe, 4);

        FILE_LOG_L(laFCT, llDEBUG) << " CANTemperatureControl hardware information received: " << Current;
        emit ReportHardwareStatus(GetModuleHandle(), hdlInfo, pCANframe->data[0], pCANframe->data[1],
                                  pCANframe->data[2], pCANframe->data[3], Current);
    }
    else
    {
        emit ReportHardwareStatus(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, 0, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'NotiInRange' and 'NotiOutOfRange' response message
 *
 *      The message is received when the measured temperature leaves or
 *      enters a predefined range.
 *
 *  \iparam pCANframe = The received CAN message
 *  \iparam InRange = Temperature in range (true) or out of range (false)
 */
/****************************************************************************/
void CTemperatureControl::HandleCANMsgNotiRange(can_frame* pCANframe, bool InRange)
{
    if(pCANframe->can_dlc == 2)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        qreal ActTemperature;

        ActTemperature = (qreal)GetCANMsgDataU16(pCANframe, 0) / 100;

        FILE_LOG_L(laFCT, llDEBUG) << " CANTemperatureControl temperature range notification received: " << ActTemperature;
        emit ReportTemperatureRange(GetModuleHandle(), hdlInfo, InRange, ActTemperature);
    }
    else
    {
        emit ReportTemperatureRange(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, InRange, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the fan parameters
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgFanWatchdogSet()
{
    CANFctModuleTempCtrl* pCANObjConfTemp;
    pCANObjConfTemp = (CANFctModuleTempCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the temperature control:
    if(pCANObjConfTemp)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDFanWatchdogSet;
        canmsg.can_id = m_unCanIDFanWatchdogSet;
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sFanSpeed, 0);
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sFanThreshold, 2);
        canmsg.can_dlc = 4;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastEventHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the heater current parameters
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgCurrentWatchdogSet()
{
    CANFctModuleTempCtrl* pCANObjConfTemp;
    pCANObjConfTemp = (CANFctModuleTempCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the temperature control:
    if(pCANObjConfTemp)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDCurrentWatchdogSet;
        canmsg.can_id = m_unCanIDCurrentWatchdogSet;
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sCurrentGain, 0);
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sHeaterCurrent, 2);
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sHeaterThreshold, 4);
        canmsg.can_dlc = 6;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastEventHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the heater current parameters
 *
 *  \iparam Index = Index of the PID controller
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgPidParametersSet(quint8 Index)
{
    CANFctModuleTempCtrl* pCANObjConfTemp;
    pCANObjConfTemp = (CANFctModuleTempCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the temperature control:
    if(pCANObjConfTemp)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDPIDParamSet;
        canmsg.can_id = m_unCanIDPIDParamSet;
        canmsg.data[0] = Index;
        canmsg.data[1] = pCANObjConfTemp->listPidControllers[Index].sMaxTemperature;
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->listPidControllers[Index].sControllerGain, 2);
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->listPidControllers[Index].sResetTime, 4);
        SetCANMsgDataU16(&canmsg, pCANObjConfTemp->listPidControllers[Index].sDerivativeTime, 6);
        canmsg.can_dlc = 8;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastEventHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set a desired temperature
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgSetTemperature()
{
    CANFctModuleTempCtrl* pCANObjConfTemp;
    pCANObjConfTemp = (CANFctModuleTempCtrl*) m_pCANObjectConfig;
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    if (pCANObjConfTemp == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    canmsg.can_id = m_unCanIDTemperatureSet;
    canmsg.data[0] = 0;
    if(m_TempCtrlState == TEMPCTRL_STATUS_ON) {
        canmsg.data[0] = 0x01;
    }
    if(m_TempCtrlOpMode == TEMPCTRL_OPMODE_HOLD) {
        canmsg.data[0] |= 0x04;
    }

    canmsg.data[1] = (quint8)m_Temperature;

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetTemperature: Data0: 0x" << std::hex << (quint8) canmsg.data[0]
                               << ", Data1: 0x" << std::hex << (quint8) canmsg.data[1];

    canmsg.data[2] = pCANObjConfTemp->bTempTolerance;
    SetCANMsgDataU16(&canmsg, pCANObjConfTemp->sSamplingPeriod, 3);
    canmsg.data[5] = 0;
    canmsg.data[6] = 0;
    canmsg.can_dlc = 7;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetTemperature: CanID: 0x" << std::hex << m_unCanIDTemperatureSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the 'Temperature' CAN-Message
 *
 *      The 'Temperature' CAN-Message contains the reference temperature and
 *      the operation mode.
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgTemperatureRequest()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDTemperatureReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::SendCANMsgTemperatureRequest canID: 0x"
                               << std::hex << m_unCanIDTemperatureReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the 'HeaterTimeSet' CAN-Message
 *
 *      The 'ServiceSensor' CAN-Message contains the temperature measured by
 *      one of the sensors of the Slave module.
 *
 *  \iparam Index = Number of the sensor
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgServiceSensorRequest(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDServiceSensorReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::SendCANMsgServiceSensorRequest canID: 0x"
                               << std::hex << m_unCanIDServiceSensorReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to reset the operating time of a heater
 *
 *  \iparam Index = Number of the heater
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgHeaterTimeSet(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDHeaterTimeSet;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::SendCANMsgHeaterTimeSet canID: 0x"
                               << std::hex << m_unCanIDHeaterTimeSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get the operating time of a heater
 *
 *  \iparam Index = Number of the heater
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgHeaterTimeReq(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDHeaterTimeReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::SendCANMsgHeaterTimeReq canID: 0x"
                               << std::hex << m_unCanIDHeaterTimeReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get speed of a ventilation fan
 *
 *  \iparam Index = Number of the fan
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgServiceFanReq(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDServiceFanReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::SendCANMsgServiceFanReq canID: 0x"
                               << std::hex << m_unCanIDServiceFanReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get hardware information
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SendCANMsgHardwareReq()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDHardwareReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CTemperatureControl::m_unCanIDHardwareReq canID: 0x"
                               << std::hex << m_unCanIDHardwareReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Request the actual temperature
 *
 *  \iparam Index = Temperature sensor index
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::ReqActTemperature(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_ACTTEMP);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Index = Index;
        FILE_LOG_L(laDEV, llINFO) << " CANTemperatureControl, Index: " << Index;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (qint32)m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the temperature control's operation mode
 *
 *  \iparam TempCtrlOpMode = Operation mode to set
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SetOperatingMode(TempCtrlOperatingMode_t TempCtrlOpMode)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_SET_OPMODE);
    if(p_ModuleCommand != NULL)
    {
        m_TempCtrlOpMode = TempCtrlOpMode;
        FILE_LOG_L(laDEV, llINFO) << " CANTemperatureControl, TempCtrlOpMode: " << (int) TempCtrlOpMode;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the operating mode
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::ReqOperatingMode()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_ACTOPMODE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the temperature control's status and set point temperature
 *
 *  \iparam TempCtrlState = Status mode to set
 *  \iparam Temperature = Set point temperature
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::SetStatus(TempCtrlStatus_t TempCtrlState, qreal Temperature)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if (Temperature < 0 || Temperature > 255)
    {
        return (DCL_ERR_INVALID_PARAM);
    }

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_SET_STATUS);
    if(p_ModuleCommand != NULL)
    {
        m_TempCtrlState = TempCtrlState;
        m_Temperature = Temperature;
        FILE_LOG_L(laDEV, llINFO) << " CANTemperatureControl, TempCtrlState: " << (qint32)TempCtrlState
                                  << "Temperature: " << Temperature;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the temp. control status
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::ReqStatus()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_ACTSTATUS);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Resets the operating time of a heater
 *
 *  \iparam Index = Index of the heating element
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::ResetHeaterOperatingTime(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_RESET_OPTIME);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANTemperatureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets the operating time of a heater
 *
 *  \iparam Index = Index of the heating element
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::GetHeaterOperatingTime(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_OPTIME);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANTemperatureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets the speed of a ventilation fan
 *
 *  \iparam Index = Index of the ventilation fan
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::GetFanSpeed(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_FANSPEED);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANTemperatureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets information of the hardware connected to the module
 *
 *      This method will return the following information from the
 *      temperature control module on the Slave: number of temperature
 *      sensors, number of ventilation fans, number of heaters, number of PID
 *      controllers in the control loop, current through the heating circuit
 *      in milliamperes.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CTemperatureControl::GetHardwareStatus()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(FM_TEMP_CMD_TYPE_REQ_HARDWARE);
    if(p_ModuleCommand != NULL)
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANTemperatureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANTemperatureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Adds a new command to the transmit queue
 *
 *  \iparam CommandType = Command type to set
 *
 *  \return Module command, if the command type can be placed, otherwise NULL
 */
/****************************************************************************/
CTemperatureControl::ModuleCommand_t *CTemperatureControl::SetModuleTask(CANTempCtrlCmdType_t CommandType)
{
    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL)) {
        for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
            if (m_ModuleCommand[i]->Type == CommandType) {
                return NULL;
            }
        }

        ModuleCommand_t *p_ModuleCommand = new ModuleCommand_t;
        p_ModuleCommand->Type = CommandType;
        p_ModuleCommand->State = MODULE_CMD_STATE_REQ;
        m_ModuleCommand.append(p_ModuleCommand);

        m_TaskID = MODULE_TASKID_COMMAND_HDL;

        return p_ModuleCommand;
    }

    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  Removes an existing command from the transmit queue
 *
 *  \iparam CommandType = Command of that type will be set to free
 */
/****************************************************************************/
bool CTemperatureControl::ResetModuleCommand(CANTempCtrlCmdType_t CommandType)
{
    bool CommandFound = false;

    for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
        if (m_ModuleCommand[i]->Type == CommandType && m_ModuleCommand[i]->State == MODULE_CMD_STATE_REQ_SEND) {
            delete m_ModuleCommand.takeAt(i);
            CommandFound = true;
            break;
        }
    }

    if(m_ModuleCommand.isEmpty())
    {
        m_TaskID = MODULE_TASKID_FREE;
    }

    return CommandFound;
}

} //namespace
