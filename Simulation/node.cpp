#include "node.h"

double Node::tr = TR;
double Node::max_energy = MAX_ENERGY;			// J단위 (200mAh)
double Node::tx = TX;					// tx energy J/byte
double Node::rx = RX;					// rx energy J/byte
double Node::data_rate = DATA_RATE;			// bytes/S -> 초당 보내는 것은 1/data_rate
double Node::active_energy = ACTIVE_ENERGY;		// J/s
double Node::sleep_energy = SLEEP_ENERGY;		// J/s
double Node::max_data_size = MAX_DATA;		// 128 바이트



Node::Node() {
	id = 0;
	x, y = 0;
	status = ACTIVE;
	mode = NORMAL;

	energy = max_energy;
	line_is_right = 0;
	
	anchor = NULL;
	memory = NULL;
}

Node::Node(FILE* fp) {

}

Node::~Node() {

}

double Node::calc_time(double size) {
	return size * 1 / data_rate;
}

double Node::calc_send_energy(double size) {
	return size * tx;
}

double Node::calc_recv_energy(double size) {
	return size * rx;
}

double Node::calc_sleep_energy(double time) {
	return time * sleep_energy;
}

bool Node::consume_energy(double consume) {
	if (consume <= 0)
		exit(1);
	energy -= consume;

	if (energy < 0) {
		energy = 0;
		status = BLACKOUT;
		printf("I dead");
		return false;
	}

	return true;
}

int Node::routing() {
	if (mode != NORMAL)
		return anchor->id;

	Node* big = NULL;
	std::vector<Node*> temp;
	for (int i = 0; i < neighbor.size(); i++) {
		if (line_is_right) {
			if (neighbor[i]->x < x)
				continue;
			temp.push_back(neighbor[i]);
		}
		else {
			if (neighbor[i]->x > x)
				continue;
			temp.push_back(neighbor[i]);
		}
	}
	big = temp[rand() % temp.size()];
	return big->id;
}

void Node::sensing() {
	if (status != ACTIVE) {
		return;
	}

	if (memory != NULL) {
		memory->size += SENSING_DATA;
		return;
	}

	Packet* packet = new Packet;

	packet->from = this->id;
	packet->to = routing();
	packet->size = SENSING_DATA;
	
	memory = packet;
}
