#ifndef HOL_SOCKETS_H
#define HOL_SOCKETS_H
//
// Copyright (c) 2016 Galik <galik.bool@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

//================================================================
// README
//----------------------------------------------------------------
//
// Usage: (normal TCP stream connection)
//
//	net::socketstream nss;
//
//	const std::string http_rq =
//	"GET /search?q=wibble HTTP/1.1\r\n"
//	"Host: " "www.google.co.uk" "\r\n"
//	"User-Agent: my-ip-sender v0.1-alpha\r\n"
//	"Accept: */*\r\n"
//	"Connection: close\r\n"
//	"\r\n";
//
//	nss.open("www.google.co.uk", "80");
//	nss << http_rq << std::flush;
//
//	for(char c; nss.get(c);)
//		std::cout.put(c);
//	std::cout << '\n';
//
// Usage: (secure SSL TCP stream connection)
//
//	net::socketstream nss(true);
//
//	const std::string http_rq =
//	"GET /search?q=wibble HTTP/1.1\r\n"
//	"Host: " "www.google.co.uk" "\r\n"
//	"User-Agent: my-ip-sender v0.1-alpha\r\n"
//	"Accept: */*\r\n"
//	"Connection: close\r\n"
//	"\r\n";
//
//	nss.open("www.google.co.uk", "443");
//	nss << http_rq << std::flush;
//
//	for(char c; nss.get(c);)
//		std::cout.put(c);
//	std::cout << '\n';
//

#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>
#include <chrono>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "bug.h"

namespace hol {
namespace sockets {
namespace net {

enum class SOCK
{
	STREAM = SOCK_STREAM,
	DGRAM = SOCK_DGRAM,
	RAW = SOCK_RAW,
};

enum class AF
{
	UNSPEC = AF_UNSPEC,
	LOCAL = AF_LOCAL,
	UNIX = AF_UNIX,
	FILE = AF_FILE,
	INET = AF_INET,
	AX25 = AF_AX25,
	IPX = AF_IPX,
	APPLETALK = AF_APPLETALK,
	NETROM = AF_NETROM,
	BRIDGE = AF_BRIDGE,
	ATMPVC = AF_ATMPVC,
	X25 = AF_X25,
	INET6 = AF_INET6,
	ROSE = AF_ROSE,
	DECnet = AF_DECnet,
	NETBEUI = AF_NETBEUI,
	SECURITY = AF_SECURITY,
	KEY = AF_KEY,
	NETLINK = AF_NETLINK,
	ROUTE = AF_ROUTE,
	PACKET = AF_PACKET,
	ASH = AF_ASH,
	ECONET = AF_ECONET,
	ATMSVC = AF_ATMSVC,
	RDS = AF_RDS,
	SNA = AF_SNA,
	IRDA = AF_IRDA,
	PPPOX = AF_PPPOX,
	WANPIPE = AF_WANPIPE,
	LLC = AF_LLC,
	IB = AF_IB,
	MPLS = AF_MPLS,
	CAN = AF_CAN,
	TIPC = AF_TIPC,
	BLUETOOTH = AF_BLUETOOTH,
	IUCV = AF_IUCV,
	RXRPC = AF_RXRPC,
	ISDN = AF_ISDN,
	PHONET = AF_PHONET,
	IEEE802154 = AF_IEEE802154,
	CAIF = AF_CAIF,
	ALG = AF_ALG,
	NFC = AF_NFC,
	VSOCK = AF_VSOCK,
	MAX = AF_MAX,
};

struct connection_type
{
	AF address_family;
	SOCK socket_type;
};

constexpr connection_type local_stream = {AF::LOCAL, SOCK::STREAM};
constexpr connection_type internet_stream = {AF::UNSPEC, SOCK::STREAM};
constexpr connection_type internet4_stream = {AF::INET, SOCK::STREAM};
constexpr connection_type internet6_stream = {AF::INET6, SOCK::STREAM};
constexpr connection_type local_datagram = {AF::LOCAL, SOCK::DGRAM};
constexpr connection_type internet_datagram = {AF::UNSPEC, SOCK::DGRAM};
constexpr connection_type internet4_datagram = {AF::INET, SOCK::DGRAM};
constexpr connection_type internet6_datagram = {AF::INET6, SOCK::DGRAM};

class socket
{
	int fd;
	connection_type type;

	static constexpr int to_int(AF address_family)
	{
		return static_cast<int>(address_family);
	}

	static constexpr int to_int(SOCK socket_type)
	{
		return static_cast<int>(socket_type);
	}

	static constexpr addrinfo to_addrinfo(connection_type ct)
	{
		return {0, to_int(ct.address_family), to_int(ct.socket_type), 0, 0, 0, 0, 0};
	}

public:
	socket(): fd(-1) {}
	socket(socket&& s) noexcept: socket() { std::swap(fd, s.fd); }
	socket(socket const&) = delete;
	socket(connection_type ct): socket() { config(ct); }
	socket& operator=(socket&& s) { std::swap(fd, s.fd); return *this; }
	socket& operator=(socket const&) = delete;

	~socket() { disconnect(); }

	int get() const { return fd; }

	void disconnect()
	{
		if(fd != -1)
			::close(fd);
		fd = -1;
	}

	AF address_family() const { return type.address_family; }
	void address_family(AF address_family) { this->type.address_family = address_family; }

	SOCK socket_type() const { return type.socket_type; }
	void socket_type(SOCK socket_type) { type.socket_type = socket_type; }

	connection_type config() const { return type; }
	void config(connection_type type) { this->type = type; }

	void connect(std::string const& host, std::string const& port)
	{
		bug_fun();
		bug_var(host);
		bug_var(port);

		disconnect();

		addrinfo hints;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;//to_int(type.address_family);
		hints.ai_socktype = to_int(type.socket_type);

		addrinfo* res;
		if(int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
			throw std::runtime_error(gai_strerror(status));

		bug_var(res);

		// try to connect to each
		int sd;
		addrinfo* p;
		for(p = res; p; p = p->ai_next)
		{
			bug_var(p);
			if((sd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				continue;
			if(!::connect(sd, p->ai_addr, p->ai_addrlen))
				break;
			::close(sd);
		}

		freeaddrinfo(res);

		bug_var(p);
		bug_var(sd);

		if(!p)
		{
			bug_var(std::strerror(errno));
			throw std::runtime_error(std::strerror(errno));
		}

		fd = sd;
		bug_var(fd);
	}

	friend void swap(socket& l, socket& r)
	{
		using std::swap;
		swap(l.fd, r.fd);
		swap(l.type, r.type);
	}
};

class connection
{
public:
	connection() {}

	connection(connection const&) = delete;
	connection(connection&& other): connection()
	{
		using std::swap;
		swap(*this, other);
	}

	connection& operator=(connection const&) = delete;
	connection& operator=(connection&& other)
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}

	virtual ~connection() = default;

	virtual void establish_with(std::string const& host, std::string const& port) = 0;
	virtual void shutdown() = 0;

	virtual ssize_t read(char* buf, std::size_t len) = 0;
	virtual ssize_t write(char const* buf, std::size_t len) = 0;

	void establish_with(std::string const& host, std::uint32_t port)
	{
		establish_with(host, std::to_string(port));
	}

	// high level

	void send(std::string const& data)
	{
		ssize_t num;
		if((num = write(data.data(), data.size())) != (ssize_t)data.size())
			throw std::runtime_error("");
	}

	std::string receive()
	{

		ssize_t len;
		char buf[1024];
		std::string data;

		while((len = read(buf, sizeof(buf))) > 0)
			data.append(buf, len);

		if(len < 0)
			throw std::runtime_error("");

		return data;
	}

	friend void swap(connection& l, connection& r)
	{
		using std::swap;
		// nothing to do (yet)
		(void) l;
		(void) r;
	}
};

class tcp_connection
: public connection
{
private:
	hol::sockets::net::socket sock;

public:
	tcp_connection(connection_type ct): sock(ct) {}
	tcp_connection(): tcp_connection(internet_stream) {}

	tcp_connection(tcp_connection const&) = delete;
	tcp_connection(tcp_connection&& other): connection(std::move(other))
	{
		using std::swap;
		swap(*this, other);
	}

	tcp_connection& operator=(tcp_connection const&) = delete;
	tcp_connection& operator=(tcp_connection&& other)
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}

	hol::sockets::net::socket& socket() { return sock; }

	void establish_with(std::string const& host, std::string const& port) override
	{
		bug_fun();
		bug_var(host);
		bug_var(port);
		sock.connect(host, port);
	}

	void shutdown() override
	{
		sock.disconnect();
	}

	ssize_t write(char const* buf, std::size_t len)
	{
		if(!len)
			return 0;

		return ::write(sock.get(), buf, len);
	}

	ssize_t read(char* buf, std::size_t len)
	{
		return ::read(sock.get(), buf, len);
	}

	friend void swap(tcp_connection& l, tcp_connection& r)
	{
		using std::swap;
		swap(l.sock, r.sock);
	}
};

template<typename Char>
class basic_socketbuf
: public std::basic_streambuf<Char>
{
public:
	typedef Char char_type;
	typedef std::basic_streambuf<char_type> buf_type;
	typedef std::basic_ostream<char_type> stream_type;
	typedef typename buf_type::int_type int_type;
	typedef typename std::basic_streambuf<Char>::traits_type traits_type;

protected:
	static const int char_size = sizeof(char_type);
	static const int SIZE = 128;

	std::unique_ptr<connection> conn;
	std::vector<char_type> ibuf;
	std::vector<char_type> obuf;

public:
	basic_socketbuf(): basic_socketbuf(nullptr) {}

	basic_socketbuf(std::unique_ptr<connection> conn)
	: conn(std::move(conn))
	, ibuf(SIZE)
	, obuf(SIZE)
	{
		buf_type::setp(obuf.data(), obuf.data() + obuf.size() - 1);
		buf_type::setg(ibuf.data(), ibuf.data(), ibuf.data());
	}

	basic_socketbuf(basic_socketbuf const&) = delete;
	basic_socketbuf(basic_socketbuf&& other)
	: buf_type(std::move(other))
	{
		using std::swap;
		swap(*this, other);
	}

	basic_socketbuf& operator=(basic_socketbuf const&) = delete;
	basic_socketbuf& operator=(basic_socketbuf&& other)
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}

//	basic_socketbuf(basic_socketbuf&& sb)
//	: conn(std::move(sb.conn))
//	, ibuf(std::move(sb.ibuf))
//	, obuf(std::move(sb.obuf))
//	{
//	}

	~basic_socketbuf()
	{
		sync();
	}

	void open(std::string const& host, std::string const& port)
	{
		bug_fun();
		bug_var(host);
		bug_var(port);
		return conn->establish_with(host, port);
	}

	void close()
	{
		conn->shutdown();
	}

	template<typename C>
	friend void swap(basic_socketbuf<C>& l, basic_socketbuf<C>& r)
	{
		using std::swap;
		swap(l.conn, r.conn);
		swap(l.ibuf, r.ibuf);
		swap(l.obuf, r.obuf);
	}

protected:
	int output_buffer()
	{
		int num = buf_type::pptr() - buf_type::pbase();
		conn->write(obuf.data(), num * char_size);
		buf_type::pbump(-num);
		return num;
	}

	int_type overflow(int_type c) override
	{
		if(c != traits_type::eof())
		{
			*buf_type::pptr() = c;
			buf_type::pbump(1);
		}

		if(output_buffer() == traits_type::eof())
			return traits_type::eof();
		return c;
	}

	int sync() override
	{
		if(output_buffer() == traits_type::eof())
			return traits_type::eof();
		return 0;
	}

	int_type underflow() override
	{
		if(buf_type::gptr() < buf_type::egptr())
			return *buf_type::gptr();

		int num = conn->read(ibuf.data(), ibuf.size() * char_size);

		if(num <= 0)
			return traits_type::eof();

		buf_type::setg(ibuf.data(), ibuf.data(), ibuf.data() + num);
		return *buf_type::gptr();
	}
};

typedef basic_socketbuf<char> socketbuf;
typedef basic_socketbuf<wchar_t> wsocketbuf;

template<typename Char>
class basic_socketstream
: public std::basic_iostream<Char>
{
public:
	typedef Char char_type;
	typedef std::basic_iostream<char_type> stream_type;
	typedef basic_socketbuf<char_type> buf_type;

protected:
	buf_type buf;

public:

	basic_socketstream(): basic_socketstream(nullptr) {}

	basic_socketstream(connection* conn)
	: basic_socketstream(std::unique_ptr<connection>(conn)) {}

	basic_socketstream(std::unique_ptr<connection> conn)
	: stream_type(&buf), buf(std::move(conn)) {}

	basic_socketstream(basic_socketstream const&) = delete;
	basic_socketstream(basic_socketstream&& other)
	: stream_type(std::move(other))
	{
		using std::swap;
		swap(*this, other);
	}

	basic_socketstream& operator=(basic_socketstream const&) = delete;
	basic_socketstream& operator=(basic_socketstream&& other)
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}

//	basic_socketstream(basic_socketstream&& ss)
//	: buf(std::move(ss.buf)) {}


	virtual ~basic_socketstream()
	{
		this->close();
	}

	void close()
	{
		buf.close();
		stream_type::clear();
	}

	void open(std::string const& host, std::string const& port)
	{
		bug_fun();
		bug_var(host);
		bug_var(port);
		buf.open(host, port);
	}

	void open(const std::string& host, uint16_t port)
	{
		bug_fun();
		bug_var(host);
		bug_var(port);
		open(host, std::to_string(port));
	}

	template<typename C>
	friend void swap(basic_socketstream<C>& l, basic_socketstream<C>& r)
	{
		using std::swap;
		swap(l.buf, r.buf);
	}
};

typedef basic_socketstream<char> socketstream;
typedef basic_socketstream<wchar_t> wsocketstream;

} // net
} // sockets
} // hol

#endif // HOL_SOCKETS_H
