CC:= gcc
BFLAGS:= -Wall -Werror -std=c23
DFLAGS:= -Wall -std=c23
LBS:= -lcurl

build:
	$(CC) $(BFLAGS) $(LBS) -DLY_BUILD_MODE main.c -o build/main

debug:
	$(CC) $(DFLAGS) $(LBS) main.c -o debug/main

clean:
	rm -rf ./debug/*
	rm -rf ./build/*
	rm -f output.csv

.PHONY: debug build
