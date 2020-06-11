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
	~Simulator();

	int field[FIELD_SIZE][FIELD_SIZE];

	std::vector<Line*> lines;
	std::vector<Cell*> cells;

	std::vector<Node*> nodes;					// 전체 노드 관리 벡터

	void create_field();
	void init_nodes();

	void find_inlines(Line line);
	void set_anchor(Cell* cell);

	Node* find_node_by_id(int id);
	void send_receive(Node* node);
	void re_packet(Node* node);

	void create_packet(Node* node);

	void start_simulator();

private:
};



#endif