#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <queue>

#include "packet.h"
#include "config.h"


typedef enum _Status {	// 노드의 상태 0 : BLACKOUT, 1: SLEEP, 2: Active {{ 0번을 제외하면 살아 있는 것으로 판단 }}
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

	int id;						// 노드 ID. 배열의 인덱스와 같음
	int x, y;					// 노드의 좌표. sink node는 (0, 0)

	Status status;		
	Mode mode;

	double energy;				// 현재 남은 에너지 (J)

	std::vector<Node*> neighbor;				// 이웃 노드 목록
	//Node* next_node;							// 다음 노드

	int line_is_right;								// 라인이 오른쪽인지 왼쪽인지	// 오른쪽이면 1 왼쪽이면 0

	//std::queue<Packet*> memory;
	Packet* memory;

	/////////////////////////////////////////////////////////////////////////////////////////
	Node* anchor;								// 앵커 노드(인라인 노드에서 사용)
	/////////////////////////////////////////////////////////////////////////////////////////

	static double tr;					// Transmission Range (m)
	static double max_energy;			// 최대 에너지 (J)
	static double tx;					// tx energy J/byte
	static double rx;					// rx energy J/byte
	static double data_rate;			// bytes/S -> 초당 보내는 것은 1/data_rate
	static double active_energy;		// active 상태에서 소비되는 에너지
	static double sleep_energy;			// sleep 상태에서 소비되는 에너지
	static double max_data_size;		// 보낼 수 있는 최대 데이터 사이즈

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