/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Include/UserSettings.h
 *
 *  \brief Definition file for class CUserSettings.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Raju123
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

#ifndef DATAMANAGER_CUSERSETTINGS_H
#define DATAMANAGER_CUSERSETTINGS_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "Global/Include/GlobalDefines.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"


namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for user settings.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CUserSettings {
    friend class CUserSettingsInterface;
private:
    int                         m_Version;              ///< Store the version number of the file
    QLocale::Language           m_Language;             ///< Language.
    Global::DateFormat          m_DateFormat;           ///< Format for date.
    Global::TimeFormat          m_TimeFormat;           ///< Format for time.
    Global::TemperatureFormat   m_TemperatureFormat;    ///< Temperature format.
    int                         m_SoundNumberError;     ///< Sound number for errors.
    int                         m_SoundLevelError;      ///< Sound level for errors.
    int                         m_SoundNumberWarning;   ///< Sound number for warnings.
    int                         m_SoundLevelWarning;    ///< Sound level for warnings.
    ErrorHash_t                 m_ErrorHash;            //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    Global::OnOffState          m_RemoteCare;           ///< True(On) if RemoteCare software is used else False(Off).
    Global::OnOffState          m_DirectConnection;     ///< True(On) if the device is directly connected esle False(Off).
    QString                     m_ProxyUserName;        ///< ProxyUserName
    QString                     m_ProxyPassword;        ///< ProxyPassword
    QString                     m_ProxyIPAddress;       ///< ProxyIPAddress
    int                         m_ProxyIPPort;          ///< ProxyIPPort
    QHash<QString, QString>     m_ValueList;            ///< User Settings Hash
    /****************************************************************************/

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    bool ReadSoundSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadLocalization(QXmlStreamReader& XmlStreamReader);
    bool ReadNetworkSettings(QXmlStreamReader& XmlStreamReader);

    /****************************************************************************/
    /*!
     *  \brief Set the version number of the Xml config file
     *
     *  \iparam Value = version number
     */
    /****************************************************************************/
    void SetVersion(int Value)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_Version = Value;
    }

protected:
public:

    CUserSettings();
    CUserSettings(const CUserSettings &);

    virtual ~CUserSettings();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CUserSettings& UserSettings);
    friend QDataStream& operator >>(QDataStream& InDataStream, CUserSettings& UserSettings);
    CUserSettings & operator = (const CUserSettings &);

    void SetDefaultAttributes();

    ErrorHash_t &GetErrors();


    /****************************************************************************/
    /*!
     *  \brief Get the version number
     *
     *  \return version number
     */
    /****************************************************************************/
    int GetVersion() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_Version;
    }    

    /****************************************************************************/
    /*!
     *  \brief Get the laguage name
     *
     *  \return language name
     */
    /****************************************************************************/
    QLocale::Language GetLanguage() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_Language;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the language
     *
     *  \iparam TheLanguage = language name
     */
    /****************************************************************************/
    void SetLanguage(QLocale::Language TheLanguage)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_Language = TheLanguage;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the Date format
     *
     *  \return Date format
     */
    /****************************************************************************/
    Global::DateFormat GetDateFormat() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_DateFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Date format
     *
     *  \iparam TheDateFormat = date format
     */
    /****************************************************************************/
    void SetDateFormat(Global::DateFormat TheDateFormat)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_DateFormat = TheDateFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the time format
     *
     *  \return time format
     */
    /****************************************************************************/
    Global::TimeFormat GetTimeFormat() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_TimeFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the time format
     *
     *  \iparam TheTimeFormat = time format
     */
    /****************************************************************************/
    void SetTimeFormat(Global::TimeFormat TheTimeFormat)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_TimeFormat = TheTimeFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the temperature format
     *
     *  \return temperature format
     */
    /****************************************************************************/
    Global::TemperatureFormat GetTemperatureFormat() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_TemperatureFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the temperature format
     *
     *  \iparam TheFormat = temperature format
     */
    /****************************************************************************/
    void SetTemperatureFormat(Global::TemperatureFormat TheFormat)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_TemperatureFormat = TheFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound number error
     *
     *  \return sound number error
     */
    /****************************************************************************/
    int GetSoundNumberError() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_SoundNumberError;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound number error
     *
     *  \iparam SoundNumberError = Error of the sound number
     */
    /****************************************************************************/
    void SetSoundNumberError(int SoundNumberError)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_SoundNumberError = SoundNumberError;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound number warning
     *
     *  \return sound number warning
     */
    /****************************************************************************/
    int GetSoundNumberWarning() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_SoundNumberWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound number warning
     *
     *  \iparam SoundNumberWarning = warning number of the sound
     */
    /****************************************************************************/
    void SetSoundNumberWarning(int SoundNumberWarning)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_SoundNumberWarning = SoundNumberWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound level error
     *
     *  \return sound level error value
     */
    /****************************************************************************/
    int GetSoundLevelError() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_SoundLevelError;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound level error
     *
     *  \iparam SoundLevel = level of the sound error
     */
    /****************************************************************************/
    void SetSoundLevelError(int SoundLevel)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_SoundLevelError = SoundLevel;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound level warning
     *
     *  \return sound level warning value
     */
    /****************************************************************************/
    int GetSoundLevelWarning() const
    {
        //QReadLocker locker(mp_ReadWriteLock);
        return m_SoundLevelWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound level warning
     *
     *  \iparam SoundLevel = volume of the sound level
     */
    /****************************************************************************/
    void SetSoundLevelWarning(int SoundLevel)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_SoundLevelWarning = SoundLevel;
    }

    QString GetValue(QString key) const;

    /****************************************************************************/
    /*!
     *  \brief Set Value (e.g. RMS_STATE)
     *
     *  \iparam key = unique key e.g. RMS_STATE
     *  \iparam value = value associated with Key
     */
    /****************************************************************************/
    void SetValue(QString key, QString value)
    {
        if (!m_ValueList.contains(key.toUpper()))
        {
            qDebug() << "UserSettings::SetValue, key not found" << key;
        }
        m_ValueList.insert(key.toUpper(), value);
    }

    /****************************************************************************/
    /*!
     *  \brief Set Value (e.g. RMS_STATE)
     *
     *  \iparam key = unique key e.g. RMS_STATE
     *  \iparam value = value associated with Key
     */
    /****************************************************************************/
    void SetValue(QString key, qint32 value)
    {
        SetValue(key, QString::number(value));
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the use of Remote Care Software to operate the device.
     *
     *  \return RemoteCare value.
     */
    /****************************************************************************/
    Global::OnOffState GetRemoteCare() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_RemoteCare;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the use of Remote Care Software to operate the device.
     *
     *  \iparam RemoteCareOnOffState = On if Remote Care Software is used to operate the
     *          device else Off.
     */
    /****************************************************************************/
    void SetRemoteCare(Global::OnOffState RemoteCareOnOffState)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_RemoteCare = RemoteCareOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the DirectConnection value.
     *
     *  \return DirectConnection value.
     */
    /****************************************************************************/
     Global::OnOffState GetDirectConnection() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_DirectConnection;
    }
    /****************************************************************************/
    /*!
     *  \brief Set the Device Direct Connection.
     *
     *  \iparam DeviceConnected = On if Device is directly connected else Off.
     */
    /****************************************************************************/
    void SetDirectConnection( Global::OnOffState DeviceConnected)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_DirectConnection = DeviceConnected;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy UserName.
     *
     *  \return Proxy UserName.
     */
    /****************************************************************************/
    QString GetProxyUserName() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_ProxyUserName;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy UserName
     *
     *  \iparam ProxyUserName = Proxy UserName
     */
    /****************************************************************************/
    void SetProxyUserName(QString ProxyUserName)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_ProxyUserName = ProxyUserName;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy Password.
     *
     *  \return Proxy Password.
     */
    /****************************************************************************/
    QString GetProxyPassword() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_ProxyPassword;
    }

    /****************************************************************************/
    /*!
     *  \brief Set Proxy Password.
     *
     *  \iparam ProxyPassword = Proxy Password.
     */
    /****************************************************************************/
    void SetProxyPassword(QString ProxyPassword)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_ProxyPassword = ProxyPassword;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy IP Address.
     *
     *  \return Proxy IP Address.
     */
    /****************************************************************************/
    QString GetProxyIPAddress() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_ProxyIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy IP Address.
     *
     *  \iparam ProxyIPAddress = Proxy IP Address
     */
    /****************************************************************************/
    void SetProxyIPAddress(QString ProxyIPAddress)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_ProxyIPAddress = ProxyIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy IP Port.
     *
     *  \return Proxy IP Port.
     */
    /****************************************************************************/
    int GetProxyIPPort() const
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        return m_ProxyIPPort;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy IP Port number.
     *
     *  \iparam ProxyIPPort = Proxy IP Port number
     */
    /****************************************************************************/
    void SetProxyIPPort(int ProxyIPPort)
    {
        //QWriteLocker locker(mp_ReadWriteLock);
        m_ProxyIPPort = ProxyIPPort;
    }
}; // end class CUserSettings

} // end namespace DataManager

#endif // DATAMANAGEMENT_CUserSettings_H
