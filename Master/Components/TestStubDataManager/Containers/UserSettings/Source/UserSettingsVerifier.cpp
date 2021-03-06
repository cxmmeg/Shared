/****************************************************************************/
/*! \file UserSettingsVerifier.cpp
 *
 *  \brief Implementation file for class UserSettingsVerifier.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2010-08-23, 2012-04-23
 *  $Author:    $ J.Bugariu, Raju
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

#include <QDebug>
#include <QFile>

#include "TestStubDataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "TestStubDataManager/Containers/UserSettings/Include/UserSettings.h"
#include "TestStubDataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"



namespace DataManager {

const int MIN_SOUND_NUMBER    = 1; ///< Minimum value for the sound
const int MIN_SOUND_LEVEL     = 2; ///< Minimum value for the sound level
const int MIN_AGITATION_SPEED = 0; ///< Minimum value for agitation speed
const int MIN_OVEN_TEMP       = 40; ///< Minimum value for oven temperature
const int MAX_SOUND_NUMBER    = 6; ///< Maximum value for the sound
const int MAX_SOUND_LEVEL     = 9; ///< Maximum value for the sound level
const int MAX_AGITATION_SPEED = 5; ///< Maximum value for agitation speed
const int MAX_OVEN_TEMP       = 70; ///< Maximum value for oven temperature
const int OVEN_TEMP_STEP      = 5; ///< Step/interval for the over temperature
const int MIN_PROXY_IP_PORT   = 1; ///< Minimum value of IP Port
const int MAX_PROXY_IP_PORT   = 65535; ///< Maximum value of IP Port
const int MIN_IP_ADDRESS_NUMBER = 0;    ///< Minimum value of IP Address Number
const int MAX_IP_ADDRESS_NUMBER = 255;  ///< Maximum value of IP Address Number
const int MAX_PROXY_USERNAME_LENGTH = 16;///< Maximum length of Proxy UserName
const int MIN_PROXY_USERNAME_LENGTH = 1; ///< Minimum length of Proxy UserName
const int MAX_PROXY_PASSWORD_LENGTH = 16; ///< Maximum length of Proxy Password
const int MIN_PROXY_PASSWORD_LENGTH = 4; ///< Minimum length of Proxy Password
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CUserSettingsVerifier::CUserSettingsVerifier() : mp_USettingsInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies user settings
 *
 *  \iparam p_UserSettingsInterface = user settings interface class object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CUserSettingsVerifier::VerifyData(CDataContainerBase* p_UserSettingsInterface)
{    
    // to store the error description
    QString ErrorDescription;
    // by default make the verification flag to true
    bool VerifiedData = true;

    // assign pointer to member variable
    mp_USettingsInterface = static_cast<CUserSettingsInterface*>(p_UserSettingsInterface);

    // check content of user settings
    CUserSettings* UserSettings = mp_USettingsInterface->GetUserSettings();
    // check the language
    switch (UserSettings->GetLanguage()) {
    case QLocale::English:
    case QLocale::German:
    case QLocale::French:
    case QLocale::Italian:
    case QLocale::Spanish:
    case QLocale::Portuguese:
    case QLocale::Dutch:
    case QLocale::Swedish:
    case QLocale::Norwegian:
    case QLocale::Danish:
    case QLocale::Polish:
    case QLocale::Czech:
    case QLocale::Russian:
        break;
    default:
        qDebug() << "Unsupported language is detected";
        m_ErrorHash.insert(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE, Global::tTranslatableStringList() << UserSettings->GetLanguage());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE, Global::tTranslatableStringList() <<UserSettings->GetLanguage(), true);
        VerifiedData = false;
        break;
    }


    // check the Date format
    switch (UserSettings->GetDateFormat()) {
    case Global::DATE_INTERNATIONAL:
    case Global::DATE_ISO:
    case Global::DATE_US:
        break;
    default:
        qDebug() << "Date format is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_DATEFORMAT, Global::tTranslatableStringList() << UserSettings->GetDateFormat());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_DATEFORMAT, Global::tTranslatableStringList() << UserSettings->GetDateFormat(), true);
        VerifiedData = false;
        break;
    }

    // check the Time format
    switch (UserSettings->GetTimeFormat()) {
    case Global::TIME_24:
    case Global::TIME_12:
        break;
    default:
        qDebug() << "Time format is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_TIMEFORMAT, Global::tTranslatableStringList() << UserSettings->GetTimeFormat());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TIMEFORMAT, Global::tTranslatableStringList() <<UserSettings->GetTimeFormat(), true);
        VerifiedData = false;
        break;
    }

    // check the Temperature format
    switch (UserSettings->GetTemperatureFormat()) {
    case Global::TEMP_FORMAT_CELSIUS:
    case Global::TEMP_FORMAT_FAHRENHEIT:
        break;
    default:
        qDebug() << "Tempeature format is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_TEMPFORMAT, Global::tTranslatableStringList() << UserSettings->GetTemperatureFormat());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TEMPFORMAT, Global::tTranslatableStringList() <<UserSettings->GetTemperatureFormat(), true);
        VerifiedData = false;
        break;
    }

    // check the sound level warnings and errors
    CheckSoundLevelWarnings(UserSettings, VerifiedData);

    // check network settings parameters
    CheckNetWorkSettings(UserSettings, VerifiedData);

    if (!((UserSettings->GetValue("Agitation_Speed").toInt() >= MIN_AGITATION_SPEED) && (UserSettings->GetValue("Agitation_Speed").toInt() <= MAX_AGITATION_SPEED))) {
        qDebug() << "Speed of agitation is not defined";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_AGITAION_SPEED, Global::tTranslatableStringList() << UserSettings->GetValue("Agitation_Speed"));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_AGITAION_SPEED, Global::tTranslatableStringList() <<UserSettings->GetValue("Agitation_Speed"), true);
        VerifiedData = false;
    }

    // check the oven start mode
    switch (Global::StringToOvenStartMode(UserSettings->GetValue("Oven_StartMode"), false)) {
    case Global::OVENSTART_AFTER_STARTUP:
    case Global::OVENSTART_BEFORE_PROGRAM:
        break;
    default:
        qDebug() << "Oven Start mode is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_OVENSTARTMODE, Global::tTranslatableStringList() << Global::StringToOvenStartMode(UserSettings->GetValue("Oven_StartMode"), false));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_OVENSTARTMODE, Global::tTranslatableStringList() <<Global::StringToOvenStartMode(UserSettings->GetValue("Oven_StartMode"), false), true);
        VerifiedData = false;
        break;
    }

    // check the Oven temperature
    qDebug() << "Value :: " << QString::number(UserSettings->GetValue("Oven_Temp").toInt() % OVEN_TEMP_STEP);
    if (!((UserSettings->GetValue("Oven_Temp").toInt() >= MIN_OVEN_TEMP) && (UserSettings->GetValue("Oven_Temp").toInt() <= MAX_OVEN_TEMP) && (UserSettings->GetValue("Oven_Temp").toInt() % OVEN_TEMP_STEP == 0))) {
        qDebug() << "Oven temperature values are not proper";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_OVENTEMP, Global::tTranslatableStringList() << UserSettings->GetValue("Oven_Temp"));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_OVENTEMP, Global::tTranslatableStringList() <<UserSettings->GetValue("Oven_Temp"), true);
        VerifiedData = false;
    }    

    // check the Rms state
    switch (Global::StringToOnOffState(UserSettings->GetValue("Rms_State"))) {
    case Global::ONOFFSTATE_ON:
    case Global::ONOFFSTATE_OFF:
        break;
    default:
        qDebug() << "Rms state is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_RMS_ONOFFSTATE, Global::tTranslatableStringList() << Global::StringToOnOffState(UserSettings->GetValue("Rms_State")));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_RMS_ONOFFSTATE, Global::tTranslatableStringList() <<Global::StringToOnOffState(UserSettings->GetValue("Rms_State")), true);
        VerifiedData = false;
        break;
    }

    // check the water type
    switch (Global::StringToWaterType(UserSettings->GetValue("Water_Type"))) {
    case Global::WATER_TYPE_TAP:
    case Global::WATER_TYPE_DISTILLED:
        break;
    default:
        qDebug() << "Water type is not valid";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_WATERTYPE, Global::tTranslatableStringList() << Global::StringToWaterType(UserSettings->GetValue("Water_Type")));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_WATERTYPE, Global::tTranslatableStringList() <<Global::StringToWaterType(UserSettings->GetValue("Water_Type")), true);
        VerifiedData = false;
        break;
    }

    if (!((UserSettings->GetValue("Leica_AgitationSpeed").toInt() >= MIN_AGITATION_SPEED) && (UserSettings->GetValue("Leica_AgitationSpeed").toInt() <= MAX_AGITATION_SPEED))) {
        qDebug() << "Speed of agitation is not defined";
        m_ErrorHash.insert(EVENT_DM_INVALID_LEICA_AGITAION_SPEED, Global::tTranslatableStringList() << UserSettings->GetValue("Leica_AgitationSpeed"));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LEICA_AGITAION_SPEED, Global::tTranslatableStringList() <<UserSettings->GetValue("Leica_AgitationSpeed"), true);
        VerifiedData = false;
    }

    // check the Oven temperature
    qDebug() << "Value :: " << QString::number(UserSettings->GetValue("Leica_OvenTemp").toInt() % OVEN_TEMP_STEP);
    if (!((UserSettings->GetValue("Leica_OvenTemp").toInt() >= MIN_OVEN_TEMP) && (UserSettings->GetValue("Leica_OvenTemp").toInt() <= MAX_OVEN_TEMP) && (UserSettings->GetValue("Leica_OvenTemp").toInt() % OVEN_TEMP_STEP == 0))) {
        qDebug() << "Oven temperature values are not proper";
        m_ErrorHash.insert(EVENT_DM_INVALID_LEICA_OVENTEMP, Global::tTranslatableStringList() << UserSettings->GetValue("Leica_OvenTemp"));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LEICA_OVENTEMP, Global::tTranslatableStringList() <<UserSettings->GetValue("Leica_OvenTemp"), true);
        VerifiedData = false;
    }

    // check the loader reagents
    CheckLoaderReagents(UserSettings, VerifiedData);

    return VerifiedData;

}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CUserSettingsVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CUserSettingsVerifier::ResetLastErrors()
{
    m_ErrorHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Returns whether verifier is a local verifier or not
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CUserSettingsVerifier::IsLocalVerifier()
{
    return true;
}

/****************************************************************************/
/*!
 *  \brief  check the loader reagent ID
 *
 *  \iparam LoaderReagentID - Reagent ID for the loader
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CUserSettingsVerifier::CheckLoaderReagentID(QString LoaderReagentID)
{
    bool VerifyLoaderReagentData = true;
    // check the reagent ID
    if (LoaderReagentID != "") {
        // check the reagent ID starts with letter 'U' or 'L'
        if ((LoaderReagentID.at(0) != 'U') && (LoaderReagentID.at(0) != 'L' && LoaderReagentID != "-1")) {
            //qDebug() << "Invalid Loader Reagent" << QString::number(ReagentCount + 1);
            VerifyLoaderReagentData = false;
        }

        bool Ok = false;
        // check the reagent ID consists of integer or not, except for the first letter
        int ReagentID = LoaderReagentID.mid(1).toInt(&Ok);
        if ((!Ok) && (!(ReagentID >= 1))) {
            //qDebug() << "Invalid Loader Reagent" << QString::number(ReagentCount + 1);
            VerifyLoaderReagentData = false;
        }
    }

    return VerifyLoaderReagentData;
}


/****************************************************************************/
/*!
 *  \brief  check the loader reagents
 *
 *  \iparam UserSettings - Pointer to CUserSettings
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ErrorDescription = Description of the error
 *
 */
/****************************************************************************/
void CUserSettingsVerifier::CheckLoaderReagents(CUserSettings* UserSettings, bool& VerifiedData)
{
    // check the first loader reagent ID
    if (!CheckLoaderReagentID (UserSettings->GetValue("Loader_Reagent1")))
    {
        qDebug() << "First loader reagent ID is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() << " 1");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() <<" 1", true);
        VerifiedData = false;
    }

    // check the second loader reagent ID
    if (!CheckLoaderReagentID (UserSettings->GetValue("Loader_Reagent2")))
    {
        qDebug() << "Second loader reagent ID is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() << " 2");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() <<" 2", true);
        VerifiedData = false;
    }

    // check the third loader reagent ID
    if (!CheckLoaderReagentID (UserSettings->GetValue("Loader_Reagent3")))
    {
        qDebug() << "Third loader reagent ID is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() << " 3");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() <<" 3", true);
        VerifiedData = false;
    }

    // check the fourth loader reagent ID
    if (!CheckLoaderReagentID (UserSettings->GetValue("Loader_Reagent4")))
    {
        qDebug() << "Fourth loader reagent ID is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() << " 4");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() <<" 4", true);
        VerifiedData = false;
    }

    // check the Fifth loader reagent ID
    if (!CheckLoaderReagentID (UserSettings->GetValue("Loader_Reagent5")))
    {
        qDebug() << "Fifth loader reagent ID is not valid";
        m_ErrorHash.insert(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() << " 5");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_LOADER_REAGENTID, Global::tTranslatableStringList() <<" 5", true);
        VerifiedData = false;
    }
}
/****************************************************************************/
/*!
 *  \brief  check the sould level warnings
 *
 *  \iparam UserSettings - Pointer to CUserSettings
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ErrorDescription = Description of the error
 *
 */
/****************************************************************************/
void CUserSettingsVerifier::CheckSoundLevelWarnings(CUserSettings* UserSettings, bool& VerifiedData)
{
    // check the error tones for the sound
    if (!((UserSettings->GetSoundNumberError() >= MIN_SOUND_NUMBER) && (UserSettings->GetSoundNumberError() <= MAX_SOUND_NUMBER))) {
        qDebug() << "Unknown error tone is detected for the sound";
        m_ErrorHash.insert(EVENT_DM_ERROR_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((UserSettings->GetSoundLevelError() >= MIN_SOUND_LEVEL) && (UserSettings->GetSoundLevelError() <= MAX_SOUND_LEVEL))) {
        qDebug() << "Unknown sound number error volume tone is detected";
        m_ErrorHash.insert(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((UserSettings->GetSoundNumberWarning() >= MIN_SOUND_NUMBER) && (UserSettings->GetSoundNumberWarning() <= MAX_SOUND_NUMBER))) {
        qDebug() << "Unknown warning warning tone is detected for the sound";
        m_ErrorHash.insert(EVENT_DM_WARN_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() << "");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_WARN_SOUND_NUMBER_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

    if (!((UserSettings->GetSoundLevelWarning() >= MIN_SOUND_LEVEL) && (UserSettings->GetSoundLevelWarning() <= MAX_SOUND_LEVEL))) {
        qDebug() << "Unknown sound number warning volume tone is detected";
        m_ErrorHash.insert(EVENT_DM_WARN_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_WARN_SOUND_LEVEL_OUT_OF_RANGE, Global::tTranslatableStringList() <<"", true);
        VerifiedData = false;
    }

}

/****************************************************************************/
/*!
 *  \brief  Check Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ErrorDescription = Description of the error
 *
 */
/****************************************************************************/
void CUserSettingsVerifier::CheckNetWorkSettings(CUserSettings* p_UserSettings, bool& VerifiedData)
{
    if (!((p_UserSettings->GetRemoteCare() == Global::ONOFFSTATE_ON || p_UserSettings->GetRemoteCare() == Global::ONOFFSTATE_OFF))) {
        qDebug() << "NETWORK SETTINGS REMOTE CONNECTION IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_REMOTECARE_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetRemoteCare());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_REMOTECARE_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetRemoteCare(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetDirectConnection() == Global::ONOFFSTATE_ON || p_UserSettings->GetDirectConnection() == Global::ONOFFSTATE_OFF))) {
        qDebug() << "NETWORK SETTINGS DIRECT CONNECTION IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_DIRECT_CONNECTION_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetDirectConnection());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_DIRECT_CONNECTION_ONOFFSTATE, Global::tTranslatableStringList() << p_UserSettings->GetDirectConnection(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetProxyUserName().isEmpty() != true  && p_UserSettings->GetProxyUserName().length() >= MIN_PROXY_USERNAME_LENGTH &&
          p_UserSettings->GetProxyUserName().length() <= MAX_PROXY_USERNAME_LENGTH))) {
        qDebug() << "PROXY USERNAME IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_INVALID_PROXY_USERNAME_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyUserName());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROXY_USERNAME_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyUserName(), true);
        VerifiedData = false;
    }

    if (!((p_UserSettings->GetProxyPassword().isEmpty() != true && p_UserSettings->GetProxyPassword().length() >= MIN_PROXY_PASSWORD_LENGTH &&
           p_UserSettings->GetProxyPassword().length() <= MAX_PROXY_PASSWORD_LENGTH))) {
        qDebug() << "PROXY PASSWORD IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_INVALID_PROXY_PASSWORD_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyPassword());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_PROXY_PASSWORD_CHAR_COUNT, Global::tTranslatableStringList() << p_UserSettings->GetProxyPassword(), true);
        VerifiedData = false;
    }

    if (!(CheckProxyIPAddress(p_UserSettings))) {
        qDebug() << "PROXY IP ADDRESS NOT IS NOT VALID";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_PROXY_IP_ADDRESS, Global::tTranslatableStringList() << p_UserSettings->GetProxyIPAddress());
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_PROXY_IP_ADDRESS, Global::tTranslatableStringList() <<  p_UserSettings->GetProxyIPAddress(), true);
        VerifiedData = false;
    }

    if (!(CheckProxyIPPort(p_UserSettings))) {
        qDebug() << "PROXY IP PORT NUMBER NOT IN RANGE";
        m_ErrorHash.insert(EVENT_DM_ERROR_INVALID_PROXY_IP_PORT, Global::tTranslatableStringList() << QString::number(p_UserSettings->GetProxyIPPort()));
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_INVALID_PROXY_IP_PORT, Global::tTranslatableStringList() << QString::number(p_UserSettings->GetProxyIPPort()), true);
        VerifiedData = false;
    }

}

/****************************************************************************/
/*!
 *  \brief Check Proxy IP Address of Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/

bool CUserSettingsVerifier::CheckProxyIPAddress(CUserSettings *p_UserSettings)
{
    QString ProxyIPAddress = p_UserSettings->GetProxyIPAddress();
    qDebug()<<"\n\n Count= "<< ProxyIPAddress.split(".").count();
    bool VerifiedIPAddress = false;
    if (ProxyIPAddress.split(".").count() == 4) {
        for (int SplitCount = 0; SplitCount < ProxyIPAddress.split(".").count(); SplitCount++)
        {
            QString AddressNumberString = (ProxyIPAddress.split(".").value(SplitCount));
            if (AddressNumberString.length() <= 3) {
                bool Result = false;
                int AddressNumber = AddressNumberString.toInt(&Result, 10);
                if (Result) {
                    qDebug()<<"\n\n AddressNumber ="<< AddressNumber;
                    if (AddressNumber < MIN_IP_ADDRESS_NUMBER || AddressNumber > MAX_IP_ADDRESS_NUMBER || AddressNumberString == "") {
                        return false;
                    }
                    else if (SplitCount == 3) {
                        if (AddressNumber == 0) {
                            return false;
                        }
                        else {
                            VerifiedIPAddress = true;
                        }
                    }
                    else {
                        VerifiedIPAddress = true;
                    }
                }
                else {
                    VerifiedIPAddress = false;
                    return VerifiedIPAddress;
                }
            }
            else {
                VerifiedIPAddress = false;
                return VerifiedIPAddress;
            }
        }
    }
    else {
        VerifiedIPAddress = false;
    }
    return VerifiedIPAddress;
}

/****************************************************************************/
/*!
 *  \brief Check Proxy IP Port of Network Settings.
 *
 *  \iparam p_UserSettings - Pointer to CUserSettings
 *  \return True - If IP address is valid else False
 */
/****************************************************************************/
bool CUserSettingsVerifier::CheckProxyIPPort(CUserSettings *p_UserSettings)
{
    bool Result = false;
    QString IPPort = QString::number(p_UserSettings->GetProxyIPPort());
    int PortNumber = IPPort.toInt(&Result, 10);
    if (Result) {
        if (PortNumber < MIN_PROXY_IP_PORT || PortNumber > MAX_PROXY_IP_PORT) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}
}  // namespace DataManager



