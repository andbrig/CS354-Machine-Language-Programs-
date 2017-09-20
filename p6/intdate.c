////////////////////////////////////////////////////////////////////////////////
// This File:        intdate.c
// Semester:         CS 354 Fall 2016
//
// Author:           Andrew Briggs
// Email:            briggs@cs.wisc.edu
// CS Login:         briggs
////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
const int alarmTime = 3;
const int exitPresses = 5;
static int cPresses = 0;

void hndlrSIGALRM() {
	time_t currentTime;
	time(&currentTime);
	printf("current time is %s", ctime(&currentTime));
	alarm(alarmTime);
}
void hndlrSIGINT() {
	cPresses++;
	if(cPresses == exitPresses) {
		printf("\nFinal control-c caught. Exiting.\n");
		exit(0);
	}
	else {
		int difference = exitPresses - cPresses;
		printf("\nControl-c caught. %d more before program ends.\n", difference);
	}
}
int main() {
	//Initializes sigaction for SIGALRM
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = hndlrSIGALRM;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	//Initializes sigaction for SIGINT
	struct sigaction si;
	memset(&si, 0, sizeof(si));
	si.sa_handler = hndlrSIGINT;
	sigemptyset(&si.sa_mask);
	si.sa_flags = SA_RESTART;

	printf("Date will be printed every 3 seconds\n");
	printf("Enter ^C to end the program: \n");
	alarm(alarmTime);
	if(sigaction(SIGALRM, &sa, NULL) != 0) {
		printf("Error\n");
	}	
	if(sigaction(SIGINT, &si, NULL) != 0) {
		printf("Error\n");
	}
	while (1) {

	}
}
