#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <queue>

#include "packet.h"
#include "config.h"


enum class Status:int {	// 노드의 상태 0 : BLACKOUT, 1: SLEEP, 2: Active {{ 0번을 제외하면 살아 있는 것으로 판단 }}
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

	int id;						// 노드 ID. 배열의 인덱스와 같음
	int x, y;					// 노드의 좌표. sink node는 (0, 0)

	Status status;		
	Mode mode;

	double energy;				// 현재 남은 에너지 (J)
	double prevEnergy;			// 이전 에너지 (J)
	double prevHarvest;			// 이전 Harvest 에너지 (J)

	double avgEnergy;			// 평균 에너지
	double avgHarvest;			// 평균 Harvest 에너지지

	std::vector<Node*> neighbor;				// 이웃 노드 목록
	Node* next_node;							
	//int next_node;								// 다음 노드 ID								

	bool line_is_right;								// 라인이 오른쪽인지 왼쪽인지	// 오른쪽이면 1 왼쪽이면 0
	bool can_receive;								// 메모리에 따른 수신 가능 여부
	bool can_transmit;								// 전송 가능?

	double baudrate;								// 전송 최대 메모리 설정
	double threshold;								// Node의 각각 Threshold

	int numblackout;

	//std::queue<Packet*> memory;
	//Packet* memory;

	int data_size;
	double num_data;

	/////////////////////////////////////////////////////////////////////////////////////////
	Node* anchor;								// 앵커 노드(인라인 노드에서 사용) ID
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