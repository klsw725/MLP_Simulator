#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <iterator>
#include <random>

#include "simulator.h"

Simulator::Simulator() {
	prevSinkdata = 0;

	lines.push_back(new Line());
	lines[0]->start = (FIELD_SIZE / W) / 2 * W;
	lines[0]->end = lines[0]->start + W-1;

	lines.push_back(new Line());
	lines.push_back(new Line());
	lines[2]->start = lines[1]->start = -lines[0]->start;
	lines[2]->end = lines[1]->end = lines[0]->end;

	create_field();
	init_nodes();

	find_inlines(*lines[0]);

	routing_all();

	drone = new Drone;

	FILE* fp;
	fp = fopen("solar_10min.txt", "r");
	char inp[1024];
	int buf_size = 1024;
	while (fgets(inp, buf_size, fp)) {
		Node::getsolar().push_back(atof(inp));
	}
	fclose(fp);
}

Simulator::Simulator(Simulator* simu) {
	lines = simu->lines;
	for (int i = 0; i < FIELD_SIZE; i++) {
		for (int j = 0; j < FIELD_SIZE; j++) {
			field[i][j] = simu->field[i][j];
		}
	}
	init_nodes();

	find_inlines(*lines[0]);

	routing_all();
	drone = new Drone;
}

Simulator::~Simulator() {

}


void Simulator::create_field() {
	//srand(time(NULL));

	for (int i = 0; i < FIELD_SIZE; i++) {			// 필드 초기화
		for (int j = 0; j < FIELD_SIZE; j++) {
			field[i][j] = 0;
		}
	}

	int density = pow(FIELD_SIZE, 2)/NODES + 1;		// 노드가 들어가야할 단위 면적
	density = sqrt(density)+1;						// 면적의 길이

	while ((double)density > TR) {		// 전송 거리보다 노드 단위당 면적의 길이가 크다면 하나씩 줄여나감
		density -= 1;
	}

	int x = 0, y = 0;
	bool equal = 0;			// 균등하게 하나씩 들어갔는지 판단. 균등하게 다 들어갔으면 그 다음부터는 아얘 랜덤하게 배분.
	int indexX=0, indexY=0;

	for (int count = 0; count < NODES; count++) {
		int i = rand() % density;
		int j = rand() % density;

		if (!equal) {
			indexX = density * x + i;
			indexY = density * y + j;
			if (indexX >= FIELD_SIZE) { // index 가 필드 사이즈를 넘었을 경우
				int prevx = x;
				x = 0; y++;
				if (density * prevx < FIELD_SIZE)		// index가 필드사이즈와 딱 맞는 경우 판별 (index == FIELD_SIZE)->배열에서는 0-FIELD_SIZE-1이므로 동일하면 문제
					indexX = rand() % (indexX - FIELD_SIZE + 1) + density * prevx;
				else {
					count--;
					continue;
				}

			}
			if (indexY >= FIELD_SIZE) {				// Y를 넘어갔다는 것은 한번은 다 돌았다는 뜻, 균등하게 하나씩 다들어갔다는 말.
				equal = 1;
				count--;
				continue;
			}
		}
		else {	// 랜덤하게 노드 배치
			indexX = rand() % FIELD_SIZE;
			indexY = rand() % FIELD_SIZE;
		}

		if (field[indexY][indexX]) {	// 동일한 위치에 있으면 다시 검사
			count--;
		}

		field[indexY][indexX] = 1;
		x++;
	}
	 
}

double get_distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void Simulator::init_nodes() {
	//FILE* fp = fopen("spec.txt", "r");
	for (int i = 0; i < NODES; i++) {
		//nodes[i] = new Node(fp);
		nodes.push_back(new Node());
	}

	int index = 0;
	for (int y = 0; y < FIELD_SIZE; y++) {
		for (int x = 0; x < FIELD_SIZE; x++) {
			if (field[y][x]) {
				nodes[index]->id = index;
				nodes[index]->set_locate(x, y);
				if (x > FIELD_SIZE/2)
					nodes[index]->line_is_right = false;
				else
					nodes[index]->line_is_right = true;
				index++;
			}
		}
	}

	for (int i = 0; i < NODES; i++) {
		for (int j = i + 1; j < NODES; j++) {
			double distance = get_distance(nodes[i]->x, nodes[i]->y, nodes[j]->x, nodes[j]->y);
			if (distance <= TR) {
				nodes[i]->neighbor.push_back(nodes[j]);
				nodes[j]->neighbor.push_back(nodes[i]);
			}
		}
	}
}

void Simulator::set_anchor(Cell* cell) {
	std::queue<Node*>temp;
	Node* big = NULL;
	Node* node1 = NULL;

	for (int i = 0; i < cell->inlines.size(); i++) {
		if (cell->inlines[i]->energy > THRESHOLD) {
			temp.push(cell->inlines[i]);
		}
	}

	if (temp.empty()) {
		big = cell->inlines[0];
		for (int i = 1; i < cell->inlines.size(); i++) {
			node1 = cell->inlines[i];
			big = big->energy > node1->energy ? big : node1;
		}
	}
	else {
		big = temp.front();
		temp.pop();

		for (int i = 0; i < temp.size(); i++) {
			node1 = temp.front();
			temp.pop();
			big = big->energy > node1->energy ? big : node1;
		}
	}
	big->mode = Mode::ANCHOR;
	big->anchor = big;

	cell->anchor = big;
	for (int i = 0; i < cell->inlines.size(); i++) {
		cell->inlines[i]->anchor = big;
		cell->inlines[i]->next_node = big->id;
	}


	//	if (cell->anchor != NULL) {
	//		cell->inlines[i]->anchor = cell->anchor;
	//		continue;
	//	}
	//	if (cell->inlines[i]->energy > THRESHOLD) {  /* 식 성립 */
	//		cell->anchor = cell->inlines[i];
	//		cell->anchor->mode = ANCHOR;
	//		cell->anchor->anchor = cell->anchor;
	//	}
	//}

}

void Simulator::find_inlines(Line line) {
	for (int y = 0; y * G < FIELD_SIZE; y++) {
		Cell* temp = new Cell;
		for (int i = 0; i < NODES; i++) {
			if (nodes[i]->x >= line.start && nodes[i]->x <= line.end && nodes[i]->y >= y * G && nodes[i]->y < (y+1)*G) {
				nodes[i]->mode = Mode::INLINE;
				temp->inlines.push_back(nodes[i]);
			}
		}
		set_anchor(temp);
		cells.push_back(temp);
	}
}

void init_cell(Cell* cell) {
	cell->anchor = NULL;
	for (int i = 0; i < cell->inlines.size(); i++) {
		cell->inlines[i]->mode = Mode::INLINE;
	}
}

Node* Simulator::find_node_by_id(int id) {
	return nodes[id];
}

void Simulator::routing_all() {
	for (int i = 0; i < NODES; i++) {
		nodes[i]->next_node = nodes[i]->routing();
	}
}

void Simulator::sensing_all()
{
	int i;

	for (i = 0; i < NODES; i++) {
		nodes[i]->sensing();
	}
}


void Simulator::transmit_nodes(Node* n) {
	int data_size = 0;
	double num_data = 0;

	while(n->data_size > 0) {
		if (n->data_size > PACKET_PAYLOAD) {
			data_size = PACKET_PAYLOAD;
			num_data = n->num_data * (double)data_size / n->data_size;

			n->data_size -= data_size;
			n->num_data -= num_data;
		}
		else {
			data_size = n->data_size;
			num_data = n->num_data;

			n->data_size = 0;
			n->num_data = 0;
		}
		if (data_size <= 0) {
			fprintf(stderr, "Packet size of %d is not current: %d\n", n->id, data_size);
			exit(1);
		}
		Packet p = {
			n->id,
			n->next_node,
			n->id,
			data_size + PACKET_HEADER,
			num_data,
		};
		if (find_node_by_id(n->next_node)->status != Status::ACTIVE) {
			p.to = n->id;
			n->data_size += p.size - PACKET_HEADER;
			n->num_data += p.num_data;
		}

		n->consume_energy(n->calc_send_energy(p.size) * n->neighbor.size());
		if (n->status == Status::BLACKOUT) {
			return;
		}
		n->consume_receive_around_energy(p.size);
		if (n->status == Status::BLACKOUT) {
			return;
		}
		network.push_back(p);
	}
}

void Simulator::transmit() {
	int i = 0;
	for (i = 0; i < NODES; i++) {
		if (nodes[i]->mode == Mode::ANCHOR || nodes[i]->status != Status::ACTIVE)
			continue;
		transmit_nodes(nodes[i]);
	}
	
}

void Simulator::transmitting() {
	Packet p;

	while (network.size() > 0) {
		p = network.front();
		network.pop_front();
		receive_packet(find_node_by_id(p.to), p);
		//printf("%d\n", network.size());
	}

}

void Simulator::receive_packet(Node* n, Packet p) {
	double energy = 0;
	p.hop++;

	if (n->status != Status::ACTIVE)
		return;

	if (n->mode == Mode::ANCHOR) {
		n->num_data += p.num_data;
		n->data_size += p.num_data * PACKET_PAYLOAD;
		return;
	}
	
	p.from = n->id;
	p.to = n->next_node;
	
	network.push_back(p);
	n->consume_energy(n->calc_send_energy(p.size) * n->neighbor.size());
	n->consume_receive_around_energy(p.size);
	return;
}

void Simulator::calc_idle_energy(int time) {
	int i = 0;
	for (i = 0; i < NODES; i++) {
		nodes[i]->consume_idle_energy( time);
	}
}

void Simulator::anchor_move() {
	for (int i = 0; i < cells.size(); i++) {
		init_cell(cells[i]);
		set_anchor(cells[i]);
	}
}

void Simulator::collect_data() {
	for (int i = 0; i < cells.size(); i++) {
		if(cells[i]->anchor->status == Status::ACTIVE)
			get_data_from_anchor(cells[i]->anchor);
	}
}

void Simulator::get_data_from_anchor(Node *n) {

	// 한 패킷을 보낼 때의 에너지
	double packet_e = n->calc_send_energy(PACKET_HEADER + PACKET_PAYLOAD);

	while (n->data_size > PACKET_PAYLOAD) {
		drone->data += PACKET_PAYLOAD;
		n->data_size -= PACKET_PAYLOAD;
		n->consume_receive_around_energy(packet_e);
		if (!(n->consume_energy(packet_e * n->neighbor.size())))
			return;
	}
	drone->data += n->data_size;
	n->consume_energy(n->calc_send_energy(n->data_size + PACKET_HEADER) * n->neighbor.size());
	n->consume_receive_around_energy(n->data_size + PACKET_HEADER);
	n->data_size = 0;
	n->num_data = 0;
}


void Simulator::start_simulator(FILE *fp) {
	int round;
	int times;
	int day;

	for (day = 0; day < MONTH; day++) {
		for (round = 0; round < DAY; round++) {
			for (times = 0; times < HOUR/TR_CYCLE; times++) {
				if(times % 2 == 0)
					anchor_move();
				sensing_all();
				transmit();
				transmitting();
				calc_idle_energy(day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times);
				write_data(fp, (day * DAY * (HOUR/TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times));
			}
			collect_data();
			//for (int i = 0; i < NODES; i++) {
			//	fprintf(fp, "%d \n", nodes[i]->data_size);
			//}
			//exit(0);
			printf("-");
			//print_field();
			/*write_data(fp, day * DAY + round);*/
		}
		
	}
	write_data_s(fp);

}

void Simulator::line_shift() {
	for (int i = 0; i < cells.size(); i++) {
		for (int j = 0; j < cells[i]->inlines.size(); j++) {
			cells[i]->anchor = NULL;
			cells[i]->inlines[j]->mode = Mode::NORMAL;
			cells[i]->inlines[j]->line_is_right = !cells[i]->inlines[j]->line_is_right;
			cells[i]->inlines[j]->anchor = NULL;
		}
		delete cells[i];
	}
	cells.erase(cells.begin(), cells.end());

	lines[1]->start -= W;
	lines[1]->end -= W;

	lines[2]->start += W;
	lines[2]->end += W;

	if (lines[1]->start < 0 || lines[2]->end > FIELD_SIZE) {
		lines[1]->start = lines[2]->start = lines[0]->start;
		lines[1]->end = lines[2]->end = lines[0]->end;

		line_release();
		find_inlines(*lines[0]);
	}
	else {
		for (int i = 1; i < lines.size(); i++) {
			find_inlines(*lines[i]);
		}
	}

	routing_all();
}

void Simulator::line_release() {
	for (int i = 0; i < NODES; i++) {
		nodes[i]->line_is_right = !nodes[i]->line_is_right;
	}

}


void Simulator::reverse_direct_in_cell() {
	for (int i = 0; i < cells.size(); i++) {
		for (int j = 0; j < cells[i]->inlines.size(); j++) {
			cells[i]->inlines[j]->line_is_right = !cells[i]->inlines[j]->line_is_right;
		}
	}
}

void Simulator::start_simulator2(FILE *fp) {
	int round;
	int times;
	int day;

	line_release();
	reverse_direct_in_cell();

	for (day = 0; day < MONTH; day++) {
		for (round = 0; round < DAY; round++) {
			line_shift();
			for (times = 0; times < HOUR / TR_CYCLE; times++) {
				if (times % 2 == 0)
					anchor_move();
				sensing_all();
				transmit();
				transmitting();
				calc_idle_energy(day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times);
				write_data(fp, day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times);
			}
			//for (int i = 0; i < NODES; i++) {
			//	fprintf(fp, "%d \n", nodes[i]->data_size);
			//}
			//exit(0);
			collect_data();
			
			printf("-");
			/*write_data(fp, day * DAY + round);*/
		}

	}
	write_data_s(fp);

}

void Simulator::print_field() {
	int array1[FIELD_SIZE][FIELD_SIZE] = { 0, };
	for (int i = 0; i < NODES; i++) {
		if (nodes[i]->status == Status::ACTIVE)
			array1[nodes[i]->y][nodes[i]->x] = 1;
		if (nodes[i]->mode == Mode::INLINE)
			array1[nodes[i]->y][nodes[i]->x] = 2;
		else if (nodes[i]->mode == Mode::ANCHOR)
			array1[nodes[i]->y][nodes[i]->x] = 3;
		if (nodes[i]->status == Status::BLACKOUT)
			array1[nodes[i]->y][nodes[i]->x] = 4;

	}
	printf("\n");
	for (int i = 0; i < FIELD_SIZE; i++) {
		for (int j = 0; j < FIELD_SIZE; j++) {

			if (array1[i][j] == 1) {
				//fprintf(fp, "%d,%d,%d\n", count, i, j);
				std::cout << "■";
				//count++;
			}
			else if (array1[i][j] == 2)
				std::cout << "★";
			else if (array1[i][j] == 3)
				std::cout << "☆";
			else  if (array1[i][j] == 4)
				std::cout << "X ";
			else {
				std::cout << "  ";
			}
		}
		std::cout << std::endl << std::endl;
	}
}

void Simulator::write_data(FILE *fp, int round) {
	int num_blackout = 0;
	double num_sinkdata = 0;
	num_sinkdata = drone->data - prevSinkdata;

	for (int i = 0; i < NODES; i++) {
		if (nodes[i]->status == Status::BLACKOUT)
			num_blackout++;
	}

	fprintf(fp, "%d, %d, %lf\n", round, num_blackout, num_sinkdata);
	prevSinkdata = drone->data;
}

void Simulator::write_data_s(FILE* fp) {
	double num_sinkdata = 0;
	num_sinkdata = drone->data - prevSinkdata;

	fprintf(fp, "0, 0, %lf\n", num_sinkdata);
	prevSinkdata = drone->data;
}

