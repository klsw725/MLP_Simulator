#include "simulator.h"

void Simulator::reverse_direct_in_cell_first() {
	for (int i = 0; i < cells.size(); i++) {
		for (int j = 0; j < cells[i]->inlines.size(); j++) {
			cells[i]->inlines[j]->line_is_right = false;
		}
	}
}


void Simulator::line_shift2() {

	lines[0]->start += static_w;
	lines[0]->end += static_w;

	for (int i = 0; i < cells.size(); i++) {
		for (int j = 0; j < cells[i]->inlines.size(); j++) {
			cells[i]->anchor = NULL;
			cells[i]->inlines[j]->mode = Mode::NORMAL;
			cells[i]->inlines[j]->anchor = NULL;
			if (lines[0]->start < 0 || lines[0]->end > FIELD_SIZE)
				continue;
			cells[i]->inlines[j]->line_is_right = !cells[i]->inlines[j]->line_is_right;
		}
		delete cells[i];
	}
	cells.erase(cells.begin(), cells.end());

	/*lines[1]->start -= static_w;
	lines[1]->end -= static_w;*/

	if (lines[0]->start < 0 || lines[0]->end > FIELD_SIZE) {
		static_w = -static_w;

		lines[0]->start = lines[0]->start + (2 * static_w);
		lines[0]->end = lines[0]->end + (2 * static_w);

		//line_release();
		find_inlines(*lines[0]);
	}
	else {
		//for (int i = 1; i < lines.size(); i++) {
		find_inlines(*lines[0]);
		//}
	}
	routing_all();
	//routing_all2();

}

void Simulator::start_shift_simulator(FILE* fp) {
	int round;
	int times;
	int day;

	//line_release();
	reverse_direct_in_cell_first();
	//anchor_move();
	for (day = 0; day < MONTH; day++) {
		for (round = 0; round < DAY; round++) {
			//print_field();
			for (times = 0; times < HOUR / TR_CYCLE; times++) {
				unsigned int time = day * DAY * (HOUR / TR_CYCLE) + (round * (HOUR / TR_CYCLE)) + times;
				reset_node();
				if (times % 2 == 0)
					anchor_random_move();
				sensing_all();
				transmit(time);
				transmitting();
				calc_idle_energy(time);
				write_data(fp, time);
			}
			//for (int i = 0; i < NODES; i++) {
			//	fprintf(fp, "%d \n", nodes[i]->data_size);
			//}
			//exit(0);
			collect_data();
			line_shift2();
			printf("-");
			/*write_data(fp, day * DAY + round);*/
		}

	}
	write_data_s(fp);

}