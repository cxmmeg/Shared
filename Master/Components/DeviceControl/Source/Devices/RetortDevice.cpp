#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include <sys/stat.h>
#include <QtDebug>
#include <unistd.h>
#include <DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h>

namespace DeviceControl
{
#define CHECK_SENSOR_TIME  (800) // in msecs
#define CHECK_CURRENT_TIME (900) // in msecs
#define CHECK_LOCKER_TIME  (700) // in msecs
const qint32 TOLERANCE = 10; //!< tolerance value for calculating inside and outside range


/****************************************************************************/
/*!
 *  \brief    Constructor of the CRetortDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *  \param    ModuleList Module List
 *  \param    InstanceID instance id
 */
/****************************************************************************/
CRetortDevice::CRetortDevice(DeviceProcessing* pDeviceProcessing, QString& Type, const DeviceModuleList_t &ModuleList,
                             quint32 InstanceID) : CBaseDevice(pDeviceProcessing, Type, ModuleList, InstanceID)
{
    Reset();
    FILE_LOG_L(laDEV, llINFO) << "Retort device created";
}//lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of CRetortDevice
 */
/****************************************************************************/
CRetortDevice::~CRetortDevice()
{
    try
    {
        Reset();
    }
    catch(...)
    {
        return;
    }
} //lint !e1579

/****************************************************************************/
/*!
 *  \brief  Reset class member variable
 */
/****************************************************************************/
void CRetortDevice::Reset()
{
    m_MainState      = DEVICE_MAIN_STATE_START;
    m_MainStateOld   = m_MainState;
    m_ErrorTaskState   = RT_DEV_ERRTASK_STATE_FREE;

    m_instanceID = DEVICE_INSTANCE_ID_RETORT;

    m_pLockDigitalOutput = NULL;
    m_pLockDigitalInput = NULL;
    memset( &m_LastGetTempTime, 0 , sizeof(m_LastGetTempTime)); //lint !e545
    memset( &m_TargetTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_TargetTempCtrlStatus)); //lint !e545 !e641
    memset( &m_CurrentTempCtrlStatus, TEMPCTRL_STATUS_UNDEF , sizeof(m_CurrentTempCtrlStatus)); //lint !e545 !e641
    memset( &m_CurrentTemperatures, 0 , sizeof(m_CurrentTemperatures)); //lint !e545
    memset( &m_TargetTemperatures, 0 , sizeof(m_TargetTemperatures)); //lint !e545
    memset( &m_MainsVoltageStatus, 0 , sizeof(m_MainsVoltageStatus)); //lint !e545
    memset( &m_pTempCtrls, 0 , sizeof(m_pTempCtrls)); //lint !e545
    memset( &m_HardwareStatus, 0, sizeof(m_HardwareStatus)); //lint !e545
    m_TargetDOOutputValue = 0;
    m_LockStatus = 0;
    m_LastGetLockStatusTime = 0;
    memset(&m_LastGetHardwareStatusTime, 0, sizeof(m_LastGetHardwareStatusTime));
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
void CRetortDevice::HandleTasks()
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
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_CONFIG)
    {
        RetVal = HandleConfigurationState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = DEVICE_MAIN_STATE_FCT_MOD_CFG;
            /// \todo maybe we need a state to ensure the reference run call!!
        }
        else
        {
            m_lastErrorHdlInfo = RetVal;
            m_MainState = DEVICE_MAIN_STATE_ERROR;
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
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
            m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_IFACE;
        }
    }
    else if(m_MainState == DEVICE_MAIN_STATE_ERROR)
    {
        HandleErrorState();
    }

    if(m_MainStateOld != m_MainState)
    {
        FILE_LOG_L(laDEV, llINFO) << "   CRetortDevice:handleTasks, new state: " << (int) m_MainState;
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
void CRetortDevice::HandleIdleState()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if(now > (m_LastSensorCheckTime + MINIMUM_CHECK_SENSOR_T))
    {
        CheckSensorsData();
        m_LastSensorCheckTime = now;
    }
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
ReturnCode_t CRetortDevice::HandleInitializationState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pBaseModule = NULL;

    FILE_LOG_L(laDEV, llINFO) << "  CRetortDevice::HandleInitializationState()";
    quint32 InstanceID;
    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortBottomTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[RT_BOTTOM] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[RT_BOTTOM] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_BOTTOMTEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL] = RT_BOTTOM;  //lint !e641
            if (m_ModuleLifeCycleRecord)
            {
                PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("temp_retort_bottom");
                if (pPartLifeCycleRecord)
                {
                    m_pTempCtrls[RT_BOTTOM]->SetPartLifeCycleRecord(pPartLifeCycleRecord);
                }
            }
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortSideTempCtrlKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pTempCtrls[RT_SIDE] = (CTemperatureControl*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pTempCtrls[RT_SIDE] == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_SIDETEMPCTRL not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_InstTCTypeMap[ CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL] = RT_SIDE;  //lint !e641
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortLockDOKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pLockDigitalOutput = (CDigitalOutput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pLockDigitalOutput == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDO);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_LOCKDO not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    InstanceID = GetFctModInstanceFromKey(CANObjectKeyLUT::m_RetortLockDIKey);
    pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
    (void)InsertBaseModule(pBaseModule);
    if(m_pDevProc->CheckFunctionModuleExistence(InstanceID))
    {
        m_pLockDigitalInput = (CDigitalInput*) m_pDevProc->GetFunctionModule(InstanceID);
        if(m_pLockDigitalInput == 0)
        {
            // the function module could not be allocated
            SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_INIT_FCT_ALLOC_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
            FILE_LOG_L(laDEV, llERROR) << "   Error at initialisation state, FCTMOD_RETORT_LOCKDI not allocated.";
            RetVal = DCL_ERR_FCT_CALL_FAILED;
        }

        if (m_ModuleLifeCycleRecord)
        {
            PartLifeCycleRecord* pPartLifeCycleRecord = m_ModuleLifeCycleRecord->m_PartLifeCycleMap.value("Retort_Lid_Lock");
            if (pPartLifeCycleRecord)
            {
                quint32 lifeCycle = pPartLifeCycleRecord->m_ParamMap.value("lid_status_LifeCycle").toUInt();
                m_pLockDigitalInput->SetLifeCycle(lifeCycle * 2);
                m_pLockDigitalInput->SetPartLifeCycleRecord(pPartLifeCycleRecord);
            }
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
ReturnCode_t CRetortDevice::HandleConfigurationState()
{

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportRefTemperature(quint32, ReturnCode_t, qreal)),
                this, SLOT(OnSetTemp(quint32, ReturnCode_t, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportRefTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportActTemperature(quint32, ReturnCode_t, quint8, qreal)),
                this, SLOT(OnGetTemp(quint32, ReturnCode_t, quint8, qreal))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActTemperature'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }


    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportSetPidAckn(quint32, ReturnCode_t, quint16, quint16, quint16, quint16)),
                this, SLOT(OnSetTempPid(quint32, ReturnCode_t, quint16, quint16, quint16, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetPidAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportSetSwitchState(quint32, ReturnCode_t, qint8, qint8)),
                this, SLOT(OnSetSwitchState(quint32, ReturnCode_t, qint8, qint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetSwitchState'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportSetSwitchState(quint32, ReturnCode_t, qint8, qint8)),
                this, SLOT(OnSetSwitchState(quint32, ReturnCode_t, qint8, qint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportSetSwitchState'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t)),
                this, SLOT(OnTempControlStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportActStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }
    if(!connect(m_pLockDigitalOutput, SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                this, SLOT(OnSetDOOutputValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDO);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportOutputValueAckn'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_BOTTOM], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
            this, SLOT(OnGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pTempCtrls[RT_SIDE], SIGNAL(ReportHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8)),
            this, SLOT(OnGetHardwareStatus(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint16, quint8))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL);
        FILE_LOG_L(laDEV, llERROR) << "   Connect temperature ctrl signal 'ReportHardwareStatus'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLockDigitalInput, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
            this, SLOT(OnGetDIValue(quint32, ReturnCode_t, quint16))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportActInputValue'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    if(!connect(m_pLockDigitalInput, SIGNAL(ReportError(quint32,quint16,quint16,quint16,QDateTime)),
                this, SLOT(OnFunctionModuleError(quint32,quint16,quint16,quint16,QDateTime))))
    {
        SetErrorParameter(EVENT_GRP_DCL_RT_DEV, ERROR_DCL_RT_DEV_CONFIG_CONNECT_FAILED, (quint16) CANObjectKeyLUT::FCTMOD_RETORT_LOCKDI);
        FILE_LOG_L(laDEV, llERROR) << "   Connect digital input signal 'ReportError'failed.";
        return DCL_ERR_FCT_CALL_FAILED;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;

}

/****************************************************************************/
/*!
 *  \brief   Read Retort device's sensors data asynchronizely
 */
/****************************************************************************/
void CRetortDevice::CheckSensorsData()
{
    if (!CBaseDevice::m_SensorsDataCheckFlag) {
        return ;
    }

    if(m_pTempCtrls[RT_BOTTOM])
    {
        (void)GetTemperatureAsync(RT_BOTTOM, 0);
        (void)GetTemperatureAsync(RT_BOTTOM, 1);
        (void)GetHardwareStatusAsync(RT_BOTTOM);
    }
    if(m_pTempCtrls[RT_SIDE])
    {
        (void)GetTemperatureAsync(RT_SIDE, 0);
        (void)GetHardwareStatusAsync(RT_SIDE);
    }
    if(m_pLockDigitalInput)
    {
        (void)GetLockStatusAsync();
    }
}

/****************************************************************************/
/*!
 *  \brief   Create and configure the device tasks
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 *
 ****************************************************************************/
ReturnCode_t CRetortDevice::ConfigureDeviceTasks()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}
void CRetortDevice::HandleErrorState()
{
    if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_IDLE)
    {
        ;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_REPORT_IFACE)
    {
        m_ErrorTaskState = RT_DEV_ERRTASK_STATE_REPORT_DEVPROC;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_REPORT_DEVPROC)
    {
        if(m_pDevProc != 0)
        {
            m_pDevProc->ThrowError(m_instanceID, m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
        m_ErrorTaskState = RT_DEV_ERRTASK_STATE_IDLE;
    }
    else if(m_ErrorTaskState == RT_DEV_ERRTASK_STATE_RESET)
    {
        m_MainState = DEVICE_MAIN_STATE_START;
        // reset error notification data
        m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        m_lastErrorGroup = 0;
        m_lastErrorCode = 0;
        m_lastErrorData = 0;
        // reset the function module references
        m_pTempCtrls[RT_BOTTOM] = 0;
        m_pTempCtrls[RT_SIDE] = 0;
        m_pLockDigitalOutput= 0;
    }
}

/****************************************************************************/
/*!
 *  \brief    Set temperature control's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  TempCtrlStatus = New temperature control status.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetTemperatureControlStatus(RTTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus)
{
    m_TargetTempCtrlStatus[Type] = TempCtrlStatus;
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        retCode = m_pTempCtrls[Type]->SetStatus(TempCtrlStatus);
        return retCode;
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   Enable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetTempCtrlON(RTTempCtrlType_t Type)
{
    return SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief   Disable temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetTempCtrlOFF(RTTempCtrlType_t Type)
{
    return SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Get the temperature sensor data captured in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Actual temperature sensor index.
 *
 *  \return  Actual temperature, UNDEFINED if failed.
 */
/****************************************************************************/
qreal CRetortDevice::GetRecentTemperature(RTTempCtrlType_t Type, quint8 Index)
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue;
    if((Now - m_LastGetTempTime[Type][Index]) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_CurrentTemperatures[Type][Index];
    }
    else
    {
        //LogDebug(QString("In Retort device, invalid temperature. Current state is: %1").arg(m_MainState));
        RetValue = UNDEFINED_4_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Get the current of temperature sensor in last 500 milliseconds.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Actual current, UNDEFINED if failed.
 */
/****************************************************************************/
quint32 CRetortDevice::GetRecentCurrent(RTTempCtrlType_t Type)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint32 RetValue;
    if((Now - m_LastGetHardwareStatusTime[Type]) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_HardwareStatus[Type].Current;
    }
    else
    {
        RetValue = UNDEFINED_4_BYTE;
    }
    return RetValue;
}

quint8 CRetortDevice::GetRecentHeaterSwitchType()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint32 RetValue = UNDEFINED_1_BYTE;
    if((Now - m_LastGetHardwareStatusTime[RT_BOTTOM]) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_HardwareStatus[RT_BOTTOM].HeaterSwitchType;

    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::StartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    ReturnCode_t retCode;
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        return DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON;
    }
    if (IsTemperatureControlOff(Type))
    {
        //Set the nominal temperature
        retCode = SetTemperature(Type, NominalTemperature, SlopeTempChange);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        //ON the temperature control
        retCode = SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control with PID parameters.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::StartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    ReturnCode_t retCode;
    m_TargetTemperatures[Type] = NominalTemperature;
    m_TargetTempCtrlStatus[Type] = TEMPCTRL_STATUS_ON;
    if (GetTemperatureControlState(Type) == TEMPCTRL_STATE_ERROR)
    {
        return DCL_ERR_DEV_TEMP_CTRL_STATE_ERR;
    }
    if (IsTemperatureControlOn(Type))
    {
        if(DCL_ERR_FCT_CALL_SUCCESS != SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_OFF))
        {
            return DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR;
        }
    }

    retCode = SetTemperaturePid(Type, MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
    if(retCode != DCL_ERR_FCT_CALL_SUCCESS)
    {
         return retCode;
    }
    //Set the nominal temperature
    retCode = SetTemperature(Type, NominalTemperature, SlopeTempChange);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //ON the temperature control
//    retCode = SetTemperatureControlStatus(Type, TEMPCTRL_STATUS_ON);
//    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
//    {
//        return retCode;
//    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   Get temperature control module's status.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  Temperature control module status.
 */
/****************************************************************************/
TempCtrlState_t CRetortDevice::GetTemperatureControlState(RTTempCtrlType_t Type)
{
    ReturnCode_t retCode = m_pTempCtrls[Type]->ReqStatus();
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode) {
        m_CurrentTempCtrlStatus[Type] = TEMPCTRL_STATUS_UNDEF;
        return TEMPCTRL_STATE_ERROR;
    }
    if(m_pDevProc)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_TEMP_CTRL_STATE);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    TempCtrlState_t controlstate = TEMPCTRL_STATE_ERROR;
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        controlstate = TEMPCTRL_STATE_ERROR;
    }
    else if (IsTemperatureControlOn(Type))
    {
        if (IsInsideRange(Type, 0))
        {
            controlstate = TEMPCTRL_STATE_INSIDE_RANGE;
        }
        else if (IsOutsideRange(Type, 0))
        {
            controlstate = TEMPCTRL_STATE_OUTSIDE_RANGE;
        }
    }
    else if (IsTemperatureControlOff(Type))
    {
        controlstate = TEMPCTRL_STATE_OFF;
    }
    return controlstate;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get temperature control status.
 *
 *  This slot is connected to the signal, ReportActStatus
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam TempCtrlStatus = Actual temperature control status
 *  \iparam MainsVoltage = Main voltage status.
 *
 */
/****************************************************************************/
void CRetortDevice::OnTempControlStatus(quint32 InstanceID, ReturnCode_t ReturnCode,
                                           TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        m_CurrentTempCtrlStatus[m_InstTCTypeMap[InstanceID]] = TempCtrlStatus;
        m_MainsVoltageStatus[m_InstTCTypeMap[InstanceID]] = MainsVoltage;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_TEMP_CTRL_STATE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature is inside the range.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \param Index =  quint8 type parameter
 *
 *  \return  True if is inside the range, else not.
 */
/****************************************************************************/
bool CRetortDevice::IsInsideRange(RTTempCtrlType_t Type, quint8 Index)
{
    if(!qFuzzyCompare(GetTemperature(Type, 0),UNDEFINED_4_BYTE))
    {
        if(!qFuzzyCompare(m_TargetTemperatures[Type],UNDEFINED_4_BYTE)
                || !qFuzzyCompare(m_CurrentTemperatures[Type][Index],UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperatures[Type][Index] > m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type][Index] < m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature is outside the range.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \param Index =  quint8 type parameter
 *
 *  \return  True if is outside the range, else not.
 */
/****************************************************************************/
bool CRetortDevice::IsOutsideRange(RTTempCtrlType_t Type, quint8 Index)
{
    if(!qFuzzyCompare(GetTemperature(Type, 0),UNDEFINED_4_BYTE))
    {
        if(!qFuzzyCompare(m_TargetTemperatures[Type],UNDEFINED_4_BYTE)
                || !qFuzzyCompare(m_CurrentTemperatures[Type][Index],UNDEFINED_4_BYTE))
        {
            if ((m_CurrentTemperatures[Type][Index] < m_TargetTemperatures[Type] - TOLERANCE)||
                            (m_CurrentTemperatures[Type][Index] > m_TargetTemperatures[Type] + TOLERANCE))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is enabled.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  True if temperature control is enabled, else not.
 */
/****************************************************************************/
bool CRetortDevice::IsTemperatureControlOn(RTTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_ON);
}

/****************************************************************************/
/*!
 *  \brief  Judge if the temperature control is disabled.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return  True if temperature control is disabled, else not.
 */
/****************************************************************************/
bool CRetortDevice::IsTemperatureControlOff(RTTempCtrlType_t Type)
{
    return (m_CurrentTempCtrlStatus[Type] == TEMPCTRL_STATUS_OFF);
}

/****************************************************************************/
/*!
 *  \brief   Start temperature control.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  NominalTemperature = Target temperature.
 *  \iparam  SlopeTempChange = Temperature drop value before level sensor
 *                             reporting state change. Only valid for
 *                             level sensor.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetTemperature(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    m_TargetTemperatures[Type] = NominalTemperature;
    ReturnCode_t retCode;
    DCLEventLoop* event = NULL;
    if(m_pTempCtrls[Type] != NULL)
    {
        event = m_pDevProc->CreateSyncCall(SYNC_CMD_RT_SET_TEMP);
        retCode = m_pTempCtrls[Type]->SetTemperature(NominalTemperature, SlopeTempChange);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    if(m_pDevProc)
    {
        retCode =  m_pDevProc->BlockingForSyncCall(event);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    return retCode;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set temperature.
 *
 *  This slot is connected to the signal, ReportRefTemperature
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Temperature = Target temperature.
 *
 */
/****************************************************************************/
void CRetortDevice::OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature)
{
    Q_UNUSED(Temperature)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set temperature successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort set temperature failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get temperature synchronously.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Index of the target temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
qreal CRetortDevice::GetTemperature(RTTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    qreal RetValue = m_CurrentTemperatures[Type][Index];
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 800 msec has passed since last read
    {
        ReturnCode_t retCode = m_pTempCtrls[Type]->ReqActTemperature(Index);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode )
        {
            RetValue = UNDEFINED_4_BYTE;
        }
        else
        {
            if(m_pDevProc)
            {
                retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_TEMP);
            }
            else
            {
                retCode = DCL_ERR_NOT_INITIALIZED;
            }
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                RetValue = UNDEFINED_4_BYTE;
            }
            else
            {
                RetValue = m_CurrentTemperatures[Type][Index];
            }
            m_LastGetTempTime[Type][Index] = Now;
        }
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief    Get actual temperature of Air-liquid device asynchronously.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  Index = Index of the target temperature control module.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::GetTemperatureAsync(RTTempCtrlType_t Type, quint8 Index)
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetTempTime[Type][Index]) >= CHECK_SENSOR_TIME) // check if 800 msec has passed since last read
    {
        m_LastGetTempTime[Type][Index] = Now;
        return m_pTempCtrls[Type]->ReqActTemperature(Index);
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get temperature.
 *
 *  This slot is connected to the signal, ReportActTemperature
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam Index = Index of the actual temperature control module.
 *  \iparam Temp = Actual temperature.
 *
 */
/****************************************************************************/
void CRetortDevice::OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp)
{
    Q_UNUSED(Index)

    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get temperature successful! ";
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = Temp;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort get temperature failed! " << ReturnCode; //lint !e641
        m_CurrentTemperatures[m_InstTCTypeMap[InstanceID]][Index] = UNDEFINED_4_BYTE;
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_TEMP, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Set PID parameters for temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Set Retort temperature PID, type: " << Type; //lint !e641
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_RT_SET_TEMP_PID);
        retCode = m_pTempCtrls[Type]->SetTemperaturePid(MaxTemperature, ControllerGain, ResetTime, DerivativeTime);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        if(m_pDevProc)
        {
            return m_pDevProc->BlockingForSyncCall(event);
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }

    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

/****************************************************************************/
/*!
 *  \brief   slot associated with set PID parameters.
 *
 *  This slot is connected to the signal, ReportSetPidAckn
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam  MaxTemperature = Maximum temperature.
 *  \iparam  ControllerGain = Controller Gain.
 *  \iparam  ResetTime = Reset time.
 *  \iparam  DerivativeTime = Derivative time.
 *
 */
/****************************************************************************/
void CRetortDevice::OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    Q_UNUSED(MaxTemperature)
    Q_UNUSED(ControllerGain)
    Q_UNUSED(ResetTime)
    Q_UNUSED(DerivativeTime)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set temperature PID successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort set temperature PID failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_TEMP_PID, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Set retort lock digital output value.
 *
 *
 *  \iparam OutputValue = Actual output value of fan's digital output module.
 *  \iparam Duration = Duration of the output(not used now).
 *  \iparam Delay = UNUSED.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::SetDOValue(quint16 OutputValue, quint16 Duration, quint16 Delay)
{
    m_TargetDOOutputValue = OutputValue;
    ReturnCode_t retCode;
    DCLEventLoop* event = NULL;
    if(m_pLockDigitalOutput)
    {
        event = m_pDevProc->CreateSyncCall(SYNC_CMD_RT_SET_DO_VALUE);
        retCode = m_pLockDigitalOutput->SetOutputValue(m_TargetDOOutputValue, Duration, Delay);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    if(m_pDevProc)
    {
        return m_pDevProc->BlockingForSyncCall(event);
    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
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
void CRetortDevice::OnSetDOOutputValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 OutputValue)
{
    Q_UNUSED(OutputValue)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Set DO output successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: AL set DO output failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_DO_VALUE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Lock the retort.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::Lock()
{
    return SetDOValue(1, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief  Unlock the retort.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::Unlock()
{
    return SetDOValue(0, 0, 0);
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get digital input data.
 *
 *  This slot is connected to the signal, OnGetDIValue
 *
 *  \iparam ReturnCode = ReturnCode of function level Layer
 *  \iparam InputValue = Actual digital input module's value.
 *
 */
/****************************************************************************/
void CRetortDevice::OnGetDIValue(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, quint16 InputValue)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get DI value successful! ";
        m_LockStatus = InputValue;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort Get DI value failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_DI_VALUE, ReturnCode);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get the Retort lid sensor data captured in last 500 milliseconds.
 *
 *  \return  Actual retort lock status, UNDEFINED if failed.
 */
/****************************************************************************/
quint16 CRetortDevice::GetRecentRetortLockStatus()
{
   // QMutexLocker Locker(&m_Mutex);
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    quint16 RetValue;
    if((Now - m_LastGetLockStatusTime) <= 1000) // check if 1000 msec has passed since last read
    {
        RetValue = m_LockStatus;
    }
    else
    {
        RetValue = UNDEFINED_2_BYTE;
    }
    return RetValue;
}

/****************************************************************************/
/*!
 *  \brief  Get Retort lock status asynchronously.
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::GetLockStatusAsync()
{
    qint64 Now = QDateTime::currentMSecsSinceEpoch();
    if((Now - m_LastGetLockStatusTime) >= CHECK_LOCKER_TIME) // check if 700 msec has passed since last read
    {
        m_LastGetLockStatusTime = Now;
        if(m_pLockDigitalInput)
        {
            return m_pLockDigitalInput->ReqActInputValue();
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
 *  \brief  Get Retort Lid status.
 *
 *  \return  1: Open, 0: Closed
 */
/****************************************************************************/
quint16 CRetortDevice::GetLidStatus()
{
    ReturnCode_t retCode;
    if(m_pLockDigitalInput)
    {
        retCode = m_pLockDigitalInput->ReqActInputValue();
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    if(m_pDevProc)
    {
        retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_RT_GET_DI_VALUE);
    }
    else
    {
        retCode = DCL_ERR_NOT_INITIALIZED;
    }
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return UNDEFINED_2_BYTE;
    }
    return m_LockStatus;
}

/****************************************************************************/
/*!
 *  \brief   Get hardware status of the temperature control module.
 *
 *  \iparam  Type = The target temperature contorl module to control.
 *
 *  \return    control module's hardware status.
 */
/****************************************************************************/
ReturnCode_t CRetortDevice::GetHardwareStatusAsync(RTTempCtrlType_t Type)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if((now - m_LastGetHardwareStatusTime[Type]) >= CHECK_CURRENT_TIME && m_pTempCtrls[Type] != NULL)
    {
        m_LastGetHardwareStatusTime[Type] = now;
        return m_pTempCtrls[Type]->GetHardwareStatus();
    }
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief   slot associated with get hardware status.
 *
 *  This slot is connected to the signal, ReportHardwareStatus
 *
 *  \iparam  InstanceID = Instance ID of the temperature control module.
 *  \iparam  ReturnCode = ReturnCode of function level Layer
 *  \iparam  Sensors = Number of sensors.
 *  \iparam  Fans = Number of fans.
 *  \iparam  Heaters = Number of heaters.
 *  \iparam  Pids = Number of PIDs.
 *  \iparam  Current = Actual current.
 *  \iparam  HeaterSwitchType = Heater Switch Type.
 *
 */
/****************************************************************************/
void CRetortDevice::OnGetHardwareStatus(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Sensors, quint8 Fans,
                                               quint8 Heaters, quint8 Pids, quint16 Current, quint8 HeaterSwitchType)
{
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: Retort Get DI value successful! ";
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Sensors = Sensors;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Fans = Fans;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Heaters = Heaters;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Pids = Pids;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].Current = Current;
        m_HardwareStatus[m_InstTCTypeMap[InstanceID]].HeaterSwitchType = HeaterSwitchType;
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: Retort Get DI value failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_GET_HW_STATUS, ReturnCode);
    }
}

ReturnCode_t CRetortDevice::SetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType)
{
    FILE_LOG_L(laDEVPROC, llINFO) << "INFO: " << Type; //lint !e641
    ReturnCode_t retCode;
    if(m_pTempCtrls[Type] != NULL)
    {
        DCLEventLoop* event = m_pDevProc->CreateSyncCall(SYNC_CMD_RT_SET_SWITCH_STATE);
        retCode = m_pTempCtrls[Type]->SetSwitchState(HeaterVoltage, AutoType);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        if(m_pDevProc)
        {
            return m_pDevProc->BlockingForSyncCall(event);
        }
        else
        {
            return DCL_ERR_NOT_INITIALIZED;
        }

    }
    else
    {
        return DCL_ERR_NOT_INITIALIZED;
    }
}

void CRetortDevice::OnSetSwitchState(quint32 InstanceID, ReturnCode_t ReturnCode, qint8 SwitchState, qint8 AutoSwitch)
{
    Q_UNUSED(InstanceID)
    Q_UNUSED(SwitchState)
    Q_UNUSED(SwitchState)
    Q_UNUSED(AutoSwitch)
    if(DCL_ERR_FCT_CALL_SUCCESS == ReturnCode)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "INFO: RV Set switch state successful! ";
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llWARNING) << "WARNING: RV set temperature switch state failed! " << ReturnCode; //lint !e641
    }
    if(m_pDevProc)
    {
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_RT_SET_SWITCH_STATE, ReturnCode);
    }
}

} //namespace
