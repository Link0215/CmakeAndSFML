#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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

struct MESSAGE
{
	string contentMessage;
	sf::Color color;
	int thickness;
};

struct LINE
{
	sf::Vector2i pos_ini;
	sf::Vector2i pos_end;
	sf::Color color;
	int thickness;
};
struct SQUARE
{
	sf::Vector2i pos_ini;
	sf::Vector2i pos_end;
	sf::Color color;
};
struct CIRCLE
{
	sf::Vector2i pos_ini;
	float radius;
	sf::Color color;
};
struct TRIANGLE
{
	sf::Vector2i pos_ini;
	float base;
	sf::Color color;
};

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
	for (int i = 1; i < 4; i++)
	{
		sendMessage(packet);
	}
}

void CreateWindows()
{
	sf::Window window(sf::VideoMode({ 640, 480 }), "Chat");

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		window.display();
	}
}

int main()
{
	uint32 numMessagesReceived = 0;
	std::string messageToUsers;
	runUdpClient(APP_PORT);

	struct LINE linea;

	CreateWindows();

	/*while (true)
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
	return 0;*/
}