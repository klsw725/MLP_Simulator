#include "simulator.h"


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
		cell->inlines[i]->next_node = big;
	}


	//	if (cell->anchor != NULL) {
	//		cell->inlines[i]->anchor = cell->anchor;
	//		continue;
	//	}
	//	if (cell->inlines[i]->energy > THRESHOLD) {  /* ½Ä ¼º¸³ */
	//		cell->anchor = cell->inlines[i];
	//		cell->anchor->mode = ANCHOR;
	//		cell->anchor->anchor = cell->anchor;
	//	}
	//}

}

void Simulator::anchor_move() {
	for (int i = 0; i < cells.size(); i++) {
		init_cell(cells[i]);
		set_anchor(cells[i]);
	}
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

void Simulator::start_simulator(FILE* fp) {
	int round;
	int times;
	int day;

	//line_release();
	//reverse_direct_in_cell();

	for (day = 0; day < MONTH; day++) {	
		for (round = 0; round < DAY; round++) {
			for (times = 0; times < HOUR / TR_CYCLE; times++) {
				unsigned int time = day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times;
				reset_node();
				if (times % 2 == 0)
					anchor_move();
				sensing_all();
				transmit(time);
				transmitting();
				calc_idle_energy(time);
				write_data(fp, time);
			}
			collect_data();
			line_shift();
			printf("-");

		}

	}
	write_data_s(fp);

}
