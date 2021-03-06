/****************************************************************************/
/*! \file Export/Components/ExportData/Include/ExportData.h
 *
 *  \brief Include file for ExportData class.
 *
 *  $Version:   $ 1.0, 2.0
 *  $Date:      $ 2012-07-12, 2013-02-05
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
#ifndef EXPORT_EXPORTDATA_H
#define EXPORT_EXPORTDATA_H

#include "EncryptionDecryption/WriteArchive/Include/WriteArchive.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfiguration.h"
#include "DataManager/Containers/ExportConfiguration/Include/ExportConfigurationVerifier.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

//lint -e429


namespace Export {

typedef QHash<QString, QStringList> FileList_t; ///< to store the archive file name and related file list to archive
/****************************************************************************/
/*!
 * \brief This class handles the archiving the files.
 */
/****************************************************************************/
class CExportData {
private:
    FileList_t m_PairList; ///< pair list for the strings    
    QStringList m_CreatedFileList; ///< to store created files in a list

    /****************************************************************************/
    /*!
     *  \brief Copy Constructor
     */
    /****************************************************************************/
    CExportData(const CExportData &);

    /****************************************************************************/
    /*!
     *  \brief Assignment Operator which copies from rhs to lhs.
     */
    /****************************************************************************/
    const CExportData & operator = (const CExportData &);

    int StartPackTheFiles(const DataManager::CExportConfiguration &ExportConfiguration);

    int WriteZipFile(const DataManager::CExportConfiguration &ExportConfiguration,
                      const QString &KeyName, const QString &DateValue);

    int WritelpkgFile(const DataManager::CExportConfiguration &ExportConfiguration,
                      const QString &KeyName, const QString &DateValue);

    int WriteArchiveFile(const QString &KeyName, const QList<QByteArray> &Files,
                                   const bool &Encryption, const bool &Compressed);

    void RemoveFiles();
    bool CheckUSBSpace(QString Destination);

public:
    CExportData();

    int CreateArchiveFiles();

};

} // end namespace Export


#endif // EXPORT_EXPORTDATA_H
