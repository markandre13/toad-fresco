#ifndef urlstreambase
#define urlstreambase urlstreambase

#include <iostream>
#include <string>

class urlstreambase:
	public virtual ios
{
	public:
		static void save_memory_file(const string &name, 
																 const char* data, unsigned len);

	protected:
		urlstreambase() {
			fd = -1;
		}
		~urlstreambase() {
			set_sb(NULL);
		}
		string url;
		enum EProtocol {
			NONE,
			MEMORY,
			FILE,
			HTTP,
			FTP
		} protocol;
		string hostname;
		int port;
		string filename;

		int fd;

		void parse(const string&);
		void iopen();
		void iopen_http();
		void iopen_file();
		void iopen_memory();
		
		void oopen();

		void set_sb(streambuf *new_sb, int new_fd = -1);
};

class iurlstream:
	public urlstreambase, public istream
{
	public:
		iurlstream(const string &url) {
			parse(url);
			iopen();
		}
};

class ourlstream:
	public urlstreambase, public ostream
{
	public:
		ourlstream(const string &url) {
			parse(url);
			oopen();
		}
};

// there ain't no such thing as a url stream for input and output

#endif
