#include "urlstream.hh"

#include <cstring>
#include <cstdio>
#include <fstream>
#include <strstream>
#include <stdexcept>

#include <map>

#include <regex.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

// i hope that this is the correct way to setup a new streambuf:

void urlstreambase::set_sb(streambuf *new_sb, int new_fd = -1) {
	streambuf *old_sb = rdbuf(new_sb);
	if (old_sb)
		delete old_sb;
	if (fd!=-1)
		close(fd);
	fd = new_fd;
}

void urlstreambase::parse(const string &url)
{
// cout << "parse url: " << url << endl;

	protocol = NONE;
	port = 0;
	hostname.erase();
	filename.erase();
	this->url = url;

	struct TType {
		EProtocol protocol;
		const char *name;
		bool  with_hostname;
	} typetable[] = {
		{ MEMORY, "memory", false },
		{ FILE, 	"file",		false },
		{ HTTP, 	"http",		true },
		{ FTP, 		"ftp",		true },
	};
	unsigned p,l;

	// get protocol
	//--------------------
	p = url.find_first_not_of(" \t");
	if (p==string::npos)
		return;
	l = url.size() - p;
	unsigned type;
	for(type=0; type<sizeof(typetable)/sizeof(TType); type++) {
		unsigned tl = strlen(typetable[type].name);
		if ( tl+3 <= l &&
				 strncasecmp(typetable[type].name, url.c_str()+p, tl) == 0 &&
				 strncmp("://", url.c_str()+p+tl, 3)==0 )
		{
			protocol = typetable[type].protocol;
			p += tl+3;
			break;
		}
	}
	
	// get hostname
	//--------------------
	if (protocol==HTTP)
		port=80;
	
	if (typetable[type].with_hostname) {
		l = url.substr(p).find_first_of(":/ \t");
		if (l==string::npos) {
			hostname = url.substr(p);
			return;
		}
		hostname = url.substr(p,l);
		p+=l;
		
		if (url[p]==':') {
			p++;
			l = url.substr(p).find_first_of("/ \t");
			sscanf(url.substr(p,l).c_str(), "%i", &port);
			if (l==string::npos)
				return;
			p+=l;
		}
	}
	
	filename = url.substr(p);
}

void urlstreambase::iopen()
{
	switch(protocol) {
		case NONE:
			throw runtime_error("unknow protocol");
			break;
		case HTTP:
			iopen_http();
			break;
		case FILE:
			iopen_file();
			break;
		case MEMORY:
			iopen_memory();
			break;
		default:
			throw runtime_error("input isn't supported for this protocol");
	}
}

void urlstreambase::oopen()
{
	switch(protocol) {
		default:
			throw runtime_error("output isn't supported for this protocol");
	}
}

void urlstreambase::iopen_http()
{
	string error;
	char sport[256];
	snprintf(sport, 255, "%d", port);
	string fn = hostname+":"+sport;

	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		error = "couldn't create socket";
		goto error1;
	}

	sockaddr_in name;
	regex_t reg_ip;
	if(regcomp(&reg_ip, 
						 "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$",
						 REG_NOSUB | REG_NEWLINE | REG_EXTENDED) !=0 )
	{
		error ="regcomp failed";
		goto error2;
	}

	if (regexec(&reg_ip, hostname.c_str(), 0, NULL, 0)==0)
	{
		int a,b,c,d;
		sscanf(hostname.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
		name.sin_addr.s_addr = (a<<24) + (b<<16) + (c<<8) + d;
		name.sin_addr.s_addr = htonl(name.sin_addr.s_addr);
	} else {
		struct hostent *hostinfo;
		hostinfo = gethostbyname(hostname.c_str());
		if (hostinfo==0) {
			error = "couldn't resolve hostname of " + hostname;
			goto error3;
		}
		name.sin_addr = *(struct in_addr *) hostinfo->h_addr;
	}

	name.sin_family = AF_INET;
	name.sin_port   = htons(port);
	
	if (connect(sock, (sockaddr*) &name, sizeof(sockaddr_in)) < 0) {
		error = "couldn't connect to " + fn;
		goto error4;
	}
	
	string cmd = "GET ";
	cmd+=filename;
	cmd+="\n";
	if (write(sock, cmd.c_str(), cmd.size())!=(int)cmd.size()) {
		error = "failed to send HTTP request to " + fn;
		goto error5;
	}
	
	regfree(&reg_ip);
	set_sb(new filebuf(sock), sock);
	return;

error5:
error4:
error3:
	regfree(&reg_ip);
error2:
	close(sock);
error1:
	throw runtime_error(error.c_str());
}

void urlstreambase::iopen_file()
{
	int fd = open(filename.c_str(), O_RDONLY);
	if (fd==-1) {
		string error = "failed to open `"+url+"': "+ strerror(errno);
		throw runtime_error(error.c_str());
	}
	set_sb(new filebuf(fd), fd);
	return;
}

struct TMemoryFile {
	const char *data;
	unsigned int len;
};

typedef map<const string, TMemoryFile> TMemoryFileSystem;

static TMemoryFileSystem memory_file_system;

void urlstreambase::save_memory_file(const string &name, 
																		 const char* data, unsigned len)
{
	TMemoryFile &mf = memory_file_system[name];
	mf.data = data;
	mf.len  = len;
}


void urlstreambase::iopen_memory()
{
	TMemoryFileSystem::iterator p = memory_file_system.find(filename);
	if (p==memory_file_system.end()) {
		string error = "failed to open `"+url+"': no such file";
		throw runtime_error(error.c_str());
	}
	set_sb(new strstreambuf(p->second.data, p->second.len));
}
