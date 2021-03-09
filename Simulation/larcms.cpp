#include "simulator.h"


void Simulator::calc_vec_table()
{
	for (int i = 0; i < NODES; i++) {
		for (int j = 0; j < NODES; j++) {
			table[i][j] = get_distance(nodes[i]->x, nodes[i]->y, nodes[j]->x, nodes[j]->y);
		}
	}
}

void Simulator::make_cross(Line* line) {
	for (int y = 0; y * G < FIELD_SIZE; y++) {
		Cell* temp = new Cell;
		for (int i = 0; i < NODES; i++) {
			if (nodes[i]->x >= line->start && nodes[i]->x <= line->end && nodes[i]->y >= y * G && nodes[i]->y < (y + 1) * G) {
				nodes[i]->mode = Mode::INLINE;
				temp->inlines.push_back(nodes[i]);
			}
		}
		set_anchor(temp);
		cells.push_back(temp);
	}

	for (int x = 0; x * G < FIELD_SIZE; x ++) {
		Cell* temp = new Cell;
		for (int i = 0; i < NODES; i++) {
			if (nodes[i]->y >= line->start && nodes[i]->y <= line->end && nodes[i]->x >= x * G && nodes[i]->x < (x + 1) * G) {
				if (nodes[i]->x >= line->start && nodes[i]->x <= line->end)
					continue;
				nodes[i]->mode = Mode::INLINE;
				temp->inlines.push_back(nodes[i]);

			}
		}
		if (temp->inlines.size() == 0) {
			delete temp;
			continue;
		}
		set_anchor(temp);
		cells.push_back(temp);
	}
}

void Simulator::routing_all2() {
	for (int i = 0; i < NODES; i++) {
		Node* short_inline = find_short_inline(nodes[i]);

		if (!short_inline)
		{
			nodes[i]->next_node = NULL;
			continue;
		}

		Node* short_neighbor = nodes[i]->neighbor[0];
		for (int j = 0; j < nodes[i]->neighbor.size(); j++)
		{
			if (table[short_inline->id][nodes[i]->neighbor[j]->id] < table[short_inline->id][short_neighbor->id] && nodes[i]->neighbor[j]->status == Status::ACTIVE)
				short_neighbor = nodes[i]->neighbor[j];

		}

		/*if (short_neighbor->status != Status::ACTIVE)
		{
			nodes[i]->next_node = NULL;
			continue;
		}*/

		nodes[i]->next_node = short_neighbor;

		//nodes[i]->next_node = nodes[i]->geographic_routing();
	}
}

Node* Simulator::find_short_inline(Node* node)
{
	Node* short_id = cells[0]->inlines[0];
	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[i]->inlines.size(); j++)
		{
			if (table[short_id->id][node->id] > table[cells[i]->inlines[j]->id][node->id])
				short_id = cells[i]->inlines[j];
		}
		
	}

	return short_id;
}

void Simulator::start_larcms_simulator(FILE* fp) {
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