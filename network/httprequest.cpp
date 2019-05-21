// Student ID : 20171600
// Name : 김윤정
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
#include <fcntl.h>

using namespace std;

int main(int argc, char *argv[]) {
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	int bytesread;
	socklen_t addrlen;
	char buf[BUFSIZ];

	if (argc != 2) {
		(void) fprintf(stderr,"usage: %s portnum \n",argv[0]);
		exit(1);
	}

	int portnum = atoi(argv[1]);

	if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}

	printf("Student ID : 20171600\n");
	printf("Name : Yunjeong Kim\n");

	// Create a Server Socket

	int serversocket  = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(serversocket < 0){
		cout << "socket failed" << endl;
		return 0;
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(portnum);

	if(bind(serversocket, (sockaddr*)&server, sizeof(server)) != 0){
		cout << "bind failed" << endl;
		close(serversocket);
		return 0;
	}

	if(listen(serversocket, 5) == -1){
		cout << "listen failed"<< cout ;
		close(serversocket);
		return 0;
	}

	unsigned int clilen;
	int client_sockfd;
	int fd;
	int n;
	char buf_out[BUFSIZ];
	int file_read_len;

	while(1) {

		if ((client_sockfd = accept(serversocket, (sockaddr *)&remote, &clilen)) == -1)
		{
				perror("accept error : ");
				close(client_sockfd);
		}

		bzero(buf, BUFSIZ);

		// 클라이언트의 리퀘스트를 받는 부분 ,buf에 저장
		n = read(client_sockfd, buf, BUFSIZ);
		cout << buf << endl;

		//cout << buf << endl;
		/* 무슨파일을 요구하는지 */
		/*  GET /biga.html */
		/* filename = biga.html */
		char* tok = strtok(buf, " ");

		char* filename;

		while(tok!=NULL){
			string tstr = tok;
			// cout << tstr << endl;

			if(tstr.compare("GET")) {
				filename = tok;
				break;
			}

			tok = strtok(NULL, " ");
		}
		// cout << filename << "\n";

		fd = open(filename + 1, O_RDONLY);
		// cout << fd << endl;
		/* filename의 파일을 오픈 하는데 없으면 404를 리스폰한다. */
		if(fd==-1){
			char header1[1000] = "HTTP/1.0 404 NOT FOUND\n"
			"Connection: close\n"
			"ID: 20171600\n"
			"Name: Yunjeong Kim\n"
			"Content-Length: 0\n"
			"Content-Type: text/html\n\n";

			write(client_sockfd, header1, strlen(header1));

			cout << "Server Error : No such file ." << filename << "!\n";
			//close(client_sockfd);
			close(fd);

			continue;
		}


		int contentlength = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		// new string(itoa(contentlength))

		char header[1000]="HTTP/1.0";
		strcat(header," 200 OK\n");
		strcat(header,"Connection: close\n");
		strcat(header,"ID: 20171600\n");
		strcat(header,"Name: Yunjeong Kim\n");
		strcat(header,"Content-Length: ");
		strcat(header, to_string(contentlength).c_str());
		strcat(header,"\n");
		strcat(header,"Content-Type: text/html\n\n");

		/* header를 보내는 부분 */
		write(client_sockfd, header, strlen(header));
		//cout << header << endl;
		int total=0;
		/* file을 송신 하는 부분 */
		while(1) {

			memset(buf, 0x00, BUFSIZ);

			/* fd를 buf에 buf 사이즈 만큼 저장 */
			file_read_len = read(fd, buf, BUFSIZ);
			// cout << buf << endl;

			int chk_write = write(client_sockfd, buf, file_read_len);
			total += chk_write;
			if(file_read_len == EOF | file_read_len == 0) {
				cout << "finish" << " " << contentlength <<" "<< total << endl;
				break;
			}

		}
		// close(client_sockfd);
		close(fd);
	}

	//close(serversocket);
}
