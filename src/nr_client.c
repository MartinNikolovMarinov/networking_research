#include "nr_client.h"

#include <errno.h>
#include <string.h>

#if defined(_WIN32)
#error "nr_client currently supports Unix-like platforms only."
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

NrTransport nrCreateUnixStreamTransport(const char* socketPath) {
    NrTransport transport = {0};

    transport.kind = NR_TRANSPORT_KIND_UNIX_STREAM;

    if (socketPath == NULL) {
        return transport;
    }

    uint64_t pathLen = (uint64_t)strlen(socketPath);
    if (pathLen >= NR_CLIENT_UNIX_PATH_MAX_LEN) {
        return transport;
    }

    strncpy(
        transport.data.unixStream.socketPath,
        socketPath,
        NR_CLIENT_UNIX_PATH_MAX_LEN - 1U
    );

    return transport;
}

int32_t nrInitClient(NrClient* client, const NrTransport* transport) {
    if (client == NULL || transport == NULL) {
        errno = EINVAL;
        return -1;
    }

    memset(client, 0, sizeof(*client));
    client->socketFd = -1;
    client->transport = *transport;

    if (transport->kind != NR_TRANSPORT_KIND_UNIX_STREAM) {
        errno = EPROTOTYPE;
        return -1;
    }

    const char* socketPath = transport->data.unixStream.socketPath;
    if (socketPath[0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    int32_t socketFd = (int32_t)socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd < 0) {
        return -1;
    }

    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, socketPath, sizeof(serverAddr.sun_path) - 1U);

    int32_t connectRet = (int32_t)connect(
        (int)socketFd,
        (const struct sockaddr*)&serverAddr,
        sizeof(serverAddr)
    );
    if (connectRet != 0) {
        close((int)socketFd);
        return -1;
    }

    client->socketFd = socketFd;
    return 0;
}

int32_t nrClientSend(NrClient* client, const void* data, uint64_t dataLen, uint64_t* bytesSent) {
    if (client == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (client->socketFd < 0) {
        errno = ENOTCONN;
        return -1;
    }
    if (dataLen > NR_CLIENT_BUFFER_SIZE) {
        errno = EMSGSIZE;
        return -1;
    }

    memcpy(client->txBuffer, data, dataLen);

    int64_t sendRet = (int64_t)send(
        (int)client->socketFd,
        client->txBuffer,
        (size_t)dataLen,
        0
    );
    if (sendRet < 0) {
        return -1;
    }

    if (bytesSent != NULL) {
        uint64_t sent = (uint64_t)sendRet;
        *bytesSent = sent;
    }

    return 0;
}

int32_t nrClientReceive(NrClient* client, void* outBuffer, uint64_t outBufferSize, uint64_t* bytesRead) {
    if (client == NULL || outBuffer == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (client->socketFd < 0) {
        errno = ENOTCONN;
        return -1;
    }
    if (outBufferSize == 0 || outBufferSize > NR_CLIENT_BUFFER_SIZE) {
        errno = EMSGSIZE;
        return -1;
    }

    int64_t recvRet = (int64_t)recv(
        (int)client->socketFd,
        client->rxBuffer,
        (size_t)outBufferSize,
        0
    );
    if (recvRet < 0) {
        return -1;
    }

    uint64_t readCount = (uint64_t)recvRet;
    memcpy(outBuffer, client->rxBuffer, (size_t)readCount);

    if (bytesRead != NULL) {
        *bytesRead = readCount;
    }

    return 0;
}

void nrDestroyClient(NrClient* client) {
    if (client == NULL) {
        return;
    }
    if (client->socketFd < 0) {
        return;
    }

    close((int)client->socketFd);
    client->socketFd = -1;
}
