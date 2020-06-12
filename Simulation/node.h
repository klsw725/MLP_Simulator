#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <queue>

#include "packet.h"
#include "config.h"


typedef enum _Status {	// ����� ���� 0 : BLACKOUT, 1: SLEEP, 2: Active {{ 0���� �����ϸ� ��� �ִ� ������ �Ǵ� }}
	BLACKOUT,
	SLEEP,
	ACTIVE,
}Status;

typedef enum _Mode {
	NORMAL,
	ANCHOR,
	INLINE,
}Mode;

class Node {
public:
	Node();
	~Node();
	Node(FILE* fp);

	int id;						// ��� ID. �迭�� �ε����� ����
	int x, y;					// ����� ��ǥ. sink node�� (0, 0)

	Status status;		
	Mode mode;

	double energy;				// ���� ���� ������ (J)

	std::vector<Node*> neighbor;				// �̿� ��� ���
	//Node* next_node;							// ���� ���

	int line_is_right;								// ������ ���������� ��������	// �������̸� 1 �����̸� 0

	//std::queue<Packet*> memory;
	Packet* memory;

	/////////////////////////////////////////////////////////////////////////////////////////
	Node* anchor;								// ��Ŀ ���(�ζ��� ��忡�� ���)
	/////////////////////////////////////////////////////////////////////////////////////////

	static double tr;					// Transmission Range (m)
	static double max_energy;			// �ִ� ������ (J)
	static double tx;					// tx energy J/byte
	static double rx;					// rx energy J/byte
	static double data_rate;			// bytes/S -> �ʴ� ������ ���� 1/data_rate
	static double active_energy;		// active ���¿��� �Һ�Ǵ� ������
	static double sleep_energy;			// sleep ���¿��� �Һ�Ǵ� ������
	static double max_data_size;		// ���� �� �ִ� �ִ� ������ ������

	double calc_time(double size);
	double calc_send_energy(double size);
	double calc_recv_energy(double size);

	double calc_sleep_energy(double time);

	bool consume_energy(double consume);

	int routing();
	void sensing();
	
	/// /////////////////////////////////////////////////////////////////////////////////////
	
	inline void set_id(int index) { id = index; }
	inline void set_locate(int x1, int y1) { x = x1; y = y1; }
	
private:
	
};


#endif