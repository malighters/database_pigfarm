#pragma once

struct Cell {
	int id;
	char name[16];
	long firstPigAddress;
	int pigCount;
};

struct Pig {
	int cellId;
	int pigId;
	char gender;
	char birth_date[16];
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer {
	int id;	
	int address;
	int exists;
};