#include "node.h"

std::vector<long double> Node::solar;


Node::Node() {
	id = 0;
	x=0 , y = 0;
	status = Status::ACTIVE;
	mode = Mode::NORMAL;

	energy = MAX_ENERGY * START_ENERGY;

	prevEnergy = energy;
	prevHarvest = 0;

	avgEnergy = prevEnergy;
	avgHarvest = prevHarvest;

	line_is_right = false;
	can_receive = true;
	can_transmit = true;

	baudrate = MAX_TRANSMIT;
	threshold = energy * THRESHOLD;
	numblackout = 0;

	data_size = 0;
	num_data = 0;

	anchor = 0;
	next_node = 0;
}


Node::~Node() {

}


double Node::calc_send_energy(double size) {
	return size * BETA_DEF * pow(TR, ALPHA_DEF);
}

double Node::calc_recv_energy(double size) {
	return size * RX;
}

double Node::calc_sleep_energy(double time) {
	return SLEEP_ENERGY * time;
}

double Node::calc_active_energy(double time) {
	return ACTIVE_ENERGY * time;
}

double Node::calc_harvest_energy(int time) {
	double energy = 0;

	int i = time % solar.size();

	if(time >= 1440 && 2880 > time) // 5일 ~ 10일 비
		energy = (solar[i] * PANEL * STORM) ;
	else if (time >= 7200 && 8640 > time) // 25일 ~ 30일 구름
		energy = (solar[i] * PANEL * CLOUD);
	else
		energy = (solar[i] * (double)PANEL);

	return energy;
}

bool Node::consume_idle_energy(int time) {
	double senergy = 0;
	double aenergy = 0;
	double henergy = 0;

	senergy = calc_sleep_energy((1 - DUTY_CYCLE) * MIN * TR_CYCLE);
	aenergy = calc_active_energy(DUTY_CYCLE * MIN * TR_CYCLE);
	henergy = calc_harvest_energy(time);

	if (status != Status::ACTIVE)
	{
		energy = energy - senergy;
	}
	else {
		energy = energy - senergy - aenergy;
	}

	/*double consume = prevEnergy - henergy - energy;

	avgEnergy = (avgEnergy + abs(consume)) / 2;
	avgHarvest = (avgHarvest + henergy) / 2;*/

	energy += henergy;

	/*prevEnergy = energy;
	prevHarvest = henergy;

	threshold = avgEnergy / avgHarvest * energy;*/

	threshold = energy * THRESHOLD;
	//printf("threshold : %lf\n", threshold);

	if (energy < BLACKOUT_ENERGY) 
	{
		if (energy < 0)
			energy = 0;
		status = Status::BLACKOUT;
		numblackout++;
		// data_size = 0;
		return false;
	}
	else 
	{
		if (energy > MAX_ENERGY)
			energy = MAX_ENERGY;

		status = Status::ACTIVE;

	}
	return true;
}

Node* Node::routing() {
	if (mode != Mode::NORMAL)
		return anchor;

	Node* big = NULL;
	std::queue<Node*> temp;
		
	if (line_is_right) {
		for (int i = 0; i < neighbor.size(); i++) {
			if (neighbor[i]->x > x && status==Status::ACTIVE) {
				temp.push(neighbor[i]);
			}
		}
	}
	else {
		for (int i = 0; i < neighbor.size(); i++) {
			if (neighbor[i]->x < x && status == Status::ACTIVE) {
				temp.push(neighbor[i]);
			}
		}

	}
	if (temp.empty()) {
		/*for (int i = 0; i < neighbor.size(); i++) {
			temp.push(neighbor[i]);
		}*/
		return NULL;
	}
	Node* node1 = temp.front();
	temp.pop();
	big = node1;
	for (int i = 0; i < temp.size(); i++) {
		node1 = temp.front();
		temp.pop();
		big = abs(y - big->y) <= abs(y - node1->y) ? big : node1;
	}
	return big;
}

void Node::sensing() {
	if (status != Status::ACTIVE) {
		return;
	}

	//if ((data_size + SENSING_DATA * (int)TR_CYCLE / SENSING) > MEMORY)
	//{
	//	can_receive = false;
	//	return;
	//}

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
		//numblackout++;
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