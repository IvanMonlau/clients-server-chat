#include <winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <map>

using namespace std;

vector<SOCKET> clients;
mutex clients_mutex;
map<SOCKET, string> clientNames; 

void handle_client(SOCKET clientSocket) {
    char buffer[1024];

    // Recibir nombre de usuario del cliente
    recv(clientSocket, buffer, 1024, 0);
    string username = buffer;
    clientNames[clientSocket] = username;
    cout << "Nuevo usuario conectado: " << username << endl;

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            cerr << username + " se ha desconectado." << endl;
            {
                lock_guard<mutex> lock(clients_mutex);
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (*it == clientSocket) {
                        clients.erase(it);
                        clientNames.erase(clientSocket); // Eliminar nombre de usuario
                        break;
                    }
                }
            }
            closesocket(clientSocket);
            return;
        }

        // Construir mensaje con nombre de usuario
        string messageWithUsername = buffer;

        {
            lock_guard<mutex> lock(clients_mutex);
            for (SOCKET client : clients) {
                if (client != clientSocket) {
                    send(client, messageWithUsername.c_str(), messageWithUsername.length(), 0);
                }
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    cout << "Servidor escuchando en el puerto 12345..." << endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

        {
           lock_guard<mutex> lock(clients_mutex);
           clients.push_back(clientSocket);
        }
        thread clientThread(handle_client, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}