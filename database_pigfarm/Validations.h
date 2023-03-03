#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Pig.h"

int getCell(struct Cell* cell, int id, char* error);

void loopThroughCells(int indAmount, struct Cell* cell, char *dummy, int *cellCount, int *pigCount);

int checkFileExists(FILE* indexTable, FILE* database, char* error) {
	if (indexTable == NULL || database == NULL) {
		strcpy(error, "DB files do not exits");
		return 0;
	}
	return 1;
}

int checkIndexExists(FILE* indexTable, char* error, int id) {
	fseek(indexTable, 0, SEEK_END);
	long indexTableSize = ftell(indexTable);
	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize) {
		strcpy(error, "no such ID in table");
		return 0;
	}
	return 1;
}

int checkRecordExists(struct Indexer indexer, char* error) {
	if (!indexer.exists) {
		strcpy(error, "the record has been deleted");
		return 0;
	}
	return 1;
}

int checkKeyPairUnique(struct Cell cell, int pigId) {
	FILE* pigDb = fopen(PIG_DATA, "a+b");
	struct Pig pig;
	fseek(pigDb, cell.firstPigAddress, SEEK_SET);
	for (int i = 0; i < cell .pigCount; i++) {
		fread(&pig, PIG_SIZE, 1, pigDb);
		fclose(pigDb);
		if (pig.pigId == pigId) {
			return 0;
		}
        pigDb = fopen(PIG_DATA, "r+b");
		fseek(pigDb, pig.nextAddress, SEEK_SET);
	}
	fclose(pigDb);
	return 1;
}

void info() {
	FILE* indexTable = fopen("cell.ind", "rb");
	if (indexTable == NULL) {
		printf("Error: database files do not exist\n");
		return;
	}
	int cellCount = 0;
	int pigCount = 0;
	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);
	struct Cell cell;
	char dummy[51];
    loopThroughCells(indAmount, &cell, dummy, &cellCount, &pigCount);
    fclose(indexTable);
	printf("Total cells: %d\n", cellCount);
	printf("Total pigs: %d\n", pigCount);
}

void loopThroughCells(int indAmount, struct Cell* cell, char* dummy, int* cellCount, int* pigCount) {
    for (int i = 1; i <= indAmount; i++) {
        if (getCell(cell, i, dummy)) {
            (*cellCount)++;
            (*pigCount) += (*cell).pigCount;
            printf("Cell #%d has %d pigs\n", i, (*cell).pigCount);
        }
    }
}
