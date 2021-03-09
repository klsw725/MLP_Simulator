#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>

#include "main.h"
#include "simulator.h"

int count = 0;

int main(void) {
	srand(time(NULL));
	Simulator *simu = new Simulator();

	FILE* fp1, *fp2, *fp3, *fp4;
	fp1 = fopen("OLP1000_10_5.csv", "w");
	fp2 = fopen("MLP1000_10_5.csv", "w");
	fp3 = fopen("SLP1000_10_5.csv", "w");
	fp4 = fopen("CLP1000_10_5.csv", "w");

	simu->start_lbdd_simulator(fp1);
	std::cout << count << std::endl;

	Simulator* simu2 = new Simulator(simu);
	simu2->start_simulator(fp2);
	std::cout << count << std::endl;
	delete simu2;

	Simulator* simu3 = new Simulator(simu);
	simu3->start_shift_simulator(fp3);
	std::cout << count << std::endl;
	delete simu3;

	Simulator* simu4 = new Simulator(simu, 4);
	simu4->start_larcms_simulator(fp4);
	std::cout << count << std::endl;
	delete simu4;

	//ThreadSimul* test = new ThreadSimul();

	//test->Run();
}