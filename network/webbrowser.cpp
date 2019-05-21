// Student ID :
// Name :
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define PROMPT() {printf("\n> ");fflush(stdout);}
#define GETCMD "wget"
#define QUITCMD "quit"

int main() {
	int socktoserver = -1;
	struct sockaddr_in server;
	char buf[BUFSIZ];

	char fname[BUFSIZ];

	printf("Student ID : 20171600\n");
	printf("Name : yunjeong Kim\n");

	PROMPT();

	for (;;) {
		if (!fgets(buf, BUFSIZ - 1, stdin)) {
			if (ferror(stdin)) {
				perror("stdin");
				exit(1);
			}
			exit(0);
		}

		char *cmd = strtok(buf, " \t\n\r");

		if((cmd == NULL) || (strcmp(cmd, "") == 0)) {
			PROMPT();
			continue;
		} else if(strcasecmp(cmd, QUITCMD) == 0) {
				exit(0);
		}

		if(!strcasecmp(cmd, GETCMD) == 0) {
			printf("Wrong command %s\n", cmd);
			PROMPT();
			continue;
		}

		// connect to a server
		char *hostname = strtok(NULL, " \t\n\r");
		char *pnum = strtok(NULL, " ");
		char *filename = strtok(NULL, " \t\n\r");

		// just for Debugging, print the URL
		printf("%s:%s%s\n", hostname, pnum, filename);

		// NEED TO IMPLEMENT HERE

		struct hostent *host;
		host = gethostbyname(hostname);

		if(host == 0){
			printf("cannot connect to server.\n");
			PROMPT();
			continue;
			// return 1;
		}

		// for(int i = 0; host->h_addr_list[i]; i++) {
		// 	inet_ntoa(*(struct in_addr*) host->h_addr_list[i]);
		// }

		// create socket
		socktoserver = socket(AF_INET, SOCK_STREAM, 0);

		bzero((char *)&server, sizeof(server));

		server.sin_family = AF_INET;
		memcpy((void *) &server.sin_addr, host->h_addr, host->h_length);
		server.sin_port = htons(atoi(pnum));


		if(connect(socktoserver, (struct sockaddr *)&server, sizeof(server))) {

			printf("cannot connect to server.\n");
			PROMPT();
			continue;
			// return 1;
		}
		//
		// printf("Connected \n---------\n");

		char requestmessage[1000] = "GET ";
		strcat(requestmessage, filename);
		strcat(requestmessage, " HTTP/1.0\r\n");
		strcat(requestmessage, "Host: ");
		strcat(requestmessage, hostname);
		strcat(requestmessage, "\r\n");
		strcat(requestmessage, "User-agent: HW1/1.0\r\n");
		strcat(requestmessage, "ID: 20171600\r\n");
		strcat(requestmessage, "Name: yoonjeong kim\r\n");
		strcat(requestmessage, "Connection: close\r\n\r\n");

		///////////////////////////////

		// filename = "/web/member/palladio.JPG";
		// int len = strlen(filename);
		//
		// char* message = "GET /web/member/palladio.JPG HTTP/1.0\r\nHost: netapp.cs.kookmin.ac.kr\r\nUser-agent: HW1/1.0\r\nID: 20171600\r\nName: yoonjeong kim\r\nConnection: close\r\n\r\n";

		if(send(socktoserver , requestmessage , strlen(requestmessage) , 0) < 0)
	    {
	        printf("cannot connect to server\n");
					PROMPT();
					continue;
	        // return 1;
					// break;
	    }

        // puts("Data Send\n");

////////////////////////
		// FILE* fr = fopen("/Users/yunjeong/Downloads/palladio.jpg", "w+");
		char* name = strrchr(filename,'/');
		name += 1;
		FILE *fr = fopen(name, "w");



		int total_len = 0;
		int received_len;
		int isHeader = 1;

		int received;
		int total;

		bzero(buf, BUFSIZ);

		int len;
		int asdf = 10;
		int notfound = 0;
		while((received_len = recv(socktoserver, buf, BUFSIZ , 0)) > 0) {

			if(isHeader) {

				char* tstr = strstr(buf, "HTTP");

				if(*(tstr + 9) == '2' && *(tstr + 10) == '0' && *(tstr + 11) == '0') {

					printf("Success!\n");

				} else {

					printf("%c%c%c: error\n", *(tstr + 9), *(tstr + 10), *(tstr + 11));
					notfound = 1;
					// PROMPT();
					break;
				}
			}

			if(isHeader) {

				char* tstr = strstr(buf, "Content-Length: ");
				tstr += 16;

				int ii = 0;
				len = 0;

				while('0' <= *(tstr + ii) && *(tstr + ii) <= '9') {

					len *= 10;
					len += (*(tstr + ii) - '0');
					ii++;
				}

				printf("Total Size %d bytes\n", len);
			}

			if(isHeader) {

				char* tstr = strstr(buf, "\r\n\r\n");
				tstr += 4;

				int bodySize = 0;

				for(auto it = buf; it != tstr; ++it) {
					++bodySize;
				}

				fwrite(tstr, sizeof(char), received_len - bodySize, fr);
				total_len += (received_len - bodySize);
				// printf("Current Downloading %d / %d (bytes) %d%%\n", total_len, len, 100 * total_len / len);

				isHeader = 0;

				continue;
			}

			fwrite(buf, sizeof(char), received_len, fr);
			total_len += received_len;

			if((100 * total_len / len) >= asdf) {

				printf("Current Downloading %d / %d (bytes) %d%%\n", total_len, len, 100 * total_len / len);

				asdf += 10;
			}


	        bzero(buf, BUFSIZ);
	    }

			if(notfound == 1) {
				PROMPT();
				continue;
			}

			printf("Download Complete!!! %d/%d \n", total_len, len);

			PROMPT();

	    fclose(fr);


		}
}
