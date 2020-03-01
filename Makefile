all:
	cat README.md

build:
	gcc fanctl.c -o fanctl

install:
	install fanctl /usr/local/bin/

install-service:
	cp fanctl.service /etc/systemd/system/fanctl.service 
	systemctl start fanctl

