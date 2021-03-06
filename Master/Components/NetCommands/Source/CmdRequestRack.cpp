/****************************************************************************/
/*! \file CmdRequestRack.cpp
 *
 *  \brief CmdRequestRack command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-02
 *   $Author:  $ C.Adaragunchi
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

#include <NetCommands/Include/CmdRequestRack.h>


namespace NetCommands {

QString CmdRequestRack::NAME = "NetCommands::CmdRequestRack";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                 Timeout for command.
 */
/****************************************************************************/
CmdRequestRack::CmdRequestRack(int Timeout) :
    Command(Timeout)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRequestRack::CmdRequestRack() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRequestRack::~CmdRequestRack()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRequestRack::GetName() const
{
    return NAME;
}

} // end namespace NetCommmands
