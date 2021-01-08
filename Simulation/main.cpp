#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>

#include "main.h"
#include "simulator.h"

int count = 0;
int main(void) {
	srand(time(NULL));
	Simulator *simu = new Simulator();
	Simulator* simu2 = new Simulator(simu);
	Simulator* simu3 = new Simulator(simu);
	FILE* fp1, *fp2, *fp3;
	fp1 = fopen("OLP1000_10_5.csv", "w");
	fp2 = fopen("MLP1000_10_5.csv", "w");
	fp3 = fopen("SLP1000_10_5.csv", "w");
	
	simu->start_simulator(fp1);
	std::cout << count << std::endl;
	
	simu2->start_simulator2(fp2);
	std::cout << count << std::endl;
	
	
}