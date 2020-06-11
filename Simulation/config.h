#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FIELD_SIZE 75	// 공간의 사이즈
#define W 5			// W, 라인의 폭
#define G 5			// 라인을 나누는 size

#define NODES	1000	// 노드의 수

#define PACKET_HEADER 24			// Byte
#define PACKET_PAYLOAD 104			// Byte
#define MAX_DATA 128				// Byte (Packet_Header + Packet_Payload)

#define SENSING_DATA 64				// Byte
#define SENSING		10				// Minute
#define DUTY_CYCLE	0.01			// 비율 99:1

#define TR 10								// Transmit Range
#define TX  0.00000016704;					// tx energy J/byte
#define RX  0.00000018912;					// rx energy J/byte
#define DATA_RATE  31250.0;			// bytes/S -> 초당 보내는 것은 1/data_rate

#define BATTERY 200		// mAh
#define MAX_ENERGY BATTERY * 3.6 * 3;	// J

//#define SOLAR
//#define SYS 

#define THRESHOLD BATTERY * 0.7			//->SYS/SOLAR * BATTERY 약 0.7
#define ACTIVE_ENERGY 0.0264			// J/s
#define SLEEP_ENERGY 0.0000495			// J/s

#define TEST_DAY	30

#endif