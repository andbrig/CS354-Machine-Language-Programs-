////////////////////////////////////////////////////////////////////////////////
// This File:        division.c
// Semester:         CS 354 Fall 2016
//
// Author:           Andrew Briggs
// Email:            briggs@cs.wisc.edu
// CS Login:         briggs
////////////////////////////////////////////////////////////////////////////////
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int numOperations = 0;

void divZeroHndlr() {
	printf("Error: a division by 0 operation was attempted.");
	printf("\nTotal number of operations completed: %d", numOperations);
	printf("\nThe program will be stopped.\n");
	exit(0);

}

void SIGINTHndlr() {
	printf("\nTotal number of operations successfully completed: %d", numOperations);
	printf("\nThe program will be halted.\n");
	exit(0);
}
int main() {
	int num1 = 0;
	int num2 = 0;
	int quotient = 0;
	int remainder = 0;
	char userInput [100];
	//Signal Handler for divide by zero
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = divZeroHndlr;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGFPE, &sa, NULL) != 0) {
		printf("Error");
	}
	//Signal Handler for control C
	struct sigaction si;
	memset(&si, 0, sizeof(si));
	si.sa_handler = SIGINTHndlr;
	sigemptyset(&si.sa_mask);
	si.sa_flags = SA_RESTART;
	if(sigaction(SIGINT, &si, NULL) != 0) {
		printf("Error");
	}
	//Infinite loop to do division between two numbers
	while(1) {
		printf("Enter first integer: ");
		fgets(userInput, 100, stdin);
		num1 = atoi(userInput);
		printf("Enter second integer: ");
		fgets(userInput, 100, stdin);
		num2 = atoi(userInput);
		quotient = (num1/num2);
		remainder = num1 % num2;
		numOperations++;	
		printf("%d/%d is %d with a remainder of %d\n", num1, num2, quotient, remainder);
	}
}
