/****************************************************************************/
/*! \file TableMoving.h
 *
 *  \brief Header file for class CTableMoving.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-23
 *   $Author:  $ M.Scherer
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

#ifndef MAINMENU_TABLEMOVING_H
#define MAINMENU_TABLEMOVING_H

#include <QAbstractTableModel>
#include <QGroupBox>

namespace MainMenu {

namespace Ui {
    class CTableMoving;
}

/****************************************************************************/
/**
 * \brief A set of buttons for the movement of rows in table views
 */
/****************************************************************************/
class CTableMoving : public QGroupBox
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CTableMoving(QWidget *p_Parent = 0);
    virtual ~CTableMoving();
    void SetTitle(qint32 StringLength, QString TitleText);

private:
    Ui::CTableMoving *mp_Ui;    //!< User interface
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CTableMoving)

protected:
    void changeEvent(QEvent *p_Event);

signals:
    /****************************************************************************/
    /*!
     *  \brief Move the row to the beginning of the table
     */
    /****************************************************************************/
    void OnBeginButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the item one row up
     */
    /****************************************************************************/
    void OnUpButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the item one row down
     */
    /****************************************************************************/
    void OnDownButtonClicked();
    /****************************************************************************/
    /*!
     *  \brief Move the row to the end of the table
     */
    /****************************************************************************/
    void OnEndButtonClicked();
};

} // end namespace MainMenu

#endif // MAINMENU_TABLEMOVING_H
