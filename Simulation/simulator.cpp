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
				if (x > lines[0]->end)
					nodes[index]->line_is_right = 0;
				else if (x < lines[0]->start)
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
	for (int i = 0; i < cell->inlines.size(); i++) {
		if (cell->anchor != NULL) {
			cell->inlines[i]->anchor = cell->anchor;
			continue;
		}
		if (cell->inlines[i]->energy > THRESHOLD) {  /* �� ���� */
			cell->anchor = cell->inlines[i];
			cell->anchor->mode = ANCHOR;
			cell->anchor->anchor = cell->anchor;
		}
	}
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


Node* Simulator::find_node_by_id(int id) {
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->id == id)
			return nodes[i];
	}
	return NULL;
}

void Simulator::send_receive(Node* node) {
	if (node->status != ACTIVE) {
		return;
	}

	int data = 0;
	Packet* temp;
	while (!node->memory.empty()) {
		temp = node->memory.front();
		node->memory.pop();

		data += temp->size - PACKET_HEADER;
		delete temp;
	}

	Packet* packet = NULL;
	while (data > 0) {
		packet = new Packet;
		packet->from = node->id;
		packet->to = node->routing();
		packet->size = PACKET_HEADER + PACKET_PAYLOAD;
		if (data - PACKET_PAYLOAD < 0) {
			packet->size = PACKET_HEADER + data;
		}
		data -= PACKET_PAYLOAD;
		node->memory.push(packet);
	}

	while (!node->memory.empty()) {
		packet = node->memory.front();
		for (int i = 0; i < node->neighbor.size(); i++) {
			if (!node->consume_energy(node->calc_send_energy(packet->size))) {
				return;
			}
			if (node->neighbor[i]->status != ACTIVE) {
				continue;
			}
			if (!node->neighbor[i]->consume_energy(node->calc_recv_energy(packet->size))) {
				continue;
			}
			if(packet->to == node->neighbor[i]->id)
				node->neighbor[i]->memory.push(packet);
		}
		node->memory.pop();
	}
}

void Simulator::re_packet(Node* node) {

}

void Simulator::start_simulator() {
	std::deque<Node*> task;
	Node* temp;
	
	for (int hour = 0; hour < 24; hour++) {
		for (int time = 0; time < 60; time++) {
			for (int i = 0; i < NODES; i++) {
				task.push_back(nodes[i]);
			}
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(task.begin(), task.end(), g);

			for (int i = 0; i < NODES; i++) {
				temp = task.front();
				task.pop_front();

				if (temp->consume_energy(temp->active_energy * DUTY_CYCLE * 60)) {
					if (time % SENSING == 0)
						temp->sensing();
					send_receive(temp);
				};
				temp->consume_energy(temp->sleep_energy * (1 - DUTY_CYCLE) * 60);
			}
		}
		printf("-");
	}
	printf("\n");

	printf("The End\n");
}

