/****************************************************************************/
/*! \file AnalogInput.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the CAnalogInput class
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

#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor for the CAnalogInput
 *
 *  \param    p_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator = pointer to communication class
 *  \param    pParentNode = pointer to base module, where this module is assigned to
 *
 ****************************************************************************/
CAnalogInput::CAnalogInput(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                           CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_ANALOG_IN_PORT, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDAnaInputConfigInput(0), m_unCanIDAnaInputConfigLimits(0), m_unCanIDAnaInputStateReq(0),
    m_unCanIDAnaInputState(0), m_ActInputValue(0)
{
    /// \todo Auto-generated constructor stub
    m_mainState = FM_MAIN_STATE_BOOTUP;
    m_SubStateConfig = FM_AINP_SUB_STATE_CONFIG_INIT;

    //module command array initialisation
    for(quint8 idx = 0; idx < MAX_MODULE_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CANAnalogInput
 *
 ****************************************************************************/
CAnalogInput::~CAnalogInput()
{
}

/****************************************************************************/
/*!
 *  \brief    Initialize this function module
 *
 *   The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *   Receiveable CAN-message are registered to the communication layer
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::Initialize()
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

    if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
    {
        FILE_LOG_L(laINIT, llERROR) << " function module initialisation failed: " << (int) RetVal;
        m_mainState = FM_MAIN_STATE_ERROR;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Initialize the module's CAN message IDs
 *
 *   The CAN-IDs are read from the CAN-Message configuration class.
 *   The CAN-ID is composed by the message key, the function
 *   module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::InitializeCANMessages()
{
    ReturnCode_t RetVal;
    quint8 bIfaceID;
    const quint8 ModuleID = MODULE_ID_ANALOG_IN;

    if (m_pCANObjectConfig == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }
    bIfaceID = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDAnaInputConfigInput  = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogInputConfigInput", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaInputConfigLimits = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogInputConfigLimits", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaInputStateReq     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogInputStateReq", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDAnaInputState        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AnalogInputState", bIfaceID, m_pParent->GetNodeID());

    FILE_LOG_L(laINIT, llDEBUG) << "  CAN-messages for fct-module: " << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo              : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning           : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError             : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError        : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogInputConfigInput : 0x" << std::hex << m_unCanIDAnaInputConfigInput;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogInputConfigLimits: 0x" << std::hex << m_unCanIDAnaInputConfigLimits;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogInputStateReq    : 0x" << std::hex << m_unCanIDAnaInputStateReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   AnalogInputState       : 0x" << std::hex << m_unCanIDAnaInputState;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Register the receive CAN-messages to communication layer
 *
 *   Each receiveable CAN-message must be registered to the communication layer.
 *   This enables the communication layer to call the 'HandleCANMessage(..)' function of this
 *   instance after receiption of the message.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::RegisterCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAnaInputState, this);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine
 *
 *   Depending on the active main state, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogInput::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIRMED)
    {
        //fall through
        m_mainState = FM_MAIN_STATE_CONFIG;
        m_SubStateConfig = FM_AINP_SUB_STATE_CONFIG_START;
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIG)
    {
        if(m_SubStateConfig == FM_AINP_SUB_STATE_CONFIG_START)
        {
            ReturnCode_t RetVal;
            RetVal = SendCANMessageConfiguration();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_SubStateConfig = FM_AINP_SUB_STATE_CONFIG_FINISHED;
                m_TaskID = MODULE_TASKID_FREE;
                m_mainState = FM_MAIN_STATE_IDLE;

                FILE_LOG_L(laCONFIG, llDEBUG) << " AnalogInput " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
            }
            else
            {
                FILE_LOG_L(laCONFIG, llERROR) << " AnalogInput " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_mainState = FM_MAIN_STATE_ERROR;
                m_SubStateConfig = FM_AINP_SUB_STATE_CONFIG_ERROR;
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CAnalogInput::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the task to execute the module commands
 *
 *   The function is called from HandleIdleState() if m_taskID == FM_AINP_TASKID_COMMAND_HDL.
 *   The function loops thru the m_ModuleCommand array and controls the commands pending for execution there
 *
 *    - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the function module on slave side by sending
 *      the corresponding CAN-message. After it the command's state will be set to MODULE_CMD_STATE_REQ_SEND
 *
 *    - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout.
 *      Usually, a module command will be confirmed and closed by receiving the expected CAN-message (e.g. acknowledge, or data)
 *      The state will be exited when the expected CAN-message was received. this is done at the HandleCANMessage... - function
 *    - Module command with state MODULE_CMD_STATE_ERR will be set to MODULE_CMD_STATE_FREE, its error informations are forwarded
 *      to the device control layer by calling whose ThrowError(...)-function
 *
 */
/****************************************************************************/
void CAnalogInput::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_MODULE_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ)
        {
            // forward the motor command to the motor function module on slave side by sending
            // the corresponding CAN-message
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].Type == FM_AI_CMD_TYPE_ACTVALUE_REQ)
            {
                //send the value request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDAnaInputState CAN-message
                FILE_LOG_L(laFCT, llINFO) << "   AnalogInput:: input value req.: send request.";
                RetVal = SendCANMsgAnalogInputStateReq();

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_AINP_TIMEOUT_READ_REQ;
                }
                else
                {
                    emit ReportActInputValue(GetModuleHandle(), RetVal, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_AI_CMD_TYPE_CONFIG_INPUT)
            {
                FILE_LOG_L(laFCT, llINFO) << "   AnalogInput:: input set state: send request.";
                RetVal = SendCANMessageConfiguration(m_ModuleCommand[idx].Enable);

                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                emit ReportSetState(GetModuleHandle(), RetVal);
            }

            //check for success
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                //trigger timeout supervision
                m_ModuleCommand[idx].ReqSendTime.Trigger();
            }
            else
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
            }
        }
        else if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check avtive motor commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].ReqSendTime.Elapsed() > m_ModuleCommand[idx].Timeout)
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;

                if(m_ModuleCommand[idx].Type == FM_AI_CMD_TYPE_ACTVALUE_REQ)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANAnalogInput:: '" << GetKey().toStdString() << "': input value req. timeout";
                    emit ReportActInputValue(GetModuleHandle(), DCL_ERR_TIMEOUT, 0);
                }
            }
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief    Handle the reception of a CAN message
 *
 *   The function is called from communication layer if a CAN message, which
 *   was registered to this class instance, was received.
 *   The message will be forwarded to the specialized function.
 *
 *  \iparam   pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CAnalogInput::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG) << "  AnalogInput::HandleCanMessage: 0x" << std::hex << pCANframe->can_id;
    FILE_LOG_L(laFCT, llDEBUG) << "                  dlc:" << (int) pCANframe->can_dlc << " Task:" << (int) m_TaskID;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgError(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDAnaInputState)
    {
        HandleCANMsgAnalogInputState(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the 'InputState' CAN message
 *
 *  \iparam pCANframe = struct contains the data of the received CAN message
 */
/****************************************************************************/
void CAnalogInput::HandleCANMsgAnalogInputState(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_AI_CMD_TYPE_ACTVALUE_REQ);
    }

    if(pCANframe->can_dlc == 2)
    {
        m_ActInputValue = ((qint16) pCANframe->data[0]) << 8;
        m_ActInputValue |= ((qint16) pCANframe->data[1]);
        FILE_LOG_L(laFCT, llINFO) << "  AnalogInput: Input value received: " << std::hex << m_ActInputValue;

        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, m_ActInputValue);

    }
    else
    {
        emit ReportActInputValue(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Send the 'Configuration' CAN-Messages (both, Config and Limits)
 *
 *      The function module's configuration parameter will be sent via
 *      CAN-Bus to the slave. The function module on slave side needs the
 *      following parameters.
 *
 *  \iparam Enable = Module enable flag
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::SendCANMessageConfiguration(bool Enable)
{
    ReturnCode_t RetVal;
    CANFctModuleAnalogInput* pCANObjConfAnaInPort;
    pCANObjConfAnaInPort = (CANFctModuleAnalogInput*) m_pCANObjectConfig;
    can_frame canmsg;

    if (pCANObjConfAnaInPort == NULL) {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration (I/O-dir): 0x" << std::hex << m_unCanIDAnaInputConfigInput;
    canmsg.can_id = m_unCanIDAnaInputConfigInput;
    canmsg.can_dlc = 5;

    canmsg.data[0] = 0;
    if(Enable)
    {
        canmsg.data[0] = 0x80;
    }
    if(pCANObjConfAnaInPort->m_bTimeStamp)
    {
        canmsg.data[0] |= 0x40;
    }
    if(pCANObjConfAnaInPort->m_bFastSampling)
    {
        canmsg.data[0] |= 0x20;
    }
    canmsg.data[1] = pCANObjConfAnaInPort->m_sInterval;
    canmsg.data[2] = pCANObjConfAnaInPort->m_sDebounce;
    SetCANMsgDataU16(&canmsg, pCANObjConfAnaInPort->m_sLimitAutoSend, 3);

    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send limits : 0x" << std::hex << m_unCanIDAnaInputConfigLimits;
        canmsg.can_id = m_unCanIDAnaInputConfigLimits;
        canmsg.can_dlc = 8;

        canmsg.data[0] = 0;
        if(pCANObjConfAnaInPort->m_bLimitValue1SendExceed)
        {
            canmsg.data[0] = 0x01;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue1SendBelow)
        {
            canmsg.data[0] |= 0x02;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue1SendWarnMsg)
        {
            canmsg.data[0] |= 0x10;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue1SendDataMsg)
        {
            canmsg.data[0] |= 0x20;
        }
        SetCANMsgDataU16(&canmsg, pCANObjConfAnaInPort->m_sLimitValue1, 1);

        canmsg.data[3] = 0;
        if(pCANObjConfAnaInPort->m_bLimitValue2SendExceed)
        {
            canmsg.data[3] = 0x01;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue2SendBelow)
        {
            canmsg.data[3] |= 0x02;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue2SendWarnMsg)
        {
            canmsg.data[3] |= 0x10;
        }
        if(pCANObjConfAnaInPort->m_bLimitValue2SendDataMsg)
        {
            canmsg.data[3] |= 0x20;
        }
        SetCANMsgDataU16(&canmsg, pCANObjConfAnaInPort->m_sLimitValue2, 4);

        SetCANMsgDataU16(&canmsg, pCANObjConfAnaInPort->m_sHysteresis, 6);

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief  Send the 'Configuration' CAN-Messages (both, Config and Limits)
 *
 *      The function module's configuration parameter will be sent via
 *      CAN-Bus to the slave. The function module on slave side needs the
 *      following parameters.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::SendCANMessageConfiguration()
{
    CANFctModuleAnalogInput* pCANObjConfAnaInPort = (CANFctModuleAnalogInput*) m_pCANObjectConfig;

    if(pCANObjConfAnaInPort->m_bEnabled)
    {
        return SendCANMessageConfiguration(true);
    }
    return SendCANMessageConfiguration(false);
}

/****************************************************************************/
/*!
 *  \brief  Send the 'InputStateRequest' CAN-Message
 *
 *  A request to trigger the slave sending it's m_unCanIDAnaInputState
 *  CAN-message will be sent via CAN-Bus.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::SendCANMsgAnalogInputStateReq()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDAnaInputStateReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Request the actual analog input value
 *
 *      The class acknowledges this request by sending the signal
 *      ReportActInputValue.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::ReqActInputValue()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if (!SetModuleTask(FM_AI_CMD_TYPE_ACTVALUE_REQ, &CmdIndex))
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CAnalogInput '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Activates or deactivates the analog input
 *
 *      The class acknowledges this request by sending the signal
 *      ReportSetState.
 *
 *  \iparam Enable = True if enable, else false.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t CAnalogInput::SetState(bool Enable)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_AI_CMD_TYPE_CONFIG_INPUT, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].Enable = Enable;
        FILE_LOG_L(laDEV, llINFO) << " CAnalogInput, State: " << (int) Enable;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << "   CAnalogInput '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}


/****************************************************************************/
/*!
 *  \brief  Helper function, sets a free module command to the given command type
 *
 *  \iparam CommandType = command type to set
 *  \iparam pCmdIndex = pointer to index within the command array the command is set to (optional parameter, default 0)
 *
 *  \return true, if the command type can be placed, otherwise false
 */
/****************************************************************************/
bool CAnalogInput::SetModuleTask(CANAnalogInputModuleCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < MAX_MODULE_CMD_IDX; idx++)
        {
            if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_FREE)
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ;
                m_ModuleCommand[idx].Type = CommandType;

                m_TaskID = MODULE_TASKID_COMMAND_HDL;
                CommandAdded  = true;
                if(pCmdIndex)
                {
                    *pCmdIndex = idx;
                }

                FILE_LOG_L(laFCT, llINFO) << " CANAnalogOutput:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/*!
 *  \brief  Set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \iparam ModuleCommandType = ModuleCommands having this command type will be set to free
 */
/****************************************************************************/
void CAnalogInput::ResetModuleCommand(CANAnalogInputModuleCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_MODULE_CMD_IDX; idx++)
    {
        if((m_ModuleCommand[idx].Type == ModuleCommandType) &&
           (m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND))
        {
            m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
        }

        if(m_ModuleCommand[idx].State != MODULE_CMD_STATE_FREE)
        {
            ActiveCommandFound = true;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

} //namespace
