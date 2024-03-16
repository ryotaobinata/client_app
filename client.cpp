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
		cout << "初期化に失敗しました:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}


	//接続先のアドレス情報を検索、格納
	char hostname[48];
	struct addrinfo* result = NULL,* ptr = NULL,hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	cout << "接続先を入力してください＞";
	cin >> hostname;

	int iResult;
	iResult = getaddrinfo(hostname, "50000", &hints, &result);
	if (iResult!=0)
	{
		cout << "getaddrinfoに失敗:" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	ptr = result;

	SOCKET ConnectSocket;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket==INVALID_SOCKET)
	{
		cout << "socket作成失敗:" << WSAGetLastError() << endl;
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

	

	//接続出来るところを探す

	freeaddrinfo(result);
	
	//どれも接続できない場合
	if (ConnectSocket==INVALID_SOCKET)
	{
		cout << "connect失敗:" << endl;
		WSACleanup();
		return -1;
	}


	//データの送受信
	int recvbuflen = DEFAULT_BUFLEN;
	int len=0;
	char sendbuf[DEFAULT_BUFLEN] ;
	char recvbuf[DEFAULT_BUFLEN];

	//ストリーム内を読み捨てる
	cin.ignore();
	//メッセージの入力
	cout << "<";
	cin.getline(sendbuf, DEFAULT_BUFLEN);
	
	//文字数をカウント
	while (sendbuf[len]!='\0')
	{
		len++;
	}
	//eosの分をプラス
	len++;
	

	//送信
	iResult = send(ConnectSocket, sendbuf, len, 0);
	if (iResult==SOCKET_ERROR)
	{
		cout << "sendに失敗:" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	cout << iResult << "Byte send" << endl;
	
	//接続のシャットダウン
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult==SOCKET_ERROR)
	{
		cout << "shutdown失敗:" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	//サーバがコネクションを閉じるまで受信
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
			cout << "recv失敗:" << WSAGetLastError();
		}

	} while (iResult>0);

	closesocket(ConnectSocket);
	WSACleanup();

}