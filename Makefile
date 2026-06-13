CC:= arduino-cli
BOARD:= esp32:esp32:esp32
PORT:= /dev/ttyUSB0
BAUD:= baudrate=9600
TT:= esptool

compile:
	$(CC) compile -b $(BOARD) main

build: compile
	$(CC) upload main -p $(PORT) -b $(BOARD)
	$(CC) monitor -p $(PORT) -c $(BAUD)

clean:
	$(TT) --chip esp32 --port $(PORT) erase-flash


.PHONY: build compile
