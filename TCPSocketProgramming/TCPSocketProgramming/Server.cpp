#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄기
#pragma comment(lib, "ws2_32.lib") // ws2_32.lib 링크

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

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
	memset(&serveraddr, 0, sizeof(serveraddr)); // 0으로 초기화
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 000으로 초기화
	serveraddr.sin_port = htons(PORTNUM); // 서버포트 9000

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
		// Client Info 출력
		printf("\nClient connection successful!  \nIP Address : %s \nPort Number : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		// inet_ntoa : (Ipv4) 인터넷 네트워크 주소를 인터넷 표준 점선 소수점 형식의 ASCII 문자열로 변환
		// ntohs : u_short TCP/IP 네트워크 바이트 순서에서 호스트 바이트 순서(Intel 프로세서의 little-endian)로 변환

		// 데이터 통신
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

			// 받은 데이터 출력
			buf[retval] = '\0'; // 마지막에 null 넣어주기
			printf("%s", buf);
		}

		closesocket(clientSocket);
		printf("\nThe connection has been terminated! \nIP Address : %s,\nPort Number : %d\n",
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	}
	closesocket(listenSocket);
	return 0;
}
// TCP 서버(IPv6)
DWORD WINAPI TCPServer6(LPVOID arg)
{
	int retval;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		//	err_quit("socket()");
	}
	// 듀얼 스택을 끈다. [Windows는 꺼져 있음(기본값). UNIX/Linux는 OS마다 다름]
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
	// 데이터 통신에 사용할 변수
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

		// 접속한 클라이언트 정보 출력
		char ipaddr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientaddr.sin6_addr, ipaddr, sizeof(ipaddr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			ipaddr, ntohs(clientaddr.sin6_port));

		// 클라이언트와 데이터 통신
		while (1) {
			// 데이터 받기
			retval = recv(client_sock, buf, BUFFERSIZE, 0);
			if (retval == SOCKET_ERROR) {
				//err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// 소켓 닫기
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			ipaddr, ntohs(clientaddr.sin6_port));
	}

	// 소켓 닫기
	closesocket(listen_sock);
	return 0;
}

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TCPServer4, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 윈속 종료
	WSACleanup();
	return 0;
}
