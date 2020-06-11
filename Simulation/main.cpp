#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>

#include "main.h"
#include "simulator.h"

int count = 0;
int main(void) {
	Simulator *simu = new Simulator();

	FILE* fp;
	fp = fopen("test.csv","w");

	for (int i = 0; i < FIELD_SIZE; i++) {
		for (int j = 0; j < FIELD_SIZE; j++) {
			
			if (simu->field[i][j]) {
				fprintf(fp, "%d,%d,%d\n", count, i, j);
				std::cout << "бс";
				count++;
			}
			else {
				std::cout << "  ";
			}
		}
		std::cout << std::endl;
	}

	simu->start_simulator();
	std::cout << count << std::endl;

}