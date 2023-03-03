#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Entities.h"
#include "Validations.h"
#include "Cell.h"

#define PIG_DATA "pig.fl"
#define PIG_GARBAGE "pig_garbage.txt"
#define PIG_SIZE sizeof(struct Pig)

struct Pig linkLoop(FILE *database, struct Cell* cell, struct Pig *previous);

int updateCell(struct Cell cell, char* error);

struct Pig linkLoop(FILE *database, struct Cell *cell, struct Pig *previous) {
    for (int i = 0; i < (*cell).pigCount; i++) {
        fread(previous, PIG_SIZE, 1, database);
        fseek(database, (*previous).nextAddress, SEEK_SET);
    }
    return (*previous);
}

void reopenDatabase(FILE* database) {
	fclose(database);
	database = fopen(PIG_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Cell cell, struct Pig pig) {
	reopenDatabase(database);
	struct Pig previous;
	fseek(database, cell.firstPigAddress, SEEK_SET);
    previous = linkLoop(database, &cell, &previous);
    previous.nextAddress = pig.selfAddress;
	fwrite(&previous, PIG_SIZE, 1, database);
}

void relinkAddresses(FILE* database, struct Pig previous, struct Pig pig, struct Cell* cell) {
	if (pig.selfAddress == cell->firstPigAddress) {
		if (pig.selfAddress == pig.nextAddress) {
            cell->firstPigAddress = -1;
		} else {
            cell->firstPigAddress = pig.nextAddress;
		}
	} else {
		if (pig.selfAddress == pig.nextAddress) {
			previous.nextAddress = previous.selfAddress;
		}
		else {
			previous.nextAddress = pig.nextAddress;
		}
		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, PIG_SIZE, 1, database);
	}
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Pig* record) {
	long* deletedIds = (long * )malloc(garbageCount * sizeof(long));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%ld", deletedIds + i);
	}

	record->selfAddress = deletedIds[0];
	record->nextAddress = deletedIds[0];

	fclose(garbageZone);
	fopen(PIG_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %ld", deletedIds[i]);
	}

	free(deletedIds);
	fclose(garbageZone);
}

void noteDeletedPig(long address) {
    FILE* garbageZone = fopen(PIG_GARBAGE, "rb");
    int garbageCount;
    fscanf(garbageZone, "%d", &garbageCount);
    long* deletedAddresses = (long * )malloc(garbageCount * sizeof(long));

    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%ld", deletedAddresses + i);
    }

    fclose(garbageZone);
    garbageZone = fopen(PIG_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount + 1);

    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbageZone, " %ld", deletedAddresses[i]);
    }

    fprintf(garbageZone, " %ld", address);
    free(deletedAddresses);
    fclose(garbageZone);
}

int getPig(struct Cell cell, struct Pig* pig, int pigId, char* error) {
    if (!cell.pigCount) {
        strcpy(error, "The cell is empty");
        return 0;
    }
    FILE* database = fopen(PIG_DATA, "rb");
	fseek(database, cell .firstPigAddress, SEEK_SET);
    fread(pig, PIG_SIZE, 1, database);

    for (int i = 0; i < cell.pigCount; i++) {
        if (pig->pigId == pigId) {
            fclose(database);
            return 1;
        }
        fseek(database, pig->nextAddress, SEEK_SET);
        fread(pig, PIG_SIZE, 1, database);
    }
    strcpy(error, "No such pig in database");
    fclose(database);
    return 0;
}

int insertPig(struct Cell cell, struct Pig pig, char* error) {
    pig.exists = 1;
	FILE* database = fopen(PIG_DATA, "a+b");
	FILE* garbageZone = fopen(PIG_GARBAGE, "a+b");
	int garbageCount;											

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageAddress(garbageCount, garbageZone, &pig);
		reopenDatabase(database);
		fseek(database, pig.selfAddress, SEEK_SET);
	} else {
		fseek(database, 0, SEEK_END);
		int dbSize = ftell(database);
        pig.selfAddress = dbSize;
		pig.nextAddress = dbSize;
	}

	fwrite(&pig, PIG_SIZE, 1, database);
	if (!cell.pigCount) {
		cell.firstPigAddress = pig.selfAddress;
	} else {
		linkAddresses(database, cell, pig);
	}
	fclose(database);
	cell.pigCount++;
    updateCell(cell, error);
	return 1;
}

int updatePig(struct Pig pig, int pigId) {
	FILE* database = fopen(PIG_DATA, "r+b");
	fseek(database, pig.selfAddress, SEEK_SET);
	fwrite(&pig, PIG_SIZE, 1, database);
	fclose(database);
	return 1;
}

void deletePig(struct Cell cell, struct Pig pig, int pigId, char* error) {
	FILE* database = fopen(PIG_DATA, "r+b");
	struct Pig previous;
	fseek(database, cell.firstPigAddress, SEEK_SET);
	do {
		fread(&previous, PIG_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}
	while (previous.nextAddress != pig.selfAddress && pig.selfAddress != cell.firstPigAddress);

	relinkAddresses(database, previous, pig, &cell);
    noteDeletedPig(pig.selfAddress);
	pig.exists = 0;

	fseek(database, pig.selfAddress, SEEK_SET);
	fwrite(&pig, PIG_SIZE, 1, database);
	fclose(database);

	cell.pigCount--;
    updateCell(cell, error);
}