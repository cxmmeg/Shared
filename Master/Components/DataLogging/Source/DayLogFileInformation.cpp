/****************************************************************************/
/*! \file DataLogging/Source/DayLogFileInformation.cpp
 *
 *  \brief Implementation file for class DayLogFileInformation.
 *
 *  \b Description:
 *          This class creates the Daily run log files in the file system for the
 *          Export component and creates the Daily run log file stream in memory
 *          for the GUI component and prepares the list of file names.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ Raju
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

#include <QFile>
#include <QDir>
#include <QXmlStreamReader>
#include <QDebug>
#include <QProcess>
#include "DataLogging/Include/DataLoggingEventCodes.h"
#include "DataLogging/Include/DayLogFileInformation.h"
#include <Global/Include/Utils.h>
#include "QCoreApplication"
#include "QEventLoop"

namespace DataLogging {

const char DELIMITER_SEMICOLON              = ';';///< Delimiter for semicolon
const char DELIMITER_UNDERSCORE             = '_';///< Delimiter for underscore
const char DELIMITER_COLON                  = ':';///< Delimiter for colon
const QString FLAG_VALUE                    = "true";///< user log flag value
const QString EMPTY_STRING                  = "";///< empty string
const QString STRING_NEWLINE                = "\n";///< new line
const QString STRING_SEMICOLON              = ";";///< string for semicolon
const QString TRANSLATE_RETURN_VALUE_1      = "\"0\":";///< translator return value
const QString DAILYRUNLOG_FILE_FIRSTLINE    = "FileName: DailyRunLog_";///< Filrst line of the DailyRunLog file
const QString DAILYRUNLOG_FILENAME          = "DailyRunLog_";///< DailyRunLog file name
const QString FILEEXTENSION_LOG             = ".log";///< Extension for the log files
const QString STRING_DAILYRUN               = "DailyRun";///< String for DailyRunLog
const QString MULTIPLEFILES                 = "*.log";///< Multiple log files string
const QString COMMAND_RMDIR                 = "rmdir "; ///< constant string for the command 'rmdir'
const QString COMMAND_ARGUMENT_C            = "-c"; ///< constant string for the command argument for shell '-c'
const QString COMMAND_ARGUMENT_R            = "-rf"; ///< constant string for the command argument for recursive files '-r'
const QString COMMAND_RM                    = "rm "; ///< constant string for the command 'rm'

// event type translations
const QString STRING_RESOLVED               = "Resolved:"; ///< string for resolved
const QString STRING_ACKNOWLEDGED           = "Acknowledged by user:"; ///< string for acknowledged
// constants for wildcharacters
const QString WILDCHAR_ASTRIK               = "*"; ///< constant for wild char
const QString DIRECTORY_SH                  = "/bin/sh"; ///< constant for the shell directory
const QString STRING_SPACE                  = " "; ///< constant string for space

const qint32 EVENTSTRING_TIMESTAMP          = 0; ///< event log timestamp number if the event is splitted
const qint32 EVENTSTRING_EVENTID            = 1; ///< event log event ID number if the event is splitted
const qint32 EVENTSTRING_EVENTTYPE          = 2; ///< event log event type number if the event is splitted
const qint32 EVENTSTRING_STRINGID           = 3; ///< enent log string id number
const qint32 EVENTSTRING_EVENTSTRING        = 4; ///< event log event string number if the event is splitted
const qint32 EVENTSTRING_USERLOG            = 5; ///< event log user log flag number if the event is splitted
const qint32 EVENTSTRING_ALTERNATETEXT      = 6; ///< event log alternate text flag number if the event is splitted
const qint32 EVENTSTRING_PARAMETERS         = 7; ///< event log parameters number if the event is splitted


/****************************************************************************/
DayLogFileInformation::DayLogFileInformation(QString FilePath, QString DailyRunLogPath, QString FileNamePrefix) :
    m_LogFilePath(FilePath), m_DailyRunLogPath(DailyRunLogPath), m_FileNamePrefix(FileNamePrefix) {

    // Store the events in the list so that these can be tranlsated in other languages
    m_ListOfEventIds << Global::EVENT_GLOBAL_STRING_ID_RESOLVED
                     << Global::EVENT_GLOBAL_STRING_ID_ACKNOWLEDGED;


    /// These events are more like concatenation of a string e.g "User acknowledged by pressing button". Here the button
    /// name should be placed.
    /// So these events also needs to be translated. These are stored in the below list
    m_ListOfBtnEventIds << EVENT_USER_ACK_BUTTON_OK
                        << EVENT_USER_ACK_BUTTON_CANCEL
                        << EVENT_USER_ACK_BUTTON_YES
                        << EVENT_USER_ACK_BUTTON_NO
                        << EVENT_USER_ACK_BUTTON_CONTINUE
                        << EVENT_USER_ACK_BUTTON_STOP;


    // store the event ids which needs to be translated when the daily run log file is created
    // these events are required when-ever the mode is changed for Water type, RMS state, Oven start mode
    // and Heated cuvvette mode.
    /// These events are more like arguments for e.g "Mode is changed to %1". Here "%1" is a argument
    /// This "%1" will be replaced with english string. So for the translated file it should not be english
    /// So these events also needs to be translated. These are stored in the below list
    m_EventIDs << Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED
               << Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CHANGED
               << Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED
               << Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_MODE
               << Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_TEMPERATURE
               << Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC;
}

/****************************************************************************/
void DayLogFileInformation::ReadAndTranslateTheFile(const QString &FileName, const QByteArray &ByteArray) {

    QByteArray& FileData = const_cast<QByteArray&>(ByteArray);
    QFile LogFile(FileName);
    // check the file existence
    if (!LogFile.open(QIODevice::ReadOnly)) {
        return;
    }
    // read entire file
    while (!LogFile.atEnd()) {
        QString ReadData = QString::fromUtf8(LogFile.readLine());
        if (ReadData.contains(STRING_SEMICOLON)) {
            if (ReadData.split(DELIMITER_SEMICOLON).count() > EVENTSTRING_USERLOG) {
                if (QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_USERLOG)).compare(FLAG_VALUE) == 0) {

                    Global::tTranslatableStringList TranslateStringList;
                    // read all the parameters
                    for (int Counter = EVENTSTRING_PARAMETERS; Counter < ReadData.split(DELIMITER_SEMICOLON).count();
                         Counter++) {
                        if (QString(ReadData.split(DELIMITER_SEMICOLON).value(Counter)).
                                compare(STRING_NEWLINE) != 0) {
                            QString StrPar = ReadData.split(DELIMITER_SEMICOLON).value(Counter);
                            if(!StrPar.startsWith("##"))// plain string
                            {
                                TranslateStringList << Global::TranslatableString(StrPar);
                            }
                            else //need to be translated
                            {
                                TranslateStringList << Global::TranslatableString(StrPar.mid(2).toUInt());//chop out ##
                            }
                        }
                    }
                    // used for alternate text
                    bool UseAlternateText = false;
                    if (QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_ALTERNATETEXT)).
                            compare(FLAG_VALUE) == 0) {
                        UseAlternateText = true;
                    }
                    // translate the parameters
//                    if (m_EventIDs.contains(QString(ReadData.split(DELIMITER_SEMICOLON).
//                                                    value(EVENTSTRING_EVENTID)).toUInt())) {
//                        TranslateTheParameters(QString(ReadData.split(DELIMITER_SEMICOLON).
//                                                       value(EVENTSTRING_EVENTID)).toUInt(), TranslateStringList);
//                    }

                    // translate the data
                    QString EventData = Global::EventTranslator::TranslatorInstance().Translate
                            (Global::TranslatableString(QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_STRINGID)).toInt(),
                                                        TranslateStringList), UseAlternateText);

                    if (EventData.isEmpty() || EventData.contains(TRANSLATE_RETURN_VALUE_1) ||
                          EventData.compare("\"" + QString(ReadData.split(DELIMITER_SEMICOLON).
                                                           value(EVENTSTRING_EVENTID)) +"\":") == 0 ||
                          EventData.compare(QString(ReadData.split(DELIMITER_SEMICOLON).
                                                             value(EVENTSTRING_EVENTID))) == 0) {

                        // get the event string from the file
                        EventData = ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTSTRING);

                        // raise the event if the event id does not exist in the event string file
                        Global::EventObject::Instance().RaiseEvent
                                (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                                 Global::FmtArgs() << ReadData.split(DELIMITER_SEMICOLON).value
                                 (EVENTSTRING_EVENTID), true);
                    }

                    // check for the non active text
                    QString NonActiveEventText = ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTSTRING);
                    if (FindAndTranslateNonActiveText(NonActiveEventText)) {
                        // append the translated non active text
                        EventData = NonActiveEventText + EventData;
                    }

                    // translate the event type
                    QString EventType = ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTTYPE);
                    TranslateEventType(EventType);

                    // join the required data
                    ReadData = QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_TIMESTAMP))
                               + STRING_SEMICOLON +
                               QString(ReadData.split(DELIMITER_SEMICOLON).value(EVENTSTRING_EVENTID))
                               + STRING_SEMICOLON + EventType + STRING_SEMICOLON + EventData + STRING_NEWLINE;
                    FileData.append(ReadData.toUtf8());
                    FileData.append(STRING_NEWLINE.toUtf8());
                }
            }
        }
        else {
            if (ReadData != STRING_NEWLINE) {
                if (ReadData.contains(m_FileNamePrefix)) {
                    ReadData = DAILYRUNLOG_FILE_FIRSTLINE +
                            ReadData.split(DELIMITER_UNDERSCORE).value(2) + STRING_NEWLINE;
                }
                // append the data
                FileData.append(ReadData.toUtf8());
                FileData.append(STRING_NEWLINE.toUtf8());
            }
        }
    }
    LogFile.close();
}


/****************************************************************************/
void DayLogFileInformation::TranslateTheParameters(const quint32 EventID,
                                                   const Global::tTranslatableStringList &TranslateStringList) {

    Global::tTranslatableStringList& StringList = const_cast<Global::tTranslatableStringList&>(TranslateStringList);
    QStringList ArgumentList;
    QStringList EventTranslatorValues;
    QList<quint32> EventValues;

    EventTranslatorValues.clear();
    // store the arguments in the string list
    for (qint32 Counter = 0; Counter < TranslateStringList.count(); Counter++) {
        Global::TranslatableString TranslateString = StringList.value(Counter);
        ArgumentList << TranslateString.GetString();
    }
    StringList.clear();
    // take which arguments needs to be translated
    switch(EventID) {
        case Global::EVENT_GLOBAL_USER_ACTIVITY_US_RMS_STATE_CHANGED:
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_ON);
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF);

            EventValues << Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_ON
                        << Global::EVENT_GLOBAL_USER_ACTIVITY_STATE_CHANGED_OFF;

            break;
        case Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED:
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_DISTILLED);
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_TAP);

            EventValues << Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_DISTILLED
                        << Global::EVENT_GLOBAL_USER_ACTIVITY_US_WATER_TYPE_CHANGED_TAP;
            break;
        case Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED:
        case Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_MODE:
        case Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC_TEMPERATURE:
        case Global::EVENT_GLOBAL_USER_ACTIVITY_REAGENT_HC:
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PERMANENT);
            EventTranslatorValues << Global::EventTranslator::TranslatorInstance().
                                     Translate(Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PROGSTART);

            EventValues << Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PERMANENT
                        << Global::EVENT_GLOBAL_USER_ACTIVITY_US_OVEN_START_MODE_CHANGED_PROGSTART;
            break;
    }
    // update the argument list
    foreach (QString Name, ArgumentList) {
        // update if the translator is having id other wise keep english text
        if (CheckAndTranslateTheEventID(Name, EventValues, EventTranslatorValues)) {
            StringList << Name;
        }
        else {
            StringList << Name;
        }
    }
}


/****************************************************************************/
bool DayLogFileInformation::CheckAndTranslateTheEventID(const QString &TranslatedString,
                                                        const QList<quint32> &EventIDList,
                                                        const QStringList &StringList) {
    QString EventType;
    // check the event type exist in the list- if it available then translate it
    if (StringList.contains(TranslatedString)) {
        qint32 Index = StringList.indexOf(TranslatedString);
        EventType = Global::UITranslator::TranslatorInstance().Translate(EventIDList.value(Index));
        QString Value = "\"" + QString::number(EventIDList.value(Index)) +"\":";
        // did not find the event type in the translator
        if (EventType.isEmpty() || EventType.contains(TRANSLATE_RETURN_VALUE_1) ||
              EventType.compare(Value) == 0 ||
              EventType.compare(QString::number(EventIDList.value(Index))) == 0) {
            Global::EventObject::Instance().RaiseEvent
                    (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                     Global::FmtArgs() << EventIDList.value(Index), true);
            // event ID does not exist, so we will keep the same string
            return false;
        }
    }
    else {
        // string does not exist, so we will keep the same string
        return false;
    }
    // update the string
    const_cast<QString&>(TranslatedString) = EventType;
    return true;
}

/****************************************************************************/
void DayLogFileInformation::TranslateEventType(const QString &TranslatedString) {
    // translate the event type also
    QStringList EventTypeList;
    QList<quint32> EventTypeIDList;
    QString EventType;

    // first store all the events in the string list
    EventTypeList << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED)
                  << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR)
                  << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR)
                  << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING)
                  << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO)
                  << Global::EventTranslator::TranslatorInstance().TranslateToLanguage(QLocale::English,Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG);
    // store all the event Ids in the list
    EventTypeIDList << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED
                    << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR
                    << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR
                    << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING
                    << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO
                    << Global::EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG;

    // check the event type exist in the list- if it available then translate it
    if (EventTypeList.contains(TranslatedString)) {
        qint32 Index = EventTypeList.indexOf(TranslatedString);
        EventType = Global::EventTranslator::TranslatorInstance().Translate(EventTypeIDList.value(Index));
        QString Value = "\"" + QString::number(EventTypeIDList.value(Index)) +"\":";
        // did not find the event type in the translator
        if (EventType.isEmpty() || EventType.contains(TRANSLATE_RETURN_VALUE_1) ||
              EventType.compare(Value) == 0 ||
              EventType.compare(QString::number(EventTypeIDList.value(Index))) == 0) {
            Global::EventObject::Instance().RaiseEvent
                    (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                     Global::FmtArgs() << EventTypeIDList.value(Index), true);
            // event ID does not exist, so we will keep the same string
            return;
        }
    }
    else {
        // string does not exist, so we will keep the same string
        return;
    }

    const_cast<QString&>(TranslatedString) = EventType;

}

/****************************************************************************/
bool DayLogFileInformation::FindAndTranslateNonActiveText(const QString &TranslatedString) {

    // check the starting value
    foreach (quint32 EventIdValue, m_ListOfEventIds) {
        QString Value = "\"" + QString::number(EventIdValue) +"\":";

        // get the english text
        QString NonActiveEventText = Global::EventTranslator::TranslatorInstance().Translate(EventIdValue);

        if (!NonActiveEventText.isEmpty() && TranslatedString.startsWith(NonActiveEventText)) {
            // get the tranlsated text for the current language
            QString NonActiveEventTranslatedText = Global::UITranslator::TranslatorInstance().Translate(EventIdValue);
            // did not find the event type in the translator
            if (NonActiveEventTranslatedText.isEmpty() || NonActiveEventTranslatedText.contains(TRANSLATE_RETURN_VALUE_1) ||
                  NonActiveEventTranslatedText.compare(Value) == 0 ||
                  NonActiveEventTranslatedText.compare(QString::number(EventIdValue)) == 0) {
                // raise the event if event not available and log it
                Global::EventObject::Instance().RaiseEvent
                        (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                         Global::FmtArgs() << EventIdValue, true);
                // if the event ID does not exist then we retain english text
                NonActiveEventTranslatedText = NonActiveEventText;
            }

            // if the string consists of "User acknowleged by pressing button" then string needs to append the
            // button value to the string
            if (Global::EventTranslator::TranslatorInstance().Translate(
                        Global::EVENT_GLOBAL_STRING_ID_ACKNOWLEDGED).compare(NonActiveEventText) == 0) {
                QString ButtonValue = TranslatedString.mid(NonActiveEventText.length()).trimmed().split(DELIMITER_COLON).value(0);
                // check the starting value
                foreach (quint32 BtnEventIdValue, m_ListOfBtnEventIds) {
                    // get the english string for the button value
                    QString BtnName = Global::EventTranslator::TranslatorInstance().Translate(BtnEventIdValue);
                    if (BtnName.compare(ButtonValue) == 0) {
                        QString Value = "\"" + QString::number(EventIdValue) +"\":";
                        QString TranlatedButtonText = Global::UITranslator::TranslatorInstance().
                                Translate(BtnEventIdValue);
                        // did not find the event type in the translator
                        if (TranlatedButtonText.isEmpty() || TranlatedButtonText.contains(TRANSLATE_RETURN_VALUE_1) ||
                              TranlatedButtonText.compare(Value) == 0 ||
                              TranlatedButtonText.compare(QString::number(BtnEventIdValue)) == 0) {
                            // raise the event if event not available and log it
                            Global::EventObject::Instance().RaiseEvent
                                    (EVENT_DATALOGGING_ERROR_EVENT_ID_NOT_EXISTS,
                                     Global::FmtArgs() << BtnEventIdValue, true);
                            // if the event ID does not exist then we retain english text
                            TranlatedButtonText = BtnName;
                        }
                        NonActiveEventTranslatedText += STRING_SPACE + TranlatedButtonText + DELIMITER_COLON + STRING_SPACE;
                        break;
                    }
                }
                const_cast<QString&>(TranslatedString) = NonActiveEventTranslatedText;

                return true;

            }
        }
    }

    return false;
}


/****************************************************************************/
void DayLogFileInformation::CreateAndListDailyRunLogFileName(const QStringList &FileNames) {
    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);

    QDir LogDirectory(m_LogFilePath);
    // iterate each file and change the name to daily run log file
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << (m_FileNamePrefix + MULTIPLEFILES))) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (!LogFileName.contains(EVENTLOG_TEMP_FILE_NAME_SUFFIX)) {
            // get the date time value from the event log file name
            QString DateAndTimeValue = LogFileName.split(DELIMITER_UNDERSCORE).value
                    (LogFileName.split(DELIMITER_UNDERSCORE).count() - 1);
            // replace the .log extension and put the empty string
            ListOfFile.append(DAILYRUNLOG_FILENAME + DateAndTimeValue.replace(FILEEXTENSION_LOG, EMPTY_STRING));
        }
    }




    qSort(ListOfFile.begin(), ListOfFile.end(), qGreater<QString>());
}


/****************************************************************************/
void DayLogFileInformation::CreateSpecificDailyRunLogFile(const QString &FileName,
                                   const QByteArray &FileContent) {

    QDir LogDirectory(m_LogFilePath);
    QByteArray& FileData = const_cast<QByteArray&>(FileContent);
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << (m_FileNamePrefix + MULTIPLEFILES))) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (LogFileName.contains(FileName.split(DELIMITER_UNDERSCORE).value
                                 (FileName.split(DELIMITER_UNDERSCORE).count() - 1))) {
            ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, FileData);
            break;
        }
    }
}

/****************************************************************************/
void DayLogFileInformation::CreateDailyRunLogFiles(const QStringList &FileNames) {

    // removes the constant cast
    QStringList& ListOfFile = const_cast<QStringList&>(FileNames);
    // set the current directory as log files
    QDir LogDirectory(m_LogFilePath);    

    static_cast<void>(QProcess::execute("mkdir " + m_DailyRunLogPath + QDir::separator() + STRING_DAILYRUN)); // to avoid lint-534

    QByteArray FileData;
    // iterate each file and find whcih event log file it is referring to it
    foreach (QString LogFileName, LogDirectory.entryList(QStringList() << (m_FileNamePrefix + MULTIPLEFILES))) {
        FileData.clear();
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        // get the date time value from the event log file name
        QString DateAndTimeValue = LogFileName.split(DELIMITER_UNDERSCORE).value
                (LogFileName.split(DELIMITER_UNDERSCORE).count() - 1);

        QString FullPatheOfTheFile = m_DailyRunLogPath + QDir::separator() + STRING_DAILYRUN
                + QDir::separator() + DAILYRUNLOG_FILENAME + DateAndTimeValue;

        // replace the .log extension and put the empty string
        ListOfFile.append(FullPatheOfTheFile);
        // translate the files
        ReadAndTranslateTheFile(m_LogFilePath + QDir::separator() + LogFileName, FileData);
        if (FileData.count() > 0) {
            // create the daily run log file
            QFile DailyRunLogFile(FullPatheOfTheFile);
            if (DailyRunLogFile.open(QIODevice::WriteOnly)) {
                qint64 LengthOfBytesWritten = 0;
                // write the data to the file
                while (LengthOfBytesWritten != FileData.count()) {
                   LengthOfBytesWritten = DailyRunLogFile.write(FileData.mid(LengthOfBytesWritten));
                }
            }
            else {
                Global::EventObject::Instance().RaiseEvent(EVENT_DATALOGGING_ERROR_FILE_NOT_OPEN);
                return;
            }
            DailyRunLogFile.close();
        }
    }    
}

}
