#ifndef __CONFIG_H__
#define __CONFIG_H__

////////////////////////  TIME  ////////////////////////////
#define SEC 1
#define MIN 60
#define HOUR 60
#define DAY 24
#define MONTH 30
#define TEST_DAY	30

/////////////////////// FIELD  ///////////////////////////////////
#define FIELD_SIZE 70	// 공간의 사이즈
#define W 10			// W, 라인의 폭
#define G 10			// 라인을 나누는 size

#define NODES	1000	// 노드의 수

////////////////////////  PACKET  ///////////////////////////////
#define PACKET_HEADER 24			// Byte
#define PACKET_PAYLOAD 104			// Byte
#define MAX_DATA (PACKET_HEADER + PACKET_PAYLOAD)				// Byte (Packet_Header + Packet_Payload)

// MicaZ mote SPEC
////////////////////////  NODE  //////////////////////////////////
#define SENSING_DATA (64 * (MIN / REAL_SENSING))				// Byte
#define REAL_SENSING 10				// SEC
#define SENSING		1				// MIN
#define TR_CYCLE	5				// 전송 주기 (Minute)

#define DUTY_CYCLE	0.1		// 비율 9:1

#define TR 10								// Transmission Range (m)
#define	ALPHA_DEF	4.0			// 에너지 소모 모델의 path loss
#define	BETA_DEF	0.0000000009313			// 에너지 소모 모델의 단위 비트 전송 에너지 (단위 J, 800 pJ/byte.m^a)
//#define TX 0.0000000009313
//#define RX 0.0000019503
//#define TX  0.00000016704					// tx energy J/byte
#define RX  0.0000019503					// rx energy J/byte
#define DATA_RATE  31250.0			// bytes/S -> 초당 보내는 것은 1/data_rate    250kbps

#define BATTERY 2000		// mAh
#define MAX_ENERGY (BATTERY * 3.6 * 3.3)	// 최대 에너지 (J)
#define START_ENERGY 0.2		// 초기 에너지 비율 설정

#define MEMORY (512 * 1024) 				// 512K byte Flash Memory
#define MAX_TRANSMIT (DATA_RATE * TR_CYCLE * DUTY_CYCLE * MIN)

#define PANEL 5.91		// 1.2(4)
#define CLOUD 0.5
#define RAINY 0.3
#define STORM 0.1
//#define HARVEST_ENERGY	

//#define SOLAR
//#define SYS 

#define THRESHOLD 0.7		//->SYS/SOLAR * BATTERY 약 0.7
#define BLACKOUT_ENERGY (MAX_ENERGY * 0.1)		// BLACKOUT 기준 에너지 약 10%
#define ACTIVE_ENERGY 0.0264			// active 상태에서 소비되는 에너지 J/s
#define SLEEP_ENERGY 0.0000495			//  sleep 상태에서 소비되는 에너지 J/s

#endif