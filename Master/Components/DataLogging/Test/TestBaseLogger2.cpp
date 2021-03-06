/****************************************************************************/
/*! \file TestBaseLogger2.cpp
 *
 *  \brief Implementation file for class TestBaseLogger.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#include <TestBaseLogger.h>
#include <DataLogging/Include/BaseLogger.h>
#include <DataLogging/Include/DataLoggingEventCodes.h>
#include <DirectoryHelper.h>

namespace DataLogging {

/****************************************************************************/
void TestBaseLogger::utOpenAppendOK() {
    QString LogSource("Logger");
    BaseLogger TestObject1(NULL, LogSource, 1);
    //*********************************************************
    // OK: - a new file is opended for append, no file open yet
    //*********************************************************
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    TestObject1.OpenFileForAppend(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    QVERIFY(QFile::exists(FileName));
    // file size will be checked when file was closed.

    //*******************************************************
    // OK: - a new file is opened for append. a file was open
    //*******************************************************
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(TestObject1.m_File.isOpen());
    QString FileName2 = m_DirHelper.AbsoluteFileName("file2.log");
    TestObject1.OpenFileForAppend(FileName2);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName2);
    QVERIFY(TestObject1.m_File.isOpen());
    QVERIFY(QFile::exists(FileName2));

    // check size of file1.log
    QFile File1(FileName);
    QCOMPARE(File1.size(), qint64(0));
    QVERIFY(File1.open(QIODevice::Append));
    // now append some bytes
    QVERIFY(File1.write("12345\n54321\n") > 0);
    QVERIFY(File1.size() > 0);
    qint64 FileSize = File1.size();
    File1.close();

    //************************************************************
    // OK: - a existing file is opened for append, a file was open
    //************************************************************
    // we open file1.log again and check what happens with the size
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(TestObject1.m_File.isOpen());
    TestObject1.OpenFileForAppend(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(QFile::exists(FileName));
    // check size of file1.log it shouldn't have changed
    QCOMPARE(File1.size(), FileSize);

    // create a readonly file
    QString Exist1FileName("exist1.log");
    QString ReadonlyFileName = m_DirHelper.AbsoluteFileName(Exist1FileName);
    m_DirHelper.CreateReadonlyFile(ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));

    //************************************************************************
    // OK: - a readonly file with that name already exists. No file open yet.
    // works as root but not as normal user.
    //************************************************************************
    TestObject1.CloseFile();
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // now try to create file
    TestObject1.OpenFileForAppend(ReadonlyFileName);
    // check results
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));
    // check size of file1.log it shouldn't have changed
    QCOMPARE(File1.size(), FileSize);

    //***************************************************************************
    // OK: - a readonly file with that name already exists. A file already open.
    // works as root but not as normal user.
    //***************************************************************************
    TestObject1.CloseFile();
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // create and open "file1.log"
    FileName = m_DirHelper.AbsoluteFileName("file1.log");
    TestObject1.OpenFileForAppend(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    // now try to create and open the readonly file
    TestObject1.OpenFileForAppend(ReadonlyFileName);
    // check results
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     ReadonlyFileName);
    QVERIFY(QFile::exists(ReadonlyFileName));
    // check size of file1.log it shouldn't have changed
    QCOMPARE(File1.size(), FileSize);
}

/****************************************************************************/
void TestBaseLogger::utOpenAppendNOK() {
    QString LogSource("Logger");
    BaseLogger TestObject1(NULL, LogSource, 1);
    QString NotExistingFileName = m_DirHelper.AbsoluteFileName("NotExistingPath/file1.log");
    QString FileName = m_DirHelper.AbsoluteFileName("file1.log");
    //************************************************************
    // open a file in an not existing directory. No file open yet.
    //************************************************************
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // now try to create file in not existing directory
    TestObject1.OpenFileForAppend(NotExistingFileName);

    //***************************************************************
    // open a file in an not existing directory. A file already open.
    //***************************************************************
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    // create and open "file1.log"
    TestObject1.OpenFileForAppend(FileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     FileName);
    QVERIFY(TestObject1.m_File.isOpen());
    // now try to create file in not existing directory
    TestObject1.OpenFileForAppend(NotExistingFileName);
}

/****************************************************************************/
void TestBaseLogger::utAppend() {
    QString LogSource("Logger");
    BaseLogger TestObject1(NULL, LogSource, 1);
    QString OldLine("old line\n");
    QString NewLine("new line");
    QString EmptyFileName = m_DirHelper.AbsoluteFileName("empty.log");
    QString NotEmptyFileName = m_DirHelper.AbsoluteFileName("notempty.log");
    // make sure EmptyFileName is empty
    QFile TheFile(EmptyFileName);
    QVERIFY(TheFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    TheFile.close();
    QCOMPARE(TheFile.size(), qint64(0));
    // make sure NotEmptyFileName has an predefined content
    TheFile.setFileName(NotEmptyFileName);
    QVERIFY(TheFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    QVERIFY(TheFile.write(OldLine.toUtf8()));
    TheFile.close();
    QCOMPARE(TheFile.size(), qint64(OldLine.size()));

    //************************************
    // NOK: try to append when file closed
    //************************************
    TestObject1.AppendLine(NewLine);

    //**************************************
    // OK: append to an existing empty file.
    //**************************************
    QVERIFY(TestObject1.m_File.fileName().isEmpty());
    QVERIFY(!TestObject1.m_File.isOpen());
    TestObject1.OpenFileForAppend(EmptyFileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     EmptyFileName);
    QVERIFY(QFile::exists(EmptyFileName));
    QCOMPARE(TestObject1.m_File.size(), qint64(0));
    TestObject1.AppendLine(NewLine);
    QCOMPARE(TestObject1.m_File.size(), qint64(NewLine.size()+1));

    //******************************************
    // OK: append to an existing non empty file.
    //******************************************
    QVERIFY(!TestObject1.m_File.fileName().isEmpty());
    QVERIFY(TestObject1.m_File.isOpen());
    TestObject1.OpenFileForAppend(NotEmptyFileName);
    QCOMPARE(TestObject1.GetLoggingSource(),    LogSource);
    QCOMPARE(TestObject1.m_FormatVersion,       1);
    QCOMPARE(TestObject1.m_File.fileName(),     NotEmptyFileName);
    QVERIFY(QFile::exists(NotEmptyFileName));
    QCOMPARE(TestObject1.m_File.size(), qint64(OldLine.size()));
    TestObject1.AppendLine(NewLine);
    QCOMPARE(TestObject1.m_File.size(), qint64(OldLine.size() + NewLine.size()+1));
}

} // end namespace DataLogging
