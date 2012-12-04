/****************************************************************************/
/*! \file Export/Components/Main/Source/Main.cpp
 *
 *  \brief Implementation file for main routine.
 *
 *  \b Description:
 *         Starting point to the application
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-07-12
 *  $Author:    $ Raju
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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Main/Include/Main.h"
#include "ExportData/Include/Startup.h"
#include "Global/Include/SystemPaths.h"


static const QString Version = "EXP_0.001"; ///< version string for the export component

/****************************************************************************/
/*!
 * \brief Main function.
 *
 *
 * \iparam   argc    Argument count.
 * \iparam   argv    Argument list.
 * \return          Return code.
 */
/****************************************************************************/
int main(int argc, char *argv[]) {

    int CmdLineOption;

    const struct option CmdLongOptions[] = {
        {"version",   no_argument,        0, 'v'},
        {"help",      no_argument,        0, 'h'},
        {0,0,0,0},
    };

    CmdLineOption = getopt_long (argc, argv, "vh", CmdLongOptions, NULL);

    switch (CmdLineOption)  {
        case 'h':
            std::cout << "Usage: " << argv[0] << " [OPTION]" << std::endl;
            std::cout << "  -v, --version    output version information and exit" << std::endl;
            std::cout << "  -h, --help       display this help and exit" << std::endl;
            return 0;

        case 'v':
            std::cout << "Export " << Version.toStdString() << std::endl;
            return 0;

        default:
            break;
    }

    // check the number of arguments
    if (argc > 0) {
        // create application object
        QCoreApplication App(argc, argv);
        // create the startup class object
        Export::CStartup Startup;
        // start application and archive the data
        int ReturnCode = Startup.Archive();
        // and exit
        return ReturnCode;

    }
    return 1;

}
