/****************************************************************************/
/*! \file DataContainerBase.h
 *
 *  \brief Definition file for class CDataContainerBase.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-05-20
 *  $Author:    $ Nikhil, Raju
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

#ifndef DATAMANAGER_DATACONTAINERBASE_H
#define DATAMANAGER_DATACONTAINERBASE_H

#include <QString>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "Global/Include/GlobalDefines.h"
#include "TestStubDataManager/Helper/Include/Types.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "Global/Include/EventObject.h"
namespace DataManager {
/****************************************************************************/
/**
 * \brief	Name of the Temporary Data Container File,
 *			Written to this before Overwriting the original file.
 */
/****************************************************************************/
const QString TEMP_CONTAINER_XMLFILE("TempDataContainer.xml");  //!< Name of Temporary File
typedef QList<IVerifierInterface*> ListOfVerifiers_t; ///< List to store the verifiers

/****************************************************************************/
/**
 * \brief	Base class for all the containers
 */
/****************************************************************************/
class CDataContainerBase
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Gets the data container type
     */
    /****************************************************************************/
    virtual DataContainerType_t GetDataContainerType() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Get the file name of the file
     */
    /****************************************************************************/
    virtual QString GetFilename() = 0;

    /****************************************************************************/
    /*!
     *  \brief  Sets the data verification mode
     *
     *  \iparam Value = Verification mode flag value
     *
     */
    /****************************************************************************/
    virtual void SetDataVerificationMode(const bool Value) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CDataContainerBase();

    /*! \warning DON'T implement(override) in derived classes! */
    ListOfErrors_t& GetErrorList();

    /****************************************************************************/
    /*!
     *  \brief  Appends the error hash in the error list
     *
     *  \iparam p_ErrorHash = pointer to error hash
     *
     */
    /****************************************************************************/
    /*! \warning DON'T implement(override) in derived classes! */
    void SetErrorList(ErrorHash_t *p_ErrorHash)
    {
        m_ListOfErrors.append(p_ErrorHash);
    }
    /*! \warning DON'T implement(override) in derived classes! */
    void ResetLastErrors();
    /*! \warning DON'T implement(override) in derived classes! */
    bool AddVerifier(IVerifierInterface* p_Verifier);
    /*! \warning DON'T implement(override) in derived classes! */
    bool DoLocalVerification();
    /*! \warning DON'T implement(override) in derived classes! */
    bool DoGroupVerification();
    /*! \warning DON'T implement(override) in derived classes! */
    bool VerifyData(bool GroupVerification = false, bool VerifyAll = false);

    /*! \warning DON'T implement(override) in derived classes! */
    bool Write(QString Filename);
    /*! \warning DON't implement(override) in derived classes! */
    bool Write();


protected:
    /****************************************************************************/
    /*!
     *  \brief Serializes the xml content
     *
     *  \iparam IODevice = Instance of the IODevice might be Buffer or File
     *  \iparam CompleteData = bool type if true writes Complete data of object
     *
     *  \return On Successfule(True) or not (False)
     */
    /****************************************************************************/
    virtual bool SerializeContent(QIODevice& IODevice, bool CompleteData) = 0;

    /****************************************************************************/
    /*!
     *  \brief  Deserializes the xml content
     *
     *  \iparam IODevice = Instance of the IODevice might be Buffer or File
     *  \iparam CompleteData = bool type if true writes Complete data of object
     *
     *  \return On Successfule(True) or not (False)
     */
    /****************************************************************************/
    virtual bool DeserializeContent(QIODevice& IODevice, bool CompleteData) = 0;    

    bool DoLocalVerification(CDataContainerBase* p_DCB_Verification);
    bool DoGroupVerification(CDataContainerBase* p_DCB_Verification);


    // to read the error list from Xml file
    void ReadErrorList(QXmlStreamReader& XmlStreamReader);
    void WriteErrorList(QXmlStreamWriter& XmlStreamWriter);

    // to read the verifier list from Xml file
    void ReadVerifierList(QXmlStreamReader& XmlStreamReader);
    void WriteVerifierList(QXmlStreamWriter& XmlStreamWriter);

private:
    ListOfErrors_t    m_ListOfErrors;   ///< This will hold Hash table of Error ID's from Container and it's verifiers.
    ListOfVerifiers_t m_VerifierList;   ///< Verifiers applied to the data
};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINERBASE_H
