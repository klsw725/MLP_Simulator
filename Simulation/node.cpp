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
	
	data_size = 0;
	num_data = 0;

	anchor = NULL;
	next_node = 0;
}

Node::Node(FILE* fp) {

}

Node::~Node() {

}


double Node::calc_send_energy(double size) {
	return size * tx;
}

double Node::calc_recv_energy(double size) {
	return size * rx;
}

double Node::calc_sleep_energy(Node *n, double time) {
	double energy;

	//if (n->status == SLEEP) {
		energy = SLEEP_ENERGY * time;
		return energy;
	//}
	
	//energy = SLEEP_ENERGY * time + 
	
}

double Node::calc_active_energy(Node* n, double time) {
	double energy = ACTIVE_ENERGY * time;
	return energy;
}

int Node::consume_idle_energy(Node *n) {
	double senergy;
	double aenergy;
	double henergy;

	senergy = Node::calc_sleep_energy(n, (1 - DUTY_CYCLE) * MIN * TR_CYCLE);
	aenergy = Node::calc_active_energy(n, DUTY_CYCLE * MIN * TR_CYCLE);
	henergy = Node::calc_harvest_energy(n, MIN * TR_CYCLE);

	if (senergy + aenergy > henergy) {
		n->energy = n->energy - senergy - aenergy + henergy;

		if (n->energy < 0) {
			n->energy = 0;
			n->status = BLACKOUT;
			return false;
		}
	}
	else {
		n->energy = n->energy - senergy - aenergy + henergy;

		if (n->energy > n->max_energy) {
			n->energy = n->max_energy;
			return false;
		}
	}
	return true;
}

double Node::calc_harvest_energy(Node* n, double time) {
	return 0;
}

bool Node::consume_energy(double consume) {
	if (consume <= 0)
		exit(1);
	energy -= consume;

	if (energy < 0) {
		energy = 0;
		status = BLACKOUT;
		//printf("I dead");
		return false;
	}

	return true;
}

int Node::routing() {
	if (mode != NORMAL)
		return anchor->id;

	Node* big = NULL;
	std::queue<Node*> temp;
	for (int i = 0; i < neighbor.size(); i++) {
		if (line_is_right) {
			if (neighbor[i]->x > x && neighbor[i]->x < FIELD_SIZE/2)
				temp.push(neighbor[i]);
		}
		else {
			if (neighbor[i]->x < x && neighbor[i]->x > FIELD_SIZE / 2)
				temp.push(neighbor[i]);
		}
	}
	Node* node1 = temp.front();
	temp.pop();
	big = node1;
	for (int i = 0; i < temp.size(); i++) {
		node1 = temp.front();
		temp.pop();
		big = abs(y - big->y) < abs(y - node1->y) ? big : node1;
	}
	return big->id;
}

void Node::sensing() {
	if (status != ACTIVE) {
		return;
	}

	data_size += SENSING_DATA * (int)TR_CYCLE/SENSING;
	num_data += (int)TR_CYCLE / SENSING;
}
