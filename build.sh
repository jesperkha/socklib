if [[ "$OSTYPE" =~ ^linux ]]; then
    LINK=""
fi

if [[ "$OSTYPE" =~ ^msys ]]; then
    LINK="-lws2_32"
fi

if [ -z "$1" ]
    then
        echo "build [example dir]"
    else
        gcc examples/$1/client.c src/*.c -Iinclude $LINK -o bin/client
        gcc examples/$1/server.c src/*.c -Iinclude $LINK -o bin/server
fi
