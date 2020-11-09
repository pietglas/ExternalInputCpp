#define WIN32_LEAN_AND_MEAN

#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
// #pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
// #pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class ProvideOutput 
{
public:
	static ProvideOutput& getInstance(char *outputaddr);

	ProvideOutput(const ProvideOutput &rhs) = delete;
	void operator =(const ProvideOutput &rhs) = delete;

	static void signalHandler(int signum);
	int openSock();
	void waitForInput();

	static int signum;

private:
	explicit ProvideOutput(char *outputaddr): output_address{outputaddr} {}

	int sendInput();
	void closeSock();

	char *output_address;
	static const size_t max_input_size = 1024;
	char input[DEFAULT_BUFLEN];
	std::string strinput;

	SOCKET sock = INVALID_SOCKET;

};