#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#define MAX_CLIENTS 100
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

SOCKET server_socket;
struct Order
{
	int time;
	string response;
	bool isRight;
};
Order GetOrder(string order);
vector<string> history;
vector<Order> orders;

int main() {
	system("title Server");

	puts("Start server... DONE.");
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 1;
	}

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		return 2;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		return 3;
	}

	listen(server_socket, MAX_CLIENTS);

	puts("Server is waiting for incoming connections...\nPlease, start one or more client-side app.");

	fd_set readfds; // https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-fd_set
	SOCKET client_socket[MAX_CLIENTS] = {};

	while (true) {
		FD_ZERO(&readfds);
		FD_SET(server_socket, &readfds);

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			SOCKET s = client_socket[i];
			if (s > 0) {
				FD_SET(s, &readfds);
			}
		}
		if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
			printf("select function call failed with error code : %d", WSAGetLastError());
			return 4;
		}
		SOCKET new_socket;
		sockaddr_in address;
		int addrlen = sizeof(sockaddr_in);
		if (FD_ISSET(server_socket, &readfds)) {
			if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
				perror("accept function error");
				return 5;
			}

			for (int i = 0; i < history.size(); i++)
			{
				cout << history[i] << "\n";
				send(new_socket, history[i].c_str(), history[i].size(), 0);
			}
			printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets at index %d\n", i);
					break;
				}
			}
		}
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			SOCKET s = client_socket[i];
			if (FD_ISSET(s, &readfds))
			{
				getpeername(s, (sockaddr*)&address, (int*)&addrlen);

				char client_message[DEFAULT_BUFLEN];

				int client_message_length = recv(s, client_message, DEFAULT_BUFLEN, 0);
				client_message[client_message_length] = '\0';

				string message = client_message;
				Order neworder = GetOrder(message);
				orders.push_back(neworder);
				string check_exit = client_message;
				if (check_exit == "off")
				{
					cout << "Client #" << i << " is off\n";
					client_socket[i] = 0;
				}
				send(client_socket[i], neworder.response.c_str(), neworder.response.length(), 0);
				Sleep(neworder.time * 1000);
				string def = "Your order is ready. Enjoy your meal!";
				if (neworder.isRight)
				{
					send(client_socket[i], def.c_str(), def.length(), 0);
				}
				orders.pop_back();

			}
		}
	}

	WSACleanup();
}
Order GetOrder(string order)
{
	Order ord;
	string response;
	int time = 0;
	int check = 0;
	bool isOrder = 0;
	transform(order.begin(), order.end(), order.begin(), ::toupper);
	size_t found = order.find("HAMBURGER");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 5;
		check += 4;
	}

	found = order.find("SPRITE");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 1;
		check += 1;
	}
	found = order.find("FRENCH FRIES");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 3;
		check += 2;
	}
	found = order.find("ICE CREAM");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 2;
		check += 3;
	}
	found = order.find("CHEESEBURGER");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 3;
		check += 5;
	}
	found = order.find("BIGMAC");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 4;
		check += 6;
	}
	found = order.find("BIGTASTY");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 5;
		check += 7;
	}
	found = order.find("ROYAL");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 4;
		check += 6;
	}
	found = order.find("FISHBURGER");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 3;
		check += 4;
	}
	found = order.find("CHEESE SAUCE");
	if (found != string::npos)
	{
		isOrder = 1;
		time += 1;
		check += 1;
	}
	if (isOrder)
	{
		response += "Thanks for your order!\n";
		response += ("Please wait for " + to_string(time) + " seconds\n");
		response += ("Your check will be $" + to_string(check));
		response += "\n";
	}
	else
		response += "Please choose something from the menu\n";
	ord.response = response;
	ord.time = time;
	ord.isRight = isOrder;
	return ord;
}