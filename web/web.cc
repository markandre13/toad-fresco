#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <iostream>

/*

 http://nomad
 ->
 Protokol		: http
 Rechnername: nomad
 Port				: 80
 Befehl     : GET /
 
 http://nomad/~mark/
 ->
 Protokol		: http
 Rechnername: nomad
 Port				: 80
 Befehl     : GET /~mark/

http://www.susi.de
 http://nomad/~mark/:8080
 ->
 Protokol		: http
 Rechnername: nomad
 Port				: 8080
 Befehl     : GET /~mark/

 file:///home/mark/public_html/index.html
 Protokol   : keines
 Rechnername: keiner
 Port       : keiner
 Befehl     : keiner
 Wie dann?  : open("/home/mark/public_html/index.html");

*/

string getURL(const string&);

int main()
{
	getURL("http://nomad/~mark/"); // => getHTTP("nomad", 80, "/~mark/");
	getURL("file://home/mark/public_html/index.html");
	getURL("ftp://ftp.uni-rostock.de/pub");
}

string getHTTP(const string &hostname, int port, const string &file)
{
	cout << "Suche IP Adresse" << endl;
	struct hostent *hostinfo;
	hostinfo = gethostbyname(hostname.c_str());
	if (hostinfo==0) {
		herror("gethostbyname");
		return "";
	}
	
	cout << "Erzeuge Socket" << endl;
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		return "";
	}

	cout << "Verbinde Socket" << endl;
	sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port   = htons(port);
	name.sin_addr = *(struct in_addr *) hostinfo->h_addr;
	
	if (connect(sock, (sockaddr*) &name, sizeof(sockaddr_in)) < 0) {
		perror("connect");
		close(sock);
		return "";
	}
	
	cout << "Schicke HTTP Befehl" << endl;
	string cmd = "GET ";
	cmd+=file;
	cmd+="\n";
	if (write(sock, cmd.c_str(), cmd.size())!=cmd.size()) {
		cout << "konnte den Befehl nicht senden" << endl;
		return "<HTML><BODY>Is nich!!!</BODY></HTML>";
	}
	
	cout << "Empfange Daten" << endl;
	int total = 0;
	string reply;
	char buffer[1025];
	while(true) {
		int n = read(sock, buffer, 1024);
		cout << "habe " << n << " Bytes" << endl;
		if (n<=0)
			break;
		reply.append(buffer,n);
	}

	cout << "Habe folgendes erhalten:" << endl;

	cout << reply << endl;

	return 0;
}

string getURL(const string &url)
{
	if (url.substr(0,7)=="http://") {
		cout << "HTTP" << endl;
		unsigned p = url.find("/", 7);
		unsigned l;
		if (p!=string::npos)
			l = p-7;
		else
			l = string::npos;
		cout << "rechner: " << url.substr(7,l) << endl;
		string file;
		if (p==string::npos)
			file="/";
		else
			file=url.substr(p);
		cout << "datei: " << file << endl;
	} else if (url.substr(0,7)=="file://") {
		cout << "FILE" << endl;
	} else {
		cout << "Waswerwas???" << endl;
	}
}
