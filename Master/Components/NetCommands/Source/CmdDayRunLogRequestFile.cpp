/****************************************************************************/
/*! \file CmdDayRunLogRequestFile.cpp
 *
 *  \brief CmdDayRunLogRequestFile command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.11.2012
 *   $Author:  $ Raju
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

#include <NetCommands/Include/CmdDayRunLogRequestFile.h>

namespace NetCommands {

QString CmdDayRunLogRequestFile::NAME = "NetCommands::CmdDayRunLogRequestFile";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout         Timeout for command.
 * \param[in]   FileName        name of the file.
 * \param[in]   LanguageName    name of the current lanaguage.
 */
/****************************************************************************/
CmdDayRunLogRequestFile::CmdDayRunLogRequestFile(int Timeout, const QString &FileName, const QString &LanguageName) :
    Command(Timeout),
    m_FileName(FileName),
    m_LanguageName(LanguageName)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdDayRunLogRequestFile::CmdDayRunLogRequestFile() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDayRunLogRequestFile::~CmdDayRunLogRequestFile()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDayRunLogRequestFile::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns file name
 *
 *  \return  Name of the file
 */
/****************************************************************************/
QString CmdDayRunLogRequestFile::GetFileName() const
{
    return m_FileName;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the current language of the system
 *
 *  \return  Name of the language
 */
/****************************************************************************/
QString CmdDayRunLogRequestFile::GetLanguageName() const
{
    return m_LanguageName;
}

} // end namespace NetCommands
