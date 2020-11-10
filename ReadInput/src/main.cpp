#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char** argv)
{
	WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Start a seperate process voor std::cin
    char* module = (char*)"C:\\Users\\Piet\\ExternalInputCpp\\ProvideInput\\build\\src\\main.exe";
    char* host = (char*)"main.exe localhost";
    STARTUPINFO startinfo;
    PROCESS_INFORMATION procinfo;

    ZeroMemory( &startinfo, sizeof(startinfo) );
    startinfo.cb = sizeof(startinfo);
    ZeroMemory( &procinfo, sizeof(procinfo) );

	if (
		CreateProcessA(module,	// module name 
			host,				// hostname (IP), command line argument to exe
			NULL,				// process handle not inheritable
			NULL,				// thread handle not inheritable	
			FALSE,				// set handle inheritance to false
			CREATE_NEW_CONSOLE,	// no creation flags
			NULL,				// use parent's environment block
			NULL,				// use parent's startup directory
			&startinfo,			// pointer to STARTUPINFO structure
			&procinfo )			// pointer to PROCESS_INFORMATION structure
		)
	{
		// Accept a client socket
	    ClientSocket = accept(ListenSocket, NULL, NULL);
	    if (ClientSocket == INVALID_SOCKET) 
	    {
	        printf("accept failed with error: %d\n", WSAGetLastError());
	        closesocket(ListenSocket);
	        WSACleanup();
	        return 1;
	    }

	    // No longer need server socket
	    closesocket(ListenSocket);

	    // Receive until the peer shuts down the connection
	    do 
	    {

	        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	        if (iResult > 0) 
	        {
	            std::cout << recvbuf << std::endl;
	            memset(recvbuf, 0, DEFAULT_BUFLEN);
	        }
	        else if (iResult == 0)
	            std::cerr << "Connection closing..." << std::endl;
	        else  
	        {
	            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
	            closesocket(ClientSocket);
	            WSACleanup();
	            return 1;
	        }

	    } while (iResult > 0);

	    // shutdown the connection since we're done
	    iResult = shutdown(ClientSocket, SD_SEND);
	    if (iResult == SOCKET_ERROR) {
	        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;
	        closesocket(ClientSocket);
	        WSACleanup();
	        return 1;
	    }

	    // cleanup
	    closesocket(ClientSocket);
	    WSACleanup();

	    // Wait until child process exits.
	    WaitForSingleObject( procinfo.hProcess, INFINITE );

	    // Close process and thread handles. 
	    CloseHandle( procinfo.hProcess );
	    CloseHandle( procinfo.hThread );
	}
	else 
	{
		std::cerr << "CreateProcessA failed with error "
			<< GetLastError() << std::endl;
	}
}