all:
	cat README.md

build:
	gcc fanctl.c -o fanctl
	gcc fanctl-pwm.c -o fanctl-pwm

install:
	install fanctl /usr/local/bin/
	install fanctl-pwm /usr/local/bin/

install-service:
	cp fanctl.service /etc/systemd/system/fanctl.service 
	systemctl start fanctl

