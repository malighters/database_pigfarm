#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Validations.h"
#include "Pig.h"

#define CELL_IND "cell.ind"
#define CELL_DATA "cell.fl"
#define CELL_GARBAGE "cell_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define CELL_SIZE sizeof(struct Cell)

void noteDeletedCell(int id) {
	FILE* garbageZone = fopen(CELL_GARBAGE, "rb");
	int garbageAmount;
	fscanf(garbageZone, "%d", &garbageAmount);
	int* deleteIds = (int *)malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(CELL_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	fprintf(garbageZone, " %d", id);
	free(deleteIds);
	fclose(garbageZone);
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Cell* record) {
	int* deleteIds = (int*)malloc(garbageCount * sizeof(int));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}
	record->id = deleteIds[0];

	fclose(garbageZone);
	fopen(CELL_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	free(deleteIds);
	fclose(garbageZone);
}

int insertCell(struct Cell record) {
	FILE* indexTable = fopen(CELL_IND, "a+b");
	FILE* database = fopen(CELL_DATA, "a+b");
	FILE* garbageZone = fopen(CELL_GARBAGE, "a+b");
	struct Indexer indexer;
	int garbageCount = 0;
	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageId(garbageCount, garbageZone, &record);
		fclose(indexTable);
		fclose(database);
		indexTable = fopen(CELL_IND, "r+b");
		database = fopen(CELL_DATA, "r+b");
		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);
	} else {
		long indexerSize = INDEXER_SIZE;
		fseek(indexTable, 0, SEEK_END);
		if (ftell(indexTable)) {
			fseek(indexTable, -indexerSize, SEEK_END);
			fread(&indexer, INDEXER_SIZE, 1, indexTable);
			record.id = indexer.id + 1;
		} else {
			record.id = 1;
		}
	}
	record.firstPigAddress = -1;
	record.pigCount = 0;
	fwrite(&record, CELL_SIZE, 1, database);

	indexer.id = record.id;
	indexer.address = (record.id - 1) * CELL_SIZE;
	indexer.exists = 1;
	printf("Cell id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET); 
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int getCell(struct Cell* cell, int id, char* error) {
	FILE* indexTable = fopen(CELL_IND, "rb");
	FILE* database = fopen(CELL_DATA, "rb");
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	struct Indexer indexer;
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fread(cell, sizeof(struct Cell), 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int updateCell(struct Cell cell, char* error) {
	FILE* indexTable = fopen(CELL_IND, "r+b");
	FILE* database = fopen(CELL_DATA, "r+b");
    struct Indexer indexer;
    int id = cell.id;
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fwrite(&cell, CELL_SIZE, 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int deleteCell(int id, char* error) {
	FILE* indexTable = fopen(CELL_IND, "r+b");
    struct Cell cell;
    struct Indexer indexer;
    if (indexTable == NULL) {
		strcpy(error, "database files are not created yet");
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
    getCell(&cell, id, error);
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	indexer.exists = 0;
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
    noteDeletedCell(id);
	
	if (cell.pigCount) {
		FILE* pigDb = fopen(PIG_DATA, "r+b");
		struct Pig pig;
		fseek(pigDb, cell.firstPigAddress, SEEK_SET);
		for (int i = 0; i < cell.pigCount; i++) {
			fread(&pig, PIG_SIZE, 1, pigDb);
			fclose(pigDb);
            deletePig(cell, pig, pig.pigId, error);
            pigDb = fopen(PIG_DATA, "r+b");
			fseek(pigDb, pig.nextAddress, SEEK_SET);
		}
		fclose(pigDb);
	}
	return 1;
}