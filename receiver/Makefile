CC: gcc
BFLAGS: -Wall -Werror -std=c23
DFLAGS: -Wall -std=c23
LIBS: -lcurl

build:
	mkdir build
	$(CC) $(BFLAGS) $(LIBS) main.c -o build/main

debug:
	mkdir debug
	$(CC) $(DFLAGS) $(LIBS) main.c -o debug/main

clear:
	rm -rf ./debug
	rm -rf ./build

.PHONY: debug build
