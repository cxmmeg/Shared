
#ifndef OVENDEVICE_H
#define OVENDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"


namespace DeviceControl
{
class CTemperatureControl;

class COvenDevice : public CBaseDevice
{
    Q_OBJECT
public:

    //! constructor
    COvenDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~COvenDevice();
    //! general task handling function
    void HandleTasks();

    ReturnCode_t SetTempCtrlON(OVENTempCtrlType_t Type);
    ReturnCode_t SetTempCtrlOFF(OVENTempCtrlType_t type);
    ReturnCode_t SetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    ReturnCode_t StartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    ReturnCode_t StartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    qreal GetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index);
    TempCtrlState_t GetTemperatureControlState(OVENTempCtrlType_t Type);

private slots:
    void Reset();
    ReturnCode_t HandleInitializationState();
    ReturnCode_t HandleConfigurationState();
    ReturnCode_t ConfigureDeviceTasks();
    //! idle state handling task
    void HandleIdleState();
    //! error handling task
    void HandleErrorState();
    // void HandleErrorState();

    bool SetTemperatureControlStatus(OVENTempCtrlType_t Type, TempCtrlStatus_t TempCtrlStatus);
    bool SetTemperature(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    qreal GetTemperature(OVENTempCtrlType_t Type, quint8 Index);
    bool GetTemperatureAsync(OVENTempCtrlType_t Type, quint8 Index);



    bool IsInsideRange(OVENTempCtrlType_t Type);
    bool IsOutsideRange(OVENTempCtrlType_t Type);
    bool IsTemperatureControlOn(OVENTempCtrlType_t Type);
    bool IsTemperatureControlOff(OVENTempCtrlType_t Type);


    void CheckSensorsData();


    void OnSetTemp(quint32 /*InstanceID*/, ReturnCode_t ReturnCode, qreal Temperature);
    void OnGetTemp(quint32 InstanceID, ReturnCode_t ReturnCode, quint8 Index, qreal Temp);
    void OnSetTempPid(quint32, ReturnCode_t ReturnCode, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    void OnTempControlStatus(quint32 /*InstanceID*/, ReturnCode_t ReturnCode,TempCtrlStatus_t TempCtrlStatus, TempCtrlMainsVoltage_t MainsVoltage);

    //! command handling task
    //  void HandleCommandRequestTask();
    //  void HandleDeviceTaskActions();

    //! Return the oven cover position type from motor position
    //  RVPosition_t GetRVPositionFromEncoderDiskPos(qint32 EDPosition);
    //! Return the motor position from oven cover position type
    // qint32 GetEDPosFromRVPos(RVPosition_t RVPosition);

private:
    //Function modules
    CTemperatureControl* m_pTempCtrls[OVEN_TEMP_CTRL_NUM];


    qreal m_CurrentTemperatures[OVEN_TEMP_CTRL_NUM];                     //!< Current temperature
    qreal m_TargetTemperatures[OVEN_TEMP_CTRL_NUM];                     //!< Current temperature
    TempCtrlStatus_t m_TargetTempCtrlStatus[OVEN_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
    TempCtrlStatus_t m_CurrentTempCtrlStatus[OVEN_TEMP_CTRL_NUM];       //!< Current temperature control status
    TempCtrlMainsVoltage_t m_MainsVoltageStatus[OVEN_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
    qint64 m_LastGetTempTime[OVEN_TEMP_CTRL_NUM][5];
    QMap<quint32, OVENTempCtrlType_t> m_InstTCTypeMap;


    /*! error task state definitiosn */
    typedef enum {
        OVEN_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        OVEN_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        OVEN_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        OVEN_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } OvenDevErrTaskState_t;

    OvenDevErrTaskState_t m_ErrorTaskState;  //!< error task state
    QMutex m_Mutex; //!< Protects the task handling thread from request functions

};
}


#endif /* OVENDEVICE_H */