/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/Stations/Include/StationBase.h
 *
 *  \brief Definition for CStationBase class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel, Ramya GJ
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

#ifndef DATAMANAGER_STATIONBASE_H
#define DATAMANAGER_STATIONBASE_H

#include <QString>
#include <QDate>
#include <QXmlStreamReader>

#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/GlobalDefines.h"

//lint -sem(DataManager::CStationBase::CopyFromOther,initializer)

namespace DataManager {


/****************************************************************************/
/*!
 *  \brief CStationBase class containing the reagents attributes
 */
/****************************************************************************/
class CStationBase
{
    friend class CDataStationList; //!< station list
private:
	/**
	* \brief serialize 
	* \iparam XmlStreamWriter xml stream writer
	* \iparam CompleteData complete data
	* \return bool
	*/
    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);

	/**
	* \brief deserialize 
	* \iparam XmlStreamWriter xml stream writer
	* \iparam CompleteData complete data
	* \return bool
	*/
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

protected:
    QString m_StationID;                        //!< Station ID
    int m_StationTemp;                          //!< Station Temperature (current temperature)
    Global::HeatingState m_HeatingState;        //!< HeatingState of Heated Cuvette/Oven.
    bool m_Defect;                              //!< Whether the station is defect or not
    bool m_Disabled;                            //!< Whether the station is disabled or not

public:
	/**
	* \brief constructor
	*/
    CStationBase();
	/**
	* \brief constructor
	* \iparam ID id
	*/
    CStationBase(const QString ID);
	/**
	* \brief constructor
	* \iparam Station  station
	*/
    CStationBase(const CStationBase& Station);
	/**
	* \brief CopyFromOther
	* \iparam Station  station
	*/
    void CopyFromOther(const CStationBase &Station);

	/**
	* \brief operator << 
	* \iparam  OutDataStream out stream
	* \iparam  Station station
	* \return QDataStream
	*/
    friend QDataStream& operator <<(QDataStream& OutDataStream, const CStationBase& Station);

	/**
	* \brief operator >>
	* \iparam  InDataStream in stream
	* \iparam  Station station
	* \return QDataStream
	*/
    friend QDataStream& operator >>(QDataStream& InDataStream, CStationBase& Station);

	/**
	* \brief operator =
	* \iparam  Station  station
	* \return CStationBase
	*/
    CStationBase& operator=(const CStationBase& Station);

	/**
	* 	\brief GetStationType
	* 	\return station type
	*/
    StationsType_t GetStationType();

    /******************** INLINE FuNCTIONS **************************************/
    /****************************************************************************/
    /*!
     *  \brief Get's the Station ID
     *
     *  \return Station Id String
     */
    /****************************************************************************/
    QString  GetStationID() {return m_StationID;}

    /****************************************************************************/
    /*!
     *  \brief Set's the Station ID
     *
     *  \iparam Value = station ID string
     */
    /****************************************************************************/
    void SetStationID(const QString Value){m_StationID = Value.trimmed();}
    

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Temperature of reagent
     *
     *  \return Station Temperature
     */
    /****************************************************************************/
    int GetStationTemperature() { return m_StationTemp; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Temperature
     *
     *  \iparam Temp = Station Temperature
     *
     */
    /****************************************************************************/
    void SetStationTemperature(const int Temp) {m_StationTemp =  Temp;}


    /****************************************************************************/
    /*!
     *  \brief Sets the heating state of heated cuvette/oven station
     *
     *  \iparam HeatingState = on/Temperature in range/out of range
     */
    /****************************************************************************/
    void SetHeatingState(Global::HeatingState HeatingState) {m_HeatingState = HeatingState;}

    /****************************************************************************/
    /*!
     *  \brief Retrieves the Heating state of heated cuvette/oven station
     *
     *  \return Heating state
     */
    /****************************************************************************/
    Global::HeatingState GetHeatingState() {return m_HeatingState;}

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Defect State
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsStationDefect() { return m_Defect; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Defect State
     *
     *  \iparam Defect = True or false, bool type
     *
     *  \return
     */
    /****************************************************************************/
    void SetStationDefect(bool Defect) { m_Defect = Defect; }

    /****************************************************************************/
    /*!
     *  \brief Gets the Station Defect State
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsStationDisabled() { return m_Disabled; }

    /****************************************************************************/
    /*!
     *  \brief Sets the Station Defect State
     *
     *  \iparam Disabled = True or false, bool type
     *
     *  \return
     */
    /****************************************************************************/
    void SetStationDisabled(bool Disabled) { m_Disabled = Disabled; }

};

} // namespace DataManager

#endif // DATAMANAGER_STATIONBASE_H
