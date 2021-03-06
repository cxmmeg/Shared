/****************************************************************************/
/*! \file ErrorMsgDlg.h
 *
 *  \brief Header file for class CErrorMsgDlg.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-29
 *   $Author:  $ Bhanu Prasad H
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

#ifndef MAINMENU_ERRORMSGDLG_H
#define MAINMENU_ERRORMSGDLG_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include <QTextEdit>
#include "MainMenu/Include/MsgBoxManager.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"

//lint -e435

namespace MainMenu {

namespace Ui {
    class CErrorMsgDlg;
}

/****************************************************************************/
/**
 * \brief Content panel for dialogs displaying text files.
 */
/****************************************************************************/
class CErrorMsgDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CErrorMsgDlg(QWidget *p_Parent = NULL , QWidget *p_MainWindow = NULL,
                          DataManager::CUserSettingsInterface *p_UserSettingsInterface=NULL);
    virtual ~CErrorMsgDlg();
    void SetCaption(QString Caption);
    void SetText(QStringList Text);
    void SetErrorMsgList();    
    QString GetDateAndTime(QString);

    /****************************************************************************/
    /**
     * \brief Retriving the Event ID and Error Message from Error Message Hash.
     *
     * \iparam  ErrorIDStructList = Hash with Error Id and Error Msg data
     *
     */
    /****************************************************************************/
    void ErrorMsgList(QList <MsgData> ErrorIDStructList);

private:

    Ui::CErrorMsgDlg *mp_Ui;            //!< User interface
    QTextEdit *mp_TextEdit;             //!< Widget displaying a text file
    MainMenu::CMainWindow *mp_MainWidow; //!< Pointer to  main window
    QList <MsgData> m_ErrorMsgList;      //!< List for Error messages
    DataManager::CUserSettingsInterface *mp_SettingsInterface;  //!< UserSettings Interface
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CErrorMsgDlg)

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void PopUp();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void RetranslateUI();
protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_ERRORMSGDLG_H
