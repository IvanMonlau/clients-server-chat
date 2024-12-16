#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

void receive_messages(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            cerr << "Conexión con el servidor cerrada." << endl;
            return;
        }
        cout.write(buffer, bytesReceived);
        cout << endl;
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Ajusta el puerto si es necesario
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Ajusta la dirección IP del servidor si es necesario

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error al conectar con el servidor." << endl;
        return 1;
    }

    cout << "Conectado al servidor." << endl;

    // Solicitar nombre de usuario
    cout << "Ingrese su nombre de usuario: ";
    string username;
    getline(cin, username);

    // Enviar nombre de usuario al servidor
    send(clientSocket, username.c_str(), username.length(), 0);

    // Crear hilo para recibir mensajes
    thread receiveThread(receive_messages, clientSocket);
    receiveThread.detach();

    // Bucle para enviar mensajes
    string message;
    while (getline(cin, message)) {
        // Construir mensaje completo con nombre de usuario (corregido)
        string fullMessage = username + ": " + message; 
        if (send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0) == SOCKET_ERROR) {
            cerr << "Error al enviar mensaje." << endl;
            break;
        }
    }

    // Cerrar socket y limpiar
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}