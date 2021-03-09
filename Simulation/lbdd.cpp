#include "simulator.h"
#include <algorithm>
#include <iterator>
#include <iostream>

void Simulator::set_random_anchor(Cell* cell) {
	std::vector<Node*>temp;
	Node* big = NULL;
	Node* node1 = NULL;

	for (int i = 0; i < cell->inlines.size(); i++) 
	{
		if (cell->inlines[i]->energy > BLACKOUT_ENERGY) 
		{
			temp.push_back(cell->inlines[i]);
		}
	}

	if (temp.empty())
	{
		for (int i = 1; i < cell->inlines.size(); i++)
		{
			temp.push_back(cell->inlines[i]);
		}
	}

	std::random_shuffle(temp.begin(), temp.end());
	big = temp.front();

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

void Simulator::anchor_random_move() {
	for (int i = 0; i < cells.size(); i++) {
		init_cell(cells[i]);
		set_random_anchor(cells[i]);
	}
}


void Simulator::start_lbdd_simulator(FILE* fp) {
	int round;
	int times;
	int day;

	for (day = 0; day < MONTH; day++) {
		for (round = 0; round < DAY; round++) {
			for (times = 0; times < HOUR / TR_CYCLE; times++) {
				unsigned int time = day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times;
				reset_node2();
				if (times % 2 == 0)
					anchor_random_move();
				sensing_all();
				transmit(time);
				transmitting();
				calc_idle_energy(time);
				write_data(fp, time);
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