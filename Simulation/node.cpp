#include "node.h"

std::vector<double> Node::solar;


Node::Node() {
	id = 0;
	x=0 , y = 0;
	status = Status::ACTIVE;
	mode = Mode::NORMAL;

	energy = MAX_ENERGY;
	line_is_right = 0;
	
	data_size = 0;
	num_data = 0;

	anchor = 0;
	next_node = 0;
}


Node::~Node() {

}


double Node::calc_send_energy(double size) {
	return size * TX;
}

double Node::calc_recv_energy(double size) {
	return size * RX;
}

double Node::calc_sleep_energy(double time) {
	/*double energy;

	energy = SLEEP_ENERGY * time;
	return energy;*/
	return SLEEP_ENERGY * time;
}

double Node::calc_active_energy(double time) {
	double energy = ACTIVE_ENERGY * time;
	return energy;
}

double Node::calc_harvest_energy(int time) {
	double energy = 0;

	int i = time % solar.size();

	energy += (solar[i] * PANEL);

	return energy;
}

bool Node::consume_idle_energy(int time) {
	double senergy = 0;
	double aenergy = 0;
	double henergy = 0;

	senergy = calc_sleep_energy((1 - DUTY_CYCLE) * MIN * TR_CYCLE);
	aenergy = calc_active_energy(DUTY_CYCLE * MIN * TR_CYCLE);
	henergy = calc_harvest_energy(time);

	if (senergy + aenergy > henergy) {
		energy = energy - senergy - aenergy + henergy;

		if (energy < BLACKOUT_ENERGY) {
			if (energy < 0)
				energy = 0;
			status = Status::BLACKOUT;
			// data_size = 0;
			return false;
		}
	}
	else {
		energy = energy - senergy - aenergy + henergy;

		if (energy < BLACKOUT_ENERGY) {
			status = Status::BLACKOUT;
			return false;
		}

		if (energy > MAX_ENERGY)
			energy = MAX_ENERGY;
		status = Status::ACTIVE;
		/*energy = MAX_ENERGY;*/
	}
	return true;
}

int Node::routing() {
	if (mode != Mode::NORMAL)
		return anchor->id;

	Node* big = NULL;
	std::queue<Node*> temp;
		
	if (line_is_right) {
		for (int i = 0; i < neighbor.size(); i++) {
			if (neighbor[i]->x > x) {
				temp.push(neighbor[i]);
			}
		}
	}
	else {
		for (int i = 0; i < neighbor.size(); i++) {
			if (neighbor[i]->x < x) {
				temp.push(neighbor[i]);
			}
		}

	}
	if (temp.empty()) {
		for (int i = 0; i < neighbor.size(); i++) {
			temp.push(neighbor[i]);
		}
	}
	Node* node1 = temp.front();
	temp.pop();
	big = node1;
	for (int i = 0; i < temp.size(); i++) {
		node1 = temp.front();
		temp.pop();
		big = abs(y - big->y) <= abs(y - node1->y) ? big : node1;
	}
	return big->id;
}

void Node::sensing() {
	if (status != Status::ACTIVE) {
		return;
	}

	data_size += SENSING_DATA * (int)TR_CYCLE/SENSING;
	num_data += (int)TR_CYCLE / SENSING;
}

bool Node::consume_energy(double consume) {
	if (consume <= 0)
		exit(1);
	energy -= consume;

	if (energy < BLACKOUT_ENERGY) {
		if(energy < 0)
			energy = 0;
		status = Status::BLACKOUT;
		//printf("I dead");
		return false;
	}
	return true;
}

void Node::consume_receive_around_energy(double size) {
	for (int i = 0; i < neighbor.size(); i++) {
		if (neighbor[i]->status == Status::ACTIVE)
			neighbor[i]->consume_energy(calc_recv_energy(size));
	}
}
