/****************************************************************************/
/*! \file Global/Include/Commands/CmdPowerFail.h
 *
 *  \brief Definition file for class CmdPowerFail.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-04-28
 *  $Author:    $ J.Bugariu
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

//lint -esym(1511, Global::Command::CopyFrom)
//  No polymorphic behaviour desired at this point

#ifndef GLOBAL_CMDPOWERFAIL_H
#define GLOBAL_CMDPOWERFAIL_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>

//lint-sem(Global::CmdPowerFail::CopyFrom,initializer)

namespace Global {

/****************************************************************************/
/**
 * \brief Class for telling that power will fail shortly.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdPowerFail : public Command {
    friend class TestCommands;
private:
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdPowerFail &rOther);
protected:
public:
    static QString  NAME;                   ///< Command name.
    PowerFailStages m_PowerFailStage;      ///< Power Fail stage

    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CmdPowerFail(PowerFailStages);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdPowerFail(const CmdPowerFail &rOther);
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdPowerFail & operator = (const CmdPowerFail &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdPowerFail();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
}; // end class CmdPowerFail

} // end namespace Global

#endif // GLOBAL_CMDPOWERFAIL_H
