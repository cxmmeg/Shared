/****************************************************************************/
/*! \file CmdExportDayRunLogRequest.h
 *
 *  \brief CmdExportDayRunLogRequest command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.12.2012
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

#ifndef NETCOMMANDS_CMDEXPORTDAYRUNLOGREQUEST_H
#define NETCOMMANDS_CMDEXPORTDAYRUNLOGREQUEST_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdExportDayRunLogRequest command.
 *
 */
/****************************************************************************/
class CmdExportDayRunLogRequest : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdExportDayRunLogRequest &);
    friend QDataStream & operator >> (QDataStream &, CmdExportDayRunLogRequest &);
public:
    static QString NAME;    ///< Command name.
    CmdExportDayRunLogRequest(int Timeout, QString Path);
    CmdExportDayRunLogRequest();
    ~CmdExportDayRunLogRequest();
    virtual QString GetName() const;
    QString GetFolderPath() const;
private:
    QString m_FolderPath;               ///< Folder path
    CmdExportDayRunLogRequest(const CmdExportDayRunLogRequest &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdExportDayRunLogRequest & operator = (const CmdExportDayRunLogRequest &);   ///< Not implemented.
}; // end class CmdExportDayRunLogRequest

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdExportDayRunLogRequest &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << Cmd.m_FolderPath;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdExportDayRunLogRequest &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_FolderPath;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDEXPORTDAYRUNLOGREQUEST_H
