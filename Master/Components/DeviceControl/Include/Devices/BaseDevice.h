/****************************************************************************/
/*! \file BaseDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    11.01.2011
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CBaseDevice
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

#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include <Global/Include/Commands/Command.h>
#include "DeviceControl/Include/SlaveModules/ModuleConfig.h"
#include <QStateMachine>

namespace DataManager
{
    class CModule;
}

namespace DeviceControl
{
class CServiceState;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of typedef ListBaseModule
 */
/****************************************************************************/
typedef QList<CBaseModule *> ListBaseModule;

class ModuleLifeCycleRecord;

/****************************************************************************/
/*!
 *  \brief This is the base class of the device classes
 *
 *      The class provides device relevant base functions
 */
/****************************************************************************/
class CBaseDevice : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CBaseDevice
     *
     *  \param pDeviceProcessing = DeviceProcessing type parameter
     *  \param Type =  QString type parameter
     *
     *  \return from CBaseDevice
     */
    /****************************************************************************/
    CBaseDevice(DeviceProcessing* pDeviceProcessing,
                QString& Type,
                const DeviceModuleList_t &ModuleList,
                quint32 InstanceID);
    virtual ~CBaseDevice();

    /*****************************************************************************/
    /*!
     *  \brief  Task handling function
     */
    /*****************************************************************************/
    virtual void HandleTasks() = 0;

    /*! main state */
    typedef enum {
        DEVICE_MAIN_STATE_START        = 0x00,  ///< first state after creation
        DEVICE_MAIN_STATE_INIT         = 0x01,  ///< init state, read the assigned function modules
        DEVICE_MAIN_STATE_CONFIG       = 0x02,  ///< configuration, connect the signals
        DEVICE_MAIN_STATE_DEV_TASK_CFG = 0x03,  ///< device tasks configuration state
        DEVICE_MAIN_STATE_FCT_MOD_CFG  = 0x04,  ///< fuction module configuration state, e.g. force reference run
        DEVICE_MAIN_STATE_IDLE         = 0x05,  ///< idle state, ready for use
        DEVICE_MAIN_STATE_ERROR        = 0x06   ///< error state, usage blocked
    } DeviceMainState_t;

    /*****************************************************************************/
    /*!
     *  \brief  Returns the main state of the device
     *
     *  \return Main state
     */
    /*****************************************************************************/
    DeviceMainState_t GetMainState() const { return m_MainState; }

    /*****************************************************************************/
    /*!
     *  \brief  Returns the instance identifier
     *
     *  \return Instance identifier
     */
    /*****************************************************************************/
    quint32 GetInstanceID() const { return m_instanceID; }

    /*****************************************************************************/
    /*!
     *  \brief  Returns the device type string
     *
     *  \return Type string
     */
    /*****************************************************************************/
    QString GetType() const { return m_Type; }

    /*****************************************************************************/
    /*!
     *  \brief  Set the device type string
     *
     *  \iparam Key = Device type string
     */
    /*****************************************************************************/
    void SetTypeKey(const QString &Key) { m_TypeKey = Key; }

    /*****************************************************************************/
    /*!
     *  \brief  Set the function module list member
     *
     *  \iparam FctModList = Function module list
     */
    /*****************************************************************************/
    void SetFunctionModuleList(const DeviceModuleList_t& FctModList) { m_ModuleList = FctModList; }

    /*****************************************************************************/
    /*!
     *  \brief  Get the device processing member
     *
     *  \return Function module list
     */
    /*****************************************************************************/
    DeviceProcessing* GetDeviceProcessing() const { return m_pDevProc; }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetFctModInstanceFromKey
     *
     *  \param Key =  QString type parameter
     *
     *  \return from GetFctModInstanceFromKey
     */
    /****************************************************************************/
    quint32 GetFctModInstanceFromKey(const QString &Key);

    //! Get Function module key from instance ID
    QString GetFctModKeyFromInstance(const quint32 instanceID);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetBaseModuleList
     *
     *  \return from GetBaseModuleList
     */
    /****************************************************************************/
    ListBaseModule GetBaseModuleList() const {return m_BaseModuleList;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function InsertBaseModule
     *
     *  \param pBase = CBaseModule type parameter
     *
     *  \return from InsertBaseModule
     */
    /****************************************************************************/
    bool InsertBaseModule(CBaseModule* pBase);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCANNodeFromID
     *
     *  \param CANNodeID = quint16 type parameter
     *
     *  \return from GetCANNodeFromID
     */
    /****************************************************************************/
    CBaseModule* GetCANNodeFromID(quint16 CANNodeID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetBaseModuleVoltage
     *
     *  \param CANNodeID = quint16 type parameter
     *
     *  \return from GetBaseModuleVoltage
     */
    /****************************************************************************/
    quint16 GetBaseModuleVoltage(quint16 CANNodeID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetBaseModuleCurrent
     *
     *  \param CANNodeID = quint16 type parameter
     *
     *  \return from GetBaseModuleCurrent
     */
    /****************************************************************************/
    quint16 GetBaseModuleCurrent(quint16 CANNodeID);

    /****************************************************************************/
    /*!
     *  \brief  Get the report error based on instance Id
     *
     *  \param  errorCode  error code of temperature module
     *  \param  instanceID  instance id of slave module
     *
     *  \return ReportError_t
     */
    /****************************************************************************/
    ReportError_t GetSlaveModuleError(quint8 errorCode, quint32 instanceID);

    /****************************************************************************/
    /*!
     *  \brief  function GetRecentBaseModuleVoltageState
     *  \return PowerState_t
     */
    /****************************************************************************/
    PowerState_t GetRecentBaseModuleVoltageState() { return m_BaseModuleVoltageState; }

    /****************************************************************************/
    /*!
     *  \brief  function GetrecentBaseModuleVoltage
     *  \return voltage
     */
    /****************************************************************************/
    quint16 GetrecentBaseModuleVoltage() { return m_BaseModuleVoltage; }

    /****************************************************************************/
    /*!
     *  \brief  function GetRecentBaseModuleCurrentState
     *  \return PowerState_t
     */
    /****************************************************************************/
    PowerState_t GetRecentBaseModuleCurrentState() { return m_BaseModuleCurrentState; }

    /****************************************************************************/
    /*!
     *  \brief  function GetrecentBaseModuleCurrent
     *  \return return from GetrecentBaseModuleCurrent
     */
    /****************************************************************************/
    quint16 GetrecentBaseModuleCurrent() { return m_BaseModuleCurrent; }
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewCANStepperMotorTask
     *
     *  \param MotorTaskID = CANStepperMotorTask::CANStepperMotorTaskID_t type parameter
     *
     *  \return from GetNewCANStepperMotorTask
     */
    /****************************************************************************/
    static CANStepperMotorTask*    GetNewCANStepperMotorTask(CANStepperMotorTask::CANStepperMotorTaskID_t MotorTaskID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewCANRFIDTask
     *
     *  \param RFIDTaskID = CANRFIDTask::CANRFIDTaskID_t type parameter
     *
     *  \return from GetNewCANRFIDTask
     */
    /****************************************************************************/
    static CANRFIDTask*            GetNewCANRFIDTask(CANRFIDTask::CANRFIDTaskID_t RFIDTaskID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewCANDigitalOutputTask
     *
     *  \param DigOutpTaskID = CANDigitalOutputTask::CANDigitalOutputTaskID_t type parameter
     *
     *  \return from GetNewCANDigitalOutputTask
     */
    /****************************************************************************/
    static CANDigitalOutputTask*   GetNewCANDigitalOutputTask(CANDigitalOutputTask::CANDigitalOutputTaskID_t DigOutpTaskID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewCANDigitalInputTask
     *
     *  \param DigInpTaskID = CANDigitalInputTask::CANDigitalInputTaskID_t type parameter
     *
     *  \return from GetNewCANDigitalInputTask
     */
    /****************************************************************************/
    static CANDigitalInputTask*    GetNewCANDigitalInputTask(CANDigitalInputTask::CANDigitalInputTaskID_t DigInpTaskID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewCANTempCtrlTask
     *
     *  \param TempCtrlTaskID = CANTemperatureCtrlTask::CANTempCtrlTaskID_t type parameter
     *
     *  \return from GetNewCANTempCtrlTask
     */
    /****************************************************************************/
    static CANTemperatureCtrlTask* GetNewCANTempCtrlTask(CANTemperatureCtrlTask::CANTempCtrlTaskID_t TempCtrlTaskID);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetNewDeviceTask
     *
     *  \param TaskState = DeviceTask::DeviceTaskState_t type parameter
     *  \param Key =  quint16 type parameter
     *
     *  \return from GetNewDeviceTask
     */
    /****************************************************************************/
    static DeviceTask* GetNewDeviceTask(DeviceTask::DeviceTaskState_t TaskState, quint16 Key);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function LogDebug
     *
     *  \param message = QString type parameter
     *
     *  \return from LogDebug
     */
    /****************************************************************************/
    inline void LogDebug(QString message)
    {
        qDebug() << message;
#if 1
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE,Global::tTranslatableStringList()<<message);
#endif
    }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetModuleLifeCycleRecord
     *
     *  \param pModuleLifeCycleRecord = Life Cycle Record
     *
     */
    /****************************************************************************/
    void SetModuleLifeCycleRecord(ModuleLifeCycleRecord* pModuleLifeCycleRecord);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetSensorDataCheckFlag
     *
     *  \param flag = sensor data check flag
     *
     */
    /****************************************************************************/
    static void SetSensorDataCheckFlag(bool flag)
    {
        m_SensorsDataCheckFlag = flag;
    }

signals:
    /****************************************************************************/
    /*!
     *  \brief  Requests the service information from the device
     */
    /****************************************************************************/
    void GetServiceInfo();
    // command request interface to DCP
    /****************************************************************************/
    /*! \brief  To trigger device configuration.
     *
     *          Triggers transition from start state to configuring state
     */
    /****************************************************************************/
    /****************************************************************************/
    /*!
     *  \brief  Resets the service information of the device
     */
    /****************************************************************************/
    void ResetServiceInfIo();


    /****************************************************************************/
    /*!
     *  \brief  Returns the service information of a device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam ModuleInfo = Contains the service information
     *  \iparam deviceName = Contains the device name
     */
    /****************************************************************************/
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString& deviceName);

    /****************************************************************************/
    /*!
     *  \brief  Acknowledges the reset of the service information of the device
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     */
    /****************************************************************************/
    void ReportResetServiceInfo(ReturnCode_t ReturnCode);

    /****************************************************************************/
    /*!
     *  \brief  function ReportSavedServiceInfor
     *
     */
    /****************************************************************************/
    void ReportSavedServiceInfor();

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnFunctionModuleError
     *  \param  InstanceID instance id
     *  \param  ErrorGroup ErrorGroup
     *  \param  ErrorCode ErrorCode
     *  \param  ErrorData  ErrorData
     *  \param  ErrorTime  ErrorTime
     */
    /****************************************************************************/
    void OnFunctionModuleError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnReportVoltageState
     *  \param  InstanceID instance id
     *  \param  HdlInfo  HdlInfo
     *  \param  State  State
     *  \param  Value   Value
     *  \param  Failures  Failures
     */
    /****************************************************************************/
    void OnReportVoltageState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnReportCurrentState
     *  \param  InstanceID instance id
     *  \param  HdlInfo  HdlInfo
     *  \param  State  State
     *  \param  Value   Value
     *  \param  Failures  Failures
     */
    /****************************************************************************/
    void OnReportCurrentState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when the node state has changed
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam NodeState  = Node state
     *  \iparam EmergencyStopReason = No stop, heartbeat, master
     *  \iparam VoltageState = Good, warning, failed, unknown
     */
    /****************************************************************************/
    void OnReportNodeState(quint32 InstanceID, ReturnCode_t HdlInfo, NodeState_t NodeState,
                         EmergencyStopReason_t EmergencyStopReason, PowerState_t VoltageState);

    /****************************************************************************/
    /*!
     *  \brief  Add device name in this slot function
     *
     *  \iparam ReturnCode = ReturnCode of Device Control Layer
     *  \iparam ModuleInfo = Contains the service information
     */
    /****************************************************************************/
    void OnReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo);

    /****************************************************************************/
    /*!
     *  \brief  function OnGetServiceInfor
     *
     */
    /****************************************************************************/
    void OnGetServiceInfor();

    /****************************************************************************/
    /*!
     *  \brief  function OnReportSavedServiceInfor
     *  \iparam deviceType device type
     */
    /****************************************************************************/
    void OnReportSavedServiceInfor(const QString& deviceType);

protected:
    /// Compact function to set the error parameter and error time by one code line
    void SetErrorParameter(quint16 errorGroup, quint16 errorCode, quint16 errorData);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetProcSettingMotionProfile
     *
     *  \param Key = QString type parameter
     *
     *  \return from GetProcSettingMotionProfile
     */
    /****************************************************************************/
    MotionProfileIdx_t GetProcSettingMotionProfile(QString Key);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetProcSettingPosition
     *
     *  \param Key = QString type parameter
     *
     *  \return from GetProcSettingPosition
     */
    /****************************************************************************/
    Position_t         GetProcSettingPosition(QString Key);

    DeviceProcessing*   m_pDevProc;         ///< pointer to the device processing class

    DeviceModuleList_t    m_ModuleList;       ///< contains function modules idetifiers

    DeviceMainState_t   m_MainState;        ///< main state
    DeviceMainState_t   m_MainStateOld;     ///< previous main state, used for logging

    QString             m_Type;             ///< Device type string, used for logging

    ReturnCode_t m_lastErrorHdlInfo;        ///< return value after a failed function call
    quint16      m_lastErrorGroup;          ///< error group of the error causing functionality
    quint16      m_lastErrorCode;           ///< error code of the error causing functionality
    quint16      m_lastErrorData;           ///< additional error data, filled by the error causing functionality
    QDateTime    m_lastErrorTime;           ///< time of error detection

    quint32 m_instanceID;           ///< identifier for instance identification

    Global::MonotonicTime m_stateTimer;     ///< timer for timeout observation
    qint16 m_stateTimespan;                 ///< max. time delay of current active timeout observation

    QString      m_TypeKey;                 ///< key, used for process settings access
    ListBaseModule m_BaseModuleList;        ///< The list contain all the base modules.
    PowerState_t m_BaseModuleVoltageState;  ///< The base module's actual voltage state
    quint16 m_BaseModuleVoltage;            ///< The base module's actual voltage
    PowerState_t m_BaseModuleCurrentState;  ///< The base module's actual current state
    quint16 m_BaseModuleCurrent;            ///< The base module's actual current
    qint64 m_LastSensorCheckTime;           ///< The last check sensor's time(in sec since Epoch)

    CServiceState *mp_Service;      //!< Service functionality of the base device
    QMap<QString, CModule *> m_ModuleMap;   //!< Maps keys to Slave module pointers

    QVector<ReportError_t>  m_ReportErrorList; //!< report error list
    QStateMachine m_machine;        //!< State machine
    ModuleLifeCycleRecord* m_ModuleLifeCycleRecord; //!< module life cycle record
    static bool m_SensorsDataCheckFlag; //!< sensor data check flag
};

} //namespace

#endif // BASEDEVICE_H
