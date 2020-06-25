#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>

#include "node.h"
#include "drone.h"
#include "packet.h"
#include "config.h"

typedef struct _Line {
	int start=0, end=0;
}Line;

typedef struct _Cell {
	Node* anchor = NULL;
	std::vector<Node*> inlines;
}Cell;

class Simulator {
public:
	Simulator();
	Simulator(Simulator* simu);
	~Simulator();

	int field[FIELD_SIZE][FIELD_SIZE];

	std::vector<Line*> lines;
	std::vector<Cell*> cells;

	std::vector<Node*> nodes;					// 전체 노드 관리 벡터

	Drone *drone;
	double prevSinkdata;

	void create_field();
	void init_nodes();

	void find_inlines(Line line);
	void set_anchor(Cell* cell);

	void routing_all();

	Node* find_node_by_id(int id);

	void sensing_all();
	void transmit();
	void transmit_nodes(Node* n);
	void transmitting();
	void receive_packet(Node *n, Packet p);

	void calc_idle_energy(int time);

	void anchor_move();

	void collect_data();
	void get_data_from_anchor(Node *n);

	void line_shift();
	void line_release();
	void reverse_direct_in_cell();

	void start_simulator(FILE *fp);
	void start_simulator2(FILE *fp);

	void print_field();
	void write_data(FILE* fp, int round);
	void write_data_s(FILE* fp);

	std::deque<Packet> network;

private:
};

void transmit_nodes(Node* n);


#endif