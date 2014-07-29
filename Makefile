CFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`

argonaut:
	cc ${CFLAGS} -o argonaut src/main.c ${LDFLAGS}
