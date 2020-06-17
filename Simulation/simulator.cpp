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
	lines.push_back(new Line());
	lines[0]->start = (FIELD_SIZE / W) / 2 * W;
	lines[0]->end = lines[0]->start + W-1;

	create_field();
	init_nodes();

	find_inlines(*lines[0]);

	drone = new Drone;

	routing_all();
}

Simulator::~Simulator() {

}

void Simulator::create_field() {
	//srand(time(NULL));

	for (int i = 0; i < FIELD_SIZE; i++) {			// �ʵ� �ʱ�ȭ
		for (int j = 0; j < FIELD_SIZE; j++) {
			field[i][j] = 0;
		}
	}

	int density = pow(FIELD_SIZE, 2)/NODES + 1;		// ��尡 ������ ���� ����
	density = sqrt(density)+1;						// ������ ����

	while ((double)density > Node::tr) {		// ���� �Ÿ����� ��� ������ ������ ���̰� ũ�ٸ� �ϳ��� �ٿ�����
		density -= 1;
	}

	int x = 0, y = 0;
	bool equal = 0;			// �յ��ϰ� �ϳ��� ������ �Ǵ�. �յ��ϰ� �� ������ �� �������ʹ� �ƾ� �����ϰ� ���.
	int indexX=0, indexY=0;

	for (int count = 0; count < NODES; count++) {
		int i = rand() % density;
		int j = rand() % density;

		if (!equal) {
			indexX = density * x + i;
			indexY = density * y + j;
			if (indexX >= FIELD_SIZE) { // index �� �ʵ� ����� �Ѿ��� ���
				int prevx = x;
				x = 0; y++;
				if (density * prevx < FIELD_SIZE)		// index�� �ʵ������� �� �´� ��� �Ǻ� (index == FIELD_SIZE)->�迭������ 0-FIELD_SIZE-1�̹Ƿ� �����ϸ� ����
					indexX = rand() % (indexX - FIELD_SIZE + 1) + density * prevx;
				else {
					count--;
					continue;
				}

			}
			if (indexY >= FIELD_SIZE) {				// Y�� �Ѿ�ٴ� ���� �ѹ��� �� ���Ҵٴ� ��, �յ��ϰ� �ϳ��� �ٵ��ٴ� ��.
				equal = 1;
				count--;
				continue;
			}
		}
		else {	// �����ϰ� ��� ��ġ
			indexX = rand() % FIELD_SIZE;
			indexY = rand() % FIELD_SIZE;
		}

		if (field[indexY][indexX]) {	// ������ ��ġ�� ������ �ٽ� �˻�
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
					nodes[index]->line_is_right = 0;
				else
					nodes[index]->line_is_right = 1;
				index++;
			}
		}
	}

	for (int i = 0; i < nodes.size(); i++) {
		for (int j = i + 1; j < nodes.size(); j++) {
			double distance = get_distance(nodes[i]->x, nodes[i]->y, nodes[j]->x, nodes[j]->y);
			if (distance < Node::tr) {
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
	big->mode = ANCHOR;
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
	//	if (cell->inlines[i]->energy > THRESHOLD) {  /* �� ���� */
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
				nodes[i]->mode = INLINE;
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
		cell->inlines[0]->mode = INLINE;
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
		n->consume_energy(Node::calc_send_energy(p.size) * n->neighbor.size());
		if (n->status == BLACKOUT) {
			return;
		}
		network.push_back(p);
	}
}

void Simulator::transmit() {
	int i = 0;
	for (i = 0; i < NODES; i++) {
		if (nodes[i]->mode == ANCHOR || nodes[i]->status != ACTIVE)
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
	n->consume_energy(Node::calc_recv_energy(p.size));
	if (n->mode == ANCHOR) {
		n->num_data += p.num_data;
		n->data_size += p.num_data * PACKET_PAYLOAD;
		return;
	}
	
	p.from = n->id;
	p.to = n->next_node;
	
	network.push_back(p);
	n->consume_energy(Node::calc_send_energy(p.size) * n->neighbor.size());
	return;
}

void Simulator::calc_idle_energy() {
	int i = 0;
	for (i = 0; i < NODES; i++) {
		Node::consume_idle_energy(nodes[i]);
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
		get_data_from_anchor(cells[i]->anchor);
	}
}

void Simulator::get_data_from_anchor(Node *n) {

	// �� ��Ŷ�� ���� ���� ������
	double packet_e = Node::calc_send_energy(PACKET_HEADER + PACKET_PAYLOAD);

	while (n->data_size > PACKET_PAYLOAD) {
		drone->data += PACKET_PAYLOAD;
		n->data_size -= PACKET_PAYLOAD;
		n->consume_energy(packet_e * n->neighbor.size());
	}
	drone->data += n->data_size;
	n->consume_energy(Node::calc_send_energy(n->data_size + PACKET_HEADER) * n->neighbor.size());
	n->data_size = 0;
	n->num_data = 0;

}

void Simulator::start_simulator() {
	int round;
	int times;
	for (int day = 0; day < MONTH; day++) {
		for (round = 1; round <= DAY; round++) {
			anchor_move();
			for (times = 0; times < HOUR/TR_CYCLE; times++) {
				sensing_all();
				transmit();
				transmitting();
				calc_idle_energy();
			}
			collect_data();
		}
		printf("-");
	}

	printf("\n");

	int array1[FIELD_SIZE][FIELD_SIZE] = { 0, };
	for (int i = 0; i < NODES; i++) {
		if (nodes[i]->status == ACTIVE)
			array1[nodes[i]->y][nodes[i]->x] = 1;
		else if(nodes[i]->status == BLACKOUT)
			array1[nodes[i]->y][nodes[i]->x] = 2;
	}

	for (int i = 0; i < FIELD_SIZE; i++) {
		for (int j = 0; j < FIELD_SIZE; j++) {

			if (array1[i][j] == 1) {
				//fprintf(fp, "%d,%d,%d\n", count, i, j);
				std::cout << "��";
				//count++;
			}
			else if (array1[i][j] == 2)
				std::cout << "X ";
			else {
				std::cout << "  ";
			}
		}
		std::cout << std::endl;
	}
}

