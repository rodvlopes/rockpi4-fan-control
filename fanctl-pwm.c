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

#define PWM_ENABLE_FILE "/sys/class/pwm/pwmchip0/pwm0/enable"
#define PWM_DUTY_CYCLE_FILE "/sys/class/pwm/pwmchip0/pwm0/duty_cycle"
#define PWM_PERIOD_FILE "/sys/class/pwm/pwmchip0/pwm0/period"
#define PWM_EXPORT_FILE "/sys/class/pwm/pwmchip0/export"
#define PERIOD 1000000 //ns (1000us in ns)
#define TEMPERATURE_FILE "/sys/class/thermal/thermal_zone0/temp"
#define TEMPERATURE_THRESHOLD_ON  70000
#define TEMPERATURE_THRESHOLD_OFF 50000
#define TEMPERATURE_THRESHOLD_DELTA (TEMPERATURE_THRESHOLD_ON - TEMPERATURE_THRESHOLD_OFF)
 
int debug = 0;

void switch_pin (int val) {
    FILE *fp;
    char str[5];
    sprintf(str, "%d", val);
	fp = fopen(PWM_ENABLE_FILE, "w");
    fputs(str, fp);
	fclose(fp);
} 

void export_pin () {
	FILE *fex, *fper;
	fex = fopen(PWM_EXPORT_FILE, "w");
	fputs("0", fex);
	fclose(fex);
	fper = fopen(PWM_PERIOD_FILE, "w");
    char str[9];
    sprintf(str, "%d", PERIOD);
	fputs(str, fper);
	fclose(fper);
    switch_pin(0);
}

void set_pwm_dc (float percentage) {
    if (percentage < 0.25) { 
        percentage = 0.25;
    }
    else if (percentage > 1) {
        percentage = 1;
    }
    int duty = percentage * PERIOD;
    debug && printf("duty: %d\n", duty);
    FILE *fp;
    char str[9];
    sprintf(str, "%d", duty);
	fp = fopen(PWM_DUTY_CYCLE_FILE, "w");
    fputs(str, fp);
	fclose(fp);
} 

int read_enable () {
    FILE *fp;
    char str[1];
    int val;
	fp = fopen(PWM_ENABLE_FILE, "r");
    fread(str, sizeof str, 1, fp);
	fclose(fp);
    val = atoi(str);
    return val;
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
    int current_cpu_temp = 0;
    
    if (argc > 1) debug = 1; //any arguments enable debug mode

    export_pin();
    int is_on = read_enable();

    debug && printf("CPU FAN Control Started on PWM0. is_on: %d\n", is_on);


    while(1) {
        current_cpu_temp = read_cpu_temperature();

        debug && printf("Current CPU TEMP is %d\n", current_cpu_temp);
        if (current_cpu_temp > TEMPERATURE_THRESHOLD_ON && !is_on) {
            switch_pin(1);
            is_on = 1;
            debug && printf("on\n");
        }
        else if (current_cpu_temp < TEMPERATURE_THRESHOLD_OFF && is_on) {
            switch_pin(0);
            is_on = 0;
            debug && printf("off\n");
        }

        if (is_on) {
            int delta_temp = current_cpu_temp - TEMPERATURE_THRESHOLD_OFF;
            float percentage = (float) delta_temp / TEMPERATURE_THRESHOLD_DELTA;
            debug && printf("percentage: %.9f, delta_temp: %d, TEMPERATURE_THRESHOLD_DELTA: %d\n", percentage, delta_temp, TEMPERATURE_THRESHOLD_DELTA);
            set_pwm_dc(percentage);
        }
        usleep(1000000); //1s
        fflush(NULL);
    }

	return(0);

}
