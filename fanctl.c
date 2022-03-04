/*
    This program has been developed to run on the ROCKPI4 to control a 5V fan connected to any GPIO through a NPN transistor.
     
    to compile: gcc fanctl.c -o fanctl
    to install: install fanctl /usr/local/bin/
    to customize: change the definions below.
    to debug: fanctl debug
    to install as a service: vi /etc/systemd/system/fanctl.service 
      (with following content, then systemctl start fanctl.)
      
      [Unit]
      Description=Fan control based on cpu temperature

      [Service]
      Type=simple
      ExecStart=/usr/local/bin/fanctl

      [Install]
      WantedBy=multi-user.target
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PINNUM "146"
#define PINVALUE_FILE "/sys/class/gpio/gpio" PINNUM "/value"
#define PINDIRECTION_FILE "/sys/class/gpio/gpio" PINNUM "/direction"
#define EXPORT_FILE "/sys/class/gpio/export"
#define TEMPERATURE_FILE "/sys/class/thermal/thermal_zone0/temp"
#define TEMPERATURE_THRESHOLD_ON  70000
#define TEMPERATURE_THRESHOLD_OFF 43000
 
int file_exist (char *fname) {
    return access( fname, F_OK ) != -1;
}

int is_pin_exported () {
    return file_exist (PINVALUE_FILE);
}

void export_pin () {
	FILE *fex, *fdir;
	fex = fopen(EXPORT_FILE, "w");
	fputs(PINNUM, fex);
	fclose(fex);
	fdir = fopen(PINDIRECTION_FILE, "w");
	fputs("out", fdir);
	fclose(fdir);
}

void switch_pin (int val) {
    FILE *fp;
    char str[5];
    sprintf(str, "%d", val);
	fp = fopen(PINVALUE_FILE, "w");
    fputs(str, fp);
	fclose(fp);
} 

int read_cpu_temperature () {
    FILE *fp;
    char str[5];
    int val;
	fp = fopen(TEMPERATURE_FILE, "r");
    fread(str, sizeof str, 1, fp);
	fclose(fp);
    val = atoi(str);
    return val;
}

int main (int argc, char *argv[]) {
    int debug = 0;
    int current_cpu_temp = 0;
    
    if (argc > 1) debug = 1; //any arguments enable debug mode

    debug && printf("CPU FAN Control Started on pin " PINNUM "\n");

    if (!is_pin_exported()) {
        export_pin();
    }

    while(1) {
        current_cpu_temp = read_cpu_temperature();

        debug && printf("Current CPU TEMP is %d\n", current_cpu_temp);
        if (current_cpu_temp > TEMPERATURE_THRESHOLD_ON) {
            switch_pin(1);
            debug && printf("on\n");
        }
        else if (current_cpu_temp < TEMPERATURE_THRESHOLD_OFF) {
            switch_pin(0);
            debug && printf("off\n");
        }
        usleep(500000); //500ms
        fflush(NULL);
    }

	return(0);

}
