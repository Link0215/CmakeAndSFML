#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>
#include <list>


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
static string host;

UdpSocket g_serverSocket;

//void ping();

class NertworkClient
{
public:
	NertworkClient() = default;
	~NertworkClient() = default;

	bool operator==(const NertworkClient& other)
	{
		return (m_ip.value().toInteger() == other.m_ip.value().toInteger()
			&& m_port == other.m_port);
	}
	bool operator!=(const NertworkClient& other)
	{
		return !this->operator==(other);
	}

public:
	optional<IpAddress> m_ip;
	uint16 m_port;
};

vector <NertworkClient> g_clientlist;

void runUdpServer(uint16 port)
{
	if (g_serverSocket.bind(port) != Socket::Done)
	{
		return;
	}

	cout << "Servidor esta escuchando en el puerto: " << port << endl;
}

void waitForMessage()
{
	NertworkClient client;
	vector<char> inBufferData;
	std::optional<sf::IpAddress> sender;
	uint16 senderPort;
	inBufferData.resize(MESSAGE_BUFFER_SIZE);
	rsize_t receiveMessageSize;

	Socket::Status retValue =
	g_serverSocket.receive(&inBufferData[0],
							inBufferData.size(),
							receiveMessageSize,
							sender,
							senderPort);
	client.m_ip = sender;
	client.m_port = senderPort;
	g_clientlist.push_back(client);
	if (retValue != Socket::Done)
	{//Error
		return;
	}
	cout << "Mensaje recibido del cliente: "
		<< client.m_ip.value() << ": "
		<< quoted(inBufferData.data()) << endl;
	/*ping();*/
	cout << endl;
}

void sendMessage(const string &message)
{
	NertworkClient client;
	int lenght = message.length();
	int size =

	g_serverSocket.send(message.data(),
						message.length() + 1,
						g_clientlist[0].m_ip.value(),
						g_clientlist[0].m_port);
}

//void ping()
//{
//	system("cls");
//	cout << "Host: ";
//	host = "192.168.1.71";
//	system(("ping " + host).c_str());
//	cout << endl;
//	return;
//}

int main()
{
	uint32 numMessagesReceived = 0;
	std::stringstream messageToUsers;
	runUdpServer(APP_PORT);

	while (true)
	{
		waitForMessage();
		++numMessagesReceived;
		messageToUsers.str("");
		messageToUsers.clear();
		messageToUsers << "Mensaje al cliente num: " << numMessagesReceived;
		sendMessage(messageToUsers.str());
	}
	return 0;
}