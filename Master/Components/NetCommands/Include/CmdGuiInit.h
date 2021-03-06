/****************************************************************************/
/*! \file CmdGuiInit.h
 *
 *  \brief CmdGuiInit command definition
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.12.2012
 *   $Author:  $ N.Kamath
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
#ifndef CMDGUIINIT_H
#define CMDGUIINIT_H
#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGuiInit
 *
 */
/****************************************************************************/
class CmdGuiInit : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdGuiInit &);
    friend QDataStream & operator >> (QDataStream &, CmdGuiInit &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdGuiInit(int Timeout, const bool InitStatus);
    CmdGuiInit();
    ~CmdGuiInit();
    virtual QString GetName() const;
    bool m_InitStatus; //!< True if GUI Init complete

private:
    CmdGuiInit(const CmdGuiInit &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdGuiInit & operator = (const CmdGuiInit &); ///< Not implemented.

}; // end class CmdGuiInit

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdGuiInit &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_InitStatus;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdGuiInit &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_InitStatus;
    return Stream;
}

} // end namespace NetCommands

#endif // CMDGUIINIT_H
