#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>

#include "node.h"
#include "drone.h"
#include "packet.h"
#include "config.h"

static int static_w = W;

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

	int cantTransmitNodes;

	void create_field();
	void init_nodes();
	void init_cell(Cell* cell);

	void find_inlines(Line line);


	void routing_all();

	Node* find_node_by_id(int id);

	void reset_node();

	void sensing_all();
	void transmit(unsigned int time);
	void transmit_packet(Node* n, unsigned int time);
	void transmitting();
	void receive_packet(Node *n, Packet p);

	void calc_idle_energy(int time);

	void collect_data();
	void get_data_from_anchor(Node *n);
	double get_distance(double x1, double y1, double x2, double y2);

	//void reverse_direct_in_cell();

	void print_field();
	void write_data(FILE* fp, int round);
	void write_data_s(FILE* fp);

	void write_total_field(FILE* fp);

	std::deque<Packet> network;

	/* LBDD */
	//////////////////////////////////////////////////////////////////////
	void set_random_anchor(Cell* cell);
	void anchor_random_move();
	void start_lbdd_simulator(FILE* fp);
	//////////////////////////////////////////////////////////////////////

	/* Proposed Scheme */
	///////////////////////////////////////////////////////////////////////
	void set_anchor(Cell* cell);
	void anchor_move();
	void line_shift();
	void line_release();
	void start_simulator(FILE* fp);
	///////////////////////////////////////////////////////////////////////

	/* Line Shift */
	///////////////////////////////////////////////////////////////////////
	void line_shift2();
	void reverse_direct_in_cell_first();
	void start_shift_simulator(FILE* fp);
	///////////////////////////////////////////////////////////////////////
	
	/* LARCMS */
	/// ///////////////////////////////////////////////////////////////////

	Simulator(Simulator* simu, int num);
	double table[NODES][NODES] = { 0, };
	void calc_vec_table();
	void make_cross(Line* line);
	void routing_all2();
	Node* find_short_inline(Node* node);

	void start_larcms_simulator(FILE* fp);

private:
};

#endif