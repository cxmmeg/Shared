/****************************************************************************/
/*! \file IDeviceProcessing.h
 *
 *  \brief Definition file for class IDeviceProcessing
 *
 *   Version: $ 0.1
 *   Date:    $ 08.08.2012
 *   Author:  $ Norbert Wiedmann / Stalin
 *
 *
 *  \b Description:
 *
 *       This module contains the interface to the IDeviceProcessing class
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

#ifndef IDEVICEPROCESSING_H
#define IDEVICEPROCESSING_H

#include <QHash>
#include <QObject>

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Devices/RotaryValveDevice.h"
#include "DeviceControl/Include/Devices/AirLiquidDevice.h"
#include "DeviceControl/Include/Devices/RetortDevice.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"
#include "DeviceControl/Include/Devices/PeripheryDevice.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Interface/IDeviceControl.h"
#include "DeviceControl/Include/Devices/DeviceManager.h"
#include <exception>

namespace DeviceControl
{
#define UNDEFINED_VALUE (999)  //!< undefine value. used for invlid sensor's data
/****************************************************************************/
/*!
 *  \brief  This is the interface class of the device control layer.
 *
 *      The class provides methods to operate with the device control layer's
 *      functionality.
 */
/****************************************************************************/
class IDeviceProcessing : public QObject, public IDeviceControl
{
    friend class TestIDeviceProcessing;
    Q_OBJECT

public:
    IDeviceProcessing(int DevProcTimerInterval = 10);
    ~IDeviceProcessing();

    //! Returns the serial number from config file
//    static bool GetSerialNumber(QString& SerialNo){return DeviceProcessing::GetSerialNumber(SerialNo);}
    hwconfigType * GetDeviceConfig() const override;

    inline IDeviceControl* WithSender(const QString& sender) override
    {
        if(m_pDeviceManager == nullptr)
        {
            throw std::runtime_error("DeviceManager not initialized yet");
            return nullptr;
        }

        m_pDeviceManager->SetSender(sender);

        return this;
    }
    //! Emergency stop
    virtual void EmergencyStop();   // should be called if the device's cover was opened by the user
    //! Switch to standby mode
    void Standby();         // should be called to change to standby mode
    //! Clean up the environment
    void Destroy();         // should be called to finish all the activities

    //! Start device control layer configuration
    ReturnCode_t StartConfigurationService();
    //! Restart device control layer configuration
    ReturnCode_t RestartConfigurationService();
    //! Start device control layer diagnstic service
    ReturnCode_t StartDiagnosticService();
    //! Finisch device control layer diagnostic service
    ReturnCode_t CloseDiagnosticService();

    //! Start adjustment service
    ReturnCode_t StartAdjustmentService();

    //! Returns Device derived class pointer specified by instanceID
    CBaseDevice* GetDevice(quint32 InstanceID);
    //! Return the pointer to the CBaseModule which is next in list
    CBaseModule* GetNode(bool First);
    //Air liquid device funcs
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlON
     *
     *  \return from ALSetPressureCtrlON
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureCtrlON();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureCtrlOFF
     *
     *  \return from ALSetPressureCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureCtrlOFF();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALReleasePressure
     *
     *  \return from ALReleasePressure
     */
    /****************************************************************************/
    ReturnCode_t ALReleasePressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALPressure
     *
     *  \return from ALPressure
     */
    /****************************************************************************/
    ReturnCode_t ALPressure(float targetPressure = AL_TARGET_PRESSURE_POSITIVE);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALVaccum
     *  \param  targetPressure - AL_TARGET_PRESSURE_NEGATIVE
     *  \return from ALVaccum
     */
    /****************************************************************************/
    ReturnCode_t ALVaccum(float targetPressure = AL_TARGET_PRESSURE_NEGATIVE);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALDraining
     *
     *  \param DelayTime = quint32 type parameter
     *
     *  \return from ALDraining
     */
    /****************************************************************************/
    ReturnCode_t ALDraining(quint32 DelayTime, float targetPressure = AL_TARGET_PRESSURE_POSITIVE, bool IgnorePressure = false);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDForceDraining
     *
     *  \param RVPos = rv pos
     *  \param targetPressure =  target pressure
     *  \param ReagentGrpID = reagent group ID
     *  \return from ALDraining
     */
    /****************************************************************************/
    ReturnCode_t IDForceDraining(quint32 RVPos, float targetPressure = AL_FORCEDRAIN_PRESSURE, const QString& ReagentGrpID = "RG2");

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALFilling
     *
     *  \param DelayTime = quint32 type parameter
     *  \param EnableInsufficientCheck = flag to indicate if Insufficient check is needed
     *  \param SafeReagent4Paraffin = Flag to indicate if Filling is in safe reagent and for paraffin
     *
     *  \return from ALFilling
     */
    /****************************************************************************/
    ReturnCode_t ALFilling(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStopCmdExec
     *
     *  \param CmdType = quint8 type parameter
     *
     *  \return from ALStopCmdExec
     */
    /****************************************************************************/
    ReturnCode_t ALStopCmdExec(quint8 CmdType);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALFillingForService
     *
     *  \param DelayTime = quint32 type parameter
     *  \param EnableInsufficientCheck = flag to indicate if Insufficient check is needed
     *
     *  \return from ALFilling
     */
    /****************************************************************************/
    ReturnCode_t ALFillingForService(quint32 DelayTime, bool EnableInsufficientCheck);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetRecentPressure
     *
     *  \return from ALGetRecentPressure
     */
    /****************************************************************************/
    qreal ALGetRecentPressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetTempCtrlON
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t ALSetTempCtrlON(ALTempCtrlType_t Type);
    ReturnCode_t ALSetTempCtrlOFF(ALTempCtrlType_t type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetTemperaturePid
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from ALSetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStartTemperatureControl
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from ALStartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetRecentTemperature
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from ALGetRecentTemperature
     */
    /****************************************************************************/
    qreal ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALGetTemperatureControlState
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return from ALGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t ALGetTemperatureControlState(ALTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOnFan
     *
     *  \return from ALTurnOnFan
     */
    /****************************************************************************/
    ReturnCode_t ALTurnOnFan();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALTurnOffFan
     *
     *  \return from ALTurnOffFan
     */
    /****************************************************************************/
    ReturnCode_t ALTurnOffFan();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALAllStop
     *
     *  \return from ALAllStop
     */
    /****************************************************************************/
    ReturnCode_t ALAllStop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALBreakAllOperation
     *
     *  \return from ALBreakAllOperation
     */
    /****************************************************************************/
    ReturnCode_t ALBreakAllOperation();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALSetPressureDrift
     *
     *  \param pressureDrift = qreal type parameter
     *
     *  \return from ALSetPressureDrift
     */
    /****************************************************************************/
    ReturnCode_t ALSetPressureDrift(qreal pressureDrift);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALStartTemperatureControlWithPID
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from ALStartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ALControlValve
     *
     *  \param ValveIndex = quint type parameter
     *  \param ValveState = quint type parameter
     *
     *  \return from ALControlValve
     */
    /****************************************************************************/
    ReturnCode_t ALControlValve(quint8 ValveIndex, quint8 ValveState);

    /****************************************************************************/
    /*!
     *  \brief  Get LA SensorHeating status
     *
     *  \param Type = ALTempCtrlType_t type parameter
     *
     *  \return true - on, false -off
     */
    /****************************************************************************/
    bool ALGetHeatingStatus(ALTempCtrlType_t Type);

    //Rotary Valve device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlON
     *
     *  \return from RVSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t RVSetTempCtrlON();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTempCtrlOFF
     *
     *  \return from RVSetTempCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t RVSetTempCtrlOFF();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTemperaturePid
     *
     *  \param MaxTemperature = quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RVSetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVStartTemperatureControl
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from RVStartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetRecentTemperature
     *
     *  \param Index = quint32 type parameter
     *
     *  \return from RVGetRecentTemperature
     */
    /****************************************************************************/
    qreal RVGetRecentTemperature(quint32 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVGetTemperatureControlState
     *
     *  \return from RVGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t RVGetTemperatureControlState();
    //! Execute the move to intial position of the RV
    ReturnCode_t RVReqMoveToInitialPosition(RVPosition_t RVPosition = RV_UNDEF);
    //! Position the oven cover
    ReturnCode_t RVReqMoveToRVPosition( RVPosition_t RVPosition);
    //! Request actual oven cover position
    RVPosition_t RVReqActRVPosition();

    /****************************************************************************/
    /*!
     *  \brief   Request the rotary valve to move to current tube position.
     *  \iparam  CurrentRVPosition = Current rotary valve encoder disk's position.
     *  \return  DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    ReturnCode_t RVReqMoveToCurrentTubePosition(RVPosition_t CurrentRVPosition);

    /****************************************************************************/
    /*!
     *  \brief  get the lower limit
     *  \return quint32
     */
    /****************************************************************************/
    quint32 GetCurrentLowerLimit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVStartTemperatureControlWithPID
     *
     *  \param NominalTemperature = qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RVStartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RVSetTemperatureSwitchState
     *  \param  HeaterVoltage heater voltage
     *  \param  AutoType  auto type
     *  \return from RVSetTemperatureSwitchState
     */
    /****************************************************************************/
    ReturnCode_t RVSetTemperatureSwitchState(qint8 HeaterVoltage, qint8 AutoType);

    //Oven device func
    /****************************************************************************/
    /*!
     *  \brief  Enable Oven temperature control
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    ReturnCode_t OvenSetTempCtrlON(OVENTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief Disable Oven temperature control
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return DCL_ERR_FCT_CALL_SUCCESS if successfull, otherwise an error code
     */
    /****************************************************************************/
    ReturnCode_t OvenSetTempCtrlOFF(OVENTempCtrlType_t type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenSetTemperaturePid
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from OvenSetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenStartTemperatureControl
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from OvenStartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetRecentTemperature
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from OvenGetRecentTemperature
     */
    /****************************************************************************/
    qreal OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Get Heating status
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return true - on, false -off
     */
    /****************************************************************************/
    bool OvenGetHeatingStatus(OVENTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetRecentLidStatus
     *
     *  \return from OvenGetRecentLidStatus
     */
    /****************************************************************************/
    quint16 OvenGetRecentLidStatus();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenGetTemperatureControlState
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *
     *  \return from OvenGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t OvenGetTemperatureControlState(OVENTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OvenStartTemperatureControlWithPID
     *
     *  \param Type = OVENTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from OvenStartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    //Retort device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTempCtrlON
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTSetTempCtrlON
     */
    /****************************************************************************/
    ReturnCode_t RTSetTempCtrlON(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTempCtrlOFF
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTSetTempCtrlOFF
     */
    /****************************************************************************/
    ReturnCode_t RTSetTempCtrlOFF(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTSetTemperaturePid
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RTSetTemperaturePid
     */
    /****************************************************************************/
    ReturnCode_t RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTStartTemperatureControl
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *
     *  \return from RTStartTemperatureControl
     */
    /****************************************************************************/
    ReturnCode_t RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTStartTemperatureControlWithPID
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param NominalTemperature =  qreal type parameter
     *  \param SlopeTempChange =  quint8 type parameter
     *  \param MaxTemperature =  quint16 type parameter
     *  \param ControllerGain =  quint16 type parameter
     *  \param ResetTime =  quint16 type parameter
     *  \param DerivativeTime =  quint16 type parameter
     *
     *  \return from RTStartTemperatureControlWithPID
     */
    /****************************************************************************/
    ReturnCode_t RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetRecentTemperature
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *  \param Index =  quint8 type parameter
     *
     *  \return from RTGetRecentTemperature
     */
    /****************************************************************************/
    qreal RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetTemperatureControlState
     *
     *  \param Type = RTTempCtrlType_t type parameter
     *
     *  \return from RTGetTemperatureControlState
     */
    /****************************************************************************/
    TempCtrlState_t RTGetTemperatureControlState(RTTempCtrlType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTUnlock
     *
     *  \return from RTUnlock
     */
    /****************************************************************************/
    ReturnCode_t RTUnlock();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTLock
     *
     *  \return from RTLock
     */
    /****************************************************************************/
    ReturnCode_t RTLock();
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of function RTSetTemperatureSwitchState
    *  \param  Type  type
    *  \param  HeaterVoltage  voltage
    *  \param  AutoType  auto type
    *  \return from RTSetTemperatureSwitchState
     */
    /****************************************************************************/
    ReturnCode_t RTSetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RTGetRecentLockStatus
     *
     *  \return from RTGetRecentLockStatus
     */
    /****************************************************************************/
    quint16 RTGetRecentLockStatus();
    //Periphery device func
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOffMainRelay
     *
     *  \return from PerTurnOffMainRelay
     */
    /****************************************************************************/
    ReturnCode_t PerTurnOffMainRelay();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerTurnOnMainRelay
     *
     *  \return from PerTurnOnMainRelay
     */
    /****************************************************************************/
    ReturnCode_t PerTurnOnMainRelay();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PerControlAlarm
     *  \param On = true: On, false: Off
     *  \param Remote =  true: Remote, false:Local
     *
     *  \return from PerTurnOnMainRelay
     */
    /****************************************************************************/
    ReturnCode_t PerControlAlarm(bool On, bool Remote);

    /****************************************************************************/
    /*!
     *  \brief  Get local or remote Alarm status in last 500 milliseconds.
     *
     *  \param  type - qint8 0 - local alarm, 1 - remote alarm
     *  \return 1 - connect, 0 - not connect, UNDEFINED_VALUE if failed
     */
    /****************************************************************************/
    quint16 PerGetRecentAlarmStatus(qint8 type);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDBottleCheck
     *
     *  \param ReagentGrpID = QString type parameter
     *  \param TubePos =  RVPosition_t type parameter
     *
     *  \return from IDBottleCheck
     */
    /****************************************************************************/
    ReturnCode_t IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDSealingCheck
     *
     *  \param  ThresholdPressure = qreal type parameter
     *
     *  \param  SealPosition = seal position
     *  \return from IDSealingCheck
     */
    /****************************************************************************/
    ReturnCode_t IDSealingCheck(qreal ThresholdPressure, RVPosition_t SealPosition);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetFunctionModuleRef
     *
     *  \param InstanceID = quint32 type parameter
     *  \param Key =  QString type parameter
     *
     *  \return from GetFunctionModuleRef
     */
    /****************************************************************************/
    CFunctionModule* GetFunctionModuleRef(quint32 InstanceID, const QString &Key);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveCurrent
     *
     *  \param Type = Slave module type
     *
     *  \return Slave current
     */
    /****************************************************************************/
    quint16 IDGetSlaveCurrent(HimSlaveType_t Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDGetSlaveVoltage
     *
     *  \param Type = Slave module type
     *
     *  \return Slave voltage
     */
    /****************************************************************************/
    quint16 IDGetSlaveVoltage(HimSlaveType_t Type);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetBaseModule
     *
     *  \param Type = Slave module type
     *
     *  \return BaseModule
     */
    /****************************************************************************/
    CBaseModule* GetBaseModule(HimSlaveType_t Type);

    /****************************************************************************/
    /*!
     *  \brief  get remote alarm connect status
     *
     *  \return 1: connected, 0: not connected
     */
    quint16 IDGetRemoteAlarmStatus();

    /****************************************************************************/
    /*!
     *  \brief  get local alarm connect status
     *
     *  \return 1: connected, 0: not connected
     */
    quint16 IDGetLocalAlarmStatus();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IDSetAlarm
     *
     *  \param opcode = operate code to turn on/off local/remote alarm
     *
     *  \return operate result
     */
    ReturnCode_t IDSetAlarm(qint8 opcode);

    /****************************************************************************/
    /*!
     *  \brief  Initialize the state machine for archieve service information
     *
     */
    /****************************************************************************/
    void InitArchiveServiceInforState();
    /****************************************************************************/
    /*!
     *  \brief  start to archieve service information
     *
     */
    /****************************************************************************/
    void ArchiveServiceInfor();
    /****************************************************************************/
    /*!
     *  \brief  report last service information is saved
     *
     */
    /****************************************************************************/
    /*!
     *  \brief  function NotifySavedServiceInfor
     *  \param  deviceType device type
     *
     */
    void NotifySavedServiceInfor(const QString& deviceType);
    /****************************************************************************/
    /*!
     *  \brief  Reset the life time of ActiveCarbonFilter
     *  \param  setVal -- lifetime for carbon filter
     *
     */
    /****************************************************************************/
    void ResetActiveCarbonFilterLifeTime(quint32 setVal);
    /****************************************************************************/
    /*!
     *  \brief  Get Current and Voltage status of all the slave devices
     *
     *  \return bool
     */
    /****************************************************************************/
    bool GetCurrentVoltageStatus(){return false;}
    /****************************************************************************/
    /*!
     *  \brief  Get report error for the specific slave module
     *
     *  \param  errorCode - Error Code of temperature module
     *  \param  devName - Device name
     *  \param  sensorName sensor name
     *
     *  \return ReportError_t
     */
    /****************************************************************************/
    ReportError_t GetSlaveModuleReportError(quint8 errorCode, const QString& devName, quint32 sensorName);

    /****************************************************************************/
    /*!
     *  \brief  Get current of the specific sensor
     *
     *  \param  DevName - Device name
     *  \param  Index index
     *
     *  \return sensor's current
     */
    /****************************************************************************/
    quint16 GetSensorCurrent(const QString& DevName, quint8 Index);

    /****************************************************************************/
    /*!
     *  \brief  Get heater switch type of the specific sensor
     *  \param  DevName - Device name
     *  \return sensor's switch type
     */
    /****************************************************************************/
    quint8 GetHeaterSwitchType(const QString& DevName);

    /****************************************************************************/
    /*!
     *  \brief  delay some time
     *  \param  DelayTime - time value
     *  \return return the exec value
     */
    /****************************************************************************/
    int DelaySomeTime(int DelayTime);

signals:
    //! Forward the 'intitialisation finished' notification
    void ReportInitializationFinished(quint32, ReturnCode_t);
    //! Forward the 'configuration finished' notification
    void ReportConfigurationFinished(quint32, ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void ReportStartNormalOperationMode(quint32, ReturnCode_t);

    /****************************************************************************/
    /*!
     *  \brief  Forward error information via a signal
     *
     *  \iparam instanceID = the error reporting object
     *  \iparam usErrorGroup = Error group ID
     *  \iparam usErrorID    = Error group ID
     *  \iparam usErrorData  = Optional error data
     *  \iparam timeStamp  = Error time stamp
     */
    /****************************************************************************/
    void ReportError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                     quint16 usErrorData, QDateTime timeStamp);
    /****************************************************************************/
    /*!
     *  \brief  Forward error information via a signal
     *
     *  \iparam instanceID = the error reporting object
     *  \iparam usErrorGroup = Error group ID
     *  \iparam usErrorID    = Error group ID
     *  \iparam usErrorData  = Optional error data
     *  \iparam timeStamp  = Error time stamp
     *  \iparam strErrorInfo  = Error information
     */
    /****************************************************************************/
    void ReportErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                             quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Forward the 'Destroy finished' to IDeviceProcessing
    void ReportDestroyFinished();
    /****************************************************************************/
    /*!
     *  \brief  Emitted when all devices should shut down
     */
    /****************************************************************************/
    void DeviceShutdown();

    /****************************************************************************/
    /*!
     *  \brief  Forward the 'level sensor change to 1' to scheduler module
     */
    /****************************************************************************/
    void ReportLevelSensorStatus1();

    /****************************************************************************/
    /*!
     *  \brief  Forward Filling TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    void ReportFillingTimeOut2Min();

    /****************************************************************************/
    /*!
     *  \brief  Forward Draining TimeOut 2Min to scheduler module
     */
    /****************************************************************************/
    void ReportDrainingTimeOut2Min();

    /*!
     *  \brief  function ReportGetServiceInfo
     * \param  ReturnCode return code
     * \param  ModuleInfo module info
     */
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString&);
    /****************************************************************************/
    /*!
     *  \brief  function ReportSavedServiceInfor
     *
     *  \iparam deviceType = device type
     */
    /****************************************************************************/
    void ReportSavedServiceInfor(const QString& deviceType);

public slots:
    //! Get error information from DeviceProcessing
    void OnError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                 quint16 usErrorData, QDateTime timeStamp);

private slots:
    //! Task handling
    void HandleTasks();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ThreadStarted
     */
    /****************************************************************************/
    void ThreadStarted();

    //! Get the 'intitialisation finished' notification
    void OnInitializationFinished(ReturnCode_t);
    //! Get the 'configuration finished' notification
    void OnConfigurationFinished(ReturnCode_t);
    //! Forward the 'normal operation mode started' notification
    void OnStartNormalOperationMode(ReturnCode_t);
    //! Get error information from DeviceProcessing
    void OnErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID,
                         quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo);

    //! Device control layer diagnostic service acknwoledge
    void OnDiagnosticServiceClosed(qint16 DiagnosticResult);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnDestroyFinished
     */
    /****************************************************************************/
    void OnDestroyFinished();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutSaveServiceInfor
     */
    /****************************************************************************/
    void OnTimeOutSaveServiceInfor();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutSaveLifeCycleRecord
     */
    /****************************************************************************/
    void OnTimeOutSaveLifeCycleRecord();

public:
    typedef struct AbstractDevice
    {
        QString Type;
        quint32 InstanceId;
    }AbstractDevice_t;

private:
    //! Handle the state 'Task request pending'
    void HandleTaskRequestState();
    bool ReadDeviceConfig();
    void CreateDeviceMapping();

    DeviceProcessing *mp_DevProc;     //!< Device processing instance
    QThread *mp_DevProcThread;        //!< Device processing thread
    QTimer *mp_DevProcTimer;          //!< Device processing timer
    Qt::HANDLE m_ParentThreadID;      //!< Parent thread ID

    //! Device processing task ID
    typedef enum {
        IDEVPROC_TASKID_INIT     = 0x00,    //!< Initialisation
        IDEVPROC_TASKID_FREE     = 0x01,    //!< Task free, nothing to do
        IDEVPROC_TASKID_ERROR    = 0x02,    //!< Error state
        IDEVPROC_TASKID_REQ_TASK = 0x03     //!< A reqest is active
    } IDeviceProcessingTaskID_t;

    //! Device processing task state
    typedef enum {
        IDEVPROC_TASK_STATE_FREE     = 0x00,    //!< Task state free, ready for action request
        IDEVPROC_TASK_STATE_REQ      = 0x01,    //!< An action was requested, next step will be to forward the command
        IDEVPROC_TASK_STATE_REQ_SEND = 0x02,    //!< Command was forwarded, wait for acknowledge, check timeout
        IDEVPROC_TASK_STATE_ERROR    = 0x03     //!< Error, e.g. timeout while waiting for acknowledge, or error CAN-message received
    } IDeviceProcessingTaskState_t;

    IDeviceProcessingTaskID_t m_taskID;         //!< Task identifier
    IDeviceProcessingTaskState_t m_taskState;   //!< Task state

    DeviceProcTask::TaskID_t m_reqTaskID;           //!< Task identification
    DeviceProcTask::TaskPrio_t m_reqTaskPriority;   //!< Task priority
    quint16 m_reqTaskParameter1;                    //!< Task parameter 1
    quint16 m_reqTaskParameter2;                    //!< Task parameter 2

    quint32 m_instanceID;                           //!< Instance identification
    QMap<QString, CRotaryValveDevice*> m_pRotaryValves;             //!< Rotary Valve device
    QMap<QString, CAirLiquidDevice*> m_pAirLiquids;                 //!< Air-liquid device
    QMap<QString, CRetortDevice*> m_pRetorts;                       //!< Retort device
    QMap<QString, COvenDevice*> m_pOvens;                           //!< Oven device
    QMap<QString, CPeripheryDevice*>m_pPeripherys;                 //!< Periphery device

//    CRotaryValveDevice* m_pRotaryValve;             //!< Rotary Valve device
//    CAirLiquidDevice* m_pAirLiquid;                 //!< Air-liquid device
//    CRetortDevice* m_pRetort;                       //!< Retort device
//    COvenDevice* m_pOven;                           //!< Oven device
//    CPeripheryDevice* m_pPeriphery;                 //!< Periphery device

    QMutex m_IMutex;                //!< Handles thread safety of IDeviceProcessing
    QMutex m_Mutex;                 //!< Handles thread safety of DeviceProcessing
    QStateMachine m_machine;        //!< State machine
    QTimer m_TimerSaveServiceInfor; //!< timer for service info
    QMap<QString, QList<AbstractDevice_t>> m_callerDeviceMap;
    QList<quint32> m_deviceList;     //!< device list
    bool    m_EnableLowerPressure;  //!< enable lower pressure
    int m_DevProcTimerInterval;              //!< timer interval for mp_DevProcTimer
    QTimer m_SaveLifeCycleRecordTimer;
    const QString hwconfigFilename;
    DeviceManager* m_pDeviceManager;
};

} //namespace

#endif /* IDEVICEPROCESSING_H */
