/****************************************************************************/
/*! \file DataLogging/Include/BaseLogger.h
 *
 *  \brief Definition file for class BaseLogger.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2013-10-16
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

#ifndef DATALOGGING_BASELOGGER_H
#define DATALOGGING_BASELOGGER_H

#include <QFile>
#include <QFileInfo>
#include "Global/Include/LoggingSource.h"
#include "Global/Include/AdjustedTime.h"
#include <unistd.h>
namespace Global {
 class EventObject;
}

namespace DataLogging {

/****************************************************************************/
/**
 * \brief Base class for all logger classes.
 *
 * This class encapsulates the base functionality for all logger classes: close
 * and open files for writing, append a line to a file, etc.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class BaseLogger : public QObject {
    Q_OBJECT
friend class TestBaseLogger;
private:
    QFile   m_File;                 ///< File in which logging is done.
    QString m_LoggingSource;        ///< DataLogger
    int     m_FormatVersion;        ///< Format version of data.
    int     m_FlushEventCount;      ///< flush event counter
    int     m_WriteFileEventCount;  ///< write file event counter
    bool    m_LogFileError;         ///< Log file error
    bool    m_IsEventRepeated;      ///< event is repeated in data logging component
    bool    m_RequiredToFlush;      ///< File flush flag
    /****************************************************************************/
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    BaseLogger();                                           ///< Not implemented.

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(BaseLogger)


    /****************************************************************************/
    /**
     * \brief Check whether event is repeated for the temporary file
     *
     */
    /****************************************************************************/
    void CheckEventRepeatingForTempFile();

protected:
    /****************************************************************************/
    /**
     * \brief Get format version.
     *
     * \return  Format version.
     */
    /****************************************************************************/
    inline int GetFormatVersion() const {
        return m_FormatVersion;
    }

    /****************************************************************************/
    /**
     * \brief Get Logging source.
     *
     * \return  Logging Source.
     */
    /****************************************************************************/
    QString GetLoggingSource() {
        return m_LoggingSource;
    }

    /****************************************************************************/
    /**
     * \brief Check if file is open.
     *
     * \return  true if file is open.
     */
    /****************************************************************************/
    inline bool IsLogFileOpen() const {
        return m_File.isOpen();
    }

    /****************************************************************************/
    /**
     * \brief Check if log file is error.
     *
     * \return  true if file is having error.
     */
    /****************************************************************************/
    inline bool IsLogFileError() const {
        return m_LogFileError;
    }

    /****************************************************************************/
    /**
     * \brief Check if event repeated.
     *
     * \return  true if file is having error.
     */
    /****************************************************************************/
    inline bool IsEventRepeated() const {
        return m_IsEventRepeated;
    }

    /****************************************************************************/
    /**
     * \brief Get file size.
     *
     * \return  Current file size as reported by Qt.
     */
    /****************************************************************************/
    inline qint64 GetFileSize() const {
        return m_File.size();
    }

    /****************************************************************************/
    /**
     * \brief Get complete base file name.
     *
     * From the Qt documentation:
     * Returns the complete base name of the file without the path.
     * The complete base name consists of all characters in the file up to
     * (but not including) the last '.' character.
     *
     * Example:
     * For a file called "/tmp/archive.tar.gz" only "archive.tar" will be returned.
     *
     * \return  Complete base file name.
     */
    /****************************************************************************/
    inline QString GetCompleteBaseFileName() const {
        return QFileInfo(m_File).completeBaseName();
    }

    /****************************************************************************/
    /**
     * \brief Get complete file name including the path.
     *
     * From the Qt documentation:
     * Returns an absolute path including the file name.
     * The absolute path name consists of the full path and the file name.
     * On Unix this will always begin with the root, '/', directory. On Windows
     * this will always begin 'D:/' where D is a drive letter, except for network
     * shares that are not mapped to a drive letter, in which case the path will
     * begin '//sharename/'. QFileInfo will uppercase drive letters. Note that
     * QDir does not do this. The code snippet below shows this.
     *
     * Example:
     * For a file called "/tmp/archive.tar.gz" "/tmp/archive.tar.gz" will be returned.
     *
     * \return  Complete file name including path.
     */
    /****************************************************************************/
    inline QString GetCompleteFileName() const {
        return QFileInfo(m_File).absoluteFilePath();
    }

    /****************************************************************************/
    /**
     * \brief Create new output file and open it for writing.
     *
     * Create a new file using m_File. m_File remains open for writing.
     *
     * \iparam   FileName    New file name to use.
     */
    /****************************************************************************/
    void CreateNewFile(const QString &FileName);

    /****************************************************************************/
    /**
     * \brief Remove file.
     *
     * \iparam   FileName    File name.
     */
    /****************************************************************************/
    void RemoveFile(const QString &FileName) const;

    /****************************************************************************/
    /**
     * \brief Opens an existing file for append.
     *
     * m_File is closed, then openend in append mode.
     * If open fails, m_File remains closed.
     *
     * \iparam   FileName    File name.
     */
    /****************************************************************************/
    void OpenFileForAppend(const QString &FileName);

    /****************************************************************************/
    /**
     * \brief Convert a timestamp to string for logging.
     *
     * The format used for conversion is "yyyy-MM-dd hh:mm:ss.zzz".
     * \iparam   TimeStamp   Time stamp to convert into string.
     * \return                  Converted time stamp.
     */
    /****************************************************************************/
    inline QString TimeStampToString(const QDateTime &TimeStamp) const {
        return TimeStamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
    }


    /****************************************************************************/
    /**
     * \brief Get current time stamp as string.
     *
     * The format is suitable for writing the file name. "yy-MM-dd"
     *
     * \return  Current time stamp.
     */
    /****************************************************************************/
    inline QString GetTimeStampFileName() const {
        return Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyyMMdd");
    }


public:
    /****************************************************************************/
    /**
     * \brief Flush the data to disk
     *
     */
    /****************************************************************************/
    void FlushToDisk();

    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pParent             Parent.
     * \iparam   TheLoggingSource    Source to set in log entry.
     * \iparam   FormatVersion       Format version for output file.
     */
    /****************************************************************************/
    BaseLogger(QObject *pParent, const QString &TheLoggingSource, int FormatVersion);

    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * All open files will be closed.
     */
    /****************************************************************************/
    virtual ~BaseLogger() {
    }

    /****************************************************************************/
    /**
     * \brief Append a line to current open log file.
     *
     * Append a line to current open log file. The trailing "\n" is also appended.
     *
     * \iparam   Line           line to append (without trailing "\n")
     * \iparam   FlushData      Flush the data to disk
     */
    /****************************************************************************/
    void AppendLine(QString Line, bool FlushData = true);

    /****************************************************************************/
    /**
     * \brief Get current time stamp as string.
     *
     * The format is suitable for writing the header information. "yyyy-MM-dd hh:mm:ss.zzz"
     *
     * \return  Current time stamp.
     */
    /****************************************************************************/
    inline QString GetTimeStampHeader() const {
        return Global::AdjustedTime::Instance().GetCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    }

    /****************************************************************************/
    /**
     * \brief Close file.
     *
     * If file is open, it will be flushed and then closed. File name will be cleared.
     */
    /****************************************************************************/
    inline void CloseFile()  {
        if(m_File.isOpen()) {
            // we DO NOT NEED the return value of flush
            static_cast<void>(m_File.flush());
            m_File.close();
            (void)fsync(m_File.handle());
        }        
        m_RequiredToFlush = false;        
        m_File.setFileName("");
    }


}; // end class BaseLogger

} // end namespace DataLogging

#endif // DATALOGGING_BASELOGGER_H
