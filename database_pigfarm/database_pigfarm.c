#include <stdio.h>
#include "Entities.h"
#include "Cell.h"
#include "Pig.h"
#include "Input.h"
#include "Output.h"

int main() {
	struct Cell cell;
	struct Pig pig;
	while (1) {
		int choice;
		int id;
		char error[51];
		printf("Enter the command:\n0 exit\n1 insert cell\n2 get cell\n3 update cell\n4 delete cell\n5 insert pig\n6 get pig\n7 update pig\n8 delete pig\n9 all information\n");
		printf("Enter the number to continue\n");

		scanf("%d", &choice);
		switch (choice) {
		case 0:
			return 0;
		case 1:
			inputCell(&cell);
			insertCell(cell);
			break;
		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			if (getCell(&cell, id, error))
				printCell(cell);
			else
				printf("Error: %s\n", error);
			break;
		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			cell.id = id;
			inputCell(&cell);
			updateCell(cell, error) ? printf("Successful update\n") : printf("Error: % s\n", error);
			break;
		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteCell(id, error) ? printf("Successful delete\n") : printf("Error: % s\n", error);
			break;
		case 5:
			printf("Enter cell ID: ");
			scanf("%d", &id);
			if (getCell(&cell, id, error)) {
				pig.cellId = id;
				printf("Enter pig ID: ");
				scanf("%d", &id);
				if (checkKeyPairUnique(cell, id)) {
					pig.pigId = id;
					inputPig(&pig);
					insertPig(cell, pig, error);
					printf("Successful insertion\n");
				}
				else {
					printf("Error: non-unique key\n");
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 6:
			printf("Enter cell ID: ");
			scanf("%d", &id);
			if (getCell(&cell, id, error)) {
				printf("Enter pig ID: ");
				scanf("%d", &id);
				if (getPig(cell, &pig, id, error))
					printPig(pig, cell);
				else
					printf("Error: %s\n", error);
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 7:
			printf("Enter cell ID: ");
			scanf("%d", &id);
			if (getCell(&cell, id, error)) {
				printf("Enter pig ID: ");
				scanf("%d", &id);
				if (getPig(cell, &pig, id, error)) {
					inputPig(&pig);
					updatePig(pig, id);
					printf("Successful update\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 8:
			printf("Enter cell ID: ");
			scanf("%d", &id);
			if (getCell(&cell, id, error)) {
				printf("Enter pig ID: ");
				scanf("%d", &id);
				if (getPig(cell, &pig, id, error)) {
					deletePig(cell, pig, id, error);
					printf("Successful delete\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 9:
			info();
			break;
		default:
			printf("Invalid input\n");
		}
		printf("\n");
	}
}