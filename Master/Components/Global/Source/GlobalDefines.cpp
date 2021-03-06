/****************************************************************************/
/*! \file Global/Source/GlobalDefines.cpp
 *
 *  \brief Implementation file for class GlobalDefines.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-08-27
 *  $Author:    $ Shuvasmita
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

#include <Global/Include/GlobalDefines.h>

namespace Global {

bool Global::AppSettings::SimulationEnabled = false;
int Global::AppSettings::SimulationAccelerationFactor = 1;
bool Global::AppSettings::ExtendedLoggingEnabled = false;
bool Global::AppSettings::TraceDebugMessagesEnabled = false;
QString Global::AppSettings::RevisionNumber = "";
bool Global::AppSettings::SettingsRolledBack = false;

} // end namespace Global
