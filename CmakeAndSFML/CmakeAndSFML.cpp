#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

void runTcpServer(unsigned short port);
void runTcpClient(unsigned short port);
void runUdpServer(unsigned short port);
void runUdpClient(unsigned short port);

int main()
{
    // Elija un puerto arbitrario para abrir sockets
    const unsigned short port = 50001;

    //Cliente o servidor
    char who;
    cout << "Quieres ser un servidor (s) o un cliente (c)? " << endl;
    cin >> who;
    if (who == 's')
    {
        runUdpServer(port);
    }
    else
    {
        runUdpClient(port);
    }
    return EXIT_SUCCESS;
}