/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ExportConfiguration/Include/UserConfiguration.h
 *
 *  \brief Definition file for class CUserConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
 *  $Author:    $ Raju, Ramya GJ
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
#ifndef USERCONFIGURATION_H
#define USERCONFIGURATION_H

#include "QXmlStreamWriter"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ExportConfiguration/Include/ConfigurationList.h"

namespace DataManager {
/******************************************************************************/
/*!
 *  \brief  This class implements user configuration for the Export component
 */
/******************************************************************************/
class CUserConfiguration
{
    friend class CExportConfiguration;
private:

    CConfigurationList m_UserConfigList; ///< to store the user configuration list
    CConfigurationList m_UserReportList; ///< to store the user report configuration list
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

public:
    CUserConfiguration();

    CUserConfiguration(const CUserConfiguration&);
    void CopyFromOther(const CUserConfiguration &Other);

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CUserConfiguration& ExportConfiguration);
    friend QDataStream& operator >>(QDataStream& InDataStream, CUserConfiguration& ExportConfiguration);
    CUserConfiguration& operator=(const CUserConfiguration&);


    /****************************************************************************/
    /*!
     *  \brief Retrieve the user configuration
     *  \return configuration class
     */
    /****************************************************************************/
    CConfigurationList GetUserConfigurationList() { return m_UserConfigList;}

    /****************************************************************************/
    /*!
     *  \brief Sets the user configuration
     *
     *  \iparam ConfigList = Configuration of the user
     */
    /****************************************************************************/
    void SetUserConfigurationList(CConfigurationList ConfigList) { m_UserConfigList = ConfigList;}

    /****************************************************************************/
    /*!
     *  \brief Retrieve the user report configuration
     *  \return configuration class
     */
    /****************************************************************************/
    CConfigurationList GetUserReportList() { return m_UserReportList;}

    /****************************************************************************/
    /*!
     *  \brief Sets the user report configuration
     *
     *  \iparam ReportList = Configuration of the user report
     */
    /****************************************************************************/
    void SetUserReportList(CConfigurationList ReportList) { m_UserReportList = ReportList;}


};
}

#endif // USERCONFIGURATION_H
