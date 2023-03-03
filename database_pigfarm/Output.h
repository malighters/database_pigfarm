#pragma once

#include <stdio.h>
#include "Cell.h"
#include "Entities.h"

void printCell(struct Cell cell) {
	printf("Cell\'s name: %s\n", cell.name);
	printf("Cell\'s pig quantity: %d\n", cell.pigCount);
}

void printPig(struct Pig pig, struct Cell cell) {
	printf("Cell info: %s\n", cell.name);
	printf("Pig gender: %c\n", pig.gender);
	printf("Pig birhdate: %s\n", pig.birth_date);
}