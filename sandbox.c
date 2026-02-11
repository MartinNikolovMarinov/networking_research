#include "nr_client.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SOCKET_PATH "/tmp/nr_socket"

int main(int argc, char** argv) {
    int32_t ret = 1;
    const char* socketPath = DEFAULT_SOCKET_PATH;
    NrTransport transport;
    NrClient client = {0};
    const char* requestMsg = "ping\n";
    char responseBuffer[NR_CLIENT_BUFFER_SIZE];
    uint64_t bytesSent = 0;
    uint64_t bytesRead = 0;

    client.socketFd = -1;

    if (argc > 1) {
        socketPath = argv[1];
    }

    if (strlen(socketPath) >= NR_CLIENT_UNIX_PATH_MAX_LEN) {
        fprintf(stderr, "Socket path too long: %s\n", socketPath);
        goto cleanup;
    }

    transport = nrCreateUnixStreamTransport(socketPath);
    if (nrInitClient(&client, &transport) != 0) {
        fprintf(stderr, "nrInitClient() failed for '%s': %s\n", socketPath, strerror(errno));
        goto cleanup;
    }

    if (nrClientSend(&client, requestMsg, strlen(requestMsg), &bytesSent) != 0) {
        fprintf(stderr, "nrClientSend() failed: %s\n", strerror(errno));
        goto cleanup;
    }

    (void)bytesSent;

    if (nrClientReceive(&client, responseBuffer, sizeof(responseBuffer) - 1U, &bytesRead) != 0) {
        fprintf(stderr, "nrClientReceive() failed: %s\n", strerror(errno));
        goto cleanup;
    }

    if (bytesRead == 0) {
        printf("Server closed connection without response.\n");
        ret = 0;
        goto cleanup;
    }

    responseBuffer[bytesRead] = '\0';
    printf("Received (%" PRIu64 " bytes): %s\n", bytesRead, responseBuffer);
    ret = 0;

cleanup:
    nrDestroyClient(&client);
    return (int)ret;
}
