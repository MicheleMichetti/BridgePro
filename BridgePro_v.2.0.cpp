// BRIDGEPRO_V1.0.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;  // Serve per poter scrivere semplicemente cout senza dover indicare lo spazio dei nomi std::
using namespace std::chrono;

#define	VTIMER1			0
#define	VTIMER2			1
#define	VTIMER3			2
#define	VTIMER4			3
#define	VTIMER5			4
#define	VTIMER6			5
#define	VTIMER7			6
#define	VTIMER8			7
#define	VTIMER9			8
#define	VTIMER10		9
#define MAXINPUTLEN		100

#define BUFLEN_ROBOT1	512		// Max Lenght of buffer
#define PORT_ROBOT1		3000	// the port
#define IPSERVERROBOT1	"192.168.1.100"    //ip address of udp server

#define BUFLEN_ROBOT2	512		// Max Lenght of buffer
#define PORT_ROBOT2		3001	// the port
#define IPSERVERROBOT2	"192.168.1.101"    //ip address of udp server

#define BUFLEN_ROBOT3	512		// Max Lenght of buffer
#define PORT_ROBOT3		3002	// the port
#define IPSERVERROBOT3	"192.168.1.103"    //ip address of udp server

#define BUFLEN_ROBOT4	512		// Max Lenght of buffer
#define PORT_ROBOT4		3003	// the port
#define IPSERVERROBOT4	"192.168.1.104" 

#define BUFLEN_PC		512		// Max Lenght of buffer
#define PORT_PC			3004	// the port


string softwareversion = "V10.0.0";

enum FiniteSteteMachineStatus {
	FSM_POWERUP = 0,
	FSM_RUN,
	FSM_READY,
	FSM_ERROR,
};
enum ROBOT {
	ROBOT1 = 1,
	ROBOT2,
	ROBOT3,
	ROBOT4,
};
/*
elenco comandi console
*/
const string c_help = "help";
const string c_end = "end";
const string c_swver = "swver";
const string c_yes = "yes";
const string c_no = "no";

auto millis() {
	auto time = high_resolution_clock::now();
	return(time);
}




struct inaddr {
	unsigned long saddr;
};
// Important: sockaddr_in can be casted to a sockaddr
struct sockaddrin {
	unsigned short sinfamily;
	unsigned short sinport;
	struct inaddr sinaddr;
	char sin_zero[8];
};
struct sockaddrin addrport;

char cmdConsole[MAXINPUTLEN];
boolean cmd_exit = false;
int FSM_Status = FSM_POWERUP;


boolean command(const string& cmd) {

	boolean find = true;
	int ixx = 0;
	size_t len = cmd.length() - 1;

	while (find && ixx <= len) {
		if (cmd[ixx] == cmdConsole[ixx])
			find = true;
		else
			find = false;
		ixx++;
	}
	return(find);

}

void CreateServerSocket() {

}
void CreateRobotSocket(int robotname) {

}
void errorEventManager() {
	if (FSM_Status == FSM_ERROR) {

	}
}
void showHelp() {
	cout << ">:COMMANDS LIST:" << endl;
	cout << ">:<help> for in line  help:" << endl;
	cout << ">:<end>  for stop program:" << endl;
	cout << ">:<swver> to show the software version:" << endl;
	cout << ">:";
}
void showSoftwareVersion() {
	cout << ">:BridgePRO: " << softwareversion << endl;
	cout << ">:";
}
void exitSystem() {
	boolean cmdfound = false;
	cout << ">:WARNING! Do you want to STOP the BRIDGE? [yes,no]" << endl;
	cout << ">:";
	cin.getline(cmdConsole, sizeof(cmdConsole));
	//cout << ">:" << cmdConsole << endl;
	//cout << ">:" << endl;
	if (cmdfound = command(c_yes))
		cmd_exit = true;

}
void badCommand() {
	cout << ">:Bad Command.." << endl;
	cout << ">:";
}

void inputConsole() {
	while (!cmd_exit) {
		boolean cmdfound = false;
		cin.getline(cmdConsole, sizeof(cmdConsole));	// attendo cmdConsole , quando premo ENTER carico e proseguo
		cout << ">:" << cmdConsole << endl;

		if (!cmdfound)
			if (cmdfound = command(c_help))
				showHelp();

		if (!cmdfound)
			if (cmdfound = command(c_end))
				exitSystem(); // cmd_exit =true esco da tutti i processi

		if (!cmdfound)
			if (cmdfound = command(c_swver))
				showSoftwareVersion(); // show software version on console


		if (!cmdfound)
			badCommand();

	}


}
void Socket_ServerPC(SOCKET s, struct sockaddr_in client, struct sockaddr_in server, char txbuffer[BUFLEN_ROBOT1], int slen, char rxbuffer[BUFLEN_ROBOT1], int recv_len) {

	/*
	questo è il thred SERVER che si collega al clientPC
	il client deve sempre conoscere l'indirizzo IP del server
	Il server non è indispensabile che conosca l'IP del server
	Il client prende sempre l'iniziativa di invio dati per primo, il serve è sempre in attesa
	il server risponde solo se viene interrogato da un client
	*/

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) // per avere il protocollo UDP: SOCK_DGRAM  TCP: SOCK_STREAM
		// esiste una tabella di errori qui https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

	{
		printf("Could not create Server socket PC : %d", WSAGetLastError()); // disconnesso cavo di rete, client spento
	}
	printf("Server Socket PC created.\n");

	// noi qui siamo un server, definiamo questo thread, noi stessi come slient
	server.sin_family = AF_INET;		 // Valori di windows
	server.sin_addr.s_addr = INADDR_ANY; // leggiamo il nostro indirizzo IP della macchina
	server.sin_port = htons(PORT_PC);	 // assegno al server la porta di connessione su cui creare il socket

	/* qui noi siamo il SERVER che parlano con il PC  che è un client*/

	//Bind con il nostro socket
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");


	while (!cmd_exit) {

		printf("Waiting for data...");
		fflush(stdout); // pulisce il buffer della console

		//clear the buffer by filling null, it might have previously received data
		memset(rxbuffer, '\0', sizeof(rxbuffer));

		//try to receive some data, this is a blocking call RICEVE I DATI E LI CARICA IN BUF E' BLOCCANTE
		if ((recv_len = recvfrom(s, rxbuffer, slen, 0, (struct sockaddr*)&client, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError()); // questa funzione ritorna eventuali errori
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received , VISUALIZZO INDIRIZZO IP E PORTA DEL CLIENT che ha trasmesso
		printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		printf("Data: %s\n", rxbuffer);

		//now reply the client with the same data INVIA IL BUFFER buf
		if (sendto(s, rxbuffer, recv_len, 0, (struct sockaddr*)&client, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}



	};

	closesocket(s);
	WSACleanup();
	cout << ">:end Client PC Socket..." << endl;

}

void Socket_ClientRobot(SOCKET s, struct sockaddr_in client, struct sockaddr_in server, char txbuffer[BUFLEN_ROBOT1], int slen, char rxbuffer[BUFLEN_ROBOT1]) {
	/*
	queesto è il thred CLient che si collega al Server
	il client deve sempre conoscere l'indirizzo IP del server
	Il server non è indispensabile che conosca l'IP del server
	Il client prende sempre l'iniziativa di invio dati per primo, il serve è sempre in attesa
	il server risponde solo se viene interrogato da un client
	*/

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) // per avere il protocollo UDP: SOCK_DGRAM  TCP: SOCK_STREAM
		// esiste una tabella di errori qui https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

	{
		printf("Could not create socket Robot1 : %d", WSAGetLastError()); // disconnesso cavo di rete, client spento
	}
	printf("Socket Robot1 created.\n");

	//Prepare the sockaddr_in structure inizializzo questo socket

	memset((char*)&rxbuffer, 0, sizeof(rxbuffer)); // non è una funzione di windwows, è una funzione di STDIO  
	/*inizializza la variabile
	riserva un'area di memoria definisce un certo numero numero di byte e li setta ad un specifico valore
	in questo caso riempie di 0 la variabile server per tutta la sua dimensione
	*/

	// noi qui siamo client, definiamo questo thread, noi stessi come slient
	client.sin_family = AF_INET;			// Valori di windows
	//server.sin_addr.s_addr = INADDR_ANY;	// Valori di windows da utilizzare solo nel server

	client.sin_port = htons(PORT_ROBOT1);	// Valore della porta su cui creare il socket

	client.sin_addr.S_un.S_addr = inet_addr(IPSERVERROBOT1); // Qui dico a questo client che indirizzo ha il server su cui effettuerà la connessione socket 
	/* qui noi siamo dei client che parlano con ROBOT1 che è un server*/



	/*
	This is an IP address that is used when we don't want to bind a socket to any specific IP.
	Basically, while implementing communication, we need to bind our socket to an IP address.
	When we don't know the IP address of our machine, we can use the special IP address INADDR_ANY.
	It allows our server to receive packets that have been targeted by any of the interfaces.
	*/

	while (!cmd_exit) {

		//printf("Enter message : ");
		//gets(message);

		//TXbufferToRobot1;

		//send the message
		if (sendto(s, txbuffer, strlen(txbuffer), 0, (struct sockaddr*)&server, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE); // termina il processo esco da tutto, valutare se serve lasciare qui questa funzione
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(txbuffer, '\0', sizeof(server));
		//try to receive some data, this is a blocking call

		if (recvfrom(s, rxbuffer, strlen(rxbuffer), 0, (struct sockaddr*)&server, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		puts(rxbuffer);


	};

	closesocket(s); // chiudono il socket
	WSACleanup();   // puliscono  da chiarire se denizializza tutto .....sembra che lo denizializza

	cout << ">:end Client Robot1 Socket..." << endl;
}
void Socket_ClientRobot2() {

	while (!cmd_exit) {



	};

	cout << ">:end Server Robot2 Socket..." << endl;
}
void Socket_ClientRobot3() {

	while (!cmd_exit) {



	};

	cout << ">:end Server Robot3 Socket..." << endl;
}
void Socket_ClientRobot4() {

	while (!cmd_exit) {



	};

	cout << ">:end Server Robot4 Socket..." << endl;
}

auto startTimer() {
	auto time = high_resolution_clock::now();
	return(time);
}
void delay(DWORD delaytime) {
	Sleep(delaytime); // equivale a delay() in millisecond
}



int main()
{
	//int sockid = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // creo il socket apre una connessione
	//int status = closesocket(sockid);					   // chiude una connessione
	//addrport.sinfamily = AF_INET;
	//addrport.sinport = htons(5100);
	//addrport.sinaddr.saddr = htonl(INADDR_ANY);
	//if (bind(sockid, (struct sockaddr*)&addrport, sizeof(addrport)) != -1) {

	//}

	SOCKET SRobot1;		//creo uno variabile di tipo SOCKET per la connessione Robot1
	SOCKET SRobot2;		//creo uno variabile di tipo SOCKET
	SOCKET SRobot3;		//creo uno variabile di tipo SOCKET
	SOCKET SRobot4;		//creo uno variabile di tipo SOCKET
	SOCKET SocketPC;	//creo uno variabile di tipo SOCKET	per la connessione PC

	struct sockaddr_in ServerRobot1, ClientRobot1ThisMachine;	// creo due strutture per la gestione della comunicazione robot1
	struct sockaddr_in ServerRobot2, ClientRobot2ThisMachine;	// creo due strutture per la gestione della comunicazione robot2
	struct sockaddr_in ServerRobot3, ClientRobot3ThisMachine;	// creo due strutture per la gestione della comunicazione robot3
	struct sockaddr_in ServerRobot4, ClientRobot4ThisMachine;	// creo due strutture per la gestione della comunicazione robot4
	struct sockaddr_in ClientPC, ServerPCThisMachine;			// creo due strutture per la gestione della comunicazione PC

	int slenRobot1, recv_lenRobot1;	// contengono i valori per gestione dipendono da gestione errori sul socket
	int slenRobot2, recv_lenRobot2;
	int slenRobot3, recv_lenRobot3;
	int slenRobot4, recv_lenRobot4;
	int slenPC, recv_PC;

	slenRobot1 = sizeof(ServerRobot1);
	slenRobot2 = sizeof(ServerRobot2);
	slenRobot3 = sizeof(ServerRobot3);
	slenRobot4 = sizeof(ServerRobot4);
	slenPC = sizeof(ClientPC);

	char RXbufferFromRobot1[BUFLEN_ROBOT1]; // Questo è il buffer ti Ricezione che contiene i dati proveniente da Robot1
	char TXbufferToRobot1[BUFLEN_ROBOT1];	// Questo è il buffer di Trasmissione che contiene i dati destinati a Robot1

	char RXbufferFromRobot2[BUFLEN_ROBOT2]; // Questo è il buffer ti Ricezione che contiene i dati proveniente da Robot2
	char TXbufferToRobot2[BUFLEN_ROBOT2];	// Questo è il buffer di Trasmissione che contiene i dati destinati a Robot2

	char RXbufferFromRobot3[BUFLEN_ROBOT3]; // Questo è il buffer ti Ricezione che contiene i dati proveniente da Robo3
	char TXbufferToRobot3[BUFLEN_ROBOT3];	// Questo è il buffer di Trasmissione che contiene i dati destinati a Robot3

	char RXbufferFromRobot4[BUFLEN_ROBOT4]; // Questo è il buffer ti Ricezione che contiene i dati proveniente da Robo4
	char TXbufferToRobot4[BUFLEN_ROBOT4];	// Questo è il buffer di Trasmissione che contiene i dati destinati a Robot4

	char RXbufferFromPC[BUFLEN_ROBOT4]; // Questo è il buffer ti Ricezione che contiene i dati proveniente dal PC (server)
	char TXbufferToPC[BUFLEN_ROBOT4];	// Questo è il buffer di Trasmissione che contiene i dati destinati al PC (server)

	WSADATA wsa;

	cout << ">:bridgePRO " << softwareversion << endl;
	cout << ">:System Restart..." << endl;
	cout << ">:";

	//Initialise winsock e fare lo startup, va fatto una volta sola per tutte le connessioni
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError()); // che tipo di errori potrebbero essere?
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//auto startTime = millis();


	// creo un thread di nome socketPC a cui assegno la funzione Socket_ServerPC
	std::thread socketPC(Socket_ServerPC, (SocketPC, ClientPC, ServerPCThisMachine, TXbufferToPC, slenPC, RXbufferFromPC, recv_PC));
	std::thread socketRobot1(Socket_ClientRobot, (SRobot1, ClientRobot1ThisMachine, ServerRobot1, TXbufferToRobot1, slenRobot1, RXbufferFromRobot1));

	//std::thread socketRobot2(Socket_ClientRobot(SRobot2, ClientRobot2ThisMachine, ServerRobot2, TXbufferToRobot2, slenRobot2, RXbufferFromRobot2));
	//std::thread socketRobot3(Socket_ClientRobot(SRobot3, ClientRobot3ThisMachine, ServerRobot3, TXbufferToRobot3, slenRobot3, RXbufferFromRobot3));
	//std::thread socketRobot4(Socket_ClientRobot(SRobot4, ClientRobot4ThisMachine, ServerRobot4, TXbufferToRobot4, slenRobot4, RXbufferFromRobot4));

	std::thread console(inputConsole);				// consol

	socketPC.join();			//lancio il thread socketPC
	socketRobot1.join();
	//socketRobot2.join();
	//socketRobot3.join();
	//socketRobot4.join();
	//TEST UNDO
	console.join();


	//findOdd(start, end);
	//findEven(start, end);



	//cout << "EvenSum: " << EvenSum << endl;

	//auto stopTime = high_resolution_clock::now();

	//auto duration = duration_cast<milliseconds>(stopTime - startTime);

	//cout << duration.count() / 1000 << endl;

	cout << ">:BridgePRO " << softwareversion << " System STOP..." << endl;

	cout << ">:..." << endl;

	cout << ">:PRESS ENTER to close the Console window..." << endl;
	cin.getline(cmdConsole, sizeof(cmdConsole));

	//delay(3000); // equivale a delay() in millisecond
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
