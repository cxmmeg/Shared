/****************************************************************************/
/*! \file   ResetTemperatureControl.h
 *
 *  \brief  Definition file for class CResetTemperatureControl
 *
 *  \version  0.1
 *  \date     2013-01-14
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

#ifndef DEVICECONTROL_RESETTEMPERATURECONTROL_H
#define DEVICECONTROL_RESETTEMPERATURECONTROL_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControl.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CTemperatureControl;

/****************************************************************************/
/*!
 *  \brief  Resets the data of a temperature control module
 */
/****************************************************************************/
class CResetTemperatureControl : public CState
{
    Q_OBJECT

public:
    explicit CResetTemperatureControl(CTemperatureControl *p_TemperatureControl,
                                      const QString &Name, QState *p_Parent = 0);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Emitted when an error occurred during communication
     *
     *  \iparam ReturnCode = Error code
     */
    /****************************************************************************/
    void ReportError(ReturnCode_t ReturnCode);

private:
    bool ResetHeaterOperatingTime(QEvent *p_Event);
    bool ReportResetHeaterOperatingTime(QEvent *p_Event);

    CTemperatureControl *mp_TemperatureControl; //!< Module whose data is reset
};

} //namespace

#endif // DEVICECONTROL_RESETTEMPERATURECONTROL_H