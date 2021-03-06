/****************************************************************************/
/*! \file Master/Components/DataManager/Helper/Include/Helper.h
 *
 *  \brief Definition file for general purpose functions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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

#ifndef DATAMANAGER_HELPER_H
#define DATAMANAGER_HELPER_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QXmlStreamReader>
#include "DataManager/Helper/Include/Types.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief Helper class, which provides some static methods for general use.
 */
/****************************************************************************/
class Helper
{
public:
        /****************************************************************************/
        /**
         * \brief Constructor.
         */
        /****************************************************************************/
        Helper();

        /****************************************************************************/
        /**
         * \brief Convert seconds to time string.
         *
         * The Leica configuration files use a specific format for time durations,
         * e.g. 3d 4h 12m 45s.
         * Internally seconds are used to define time durations.
         * This method converts a given number of seconds into an string value
         * containing the corresponding formated string for this time duration.
         *
         * \param[in] TimeDurationInSeconds  Time duration given in seconds.
         *
         * \return  QString     Leica specific format for time durations.
         */
        /****************************************************************************/
        static QString ConvertSecondsToTimeString(int TimeDurationInSeconds);

        /****************************************************************************/
        /**
         * \brief Convert time string to seconds.
         *
         * The Leica configuration files use a specific format for time durations,
         * e.g. 3d 4h 12m 45s.
         * Internally seconds are used to define time durations.
         * This method converts a given string into an integer value containing
         * the corresponding number of seconds.
         *
         * \param[in]   TimeDuration       Time duration given in seconds.
         *
         * \return  int     Time duration in seconds.
         */
        /****************************************************************************/
        static int ConvertTimeStringToSeconds(QString TimeDuration);

        /**
          * \brief convert date string to qdate string
          * return qdate
        */
        static QDate ConvertDateStringToQDate(QString Date);

        /**
          * \brief convert date string to qdate string
          * \iparam DateTime datetime
          * return QDateTime
        */
        static QDateTime ConvertDateTimeStringToQDateTime(QString DateTime);
        /**
          * \brief read xml node
          * \iparam XmlStreamReader xml stream reader
          * \iparam NodeName node name
          * \return read status
        */
        static bool ReadNode(const QXmlStreamReader& XmlStreamReader, QString NodeName);

        /**
          * \brief  error id to string
          * \iparam ErrorList error list
          * \iparam ErrorString error string
        */
        static void ErrorIDToString(ListOfErrors_t &ErrorList, QString &ErrorString);
};

} // namespace DataManager

#endif // DATAMANAGER_HELPER_H
