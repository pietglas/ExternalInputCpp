#include "ProvideOutput.h"
#include <iostream>
#include <cstdlib>
#include <csignal>

ProvideOutput& ProvideOutput::getInstance(char *outputaddr)
{
	static ProvideOutput handler{outputaddr};

	return handler;
}

void ProvideOutput::waitForInput()
{
	std::signal(SIGINT, signalHandler);

	while(signum == 0)
	{
		// std::getline(std::cin, strinput);
		// input = strinput.c_str();
		std::cin.getline(input, DEFAULT_BUFLEN);

		if (strinput.length() != 0)
		{
			int result = sendInput();
			if (result == SOCKET_ERROR)
			{
				break;
			}
		}
	}
	closeSock();
}

int ProvideOutput::sendInput()
{
	int result = 0;

	result = send(sock, input, static_cast<int>(strlen(input)), 0);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "An error occured when trying to send the data"
		", closing the socket." << std::endl;
		closeSock();
	}

	return result;
}

int ProvideOutput::openSock()
{
	WSADATA wsa_data;
	struct addrinfo *res = nullptr, 
					*ptr = nullptr,
					hints;

	// initialize winsock
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != 0)
	{
		std::cerr << "WSAStartup failed with error " << result << std::endl;
		return result;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// get server adres and info 
	result = getaddrinfo(output_address, DEFAULT_PORT, &hints, &res);
	if (result != 0)
	{
		std::cerr << "getaddrinfo failed with error " << result << std::endl;
		WSACleanup();
		return result;
	}

	// attempt to connect to an address
	for (ptr = res; ptr != nullptr; ptr = ptr->ai_next)
	{
		// create a socket to connect to external party
		sock = socket(ptr->ai_family, 
						ptr->ai_socktype, 
						ptr->ai_protocol
						);
		if (sock == INVALID_SOCKET)
		{
			std::cerr << "socket() failed with error " << sock << std::endl;
			WSACleanup();
			return 1;
		}

		// connect to external party
		result = connect(sock, 
						ptr->ai_addr,
						static_cast<int>(ptr->ai_addrlen)
						);
		if (result == SOCKET_ERROR)
		{
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(res);

	if (sock == INVALID_SOCKET)
	{
		std::cerr << "unable to connect to external party" << std::endl;
		WSACleanup();
		return 2;
	}

	return 0;
}

void ProvideOutput::closeSock()
{
	closesocket(sock);
	WSACleanup();
}

void ProvideOutput::signalHandler(int signm)
{
	signum = signm;
}

int ProvideOutput::signum = 0;	// C++17: can define in class using 'inline'