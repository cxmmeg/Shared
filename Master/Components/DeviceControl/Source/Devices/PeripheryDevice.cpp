
#include "DeviceControl/Include/Devices/PeripheryDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>

namespace DeviceControl
{
#define CHECK_SENSOR_TIME (1000) // in msecs
/****************************************************************************/
/*!
 *  \brief    Constructor of the CPeripheryDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    ModuleList Module List
 *  \param    InstanceID instance id
 */
/****************************************************************************/
CPeripheryDevice::CPeripheryDevice(DeviceProcessing* pDeviceProcessing,
                                   QString& Type,
                                   const DeviceModuleList_t &ModuleList,
                                   quint32 InstanceID)
                                   :CBaseDevice(pDeviceProcessing,
                                                Type, ModuleList,
                                                InstanceID)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Retort device created";
}//lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of CAirLiquidDevice
 */
/****************************************************************************/
CPeripheryDevice::~CPeripheryDevice()
{
    try
    {
        Reset();
    }
    catch(...)
    {
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Reset class member variable
 */
/****************************************************************************/
void CPeripheryDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = PER_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_MAIN_CONTROL;
    m_LastGetLocalAlarmStatusTime = 0;
    m_LastGetRemoteAlarmStatusTime = 0;

    memset( &m_pDigitalOutputs, 0 , sizeof(m_pDigitalOutputs));  //lint !e545
    memset( &m_TargetDOOutputValues, 0 , sizeof(m_TargetDOOutputValues)); //lint !e545
    memset( &m_pDigitalInputs, 0 , sizeof(m_pDigitalInputs));  //lint !e545
    memset( &m_TargetDIInputValues, 0 , sizeof(m_TargetDIInputValues)); //lint !e545

}

/****************************************************************************/
/*!
 *  \brief  Handles the internal state machine
 *
 *      This function should be called periodically, it handles the state
 *      machine of the class. Furthermore, the HandleTask-function of the
 *      interface class will be called. Each main state has a individual
 *      handling function, which will be called according to the currently
 *      active main state.
 */
/****************************************************************************/
void CPeripheryDevice::HandleTasks()
{
    ReturnCode_t RetVal;

    if(m_MainState == DEVICE_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_MainState == DEVICE_MAIN_STATE_START)
    {
        //fall through
        m_MainState = DEVICE_MAIN_STATE_INIT;
    }
    else if(m_MainState == DEVICE_MAIN_STATE_INIT)
    {
        RetVal = HandleInitializationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_CONFIG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_FCT_MOD_CFG)
    {
        RetVal = ConfigureDeviceTasks();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_IDLE;
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CPeripheryDevice:handleTasks, new state: " << (int) m_MainState;
        m_MainStateOld = m_MainState;
    }
}
/****************************************************************************/
/*!
 *  \brief    Internal function for idle state machine processing
 *
 *   The function handles the idle state, which is active if the class is 'ready for tasks'
 *   Depending on the pending task, which is stored in m_TaskID, the task specific handling
 *   function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void CPeripheryDevice::HandleIdleState()
{
    CheckSensorsData();
}


/****************************************************************************/
/*!
 *  \brief   Handles the classes initialization state.
 *
 *           This function attaches the function modules pointer variables
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 *
 ****************************************************************************/
ReturnCode_t CPeripheryDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pBaseModule = NULL;

    FILE_LOG_L(laDEV, llINFO) << "  CPeripheryDevice::HandleInitializationState()";

    quint32 InstanceID;
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerMainRelayDOKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pDigitalOutputs[PER_MAIN_RELAY] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pDigitalOutputs[PER_MAIN_RELAY] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_BOTTOMTEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstDOTypeMap[CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO] = PER_MAIN_RELAY;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerRemoteAlarmCtrlDOKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMCTRLDO not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstDOTypeMap[CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO] = PER_REMOTE_ALARM_CTRL;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerLocalAlarmCtrlDOKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL] = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMCTRLDO);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMCTRLDO not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstDOTypeMap[CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO] = PER_LOCAL_ALARM_CTRL;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerLocalAlarmDIKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pDigitalInputs[PER_LOCAL_ALARM_STATUS] = (CDigitalInput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMDI);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_LOCALALARMDI not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstDITypeMap[CANObjectKeyLUT::FCTMOD_PER_LOCALALARMDI] = PER_LOCAL_ALARM_STATUS;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_PerRemoteAlarmDIKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pDigitalInputs[PER_REMOTE_ALARM_STATUS] = (CDigitalInput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMDI);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_PER_REMOTEALARMDI not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstDITypeMap[CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMDI] = PER_REMOTE_ALARM_STATUS;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;

}
/****************************************************************************/
/*!
 *  \brief   Handles the classes configuration state.
 *
 *           This function connects each function module's signals to the internal slots.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if configuration was successfully executed
 *           otherwise DCL_ERR_FCT_CALL_FAILED
 *
 ****************************************************************************/
ReturnCode_t CPeripheryDevice::HandleConfigurationState()
{

    if(!connect(m_pDigitalOutputs[PER_MAIN_RELAY], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_MAINRELAYDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMCTRLDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    /*
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_SET], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMSETDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pDigitalOutputs[PER_REMOTE_ALARM_CLEAR], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMCLEARDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital output signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
*/
    if(!connect(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS], SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnGetDIValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_LOCALALARMDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS], SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnGetDIValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_MC_DEV, ERROR_DCL_PER_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_PER_REMOTEALARMDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Read device's sensors data asynchronizely
 */
/****************************************************************************/
void CPeripheryDevice::CheckSensorsData()
{
    if (!CBaseDevice::m_SensorsDataCheckFlag) {
        return ;
    }

    if(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS])
    {
        (void)GetLocalAlarmStatusAsync();
    }
    if(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS])
    {
        (void)GetRemoteAlarmStatusAsync();
    }
}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CPeripheryDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}
void CPeripheryDevice::HandleErrorState()
{
    if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        m_ErrorTaskState = PER_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = PER_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == PER_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pDigitalOutputs[PER_MAIN_RELAY] = 0;
        m_pDigitalOutputs[PER_REMOTE_ALARM_CTRL] = 0;
        m_pDigitalOutputs[PER_LOCAL_ALARM_CTRL] = 0;
        m_pDigitalInputs[PER_LOCAL_ALARM_STATUS] = 0;
        m_pDigitalInputs[PER_REMOTE_ALARM_STATUS] = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief   Set digital output value.
 *
 *
 *  \iparam Type = Actual digital output module.
 *  \iparam OutputValue = Actual output value of fan's digital output module.
 *  \iparam Duration = Duration of the output(not used now).
 *  \iparam Delay = UNUSED.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::SetDOValue(PerDOType_t Type, quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    m_TargetDOOutputValues[Type] = OutputValue;

    DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_PER_SET_DO_VALUE);
    ReturnCode_t retCode = m_pDigitalOutputs[Type]->SetOutputValue(m_TargetDOOutputValues[Type], Duration, Delay);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(event);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set digital output value.
 *
 *  This slot is connected to the signal, ReportOutputValueAckn
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam OutputValue = Output Value.
 *
 */
/****************************************************************************/
void CPeripheryDevice::OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue)
{
    Q_UNUSED(OutputValue)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Periphery Set DO output successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Periphery set DO output failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_PER_SET_DO_VALUE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Turn on the main relay.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOnMainRelay()
{
    return SetDOValue(PER_MAIN_RELAY,1, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Turn off the main relay.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOffMainRelay()
{
    return SetDOValue(PER_MAIN_RELAY, 0, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Turn on the local alarm.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOnLocalAlarm()
{
    return SetDOValue(PER_LOCAL_ALARM_CTRL,0, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Turn off the local alarm.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOffLocalAlarm()
{
    return SetDOValue(PER_LOCAL_ALARM_CTRL, 1, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Turn on the remote alarm.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOnRemoteAlarm()
{
    return SetDOValue(PER_REMOTE_ALARM_CTRL,0, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Turn off the remote alarm.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::TurnOffRemoteAlarm()
{
    return SetDOValue(PER_REMOTE_ALARM_CTRL, 1, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Get Local Alarm status asynchronously.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::GetLocalAlarmStatusAsync()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetLocalAlarmStatusTime) >= CHECK_SENSOR_TIME) // check if 1000 msec has passed since last read
    {
        m_LastGetLocalAlarmStatusTime = Now;
        if(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS])
        {
            return m_pDigitalInputs[PER_LOCAL_ALARM_STATUS]->ReqActInputValue();
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Get Remote Alarm status asynchronously.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPeripheryDevice::GetRemoteAlarmStatusAsync()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetRemoteAlarmStatusTime) >= CHECK_SENSOR_TIME) // check if 1000 msec has passed since last read
    {
        m_LastGetRemoteAlarmStatusTime = Now;
        if(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS])
        {
            return m_pDigitalInputs[PER_REMOTE_ALARM_STATUS]->ReqActInputValue();
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}
/****************************************************************************/
/*!
 *  \brief   slot associated with get digital input value.
 *
 *  This slot is connected to the signal, ReportActInputValue
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam InputValue = Actual digital input value.
 *
 */
/****************************************************************************/
void CPeripheryDevice::OnGetDIValue(quint32 InstanceID, ReturnCode_t ReturnCode, quint16 InputValue)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Per Get DI value successful! ";
        m_TargetDIInputValues[m_InstDITypeMap[InstanceID]] = InputValue;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Per Get DI value failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_PER_GET_DI_VALUE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Get Local Alarm Connection status.
 *
 *  \return  1: Conencted, 0: Not connected
 */
/****************************************************************************/
quint16 CPeripheryDevice::GetLocalAlarmStatus()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;
    if(m_pDigitalInputs[PER_LOCAL_ALARM_STATUS])
    {
        retCode = m_pDigitalInputs[PER_LOCAL_ALARM_STATUS]->ReqActInputValue();
    }
    else
    {
        return UNDEFINED_2_BYTE;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_PER_GET_DI_VALUE);
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    return m_TargetDIInputValues[PER_LOCAL_ALARM_STATUS];
}

/****************************************************************************/
/*!
 *  \brief   Get the Remote Alarm status captured in last 500 milliseconds.
 *
 *  \return  Actual connection status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 CPeripheryDevice::GetRecentRemoteAlarmStatus()
{
    // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue;
    if((Now - m_LastGetRemoteAlarmStatusTime) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_TargetDIInputValues[PER_REMOTE_ALARM_STATUS];
    }
    else
    {
        RetValue = UNDEFINED_2_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Get the Local Alarm status captured in last 500 milliseconds.
 *
 *  \return  Actual connection status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 CPeripheryDevice::GetRecentLocalAlarmStatus()
{
    // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue;
    if((Now - m_LastGetLocalAlarmStatusTime) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_TargetDIInputValues[PER_LOCAL_ALARM_STATUS];
    }
    else
    {
        RetValue = UNDEFINED_2_BYTE;
    }
    return RetValue;
}


/****************************************************************************/
/*!
 *  \brief  Get Remote Alarm Connection status.
 *
 *  \return  1: Conencted, 0: Not connected
 */
/****************************************************************************/
quint16 CPeripheryDevice::GetRemoteAlarmStatus()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_FAILED;
    if(m_pDigitalInputs[PER_REMOTE_ALARM_STATUS])
    {
        retCode = m_pDigitalInputs[PER_REMOTE_ALARM_STATUS]->ReqActInputValue();
    }
    else
    {
        return UNDEFINED_2_BYTE;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_PER_GET_DI_VALUE);
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    return m_TargetDIInputValues[PER_REMOTE_ALARM_STATUS];
}



} //namespace
