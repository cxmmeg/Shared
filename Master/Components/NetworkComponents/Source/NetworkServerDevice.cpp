/****************************************************************************/
/*! \file NetworkServerDevice.cpp
 *
 *  \brief NetworkServerDevice implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.11.2010
 *   $Author:  $ Y.Novak
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

#include <NetworkComponents/Include/NetworkServerDevice.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *
 *  \param[in] stype = type of server to create
 *  \param[in] client = type of client to accept
 *  \param[in] path = path to settings folder
 *  \param[in] pParent = pointer to parent
 *
 ****************************************************************************/
NetworkServerDevice::NetworkServerDevice(NetworkServerType_t stype, const QString &client, const QString &path, QObject *pParent) :
        NetworkDevice::NetworkDevice(CML_TYPE_SERVER, path, pParent),
        m_myServer(NULL),
        m_myServerType(stype),
        m_myClient(client),
        m_dtTimerPeriod(0),
        m_DateTimeSyncTimer(this)
{
    this->setParent(pParent);
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 *
 *
 ****************************************************************************/
NetworkServerDevice::~NetworkServerDevice()
{
    try {
        if(m_myServer != NULL) {
            delete m_myServer;
        }
        m_myServer = NULL;
    } catch (...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/*!
 *  \brief    API function to call before NetworkServerDevice can be used
 *
 *      This function does everything to initialize the NetworkServerDevice.
 *      It shall be called before NetworkServerDevice can be used.
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool NetworkServerDevice::InitializeDevice()
{
    qDebug() << "NetworkServerDevice: initializing...";

    try {
        if (!NetworkDevice::InitializeDevice()) {
            qDebug() << "NetworkServerDevice: cannot initialize my NetworkDevice !";
            return false;
        }

        if (!InitializeNetwork()) {
            qDebug() << "NetworkServerDevice: cannot initialize my Server.";
            return false;
        }

        // connect Server's error emitter to error processing slot
        CONNECTSIGNALSLOT(m_myServer, ConnectionError(NetworkBase::DisconnectType_t, const QString &),
                          this, HandleNetworkError(NetworkBase::DisconnectType_t, const QString &));
        CONNECTSIGNALSLOT(m_myServer, ClientConnected(const QString &), this, PeerConnected(const QString &));
        CONNECTSIGNALSLOT(m_myServer, ClientDisconnected(const QString &), this, PeerDisconnected(const QString &));
    } catch (...) {
        /// \todo: handler error
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief This slot handles errors reported by the TCPIP Server.
 *
 *  \param[in]  dtype  = the server error
 *  \param[in]  client = client name
 */
/****************************************************************************/
void NetworkServerDevice::HandleNetworkError(DisconnectType_t dtype, const QString &client)
{
    Q_UNUSED(client)
    /// \todo: do we make NetworkServerDevice a logging object? or do we forward
    /// errors further up? who reports them?

    qDebug() << "NetworkServerDevice: NetworkServer reported problem --> " + QString::number(static_cast<int>(dtype), 10);

    switch (dtype) {
    case SOCKET_DISCONNECT:
    case USER_REQUEST:
        // client disconnected, which is reported by server's client Disconnected signal
        // nothing to do here
        break;
    case CONNECT_SIGNAL_FAIL:
    case CANNOT_ALLOCATE_MEMORY:
    case SOCKET_ERROR:
        // fatal errors, server cannot work
        /// \todo: log them
        break;
    case NULL_POINTER_IN_HASH:
    case NO_FREE_REFERENCES:
        // fatal, but Server re-initialization might help, restart server here?
        /// \todo: log them
        break;
    default:
        /// \todo: all others just need to be logged
        break;
    } //switch
}

/****************************************************************************/
/*!
 *  \brief   Initialize the TCPIP Server for communication with local client.
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool NetworkServerDevice::InitializeNetwork()
{
    qDebug() << "NetworkServerDevice: initializing Server...";

    try {
        /// \todo: set correct subsource:
        if (m_myServer == NULL) {
            m_myServer = new NetworkServer(m_myServerType, this);
        }
        if (m_myServer->InitializeServer() != NS_ALL_OK) {
            /// \todo: log error
            qDebug() << "NetworkServerDevice: cannot initialize Server";
            delete m_myServer;
            m_myServer = NULL;
            return false;
        }
        if (!m_myServer->RegisterMessageHandler(this, m_myClient)) {
            /// \todo: log error
            qDebug() << "NetworkServerDevice: cannot register protocol handler";
            delete m_myServer;
            m_myServer = NULL;
            return false;
        }
    }
    catch (const std::bad_alloc &) {
        /// \todo: log error
        qDebug() << "NetworkServerDevice: Cannot allocate memory!";
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  This method starts Network Server Device operation.
 *
 *          This method shall be called after the successful
 *          InitializeDevice call.
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool NetworkServerDevice::StartDevice()
{
    if(m_myServer == NULL) {
        qDebug() << (QString)("NetworkServerDevice: my Server is NULL !");
        return false;
    }

    if (m_myServer->StartServer() != NS_ALL_OK) {
        qDebug() << (QString)("NetworkServerDevice: cannot start my Server!");
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Destroy Network Server Device.
 *
 */
/****************************************************************************/
void NetworkServerDevice::DestroyDevice()
{
    static_cast<void>(
            // cannot use return value here anyway
            this->disconnect()
    );
    // delete all instantiated objects:
    if (m_myServer != NULL) {
        static_cast<void>(
                // cannot use return value here anyway
                m_myServer->disconnect()
        );
        delete m_myServer;
    }
    m_myServer = NULL;
    qDebug() << (QString)("NetworkServerDevice: Device destroyed.");
}

/****************************************************************************/
/*!
 *  \brief    This function disconnects Network Server Device from peer
 *
 *
 ****************************************************************************/
void NetworkServerDevice::DisconnectPeer()
{
    // stop periodic heartbeat timer:
    NetworkDevice::DisconnectPeer();
    // stop DateAndTime sync timer:
    m_DateTimeSyncTimer.stop();
    qDebug() << "NetworkServerDevice: peer disconnected.";
    if (m_myServer != NULL) {
        m_myServer->DestroyAllConnections();
    }
}

/****************************************************************************/
/*!
 *  \brief  This method starts Date and Time sync with the peer.
 *          Call this method if you want to use Server's DateTime
 *          synchronisation service.
 *
 *  \param  period = period between sync messages, in ms.
 *                   If period=0, default will be used.
 *
 *  \return true if successfully started, false otherwise
 */
/****************************************************************************/
bool NetworkServerDevice::StartDateTimeSync(int period)
{
    // set period if needed
    if (period == 0) {
        m_dtTimerPeriod = DATETIME_SYNC_DEFAULT_DELAY;
    }
    else {
        m_dtTimerPeriod = period;
    }
    // connect timer
    if (!QObject::connect(&m_DateTimeSyncTimer, SIGNAL(timeout()), this, SLOT(SyncDateAndTime()))) {
        qDebug() << "NetworkServerDevice: DateTimeSync cannot be started !";
        return false;
    }
    // start the sync process
    SyncDateAndTime();
    return true;
}

/****************************************************************************/
/*!
 *  \brief  This slot sends the next Date and Time sync message.
 *          Time Format: DD:MM:YYYY HH:MM:SS
 *
 *  \note   This slot is called by a periodic sync timer. But if immediate
 *          synchronisation is needed, just trigger this slot directly.
 */
/****************************************************************************/
void NetworkServerDevice::SyncDateAndTime()
{
    // stop DateTimeSync timer to set new period.
    m_DateTimeSyncTimer.stop();
    m_DateTimeSyncTimer.start(m_dtTimerPeriod);
    // create command processing object
    ProtocolTxCommand *pC = CreateNewOutgoingCommand<ProtocolTxCommand, BaseTxPCmd>((QString)"DateAndTime");
    if (pC == NULL) {
        emit SigDateTimeSyncError(NetworkBase::CMH_MSG_SENDING_FAILED);
        return;
    }
    // run created command:
    if (!pC->Execute()) {
        emit SigDateTimeSyncError(NetworkBase::CMH_MSG_SENDING_FAILED);
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  This method processes peer disconnection event.
 *
 *          All periodic perr activities shall be stopped till peer
 *          connects again.
 *
 *  \param  name = name of the disconnected peer
 */
/****************************************************************************/
void NetworkServerDevice::PeerDisconnected(const QString &name)
{
    // stop DateTimeSync timer to set new period.
    m_DateTimeSyncTimer.stop();
    NetworkDevice::PeerDisconnected(name);
}

} // end namespace NetworkBase
