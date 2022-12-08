#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
using std::string;
using StringStream = std::basic_stringstream<char>;
using sf::UdpSocket;
using sf::Socket;
using sf::IpAddress;
using std::quoted;
//forward
enum class MESSAGE_TYPE;
const uint16 APP_PORT = 35001;
const uint32 MESSAGE_BUFFER_SIZE = 128;
std::optional<sf::IpAddress> g_serverIP;
std::optional<sf::IpAddress> g_clientIP;
UdpSocket g_serverSocket;
optional<IpAddress> g_senderIP;
uint16 g_senderPort;
uint32 g_msgAutoIncrement;
sf::RenderWindow g_window;
int g_selectedTool = 1;
int g_selectedColor = 5;
UdpSocket g_clientSocket;
void waitForMessage();
void sendMessage(const string& message);
template<typename T>
void SendShapeMsg(const T& shape, MESSAGE_TYPE type, const IpAddress& remoteAdress, unsigned short remotePort);
void Ping();
uint32 GetNewMessageID();
sf::Color g_color;

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
	size_t recievedMessageSize;
	Socket::Status retValue =
			g_serverSocket.receive(&inBufferData[0],
			inBufferData.size(),
			recievedMessageSize,
			g_senderIP,
			g_senderPort);
	if (retValue != Socket::Done)
	{ // ERROR, you absolute unit
		return;
	}
	cout << "Mensaje recibido del: " << g_senderIP.value() << ": " << quoted(inBufferData.data()) << endl;
}

void sendMessage(const string& message)
{
	if (message == "cmd::ping")
	{
		Ping(message);
	}
	int lenght = message.length();
	int size =

		g_clientSocket.send(message.data(),
			message.length() + 1,
			g_clientIP.value(),
			APP_PORT);
}


void Ping(const string& message)
{
	double initialTime = clock();
	sendMessage(message);
	double finalTime = clock();
	double totalPing = finalTime - initialTime;
	cout << "Total ping: " << totalPing << " ms" << endl;
}

enum class MESSAGE_TYPE
{
	MSG_ERROR = 0,
	MSG_SQUARE,
	MSG_CIRCLE,
	MSG_CHAT,
	MSG_BRUSH,
	/*MSG_TRIANGLE,*/
};
enum class COLOR_TYPE
{
	CLR_NONE = 0,
	CLR_RED,
	CLR_CYAN,
	CLR_BLACK,
	CLR_BLUE,
	CLR_GREEN,
	CLR_MAGENTA,
	CLR_YELLOW,
};

struct NETWORK_MESSAGE
{
	~NETWORK_MESSAGE()
	{
		if (data)
		{
			delete data;
		}
	}
	uint32 id = 0;
	MESSAGE_TYPE type = MESSAGE_TYPE::MSG_ERROR;
	uint32 checksum = 0;
	size_t data_size = 0;
	void* data = nullptr;
};
struct _MSG
{
	string contentMessage;
	sf::Color color = g_color;
	int thickness;
};
struct LINE
{
	sf::Vector2i pos_ini;
	sf::Vector2i pos_end;
	sf::Color color = g_color;
	int thickness;
};LINE g_line;
struct SQUARE
{
	int id = 2;
	sf::Vector2i pos_ini;
	sf::Vector2i pos_end;
	sf::Color color = g_color;
	sf::RectangleShape shape;
}; SQUARE g_square;
struct CIRCLE
{
	sf::Vector2i pos_ini;
	sf::Vector2i pos_end;
	float radius;
	sf::Color color = g_color;
	sf::CircleShape shape;
}; CIRCLE g_circle;
struct TRIANGLE
{
	sf::Vector2i pos_ini;
	float base;
	sf::Color color = g_color;
	//sf::CircleShape triangle(50, 3);
}; TRIANGLE g_triangle;

struct BRUSH
{
	sf::Vector2f pos_ini;
	float radius;
	sf::Color color;
	sf::CircleShape shape;
}; BRUSH g_brush;

float g_brushSize = 10;
std::vector<BRUSH> g_brushVector;
std::vector<SQUARE> g_squareVector;
std::vector<CIRCLE> g_circleVector;
std::vector<TRIANGLE> g_triangleVector;
std::vector<LINE> g_lineVector;

uint32 GetNewMessageID()
{
	return ++g_msgAutoIncrement;
}

uint32 Checksum(const void* pData, size_t dataSize)
{
	return uint32();
}

void sendMessage(const NETWORK_MESSAGE& message, const IpAddress& remoteAdress, unsigned short remotePort)
{
	// size of the message
	size_t msgSize = sizeof(message.id) + sizeof(message.type) + sizeof(message.checksum) + sizeof(message.data_size) + message.data_size;
	NETWORK_MESSAGE* msg = const_cast<NETWORK_MESSAGE*>(&message);
	vector<char> buffer;
	buffer.reserve(msgSize);
	//buffer.insert(buffer.end(), sizeof(message.id), *reinterpret_cast<char*>(&message.id));
	buffer.insert(buffer.end(), sizeof(message.id), *reinterpret_cast<char*>(msg->id));
	buffer.insert(buffer.end(), sizeof(message.type), *reinterpret_cast<char*>(msg->type));
	buffer.insert(buffer.end(), sizeof(message.checksum), *reinterpret_cast<char*>(msg->checksum));
	buffer.insert(buffer.end(), msg->data_size, *reinterpret_cast<char*>(msg->data));
	// [ NM::id, NM::Type, NM:: checksum, NM:: data_size, data[data_size] ]
	g_serverSocket.send(buffer.data(),
						buffer.size(), // send messages
						remoteAdress,
						remotePort);
}

template<typename TYPE> void SendShapeMsg(const TYPE& shape, MESSAGE_TYPE type, const IpAddress& remoteAdress, unsigned short remotePort)
{
	NETWORK_MESSAGE newMessage;
	newMessage.id = GetNewMessageID();
	newMessage.type = type;
	newMessage.data = new TYPE;
	newMessage.data_size = sizeof(TYPE);
	memcpy(newMessage.data, &shape, sizeof(TYPE));
	newMessage.checksum = 0xFFFFFFFF;
	sendMessage(newMessage, remoteAdress, remotePort);
	delete newMessage.data;
}

void ProcessEvent(sf::Event event);
bool g_brushMoving = false;

void UpdateScreen();

void ProcessEvent(sf::Event event)
{
	if (event.type)
	{
		// window closed
		if (event.type == sf::Event::Closed ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			g_window.close();
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Num1) // SQUARE
			{
				g_selectedTool = 1;
			}
			else if (event.key.code == sf::Keyboard::Num2) // CIRCLE
			{
				g_selectedTool = 2;
			}
			else if (event.key.code == sf::Keyboard::Num3) // CHAT
			{
				g_selectedTool = 3;
			}
			else if (event.key.code == sf::Keyboard::Num4) // BRUSH
			{
				g_selectedTool = 4;
			}
			//else if (event.key.code == sf::Keyboard::Num5) // TRIANGLE
			//{
			//	g_selectedTool = 5;
			//}
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				g_brushMoving = true;
				g_square.pos_ini.x = event.mouseButton.x;
				g_square.pos_ini.y = event.mouseButton.y;
				g_circle.pos_ini.x = event.mouseButton.x;
				g_circle.pos_ini.y = event.mouseButton.y;
				g_triangle.pos_ini.x = event.mouseButton.x;
				g_triangle.pos_ini.y = event.mouseButton.y;
			}
			else
			{

			}
		}
		if (event.type == sf::Event::MouseMoved)
		{
			/// <summary>
			/// in case the brush tool is selected and moving
			/// </summary>
			/// <param name="event"></param>
			if (g_selectedTool == (int)MESSAGE_TYPE::MSG_BRUSH && g_brushMoving)
			{
				sf::Vector2i position = sf::Mouse::getPosition();
				RECT clientRect;
				GetClientRect(g_window.getSystemHandle(), &clientRect);
				sf::Vector2f positionFloat;
				positionFloat.x = position.x - g_brushSize / 2;// x position of circle
				positionFloat.y = position.y - g_brushSize / 2;// y position of circle
				sf::CircleShape shape(g_brushSize, 30);
				shape.setPosition(positionFloat);
				g_brush.shape = shape;
				g_brush.color = g_color;
				g_brush.pos_ini = positionFloat;
				g_brushVector.push_back(g_brush);
				/*SendShapeMsg(shape, MESSAGE_TYPE::MSG_BRUSH, g_clientIP, APP_PORT);*/
			}
		}
		if (event.type == sf::Event::MouseButtonReleased)
		{
			g_brushMoving = false;
			g_square.pos_end.x = event.mouseButton.x;
			g_square.pos_end.y = event.mouseButton.y;
			g_circle.pos_end.x = event.mouseButton.x;
			g_circle.pos_end.y = event.mouseButton.y;
			//g_triangle.pos_end.x = event.mouseButton.x;
			//g_triangle.pos_end.y = event.mouseButton.y;
			if (g_selectedTool == (int)MESSAGE_TYPE::MSG_SQUARE)
			{
				unsigned int sizeX;
				unsigned int sizeY;
				sf::Vector2i position = sf::Mouse::getPosition();
				RECT clientRect;
				GetClientRect(g_window.getSystemHandle(), &clientRect);
				sf::Vector2f positionFloat;
				if (g_square.pos_end.x > g_square.pos_ini.x)
				{
					sizeX = g_square.pos_end.x - g_square.pos_ini.x;
					positionFloat.x = position.x - clientRect.left - sizeX;
				}
				else
				{
					sizeX = g_square.pos_ini.x - g_square.pos_end.x;
					positionFloat.x = position.x + clientRect.left;
				}
				if (g_square.pos_end.y > g_square.pos_ini.y)
				{
					sizeY = g_square.pos_end.y - g_square.pos_ini.y;
					positionFloat.y = position.y - clientRect.top - sizeY;
				}
				else
				{
					sizeY = g_square.pos_ini.y - g_square.pos_end.y;
					positionFloat.y = position.y + clientRect.top;
				}
				sf::RectangleShape shape(sf::Vector2f(sizeX, sizeY));
				g_square.shape = shape;
				shape.setFillColor(g_square.color);
				g_square.shape.setPosition(positionFloat);
				g_squareVector.push_back(g_square);
				/*SendShapeMsg(shape, MESSAGE_TYPE::MSG_SQUARE, g_clientIP, APP_PORT);*/
			}
			else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_CIRCLE)
			{
				unsigned int sizeX;
				unsigned int sizeY;
				sf::Vector2i position = sf::Mouse::getPosition();
				RECT clientRect;
				GetClientRect(g_window.getSystemHandle(), &clientRect);
				sf::Vector2f positionFloat;
				if (g_circle.pos_end.x > g_circle.pos_ini.x)
				{
					sizeX = g_circle.pos_end.x - g_circle.pos_ini.x;
					positionFloat.x = position.x - clientRect.left - sizeX;
				}
				else
				{
					sizeX = g_circle.pos_ini.x - g_circle.pos_end.x;
					positionFloat.x = position.x + clientRect.left;
				}
				if (g_circle.pos_end.y > g_circle.pos_ini.y)
				{
					sizeY = g_circle.pos_end.y - g_circle.pos_ini.y;
					positionFloat.y = position.y - clientRect.top - sizeY;
				}
				else
				{
					sizeY = g_circle.pos_ini.y - g_circle.pos_end.y;
					positionFloat.y = position.y + clientRect.top;
				}
				sf::CircleShape shape(1, 30);
				float sizeYCircle = sizeY / 2;
				float sizeXCircle = sizeX / 2;
				sf::Vector2f scaleCircle;
				scaleCircle.x = sizeXCircle;
				scaleCircle.y = sizeYCircle;
				shape.setScale(scaleCircle);
				g_circle.shape = shape;
				g_circle.shape.setFillColor(g_circle.color);
				g_circle.shape.setPosition(positionFloat);
				g_circleVector.push_back(g_circle);
				/*SendShapeMsg(shape, MESSAGE_TYPE::MSG_CIRCLE, g_clientIP, APP_PORT);*/
			}
			else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_CHAT)
			{
				unsigned int sizeX;
				unsigned int sizeY;
				sf::Vector2i position = sf::Mouse::getPosition();
				RECT clientRect;
				GetClientRect(g_window.getSystemHandle(), &clientRect);
				sf::Vector2f positionFloat;
				if (g_circle.pos_end.x > g_circle.pos_ini.x)
				{
					sizeX = g_circle.pos_end.x - g_circle.pos_ini.x;
					positionFloat.x = position.x - clientRect.left - sizeX;
				}
				else
				{
					sizeX = g_circle.pos_ini.x - g_circle.pos_end.x;
					positionFloat.x = position.x + clientRect.left - sizeX +
						sizeX;
				}
				if (g_circle.pos_end.y > g_circle.pos_ini.y)
				{
					sizeY = g_circle.pos_end.y - g_circle.pos_ini.y;
					positionFloat.y = position.y - clientRect.top - sizeY;
				}
				else
				{
					sizeY = g_circle.pos_ini.y - g_circle.pos_end.y;
					positionFloat.y = position.y + clientRect.top - sizeY +
						sizeY;
				}
				sf::CircleShape shape(sizeX, 30);
				g_circle.shape = shape;
				g_circle.shape.setFillColor(g_circle.color);
				g_circle.shape.setPosition(positionFloat);
				g_circleVector.push_back(g_circle);
				/*SendShapeMsg(shape, MESSAGE_TYPE::MSG_CHAT, g_clientIP, APP_PORT);*/
			}
			/*else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_TRIANGLE)
			{
				g_triangle.pos_ini.x = event.mouseButton.x;
				g_triangle.pos_ini.y = event.mouseButton.y;
				unsigned int sizeX;
				unsigned int sizeY;
				sf::Vector2i position = sf::Mouse::getPosition();
				RECT clientRect;
				GetClientRect(g_window.getSystemHandle(), &clientRect);
				sf::Vector2f positionFloat;
				if (g_square.pos_end.x > g_square.pos_ini.x)
				{
					sizeX = g_triangle.pos_ini.x - g_square.pos_ini.x;
					positionFloat.x = position.x - clientRect.left - sizeX;
				}
				else
				{
					sizeX = g_square.pos_ini.x - g_square.pos_end.x;
					positionFloat.x = position.x + clientRect.left - sizeX + sizeX;
				}
				if (g_square.pos_end.y > g_square.pos_ini.y)
				{
					sizeY = g_square.pos_end.y - g_square.pos_ini.y;
					positionFloat.y = position.y - clientRect.top - sizeY;
				}
				else
				{
					sizeY = g_square.pos_ini.y - g_square.pos_end.y;
					positionFloat.y = position.y + clientRect.top - sizeY + sizeY;
				}
				sf::RectangleShape shape(sf::Vector2f(sizeX, sizeY));
				g_square.shape = shape;
				shape.setFillColor(g_square.color);
				g_square.shape.setPosition(positionFloat);
				g_squareVector.push_back(g_square);
				SendShapeMsg(shape, MESSAGE_TYPE::MSG_TRIANGLE, g_clientIP, APP_PORT);
			}*/
		}
	}
	if (event.type == sf::Event::MouseWheelScrolled)
	{
		if (event.mouseWheelScroll.delta > 0) // moving up
		{
			++g_brushSize;
		}
		else if (event.mouseWheelScroll.delta < 0) // moving down
		{
			if (1 <= g_brushSize)
			{
				--g_brushSize;
			}
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		g_color = sf::Color::Red;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		g_color = sf::Color::Cyan;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::B))
	{
		g_color = sf::Color::Black;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::V))
	{
		g_color = sf::Color::Blue;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
		g_color = sf::Color::Green;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		g_color = sf::Color::Magenta;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
	{
		g_color = sf::Color::Yellow;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		g_window.clear(sf::Color::Black);
		UpdateScreen();
		// Display window contents on screen
		g_window.display();
	}
}

void RenderStuff();

void UpdateScreen()
{
	RenderStuff();
}

void RenderStuff()
{
	sf::Font font;
	std::string textFontName = "bahnschrift.ttf";
	if (!font.loadFromFile(textFontName))
	{
		std::cout << "ERROR::SocketTest::main.cpp: Error loading text font " << textFontName << std::endl;
	}
	sf::Text textTool;
	std::string toolSelected = "Herramienta Actual: ";
	sf::Text BrushSize;
	std::string BrushGirth = "Tamaño del pincel: ";
	sf::Text colorSelected;
	std::string colorSelectedText = "Color Actual: ";
	/// GETTING WHAT IS BEING SELECTED
	if (g_selectedTool == (int)MESSAGE_TYPE::MSG_SQUARE)
	{
		toolSelected += "Square";
	}
	else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_CIRCLE)
	{
		toolSelected += "Circle";
	}
	else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_CHAT)
	{
		toolSelected += "Text";
	}
	/*else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_TRIANGLE)
	{
		toolSelected += "Triangle";
	}*/
	else if (g_selectedTool == (int)MESSAGE_TYPE::MSG_BRUSH)
	{
		toolSelected += "Brush";
	}
	std::string brushS = std::to_string(g_brushSize);
	BrushGirth += brushS;
	textTool.setFont(font);
	textTool.setString(toolSelected);
	textTool.setCharacterSize(36);
	textTool.setFillColor(sf::Color::White);
	BrushSize.setFont(font);
	BrushSize.setString(BrushGirth);
	BrushSize.setCharacterSize(36);
	BrushSize.setFillColor(sf::Color::White);
	sf::Vector2f positionBrushTooltip;
	positionBrushTooltip.x = 1400;
	positionBrushTooltip.y = textTool.getPosition().y;
	BrushSize.setPosition(positionBrushTooltip);
	colorSelected.setFont(font);
	colorSelected.setString(colorSelectedText);
	colorSelected.setCharacterSize(36);
	colorSelected.setFillColor(sf::Color::White);
	sf::Vector2f positionColorTooltip;
	positionColorTooltip.x = 1400;
	positionColorTooltip.y = textTool.getPosition().y + 36;
	colorSelected.setPosition(positionColorTooltip);
	sf::RectangleShape colorDisplayer;
	colorDisplayer.setFillColor(g_color);
	sf::Vector2f displayScale;
	displayScale.x = 30;
	displayScale.y = 30;
	colorDisplayer.setSize(displayScale);
	sf::Vector2f positionCubeTooltip;
	positionCubeTooltip.x = 1760;
	positionCubeTooltip.y = 43;
	colorDisplayer.setPosition(positionCubeTooltip);
	for (int i = 0; i < g_circleVector.size(); ++i)
	{
		g_circleVector[i].shape.setFillColor(g_color);
		g_window.draw(g_circleVector[i].shape);
	} 
	for (int j = 0; j < g_squareVector.size(); ++j)
	{
		g_squareVector[j].shape.setFillColor(g_color);
		g_window.draw(g_squareVector[j].shape);
	} 
	for (int k = 0; k < g_brushVector.size(); ++k)
	{
		g_brushVector[k].shape.setFillColor(g_brushVector[k].color);
		g_brushVector[k].shape.setPosition(g_brushVector[k].pos_ini);
		g_window.draw(g_brushVector[k].shape);
	}
	g_window.draw(textTool);
	g_window.draw(BrushSize);
	g_window.draw(colorSelected);
	g_window.draw(colorDisplayer);
}
void RenderScreen()
{
	while (g_window.isOpen())
	{
		// Process events
		sf::Event event;
		while (g_window.pollEvent(event))
		{
			ProcessEvent(event);
		}
		// Clear the screen (fill it with black color)
		g_window.clear();
		UpdateScreen();
		// Display window contents on screen
		g_window.display();
	}
}
int main()
{
	uint32 numMessagesReceived = 0;
	string messageToUser;
	int screenWidth = 1920;
	int screenHeight = 1080;
	int fps = 240;
	runUdpClient(APP_PORT);
	g_window.create(sf::VideoMode(sf::Vector2u(screenWidth, screenHeight)), "Drawing screen running", sf::Style::Fullscreen);
	g_window.setFramerateLimit(fps);
	while (true)
	{
		waitForMessage();
		++numMessagesReceived;
		RenderScreen();
	} return 0;
}