#include <SFML/Network.hpp>

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

////////////////////////////////////////////////////////////
/// Inicie un servidor, espere una conexión entrante,
/// envía un mensaje y espera la respuesta.
///
////////////////////////////////////////////////////////////
void runTcpServer(unsigned short port)
{
	// Crear un socket de servidor para aceptar nuevas conexiones
	sf::TcpListener listener;

	// Escuche el puerto dado para las conexiones entrantes
	if (listener.listen(port) != sf::Socket::Done)
		return;
	cout << "El servidor esta escuchando el puerto " << port << ", esperando conexiones..." << endl;

	// Espera una conexión
	sf::TcpSocket socket;
	if (listener.accept(socket) != sf::Socket::Done)
		return;
	cout << "Cliente conectado: " << socket.getRemoteAddress().value() << endl;

	// Mandar un mensaje al cliente conectado
	char		in[128];
	std::size_t received;
	if (socket.receive(in, sizeof(in), received) != sf::Socket::Done)
		return;
	cout << "Respuesta recibida del cliente: " << std::quoted(in) << endl;
}

////////////////////////////////////////////////////////////
/// Cree un cliente, conéctelo a un servidor, muestre el
/// mensaje de bienvenida y enviar una respuesta.
///
////////////////////////////////////////////////////////////
void runTcpClient(unsigned short port)
{
	// Pregunta por la dirección del servidor
	std::optional<sf::IpAddress> server;
	do
	{
		std::cout << "Escriba la direccion o el nombre del servidor al que conectarse: " << endl;
		cin >> server;
	} while (!server.has_value());

	// Crear un socket para comunicarse con el servidor
	sf::TcpSocket socket;

	// Conectarse al servidor
	if (socket.connect(server.value(), port) != sf::Socket::Done)
		return;
	cout << "Conectado al servidor " << server.value() << endl;

	// Recibir un mensaje del servidor
	char        in[128];
	std::size_t received;
	if (socket.receive(in, sizeof(in), received) != sf::Socket::Done)
		return;
	cout << "Mensaje enviado al servidor: " << std::quoted(in) << endl;

	// Enviar una respuesta al servidor
	const char out[] = "Hola soy un cliente";
	if (socket.send(out, sizeof(out) != sf::Socket::Done))
		return;
	cout << "Mensaje enviado al servidor: " << std::quoted(out) << endl;
}