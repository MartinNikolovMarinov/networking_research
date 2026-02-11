#!/bin/bash

set -euo pipefail
rm -f /tmp/nr_socket
( printf 'pong from server\n' | ncat -lU /tmp/nr_socket ) &
server_pid=$!
sleep 0.2
./build/sandbox /tmp/nr_socket
wait "$server_pid"
