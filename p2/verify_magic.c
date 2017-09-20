////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        verify_magic.c
// Other Files:      generate_magic.c
// Semester:         CS 354 Fall 2016
//
// Author:           Andrew Briggs
// Email:            briggs@cs.wisc.edu
// CS Login:         briggs
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

Square * construct_square(char *filename);
int verify_magic(Square * square);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	char *newFile = argv[1];
	// Construct square
	Square* square = construct_square(newFile); 
	// Verify if it's a magic square and print true or false
	if(verify_magic(square) == 1) {
		printf("true\n");
	}
	else
		printf("false\n");
	//Free all heap memory used
	for(int i = 0; i < (*square).size; i++) {
		free(*((*square).array + i));
	}
	free((*square).array);
	free(square);
	return 0;
}

// construct_square reads the input file to initialize a square struct
// from the contents of the file and returns the square.
// The format of the file is defined in the assignment specifications
Square * construct_square(char *filename)
{
	Square* square;
	FILE *file;
	//open the file
 	file = fopen(filename, "r");
	//ensure the input file is opened correctly
	if(file == NULL) {
		fprintf(stderr, "Cannot open file for reading.\n");
		exit(1);
	}
	int numChars = 0;
	int squareSize = 0;
	char line[80];
	int row = 0;
	int column = 0;
	char tempChar;
    	// Read the first line to get the square size
	while((tempChar = fgetc(file)) != '\n') { 
		line[numChars] = tempChar;
	}
	squareSize = atoi(line);
	line[0] = '\0';
	numChars = 0;
 	// Initialize a new Square struct of that size
	square = (Square*)malloc(sizeof(Square));
	//dynamically allocate all memory necessary
	(*square).array = (int**)malloc(sizeof(int*) * squareSize);
	for(int i = 0; i < squareSize; i++) {
		*((*square).array +i) = (int *)malloc(sizeof(int) * squareSize);
	}
    	(*square).size = squareSize;
	
 	// Read the rest of the file to fill up the square
	while((tempChar = fgetc(file)) != EOF) {
		if(tempChar == ',') {
			int tempInt2 = atoi(line);
			//add each element to fill up the square
                        *(*((*square).array + row) + column) = tempInt2;
                        numChars = 0;
			//sets every element to null
			for(int i = 0; i < numChars; i++) {
                        	line[0] = '\0';
			}
                        column++; 
		}
		//if a newline is encountered, go to the next row
		else if(tempChar == '\n') {
			*(*((*square).array + row) + column) = atoi(line);
			numChars = 0;
			for(int i = 0; i < numChars; i++) {
				line[0] = '\0';
			}
			row++;
			column = 0;
		}
		else {
			line[numChars] = tempChar;
                        numChars++;
		}
	}
	//Possibly close it
	fclose(file);
 	return square;
}

// verify_magic verifies if the square is a magic square
// returns 1(true) or 0(false)
int verify_magic(Square * square)
{	
	//local variables to aid in the method
	int size =(*square).size;
	int rowTotal = 0;
	int rowComparisonTotal = 0;
	int columnTotal = 0;
	int columnComparisonTotal = 0;
	//loops through checking all the rows for equality
	for(int i = 0; i < size; i++) {
		rowTotal = 0;
		for(int j = 0; j < size; j++) {
			rowTotal = rowTotal + *(*((*square).array + i) + j);
		}
		if(i == 0)
			rowComparisonTotal = rowTotal;
		if(rowTotal != rowComparisonTotal) {
			return 0;
		} 	
	}
	// Check all cols sum to same number
	for(int i = 0; i < size; i++) {
		columnTotal = 0;
		for(int j = 0; j < size; j++) {
			columnTotal = columnTotal + *(*((*square).array + j) + i);
			}
			if(i == 0)
				columnComparisonTotal = columnTotal;
			//compares current column total to the first column
			if(columnTotal != columnComparisonTotal)
				return 0;
	}

	// Check main diagonal
	int rowPos = 0;
	int colPos = 0;
 	int currentVal = 0;
	//incrementing while loop to check first diagonal
	while(colPos < size) {
		currentVal = currentVal + *(*((*square).array + rowPos) + colPos);
		rowPos++;
		colPos++;
	}
	
	//Decrementing while loop to check secondary diagonal
	rowPos = 0;
	colPos = size - 1;
	int currentVal2 = 0;
	while (colPos >= 0) {
		currentVal2 = currentVal2 + *(*((*square).array + rowPos) + colPos);
		rowPos++;
		colPos--;
	}
	//compares diagonals to each other
	if(currentVal != currentVal2) 
		return 0;	
	return 1;
}
