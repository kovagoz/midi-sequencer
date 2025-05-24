port := /dev/ttyUSB0
baud := 115200

vm := idf

.PHONY: build
build:
	idf.py build

.PHONY: install
install:
	idf.py -p $(port) -b $(baud) flash

.PHONY: clean
clean:
	idf.py fullclean

.PHONY: reconfigure ## Clean the compiler cache
reconfigure:
	idf.py reconfigure

.PHONY: monitor
monitor:
	idf.py -p $(port) -b $(baud) monitor

.PHONY: up
up:
	utmctl start $(vm)

.PHONY: down
down:
	utmctl exec $(vm) --cmd shutdown -h now

.PHONY: shell
shell:
	ssh -i ~/.ssh/id_rsa esp@192.168.68.65

.PHONY: router
router:
	source .pyvenv/bin/activate
	python3 router.py
