port := /dev/ttyS1
baud := 115200

.PHONY: build
build:
	idf.py build

.PHONY: install
install:
	idf.py -p $(port) -b $(baud) flash

.PHONY: monitor
monitor:
	idf.py -p $(port) -b $(baud) monitor
