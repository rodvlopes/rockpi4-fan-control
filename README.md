# Description

This program has been developed to run on the ROCKPI4 to control a 5V fan connected to any GPIO through a NPN transistor.
     
# Instructions

- to compile: gcc fanctl.c -o fanctl
- to install: install fanctl /usr/local/bin/
- to customize: change the definions below.
- to debug: fanctl debug
- to install as a service: vi /etc/systemd/system/fanctl.service 
(with following content, then systemctl start fanctl.)
    
    [Unit]
    Description=Fan control based on cpu temperature
  
    [Service]
    Type=simple
    ExecStart=/usr/local/bin/fanctl
  
    [Install]
    WantedBy=multi-user.target

# Author

rodvlopes

# Discussion

https://forum.radxa.com/t/possible-to-use-5v-relay-module-with-gpio/2274/
