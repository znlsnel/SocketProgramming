#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ����
#pragma comment(lib, "ws2_32.lib") // ws2_32.lib ��ũ

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

#define BUFFERSIZE 512
#define PORTNUM 9000

DWORD WINAPI TCPServer4(LPVOID arg)
{
	int retval; // return value

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		// err
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr)); // 0���� �ʱ�ȭ
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 000���� �ʱ�ȭ
	serveraddr.sin_port = htons(PORTNUM); // ������Ʈ 9000

	retval = bind(listenSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		// err
	}

	retval = listen(listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		// err
	}

	SOCKET clientSocket;
	struct sockaddr_in clientAddr;
	int addrLength;
	char buf[BUFFERSIZE + 1];

	while (true)
	{
		addrLength = sizeof(clientAddr);
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLength);

		if (clientSocket == INVALID_SOCKET) {
			//err
			break;
		}
		// Client Info ���
		printf("\nClient connection successful!  \nIP Address : %s \nPort Number : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		// inet_ntoa : (Ipv4) ���ͳ� ��Ʈ��ũ �ּҸ� ���ͳ� ǥ�� ���� �Ҽ��� ������ ASCII ���ڿ��� ��ȯ
		// ntohs : u_short TCP/IP ��Ʈ��ũ ����Ʈ �������� ȣ��Ʈ ����Ʈ ����(Intel ���μ����� little-endian)�� ��ȯ

		// ������ ���
		while (true)
		{
			// receive
			retval = recv(clientSocket, buf, BUFFERSIZE, 0);
			if (retval == SOCKET_ERROR) {
				// err
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0'; // �������� null �־��ֱ�
			printf("%s", buf);
		}

		closesocket(clientSocket);
		printf("\nThe connection has been terminated! \nIP Address : %s,\nPort Number : %d\n",
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	}
	closesocket(listenSocket);
	return 0;
}
// TCP ����(IPv6)
DWORD WINAPI TCPServer6(LPVOID arg)
{
	int retval;

	// ���� ����
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		//	err_quit("socket()");
	}
	// ��� ������ ����. [Windows�� ���� ����(�⺻��). UNIX/Linux�� OS���� �ٸ�]
	int no = 1;
	setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&no, sizeof(no));

	// bind()
	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(PORTNUM);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		//	err_quit("bind()");
	}
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		//err_quit("listen()");
	}
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	struct sockaddr_in6 clientaddr;
	int addrlen;
	char buf[BUFFERSIZE + 1];

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			//err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char ipaddr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientaddr.sin6_addr, ipaddr, sizeof(ipaddr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			ipaddr, ntohs(clientaddr.sin6_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFFERSIZE, 0);
			if (retval == SOCKET_ERROR) {
				//err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// ���� �ݱ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			ipaddr, ntohs(clientaddr.sin6_port));
	}

	// ���� �ݱ�
	closesocket(listen_sock);
	return 0;
}

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TCPServer4, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// ���� ����
	WSACleanup();
	return 0;
}
