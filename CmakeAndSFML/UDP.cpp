#include <SFML/Network.hpp>

#include <iomanip>
#include <iostream>
#include <optional>
#include <fstream>
#include <conio.h>
#include <string>
#include "bits-stdc++.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

////////////////////////////////////////////////////////////
/// Inicie un servidor, espere un mensaje, envíe una respuesta.
///
////////////////////////////////////////////////////////////

bool checkUsers(string nombre, string contra)
{
	std::fstream usuarios;

	// Servirá para avanzar en las casillas del arreglo
	int contador = 0;

	// Variable temporal donde se almacenarán usuario y contraseña separados por coma y obtenidos desde el archivo de texto
	string linea;

	// variable que contendrá la posición donde se encuentra la coma que separa el usuario de la contraseña
	size_t pos = 0;

	// Contendrá temporalmente el usuario o la contraseña mientras se separan
	string dato;

	// Este arreglo servirá para separar el usuario y la contraseña guardándolos en una casilla distinta
	string usuarioContra[2];

	// Servirá para darle fin a un bucle
	int fin = 0;

	usuarios.open("usuarios.txt", std::ios_base::app | std::ios_base::in);
	if (usuarios.is_open())
	{
		while (std::getline(usuarios, linea) && fin == 0)
		{
			while ((pos = linea.find(",")) != string::npos)
			{
				dato = linea.substr(0, pos);
				usuarioContra[contador] = dato;
				linea.erase(0, pos + 1);
				contador++;
			}
			if (usuarioContra[0].find(nombre, 0) != string::npos)
			{
				fin = 1;
			}
			contador = 0;
		}
		if (nombre == usuarioContra[0] && contra == usuarioContra[1])
		{
			cout << "Bienvenido al servidor de Bernardo" << endl;
			system("pause");
			system("cls");
			usuarios.close();
			return true;
		}
		else
		{
			if (nombre != usuarioContra[0])
			{
				system("cls");
				cout << "Nombre de usuario incorrecto, intentelo de nuevo" << endl;
				usuarios.close();
				return false;
			}
			else if (contra != usuarioContra[1])
			{
				system("cls");
				cout << "Contra incorrecta, intentelo de nuevo" << endl;
				usuarios.close();
				return false;
			}
		}
	}
	return false;
}

void runUdpServer(unsigned short port)
{
	string exit = "";
	// Crear un socket para recibir un mensaje de cualquier persona
	sf::UdpSocket socket;

	
	char						 in[128];
	std::size_t					 received;
	std::optional<sf::IpAddress> sender;
	unsigned short				 senderPort;
	// Escuche los mensajes en el puerto especificado
	cout << "El servidor esta escuchando el puerto " << port << ", esperando conexiones..." << endl;
	do
	{
		if (socket.bind(port) != sf::Socket::Done)
		{
			return;
		}
		// Espera un mensaje
		if (socket.receive(in, sizeof(in), received, sender, senderPort) != sf::Socket::Done)
			return;
		cout << "Mensaje recibido del cliente con la IP: " << sender.value() << ": " << std::quoted(in) << endl;

		// Enviar una respuesta al cliente
		if (socket.send(in, sizeof(in), sender.value(), senderPort) != sf::Socket::Done)
			return;
		cout << "Mensaje enviado al cliente: " << std::quoted(in) << endl;
	} while (true);
}

////////////////////////////////////////////////////////////
/// Envía un mensaje al servidor, espera la respuesta
///
////////////////////////////////////////////////////////////
void runUdpClient(unsigned short port)
{
	string						 destinatario = "";
	int							 nuevoDestinatario;
	string						 nombre;
	string						 contra;
	string					     confirmContra;
	char						 in[128];
	string				         out;
	out.resize(128, 0);
	std::size_t					 received;
	std::optional<sf::IpAddress> sender;
	unsigned short				 senderPort;

	// Variable dicta si las contraseñas son iguales a la hora de crear un usario
	bool contraIgual = false;

	// Variable que determina si lo que ingreso el usuario son correctos o no
	bool usuarioContraCorrectos = false;

	// Variable que manejara el archivo donde estan los usuarios
	std::fstream usuarios;

	// Variables para que el usuario eliga entre distintas opciones
	int UsuarioNu;

	// Pregunta por la dirección del servidor
	std::optional<sf::IpAddress> server;
	do
	{
		cout << "Escriba la direccion o el nombre del servidor al que conectarse: " << endl;
		cin >> server;
		system("cls");
	} while (!server.has_value());

	// Crear un socket para comunicarse con el servidor
	sf::UdpSocket socket;
	usuarios.open("usuarios.txt", std::ios_base::app | std::ios_base::in);

	if (usuarios.is_open())
	{
		cout << "Quiere registrarse o ingresar a una cuenta ya existente?" << endl;
		cout << "1) Registrarse\n2) Ingresar" << endl;
		cin >> UsuarioNu;

		switch (UsuarioNu)
		{
		case 1:
			cout << "Inserte su nombre de usuario: ";
			cin >> nombre;
			do
			{
				cout << "Inserte la contra: ";
				cin >> contra;
				cout << "Ingresela de nuevo: ";
				cin >> confirmContra;
				if (confirmContra == contra)
				{
					contraIgual = true;
				}
				else
				{
					system("clear");
					cout << "Inserte su nombre de usuario: " << nombre << endl;
					cout << "Las contraseñas no coinciden, vuelva a intentarlo" << endl;
				}
			} while (contraIgual == false);
			usuarios << nombre << "," << contra << "," << endl;
			cout << "Felicidades, se ha creado exitosamente su usuario" << endl;
			system("pause");
			system("cls");
			break;
		case 2:
			do
			{
				cout << "Ingresa tu nombre de usuario: ";
				cin >> nombre;
				cout << "Ingresa tu contra: ";
				cin >> contra;
				usuarioContraCorrectos = checkUsers(nombre, contra);
			} while (usuarioContraCorrectos == false);
			break;
		default:
			break;
		}
	}
	usuarios.close();

	cout << "Con que usuario quieres hablar? ";
	cin >> destinatario;
	do
	{
		// Enviar un mensaje al servidor
		cout << "Mensaje: " << endl;
		cin.clear();
		/*cin.getline(&out[0], 128);*/
		cin >> out;
		if (socket.send(&out[0], out.size(), server.value(), port) != sf::Socket::Done)
			return;
		if (destinatario == nombre)
		{
			// Reciba una respuesta de cualquier persona (pero probablemente del servidor)
			if (socket.receive(in, sizeof(in), received, sender, senderPort) != sf::Socket::Done)
				return;
			cout << "Mensaje recibido de " << sender.value() << ": " << std::quoted(in) << endl;
		}
		cout << "Hablar con alguien mas? " << endl;
		cout << "1) Si\n2) No" << endl;
		cin >> nuevoDestinatario;
		if (nuevoDestinatario == 1)
		{
			cout << "Ingrese nuevo destinatario: " << endl;
			cin >> destinatario;
			system("cls");
		}
		else
		{

		}
	} while (true);
}