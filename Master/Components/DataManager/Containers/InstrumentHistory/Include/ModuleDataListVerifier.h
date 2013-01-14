/****************************************************************************/
/*! \file ModuleDataListVerifier.h
 *
 *  \brief Definition for CModuleDataListVerifier class.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-08
 *   $Author:  $ Soumya D
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
#ifndef DATAMANAGER_MODULEDATALISTVERIFIER_H
#define DATAMANAGER_MODULEDATALISTVERIFIER_H

#include <QStringList>
#include <QDebug>

#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief  This class implements Module List verifier
 */
/****************************************************************************/
class CModuleDataListVerifier :public IVerifierInterface
{
public:
    CModuleDataListVerifier();
    /****************************************************************************/
    /*!
     *  \brief  Destructor
     */
    /****************************************************************************/
    virtual ~CModuleDataListVerifier() {}

    bool VerifyData(CModuleDataList* p_ModuleDataList);
    ErrorHash_t &GetErrors();

    void ResetLastErrors();

    bool IsLocalVerifier();

private:
    CModuleDataList *mp_MDL;  //!< Module List Container
    ErrorHash_t m_ErrorsHash; //!< To store Error ID and any arguments associated

};

}
#endif // DATAMANAGER_MODULEDATALISTVERIFIER_H