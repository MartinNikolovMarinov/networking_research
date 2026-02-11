#ifndef NR_CLIENT_H
#define NR_CLIENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NR_CLIENT_BUFFER_SIZE 2048
#define NR_CLIENT_UNIX_PATH_MAX_LEN 108

typedef enum NrTransportKind {
    NR_TRANSPORT_KIND_INVALID = 0,
    NR_TRANSPORT_KIND_UDP,
    NR_TRANSPORT_KIND_TCP,
    NR_TRANSPORT_KIND_UNIX_STREAM,
} NrTransportKind;

typedef struct NrUnixStreamTransport {
    char socketPath[NR_CLIENT_UNIX_PATH_MAX_LEN];
} NrUnixStreamTransport;

typedef struct NrTransport {
    NrTransportKind kind;
    union {
        NrUnixStreamTransport unixStream;
    } data;
} NrTransport;

typedef struct NrClient {
    int32_t socketFd;
    NrTransport transport;
    uint8_t txBuffer[NR_CLIENT_BUFFER_SIZE];
    uint8_t rxBuffer[NR_CLIENT_BUFFER_SIZE];
} NrClient;

NrTransport nrCreateUnixStreamTransport(const char* socketPath);
int32_t nrInitClient(NrClient* client, const NrTransport* transport);
int32_t nrClientSend(NrClient* client, const void* data, uint64_t dataLen, uint64_t* bytesSent);
int32_t nrClientReceive(NrClient* client, void* outBuffer, uint64_t outBufferSize, uint64_t* bytesRead);
void nrDestroyClient(NrClient* client);

#ifdef __cplusplus
}
#endif

#endif
