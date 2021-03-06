/****************************************************************************/
/*! \file   InfoTemperatureControl.cpp
 *
 *  \brief  Implementaion file for class CInfoTemperatureControl
 *
 *  \version  0.1
 *  \date     2012-01-31
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoTemperatureControl.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

//! Signal transition for CInfoTemperatureControl
typedef CSignalTransition<CInfoTemperatureControl> CInfoTemperatureControlTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoTemperatureControl
 *
 *  \iparam p_TemperatureControl = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoTemperatureControl::CInfoTemperatureControl(CTemperatureControl *p_TemperatureControl,
                                                 DataManager::CSubModule *p_SubModule, QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_TemperatureControl(p_TemperatureControl), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoTemperatureControl)
    CState *p_Init = new CState("Init", this);
    CState *p_GetHeaterOperatingTime = new CState("GetHeaterOperatingTime", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoTemperatureControlTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoTemperatureControl::GetHeaterOperatingTime,
        p_GetHeaterOperatingTime));

    p_GetHeaterOperatingTime->addTransition(new CInfoTemperatureControlTransition(
        mp_TemperatureControl, SIGNAL(ReportHeaterOperatingTime(quint32, ReturnCode_t, quint8, quint32)),
        *this, &CInfoTemperatureControl::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("OperationTime", "seconds", QString());
    mp_SubModule->AddParameterInfo("OperationCycles", QString());
}

/****************************************************************************/
/*!
 *  \brief  Requests the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoTemperatureControl::GetHeaterOperatingTime(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:Begin";
    ReturnCode_t ReturnCode = mp_TemperatureControl->GetHeaterOperatingTime(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:E1";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR10);
        return false;
    }
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:End";
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives the life time data from the function module
 *
 *  \iparam p_Event = Parameters of the signal ReportHeaterOperatingTime
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoTemperatureControl::Finished(QEvent *p_Event)
{
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl:Begin";
    ReturnCode_t ReturnCode;
    quint32 OperatingTime;
    QString Version = QString().setNum(mp_TemperatureControl->GetBaseModule()->GetModuleSWVersion(mp_TemperatureControl->GetType()));

    ReturnCode = CInfoTemperatureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl,E2";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR11);
        return false;
    }
    if (!CInfoTemperatureControlTransition::GetEventValue(p_Event, 3, OperatingTime)) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl,E3";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR12);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl,E4";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR13);
        return false;
    }

    quint32 history_OperationTime = 0;
    PartLifeCycleRecord* pPartLifeCycleRecord = mp_TemperatureControl->GetPartLifeCycleRecord();
    if (pPartLifeCycleRecord)
    {
        history_OperationTime = pPartLifeCycleRecord->m_ParamMap.value("History_OperationTime").toUInt();
    }

    if (!mp_SubModule->UpdateParameterInfo("OperationTime", QString().setNum(OperatingTime + history_OperationTime))) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl,E5";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR14);
        return false;
    }

    quint32 lifeCycle = mp_TemperatureControl->GetLifeCycle();
    if (!mp_SubModule->UpdateParameterInfo("OperationCycles", QString().setNum(lifeCycle))) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl,E6";
        emit ReportError(DCL_ERR_DEV_INTER_INTER_LIFE_CYCLE_ERROR15);
        return false;
    }


    if (!pPartLifeCycleRecord)
    {

        return true;
    }

    QString paramName = mp_SubModule->GetSubModuleName() + "_LifeCycle";
    QString strLifeTimeNew = QString().setNum(mp_TemperatureControl->GetLifeCycle());
    QMap<QString, QString>::const_iterator iter = pPartLifeCycleRecord->m_ParamMap.find(paramName);
    if (iter != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap[paramName] = strLifeTimeNew;
     //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoTemperatureControl::Finished() end";
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:
