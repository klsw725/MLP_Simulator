#ifndef __DRONE_H__
#define __DRONE_H__

class Drone {
public:
	Drone() :energy(0), data(0) {}

	double energy;				// 현재 남은 에너지 (J)
	int data;

};

#endif