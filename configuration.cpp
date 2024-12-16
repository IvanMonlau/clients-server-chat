#include <winsock2.h>
#include <iostream>

using namespace std;

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup falló: " << iResult << endl;
        return 1;
    }

    cout << "Sockets configurados correctamente" << endl;

    WSACleanup();
    return 0;
}