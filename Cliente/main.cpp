#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <winsock.h>

using uint16 = unsigned short;
using uint32 = unsigned int;

using int16 = short;
using int32 = int;

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::list;
using std::size_t;
using std::optional;
using std::quoted;
using std::string;
using std::stringstream;

using sf::UdpSocket;
using sf::Socket;
using sf::IpAddress;

const uint16 APP_PORT = 35001;
const uint32 MESSAGE_BUFFER_SIZE = 128;

UdpSocket g_clientSocket;
std::optional<sf::IpAddress> g_serverIP;
std::optional<sf::IpAddress> g_clientIP;

void runUdpClient(uint16 port)
{
	do
	{
		cout << "Escribe el nombre del servidor o IP: ";
		cin >> g_serverIP;
	} while (!g_serverIP.has_value());
	do
	{
		cout << "Escribe tu IP: ";
		cin >> g_clientIP;
	} while (!g_clientIP.has_value());

}

void waitForMessage()
{
	vector<char> inBufferData;
	inBufferData.resize(MESSAGE_BUFFER_SIZE);
	rsize_t receiveMessageSize;
	std::optional<sf::IpAddress> sender;
	uint16 port;

	Socket::Status retValue =
		g_clientSocket.receive(&inBufferData[0],
								inBufferData.size(),
								receiveMessageSize,
								sender,
								port);
	if (retValue != Socket::Done)
	{//Error
		return;
	}
	cout << "Mensaje recibido del: "
		<< g_serverIP.value() << ": "
		<< quoted(inBufferData.data()) << endl;
}

void sendMessage(const string& message)
{
	int lenght = message.length();
	int size =

		g_clientSocket.send(message.data(),
							message.length() + 1,
							g_clientIP.value(),
							APP_PORT);
}

void ping()
{
	list <double> pingPackets;
	double ping;
	int packetN = 1;
	string packet = "PING";
	unsigned t0, t1;
	for (int i = 1; i < 4; i++)
	{
		sendMessage(packet);
	}
}

int main()
{
	uint32 numMessagesReceived = 0;
	std::string messageToUsers;
	runUdpClient(APP_PORT);

	while (true)
	{
		++numMessagesReceived;
		std::getline(cin, messageToUsers);
		if (messageToUsers == "ping" || messageToUsers == "PING" || messageToUsers == "Ping")
		{
			ping();
		}
		else 
		{
			sendMessage(messageToUsers);
			waitForMessage();
		}
	}
	return 0;
}