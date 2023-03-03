#pragma once
#include <stdio.h>
#include <string.h>
#include "Entities.h"
#include "Cell.h"

void inputCell(struct Cell* cell) {
	char name[16];

	name[0] = '\0';
	printf("Enter cell\'s name: ");
	scanf("%s", name);
	strcpy(cell->name, name);
}

void inputPig(struct Pig* pig) {
	char gender;
	char birth_date[16];

	birth_date[0] = '\0';
	printf("Enter pig gender: ");
	scanf(" %c", &gender);
	pig->gender = gender;

	printf("Enter pig birthdate: ");
	scanf("%s", &birth_date);
	strcpy(pig->birth_date, birth_date);

	
}