/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/ContainerBase/Source/DataContainerBase.cpp
 *
 *  \brief Implementation file for common functions for all the containers.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-05-20
 *  $Author:    $ Nikhil, Raju, Ramya GJ
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

#include "DataManager/Containers/ContainerBase/Include/DataContainerBase.h"
#include "Global/Include/GlobalDefines.h"
#include <Global/Include/Exception.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>
#include <unistd.h> //for fsync
#include <QUuid>
//lint -e1536
//lint -e593

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Writes the data from Container to the file
 *  \iparam Filename = Name of the Output File
 *
 *  \return true-write success , false - write failed
 */
/****************************************************************************/
bool CDataContainerBase::Write(QString Filename)
{
    //QReadLocker locker(mp_ReadWriteLock);
    try {
        // if file already exists, delete it
        if (QFile::exists(Filename)) {
            if (!QFile::remove(Filename)) {
                qDebug() << "File remove failed in Write: " << Filename;
                return false;
            }
        }
        // create the file
        QFile File(Filename);
        if (!File.open(QFile::WriteOnly | QFile::Text)) {
            qDebug() << "open file failed in Write: " << Filename;
            return false;
        }
        if (!SerializeContent(File, false)) {
            qDebug() << "### CDataContainerBase::Write failed for file: " << Filename;
            File.close();
            return false;
        }

		(void)File.flush();
		(void)fsync(File.handle());
        File.close();
        //flush data
        const QString MD5sumGenerator = QString("%1%2 %3").arg(Global::SystemPaths::Instance().GetScriptsPath()).
                arg(QString("/EBox-Utils.sh update_md5sum_for_file_in_settings")).arg(GetFilename());
        (void)system(MD5sumGenerator.toStdString().c_str());

        return true;
    }
    CATCHALL();

    qDebug() << "### Exception in CDataContainerBase::Write(QString Filename)";
    // The File closing is not necessary as the QFile destructor closes the File.
    return false;
}

/****************************************************************************/
/*!
 *  \brief Writes the data from Container to the already read file
 *
 *  \return true-write success , false - write failed
 */
/****************************************************************************/
bool CDataContainerBase::Write()
{    
    if (!QString::compare("UNDEFINED" , GetFilename())) {
        qDebug() << "File is not read";
        return false;
    }

    QString tempfile = QUuid::createUuid().toString();
    try {
        QFile File(tempfile);
        if (!File.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
            qDebug() << "open file failed in Write: " << tempfile;
            return false;
        }

        if (!SerializeContent(File, false)) {
            qDebug() << "### CDataContainerBase::Write failed for file: " << tempfile;
            File.close();
            return false;
        }
        // if Old file already exists, delete it
        if (QFile::exists(GetFilename())) {
            if (!QFile::remove(GetFilename())) {
                qDebug() << "File remove failed in Write: " << GetFilename();
                return false;
            }
        }

        File.flush();
        fsync(File.handle());
        File.close();

        if (!QFile::rename(tempfile, GetFilename())) {
            qDebug() << "File renamed failed in Write: " << tempfile;
            return false;
        }
        const QString MD5sumGenerator = QString("%1%2 %3").arg(Global::SystemPaths::Instance().GetScriptsPath()).
                arg(QString("/EBox-Utils.sh update_md5sum_for_file_in_settings")).arg(GetFilename());
        (void)system(MD5sumGenerator.toStdString().c_str());
        return true;
    }
    CATCHALL();

    qDebug() << "### Exception in CDataContainerBase::Write";
    // The File closing is not necessary as the QFile destructor closes the File.
    return false;
}

/****************************************************************************/
/*!
 *  \brief      Get the last error
 *  \warning    Error List is deleted/cleared after retrieval.
 *  \return     List of errors
 */
/****************************************************************************/
ListOfErrors_t& CDataContainerBase::GetErrorList()
{
    return m_ListOfErrors;
}

/****************************************************************************/
/*!
 *  \brief  Reset the error list
 */
/****************************************************************************/
void CDataContainerBase::ResetLastErrors()
{
    //qint32 I = 0;
    while(!m_ListOfErrors.isEmpty()) {
        const_cast<ErrorMap_t *>(m_ListOfErrors.at(0))->clear();
        m_ListOfErrors.removeFirst();
        //I++;
    }
    m_ListOfErrors.clear();
}

/****************************************************************************/
/*!
 *  \brief  Adds a verifier for the rack list
 *
 *  \iparam p_Verifier = The new verifier
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::AddVerifier(IVerifierInterface* p_Verifier)
{
    // add every verifier only once
    if (m_VerifierList.indexOf(p_Verifier) != -1) {
        qDebug() << "### CDataContainerBase::AddVerifier failed: Verifier already exists.";
        return false;
    }
    else {
        m_VerifierList.append(p_Verifier);
        return true;
    }

}

/****************************************************************************/
/*!
 *  \brief  Verifies the data present in the container
 *
 *  \iparam GroupVerification = Flag for the special verification
 *  \iparam VerifyAll = Verifies the local and group verification
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::VerifyData(bool GroupVerification, bool VerifyAll)
{    
    if (!GroupVerification || VerifyAll) {
        if (!DoLocalVerification(this)) {
            qDebug() << "### CDataContainerBase::Local verifiaction failed";
            return false;
        }
    }

    if (GroupVerification || VerifyAll) {
        if (!DoGroupVerification(this)) {
            qDebug() << "### CDataContainerBase::Group verifiaction failed";
            return false;
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Only uses by local verifiers
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::DoLocalVerification()
{
    return DoLocalVerification(this);
}

/****************************************************************************/
/*!
 *  \brief  Only uses by local verifiers
 *
 *  \iparam p_DCB_Verification = Pointer to a CDataContainerBase class
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::DoLocalVerification(CDataContainerBase* p_DCB_Verification)
{
    bool VerifierResult = true;
    for(int ListCount = 0; ListCount < m_VerifierList.count(); ListCount++) {
        IVerifierInterface* p_Verifier = m_VerifierList.value(ListCount, NULL);
        if (p_Verifier != NULL) {
            if (p_Verifier->IsLocalVerifier()) {
                // clear the error before verifying the data
                p_Verifier->ResetErrors();
                // verify the data
                if (!p_Verifier->VerifyData(p_DCB_Verification)) {
                    VerifierResult = false;
                    // append the string list with verifier error list
                    m_ListOfErrors.append(&(p_Verifier->GetErrors()));
                    break;
                }
            }
        }
    }
    return VerifierResult;
}

/****************************************************************************/
/*!
 *  \brief  Only uses by Group verifiers
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::DoGroupVerification()
{
    return DoGroupVerification(this);
}

/****************************************************************************/
/*!
 *  \brief  Only uses by Group verifiers
 *
 *  \iparam p_DCB_Verification = Pointer to a CDataContainerBase class
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDataContainerBase::DoGroupVerification(CDataContainerBase* p_DCB_Verification)
{
    bool VerifierResult = true;
    for(int ListCount = 0; ListCount < m_VerifierList.count(); ListCount++) {
        IVerifierInterface* p_Verifier = m_VerifierList.value(ListCount, NULL);
        if (p_Verifier != NULL) {
            if (!p_Verifier->IsLocalVerifier()) {
                // clear the error before verifying the data
                p_Verifier->ResetErrors();
                // verify the data
                if (!p_Verifier->VerifyData(p_DCB_Verification)) {
                    VerifierResult = false;
                    // append the string list with verifier error list
                    m_ListOfErrors.append(&(p_Verifier->GetErrors()));
                }
            }
        }
    }
    return VerifierResult;
}


/****************************************************************************/
/*!
 *  \brief  Write the last errors in the Xml file
 *
 *  \iparam XmlStreamWriter = Xml write to write the XML contents
 */
/****************************************************************************/
void CDataContainerBase::WriteErrorList(QXmlStreamWriter& XmlStreamWriter)
{
    Q_UNUSED(XmlStreamWriter);
//    if (m_LastErrors.count() > 0) {
//        // start the special tag for error list to store
//        XmlStreamWriter.writeStartElement("ErrorData");
//        for (int ErrorListCount = 0; ErrorListCount < m_LastErrors.count(); ErrorListCount++) {
//            // start of the error string
//            XmlStreamWriter.writeStartElement("Error");
//            // write the description of the error string
//            XmlStreamWriter.writeAttribute("Description", m_LastErrors.value(ErrorListCount));
//            // end of the error string
//            XmlStreamWriter.writeEndElement();
//        }
//        // end element for the error data
//        XmlStreamWriter.writeEndElement();
//    }
}

/****************************************************************************/
/*!
 *  \brief  Read the last error list from the Xml file
 *
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
 */
/****************************************************************************/
void CDataContainerBase::ReadErrorList(QXmlStreamReader& XmlStreamReader)
{
    Q_UNUSED(XmlStreamReader)
    // clear the error list and start add the data which are read from the XML file
}

/****************************************************************************/
/*!
 *  \brief  Write the verifier list in the Xml file
 *
 *  \iparam XmlStreamWriter = Xml write to write the XML contents
 */
/****************************************************************************/
void CDataContainerBase::WriteVerifierList(QXmlStreamWriter& XmlStreamWriter)
{
    // store the verifier count
    XmlStreamWriter.writeAttribute("VerifierCount", QString::number(m_VerifierList.count()));
    for (qint32 I = 0; I < m_VerifierList.count(); I++) {
        QString VerifierPtr;
        // make the text stream input as string
        QTextStream VerifierPointer(&VerifierPtr);
        // get the verifier list pointer address
        VerifierPointer << m_VerifierList.at(I);
        qDebug() << "\n\n Real Verifier Pointer" << m_VerifierList.at(I);
        XmlStreamWriter.writeAttribute(QString("Verifier%1Pointer").arg(I + 1), VerifierPointer.readAll());
    }
}

/****************************************************************************/
/*!
 *  \brief  Read the verifier list from the Xml file
 *
 *  \iparam XmlStreamReader = Xml reader to read the XML contents
 */
/****************************************************************************/
void CDataContainerBase::ReadVerifierList(QXmlStreamReader& XmlStreamReader)
{
    // get the verifier count
    int VerifierCount = XmlStreamReader.attributes().value("VerifierCount").toString().toInt();
    // clear the list
    m_VerifierList.clear();

    // start adding the verifiers
    for (qint32 I = 0; I < VerifierCount; I++) {
        if (!XmlStreamReader.attributes().hasAttribute(QString("Verifier%1Pointer").arg(I + 1))) {
            //            qDebug() << "### attribute <VerifierPointer> is missing";
            //            return false;
        }
        // get the verifier interface address from the XML file
        IVerifierInterface* p_VerifierInterface = reinterpret_cast<IVerifierInterface*>(XmlStreamReader.attributes().
                                                                                        value(QString("Verifier%1Pointer").
                                                                                              arg(I + 1)).toString().toInt(0, 16));
        qDebug() << "\n\n Verifier Pointer" << p_VerifierInterface;
        // append the verifier
        m_VerifierList.append(p_VerifierInterface);
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
CDataContainerBase::~CDataContainerBase()
{
}

} // end of namespace datamanager

