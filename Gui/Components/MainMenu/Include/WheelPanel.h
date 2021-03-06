/****************************************************************************/
/*! \file WheelPanel.h
 *
 *  \brief Header file for class CWheelPanel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-10
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

#ifndef MAINMENU_WHEELPANEL_H
#define MAINMENU_WHEELPANEL_H

#include "MainMenu/Include/ScrollWheel.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

//lint -e429

namespace MainMenu {

class CSemiTransparentOverlay;
/****************************************************************************/
/**
 * \brief This class is the frame for one or multiple scroll wheels.
 */
/****************************************************************************/
class CWheelPanel : public QWidget
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    //!< Separator between two scoll wheels
    typedef enum {
        NONE,
        COLON,
        FULLSTOP
    } SeparatorType_t;

    explicit CWheelPanel(QWidget *p_Parent = 0);
    virtual ~CWheelPanel();
    void Init(qint32 Count);
    void SetTitle(QString Title);
    void SetSubtitle(QString Subtitle, qint32 Position);
    void AddScrollWheel(MainMenu::CScrollWheel *p_Wheel, qint32 Position);
    void AddSeparator(SeparatorType_t Type, qint32 Position);
    void SetDisabled(bool Disabled);
    void SetThreeDigitMode(bool Mode);
    void SetLayoutContentsMargin(int LeftMargin, int TopMargin, int RightMargin, int BottonMargin);
    void SetVerticalSpacing(int VerticalSpacing);
    void SetTitleWithMaxLength(qint32 StringLength, QString TitleText);

    /****************************************************************************/
    /**
     * \brief Gets the wheel panel size.
     *
     *  \return Size.
     */
    /****************************************************************************/
    QSize GetWheelPanelSize() { return size();}

private:
    void paintEvent(QPaintEvent *);
    QGridLayout *mp_Layout;                 //!< Main layout
    QLabel *mp_Title;                       //!< Title above the scroll wheels
    QList<QLabel *> m_SubtitleList;         //!< Titles below a single scroll
    QList<QLabel *> m_SeparatorList;        //!< Separators between wheel
    QPixmap  m_PanelPixmap;                 //!< Digit  panel Pixmap
    CSemiTransparentOverlay *mp_SemiTransparentOverlay; //!< Semitransparent widget to give disabled look.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CWheelPanel)
};

} // end namespace MainMenu


#endif // MAINMENU_WHEELPANEL_H
