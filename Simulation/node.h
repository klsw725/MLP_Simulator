#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <queue>

#include "packet.h"
#include "config.h"


enum class Status:int {	// ����� ���� 0 : BLACKOUT, 1: SLEEP, 2: Active {{ 0���� �����ϸ� ��� �ִ� ������ �Ǵ� }}
	BLACKOUT,
	SLEEP,
	ACTIVE,
};

enum class Mode:int {
	NORMAL,
	ANCHOR,
	INLINE,
};

class Node {
public:
	Node();
	~Node();

	int id;						// ��� ID. �迭�� �ε����� ����
	int x, y;					// ����� ��ǥ. sink node�� (0, 0)

	Status status;		
	Mode mode;

	double energy;				// ���� ���� ������ (J)
	double prevEnergy;			// ���� ������ (J)
	double prevHarvest;			// ���� Harvest ������ (J)

	double avgEnergy;			// ��� ������
	double avgHarvest;			// ��� Harvest ��������

	std::vector<Node*> neighbor;				// �̿� ��� ���
	Node* next_node;							
	//int next_node;								// ���� ��� ID								

	bool line_is_right;								// ������ ���������� ��������	// �������̸� 1 �����̸� 0
	bool can_receive;								// �޸𸮿� ���� ���� ���� ����
	bool can_transmit;								// ���� ����?

	double baudrate;								// ���� �ִ� �޸� ����
	double threshold;								// Node�� ���� Threshold

	int numblackout;

	//std::queue<Packet*> memory;
	//Packet* memory;

	int data_size;
	double num_data;

	/////////////////////////////////////////////////////////////////////////////////////////
	Node* anchor;								// ��Ŀ ���(�ζ��� ��忡�� ���) ID
	/////////////////////////////////////////////////////////////////////////////////////////

	static std::vector<long double>solar;
	inline static std::vector<long double>& getsolar() { return solar; }

	double calc_send_energy(double size);
	double calc_recv_energy(double size);

	double calc_sleep_energy(double time);
	double calc_active_energy(double time);
	double calc_harvest_energy(int time);

	bool consume_idle_energy(int time);
	bool consume_energy(double consume);
	void consume_receive_around_energy(double size);

	Node* routing();
	void sensing();
	
	/// /////////////////////////////////////////////////////////////////////////////////////
	
	//inline void set_id(int index) { id = index; }
	inline void set_locate(int x1, int y1) { x = x1; y = y1; }
	inline Node* get_next_node() { return next_node; }
	
private:
	
};


#endif