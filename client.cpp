#include<iostream>
#include<iomanip>
#include<string>
#include<WinSock2.h>
#include<WS2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define DEFAULT_BUFLEN 512

#pragma comment(lib,"Ws2_32.lib")

using namespace std;

int main() {
	WSADATA wsadata;
	if (WSAStartup(WINSOCK_VERSION, &wsadata) != 0)
	{
		cout << "�������Ɏ��s���܂���:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}


	//�ڑ���̃A�h���X���������A�i�[
	char hostname[48];
	struct addrinfo* result = NULL,* ptr = NULL,hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	cout << "�ڑ������͂��Ă���������";
	cin >> hostname;

	int iResult;
	iResult = getaddrinfo(hostname, "50000", &hints, &result);
	if (iResult!=0)
	{
		cout << "getaddrinfo�Ɏ��s:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	ptr = result;

	SOCKET ConnectSocket;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket==INVALID_SOCKET)
	{
		cout << "socket�쐬���s:" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	for(;ptr!=NULL;ptr->ai_next)
	{
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
		else if(iResult==INVALID_SOCKET)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	

	//�ڑ��o����Ƃ����T��

	freeaddrinfo(result);
	
	//�ǂ���ڑ��ł��Ȃ��ꍇ
	if (ConnectSocket==INVALID_SOCKET)
	{
		cout << "connect���s:" << endl;
		WSACleanup();
		return -1;
	}


	//�f�[�^�̑���M
	int recvbuflen = DEFAULT_BUFLEN;
	int len=0;
	char sendbuf[DEFAULT_BUFLEN] ;
	char recvbuf[DEFAULT_BUFLEN];

	//�X�g���[������ǂݎ̂Ă�
	cin.ignore();
	//���b�Z�[�W�̓���
	cout << "<";
	cin.getline(sendbuf, DEFAULT_BUFLEN);
	
	//���������J�E���g
	while (sendbuf[len]!='\0')
	{
		len++;
	}
	//eos�̕����v���X
	len++;
	

	//���M
	iResult = send(ConnectSocket, sendbuf, len, 0);
	if (iResult==SOCKET_ERROR)
	{
		cout << "send�Ɏ��s:" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	cout << iResult << "Byte send" << endl;
	
	//�ڑ��̃V���b�g�_�E��
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult==SOCKET_ERROR)
	{
		cout << "shutdown���s:" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	//�T�[�o���R�l�N�V���������܂Ŏ�M
	do
	{
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult>0)
		{
			cout << iResult << "Byte recv" << endl;
		}
		else if (iResult == 0) {
			cout << "connection closed" << endl;
		}
		else
		{
			cout << "recv���s:" << WSAGetLastError();
		}

	} while (iResult>0);

	closesocket(ConnectSocket);
	WSACleanup();

}