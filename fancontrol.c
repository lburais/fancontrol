#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

const char PWM0GENERATOR[]="/sys/class/pwm/pwmchip1/export";
const char PWM0[]="/sys/class/pwm/pwmchip1/pwm0";
const char CPUTMP[]="/sys/class/thermal/thermal_zone0/temp";
const char PERIOD[]="period";
const char POLARITY[]="polarity"; 
const char DUTY_CYCLE[]="duty_cycle";
const char ENABLE[]="enable";

char buf[256];
int currentFanLevel = 0;

int isFanDeviceAvailable()
{
	snprintf(buf, sizeof buf, "%s/%s", PWM0,ENABLE);
	if( access( buf, F_OK ) != -1 ) {
		return 1;
	} 
	else {
		return 0;
	}
}

void WriteDataToFile(const char* fname, const char* data, int length){
	int fd = open(fname, O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open %s\n%s\n", fname, strerror(errno));
        //perror("Unable to open %s",PWM1);
        exit(1);
    }

    if (write(fd, data, length) != length) {
		fprintf(stderr, "Error writing to %s\n%s\n", fname, strerror(errno));
        //perror("Error writing to %s",PWM1);
        exit(1);
    }
    close(fd);
}

void ReadDataFromFile(const char* fname, char *data, int size){
	int fd = open(fname, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open %s\n%s\n", fname, strerror(errno));
        exit(1);
    }
	read(fd, data, size);
	close(fd);
}

void EnablePWM()
{
        fprintf(stderr, "  Enable PWM\n");
	if (!isFanDeviceAvailable()){
		WriteDataToFile((char *)PWM0GENERATOR,"0",1);
	}
}

int isFanRunning()
{
	char enabled;
	snprintf(buf, sizeof(buf), "%s/%s", PWM0,ENABLE);
	ReadDataFromFile(buf, &enabled,1);
	return (enabled=='1');
}

int GetCpuTemp(){
	ReadDataFromFile(CPUTMP, buf,sizeof(buf));
	int num = atoi(buf);
	return num/1000;
}

void SetFanLow(){
	if (currentFanLevel != 1)
	{
	        fprintf(stderr, "  Set Fan Low\n");
		snprintf(buf, sizeof(buf), "%s/%s", PWM0,DUTY_CYCLE);
		WriteDataToFile(buf,"3000000",7);
		currentFanLevel = 1;
	}
}

void SetFanMed(){
	if (currentFanLevel != 2)
	{
	        fprintf(stderr, "  Set Fan Med\n");
		snprintf(buf, sizeof(buf), "%s/%s", PWM0,DUTY_CYCLE);
		WriteDataToFile(buf,"6000000",7);
		currentFanLevel = 2;
	}
}

void SetFanHigh(){
	if (currentFanLevel != 3)
	{
        	fprintf(stderr, "  Set Fan High\n");
		snprintf(buf, sizeof(buf), "%s/%s", PWM0,DUTY_CYCLE);
		WriteDataToFile(buf,"9000000",7);
		currentFanLevel = 2;
	}
}

void SetFanOn(){
        fprintf(stderr, "  Set Fan On\n");
	snprintf(buf, sizeof(buf), "%s/%s", PWM0,ENABLE);
	WriteDataToFile(buf,"1",1);
}

void SetFanOff(){
        fprintf(stderr, "  Set Fan Off\n");
	snprintf(buf, sizeof(buf), "%s/%s", PWM0,ENABLE);
	WriteDataToFile(buf,"0",1);
	currentFanLevel = 0;
}

void InitPwm()
{
	snprintf(buf, sizeof(buf), "%s/%s", PWM0,PERIOD);
	WriteDataToFile(buf,"10000000",8);
	snprintf(buf, sizeof(buf), "%s/%s", PWM0,POLARITY);
	WriteDataToFile(buf,"normal",6);
}

void Loop(){
	int tmp;
	while (1==1){
		tmp = GetCpuTemp();
		fprintf(stderr, "  Temp is %d°\n", tmp);
		if (tmp < 40)
			SetFanOff();
		else{
			SetFanOn();
			if (tmp < 70){
				SetFanLow();
			}
			else if ( tmp < 75){
				SetFanMed();
			}
			else if (tmp <80){
				SetFanHigh;
			}
		}
		sleep(60);
	}
}

int main(int argc, char **argv)
{
        fprintf(stderr, "Fan Control\n");
	EnablePWM();
	if (isFanDeviceAvailable()){
		if (isFanRunning())
		{
			SetFanOff();
		}
		else
		{
			InitPwm();
			Loop();
		}
	}
	return (EXIT_SUCCESS);
}

