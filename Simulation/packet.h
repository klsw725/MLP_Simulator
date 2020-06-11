#ifndef __PACKET_H__
#define __PACKET_H__


typedef struct _Data {
	int header = 0;
	int payload = 0;
}Data;

typedef struct _Packet {
	int from = 0;			// sender. ���� sender�� �ƴ�	
	int to = 0;				// receiver. ���� receiver�� �ƴ�

	//int source;
	//	int destination;	// ���� ������. (������ sink�̱� ������ ����)
	int size = 0;			// ��Ŷ�� ũ��

	double num_data = 0;	// ��Ŷ�� ���Ե� ���� �������� ��
	int hop = 0;			// ��Ŷ�� ���Ŀ� hop ��
	unsigned int time = 0;	// ��Ŷ�� ������ �ð�
}Packet;

#endif
