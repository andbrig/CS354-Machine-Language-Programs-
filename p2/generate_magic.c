////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Other Files:      verify_magic.c
// Semester:         CS 354 Fall 2016
//
// Author:           Andrew Briggs
// Email:            briggs@cs.wisc.edu
// CS Login:         briggs
//

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(Square * square, char *filename);

int main(int argc, char *argv[])
{
	Square* square;
	int squareSize = 0;			
	// Check input arguments to get filename
	char *newFile = argv[1];
	// Get size from user
	squareSize = get_square_size();
	// Generate the magic square
	square = generate_magic(squareSize);
	// Write the square to the output file
	write_to_file(square, newFile);
	//Before ending the program, frees all memory dynamically used
	for(int i = 0; i < (*square).size; i++) {
		free(*((*square).array + i));
	}
	free((*square).array);
	free(square);
	return 0;
}

// get_square_size prompts the user for the magic square size
// checks if it is an odd number >= 3 and returns the number
int get_square_size()
{
	int number;
	printf("Enter size of magic square, must be odd\n");
	scanf("%d", &number);
	//ensures the number is odd and greater than or equal to 3
	if(number>=3) {
		if(number%2 == 0) {
			printf("Size must be an odd number >= 3.");
			exit(1);
		}
			
	}
	else
		exit(1);
	return number;
}

// generate_magic constructs a magic square of size n
// using the Siamese algorithm and returns the Square struct
Square * generate_magic(int n)
{
	Square* square;
	//Dynamically allocates memory for the square object
	square = (Square*)malloc(sizeof(Square));
	(*square).array = (int**)malloc(sizeof(int*) * n);
	for(int i = 0; i < n; i++) {
		*((*square).array + i) = (int *)calloc(n,sizeof(int));
	
	}
	//set #1 to first row middle column
	(*square).size = n;
	int size = n;
	int currentRow = 0;
	int currentColumn = size/2;
	*(*((*square).array + currentRow) + currentColumn) = 1;
	//Use the siamese algorithm for the rest
	for(int tempNum = 2; tempNum <= size*size; tempNum++) {
		//checks to see if potential space is occupied
		if(*(*((*square).array + ((currentRow + size -1) % n)) + ((currentColumn +1) % size)) == 0) {
			//moves up and to the right
			currentColumn = (currentColumn + 1) % size;
			currentRow = (currentRow + size - 1) % size;
		}
		else {
			//decrements row if space is filled
			currentRow = (currentRow + 1) % n;
		}
		//places the current number in currentRow and currentColumn
		*(*((*square).array + currentRow) + currentColumn) = tempNum; 
	}
	return square;
}

// write_to_file opens up a new file(or overwrites the existing file)
// and writes out the square in the format expected by verify_magic.c
void write_to_file(Square * square, char *filename)
{	
	FILE *file;
	file = fopen(filename, "w");
	int size = (*square).size;
	fprintf(file, "%d\n", size);
	for(int row = 0; row < size; row++) {
		for(int col = 0; col < size; col++) {
			//Checks to see if the pointer is at the end of the row, moves to next line if so
			if((col + 1) == size) {
				fprintf(file, "%d\n", *(*((*square).array + row) + col));
			}
			//otherwise simply outputs the number to the file
			else {
				fprintf(file, "%d,", *(*((*square).array + row) + col));
			}
		}
	}
	fclose(file);
}
