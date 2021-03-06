/****************************************************************************/
/*! \file MainWindow.h
 *
 *  \brief Header file for class CMainWindow.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#ifndef MAINMENU_MAINWINDOW_H
#define MAINMENU_MAINWINDOW_H

#include "Global/Include/Utils.h"
#include "MainMenu/Include/WaitDialog.h"
#include <QDateTime>
#include <QMainWindow>
#include <QTimer>
#include <QLabel>

//lint -e578
namespace MainMenu {

namespace Ui {
    class CMainWindow;
}

/****************************************************************************/
/**
 * \brief Tests cases for the left and right gripper arm.
 */
/****************************************************************************/
class CMainWindow : public QMainWindow
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    //!< The role of the GUI user
    typedef enum {
        Admin,
        Operator,
        Service
    } UserRole_t; 

    //!<
    /****************************************************************************/
    /*!
     *  \brief Enum for displaying status bar icons
     *
     */
    /****************************************************************************/
    typedef enum {
        ProcessRunning = 1,
        RemoteCare = 2,
        Warning = 4,
        Error = 8
    } Status_t;

    explicit CMainWindow(QWidget *p_Parent = 0);
    virtual ~CMainWindow();

    void AddMenuGroup(QWidget *p_MenuGroup, QString Label);
    void AddMenuGroup(QWidget *p_MenuGroup, QPixmap Pixmap);
    QWidget *GetCurrentGroup();
    void SetVerticalOrientation(bool Vertical);
    void SetUserIcon(UserRole_t UserRole);
    bool SetStatusIcons(Status_t Status);
    bool UnsetStatusIcons(Status_t Status);
    void SetTabWidgetIndex();
    void SetTabWidgetIndex(int Index);
    void SetTabEnabled(bool Status);
    void SetTabEnabled(int Index, bool Status);
    void SetUserMode(QString Mode);

    /****************************************************************************/
    /*!
     *  \brief This function sets the User Role
     *
     *  \iparam UserRole = Type of user role
     */
    /****************************************************************************/
    void SetUserRole(UserRole_t UserRole)
    {
        m_CurrentUserRole = UserRole;
        emit UserRoleChanged();
    }
    /****************************************************************************/
    /*!
     *  \brief This function returns the current User Role
     *
     *  \return m_CurrentUserRoles
     */
    /****************************************************************************/
    static UserRole_t GetCurrentUserRole() { return m_CurrentUserRole;}

    /****************************************************************************/
    /*!
     *  \brief This function returns whether process is running or not
     *
     *  \return m_ProcessRunning
     */
    /****************************************************************************/
    static bool GetProcessRunningStatus() { return m_ProcessRunning; }

    /****************************************************************************/
    /*!
     *  \brief This function sets the date and time format
     *
     *  \iparam DateFormat = ISO/International/US format
     *  \iparam TimeFormat = 12 hr/24 hr format
     */
    /****************************************************************************/
    void SetDateTimeFormat(Global::DateFormat DateFormat, Global::TimeFormat TimeFormat)
    {
        m_DateFormat = DateFormat;
        m_TimeFormat = TimeFormat;
        RefreshDateTime();
    }

    /****************************************************************************/
    /*!
     *  \brief This function gets current the date format
     *
     *  \return DateFormat = ISO/International/US format
     */
    /****************************************************************************/
    Global::DateFormat GetDateFormat() { return m_DateFormat; }

    /****************************************************************************/
    /*!
     *  \brief This function sets the time format
     *
     *  \return TimeFormat = 12 hr/24 hr format
     */
    /****************************************************************************/
    Global::TimeFormat GetTimeFormat() { return m_TimeFormat; }

    /****************************************************************************/
    /*!
     *  \brief This function sets the service and manufacturing SW mode
     *  \iparam UserMode = Service or Manuacturing user mode
     */
    /****************************************************************************/
    void SetSaMUserMode(QString UserMode) {
        m_SaMUserMode = UserMode;
    }

    /****************************************************************************/
    /*!
     *  \brief This function returns the service and manufacturing SW mode
     *
     *  \return S.a.M user mode
     */
    /****************************************************************************/
    QString GetSaMUserMode() {
        return m_SaMUserMode;
    }

    /****************************************************************************/
    /*!
     *  \brief This function clears the TabWidget
     */
    /****************************************************************************/
    void Clear();

private:
    Ui::CMainWindow *mp_Ui;                     //!< User interface
    QTimer mTimeRefreshTimer;                   //!< Timer for updating time display
    Global::DateFormat m_DateFormat;            //!< Date format for the status bar
    Global::TimeFormat m_TimeFormat;            //!< Time format for the status bar
    Global::TemperatureFormat m_TemperatureFormat; //!< Temperature format
    QDateTime m_CurrentDateTime;                //!< Adjusted current time
    bool m_StatusLabel1Free;                    //!< Informs if status label 1 is free
    bool m_StatusLabel2Free;                    //!< Informs if status label 2 is free
    bool m_StatusLabel3Free;                    //!< Informs if status label 3 is free
    static bool m_ProcessRunning;               //!< Informs if Process running label is displayed
    bool m_RemoteService;                       //!< Informs if Remote service label is displayed
    bool m_Error;                               //!< Informs if Error label is displayed
    bool m_Warning;                             //!< Informs if Warning label is displayed
    bool m_WarningErrorFlag;                    //!< True if warning or error label is displayed
    QLabel *mp_Label;                           //!< Label pointer to backup pointer  to warning or error label
    QLabel *mp_ProcessRunningLabel;             //!< Pointer to Process running label
    QLabel *mp_RemoteLabel;                     //!< Pointer to Remote label
    QLabel *mp_WarningLabel;                    //!< Pointer to Warning label
    QLabel *mp_ErrorLabel;                      //!< Pointer to Error Label
    Status_t m_Status;                          //!< Informs Status of system
    static UserRole_t m_CurrentUserRole;        //!< current user type
    QPixmap *mp_ProcPixmap;                     //!< Pixmap object for Process icon.
    QPixmap *mp_RemotePixMap;                   //!< Pixmap object for Remote Care icon.  
    QString m_SaMUserMode;                      //!< S.a.M software user mode
  
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CMainWindow)

    void RetranslateUI();
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void RefreshDateTime();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnCurrentTabChanged(int);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ResetWindowStatusTimer();

protected:
    bool eventFilter(QObject *, QEvent *);
    void changeEvent(QEvent *p_Event);

signals:

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever mainwindow is activated.
     *
     */
    /****************************************************************************/
    void OnWindowActivated();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever changeEvent is triggered.
     *
     */
    /****************************************************************************/
    void onChangeEvent();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever User role is changed.
     *
     */
    /****************************************************************************/
    void UserRoleChanged();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever Process State is changed.
     *
     */
    /****************************************************************************/
    void ProcessStateChanged();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever Temperature format is changed.
     *
     *  \iparam TemperatureFormat = Temperature format Degree Celsius or Fahrenheit.
     *
     */
    /****************************************************************************/
    void TemperatureFormatChanged(Global::TemperatureFormat TemperatureFormat);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever DateTime format is changed.
     *
     *  \iparam DateFormat = Date format
     *  \iparam TimeFormat = Time format 12h or 24h.
     *
     */
    /****************************************************************************/
    void DateTimeFormatChanged(Global::DateFormat DateFormat, Global::TimeFormat TimeFormat);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever Tab is changed.
     *
     */
    /****************************************************************************/
    void CurrentTabChanged(int);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever error message event is sent from Main.
     *
     */
    /****************************************************************************/
    void ShowErrorMsgDlg();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted whenever warning message event is sent from Main.
     *
     */
    /****************************************************************************/
    void ShowWarningMsgDlg();
};

} // end namespace MainMenu

#endif // MAINMENU_MAINWINDOW_H
