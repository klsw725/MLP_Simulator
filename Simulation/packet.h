#ifndef __PACKET_H__
#define __PACKET_H__


typedef struct _Data {
	int header = 0;
	int payload = 0;
}Data;

typedef struct _Packet {
	int from = 0;			// sender. 최초 sender가 아님	
	int to = 0;				// receiver. 최종 receiver가 아님

	//int source;
	//	int destination;	// 최종 목적지. (무조건 sink이기 때문에 삭제)
	int size = 0;			// 패킷의 크기

	double num_data = 0;	// 패킷에 포함된 센싱 데이터의 수
	int hop = 0;			// 패킷이 거쳐온 hop 수
	unsigned int time = 0;	// 패킷이 생성된 시간
}Packet;

#endif
