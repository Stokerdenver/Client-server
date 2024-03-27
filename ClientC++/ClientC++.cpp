#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

using namespace std;
#pragma warning(disable: 4996)

const char* idServ1 = "127.0.0.1";
const char* idServ2 = "127.0.0.2";

const u_short portServ1 = 1111;
const u_short portServ2 = 1112;

bool connect1 = false;
bool connect2 = false;



void CreateConnect(int numServ) {

	// структура для хранения адреса
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);

	//адрес и порт для подключения устанавливаются в соответствии с номером сервера
	addr.sin_addr.s_addr = numServ == 1 ? inet_addr(idServ1) : inet_addr(idServ2);//ip - local host
	addr.sin_port = numServ == 1 ? htons(portServ1) : htons(portServ2);//незарезервированный хост
	addr.sin_family = AF_INET;//константа для интернет протокола

	//создание сокета
	SOCKET Connection = socket(
		AF_INET,//семейство интернет протоколов
		SOCK_STREAM,//протокод соединения
		NULL);

	//осуществление подключение и проверка на успех
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Попытка подключения к серверу не удалась.\n";
		return;
	}

	cout << "Попытка соединения с сервером " << numServ << " удалась!\n";
	char msg[256];

	//если это сервер1 отправляем ему рандомные координаты для перемещения окна
	if (numServ == 1) {
		connect1 = true;

		int x = rand() % 1200, y = rand() % 600;

		char mess[256];
		sprintf(mess, "%d", x);
		send(Connection, mess, sizeof(mess), NULL);
		sprintf(mess, "%d", y);
		send(Connection, mess, sizeof(mess), NULL);

		recv(Connection, msg, sizeof(msg), NULL);
		cout << msg << std::endl;
		recv(Connection, msg, sizeof(msg), NULL);
		cout << msg << std::endl;


	}
	else {

		connect2 = true;
		//чтение всех сообщений и вывод их на экран
		while (true) {

			if (recv(Connection, msg, sizeof(msg), NULL) == SOCKET_ERROR)break;
			else cout << msg << std::endl;
		}
	}





}


//Выполняется в главном потоке рекурсивно непрерывно, для распознования команд пользователя
void Menu() {

	int result;
	cin >> result;

	switch (result)
	{
	//для подключения к двум серверам используются отдельные потоки, которые выполняют функции подключения
	case 1: {
		//логическая переменная connect1 позволяет проверить создано ли уже подключение
		if (!connect1) {
			//в поток передается функция для подключения (LPTHREAD_START_ROUTINE)CreateConnect и номер сервера  (LPVOID)(1)
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CreateConnect, (LPVOID)(1), NULL, NULL);
			
			
		}
		else {
			std::cout << "Нельзя повторно подключиться к серверу!";}
		Menu();
		break;
	}
	case 2: {
		//логическая переменная connect2 позволяет проверить создано ли уже подключение
		if (!connect2) {
			//в поток передается функция для подключения (LPTHREAD_START_ROUTINE)CreateConnect и номер сервера (LPVOID)(2)
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CreateConnect, (LPVOID)(2), NULL, NULL);
		}
		else{
			std::cout << "Нельзя повторно подключиться к серверу!";}
		Menu();
		break;
	}

	default:
		Menu();
		break;
	}
}


int main(int argc, char* argv[]) {

	//установка русского языка для вывода в консоль
	setlocale(LC_ALL, "Russian");

	//Загрузка библиотеки для осуществления подключния
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	//вывод меню для работы с приложением
	std::cout << "\nКурсовая БСТ1901 Авдонин Клиент-серверное приложение\n\nВыберите к какому серверу нужно подключиться с помощью цифр 1/2\n";
	Menu();
	
	//окончание работы
	system("pause");
	return 0;
}



