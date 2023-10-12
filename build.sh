if [[ "$OSTYPE" =~ ^linux ]]; then
    LINK=""
fi

if [[ "$OSTYPE" =~ ^msys ]]; then
    LINK="-lws2_32"
fi

gcc examples/tcp_client.c src/*.c -Iinclude $LINK -o bin/client
gcc examples/tcp_server.c src/*.c -Iinclude $LINK -o bin/server
