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
	UNSPEC = PF_UNSPEC,
	LOCAL = PF_LOCAL,
	UNIX = PF_UNIX,
	FILE = PF_FILE,
	INET = PF_INET,
	AX25 = PF_AX25,
	IPX = PF_IPX,
	APPLETALK = PF_APPLETALK,
	NETROM = PF_NETROM,
	BRIDGE = PF_BRIDGE,
	ATMPVC = PF_ATMPVC,
	X25 = PF_X25,
	INET6 = PF_INET6,
	ROSE = PF_ROSE,
	DECnet = PF_DECnet,
	NETBEUI = PF_NETBEUI,
	SECURITY = PF_SECURITY,
	KEY = PF_KEY,
	NETLINK = PF_NETLINK,
	ROUTE = PF_ROUTE,
	PACKET = PF_PACKET,
	ASH = PF_ASH,
	ECONET = PF_ECONET,
	ATMSVC = PF_ATMSVC,
	RDS = PF_RDS,
	SNA = PF_SNA,
	IRDA = PF_IRDA,
	PPPOX = PF_PPPOX,
	WANPIPE = PF_WANPIPE,
	LLC = PF_LLC,
	IB = PF_IB,
	MPLS = PF_MPLS,
	CAN = PF_CAN,
	TIPC = PF_TIPC,
	BLUETOOTH = PF_BLUETOOTH,
	IUCV = PF_IUCV,
	RXRPC = PF_RXRPC,
	ISDN = PF_ISDN,
	PHONET = PF_PHONET,
	IEEE802154 = PF_IEEE802154,
	CAIF = PF_CAIF,
	ALG = PF_ALG,
	NFC = PF_NFC,
	VSOCK = PF_VSOCK,
	MAX = PF_MAX,
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
		disconnect();

		addrinfo hints;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = to_int(type.address_family);
		hints.ai_socktype = to_int(type.socket_type);

		addrinfo* res;
		if(int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
			throw std::runtime_error(gai_strerror(status));

		// try to connect to each
		int sd;
		addrinfo* p;
		for(p = res; p; p = p->ai_next)
		{
			if((sd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
				continue;
			if(!::connect(sd, p->ai_addr, p->ai_addrlen))
				break;
			::close(sd);
		}

		freeaddrinfo(res);

		if(!p)
			throw std::runtime_error(std::strerror(errno));

		fd = sd;
	}
};

class ssl_context
{
	SSL_CTX* ctx;

public:
	ssl_context(): ctx(nullptr) {}
	ssl_context(ssl_context&& sc) noexcept: ssl_context() { std::swap(ctx, sc.ctx); }
	ssl_context(ssl_context const&) = delete;
	~ssl_context() { clear(); }

	void init()
	{
		if(!(ctx = SSL_CTX_new(SSLv23_client_method())))
		{
			char err[128];
			throw std::runtime_error(ERR_error_string(ERR_get_error(), err));
		}
	}

	void clear()
	{
		if(ctx)
			SSL_CTX_free(ctx);
		ctx = nullptr;
	}

	ssl_context& operator=(ssl_context&& sc) noexcept { std::swap(ctx, sc.ctx); return *this; }
	ssl_context& operator=(ssl_context const&) = delete;

	SSL_CTX* get() const { return ctx; }
};

class ssl_handle
{
	SSL* ssl;

public:
	ssl_handle(): ssl(nullptr) {}
	ssl_handle(ssl_handle&& sh) noexcept: ssl_handle() { std::swap(ssl, sh.ssl); }
	ssl_handle(ssl_handle const&) = delete;
	~ssl_handle() { clear(); }

	SSL* get() const { return ssl; }

	void connect(socket const& sock, ssl_context const& ctx)
	{
		if(!(ssl = SSL_new(ctx.get())))
			throw std::runtime_error(ERR_error_string(ERR_get_error(), nullptr));

		if(!SSL_set_fd(ssl, sock.get()))
			throw std::runtime_error(ERR_error_string(ERR_get_error(), nullptr));

		if(SSL_connect(ssl) < 1)
			throw std::runtime_error(ERR_error_string(ERR_get_error(), nullptr));
	}

	void clear()
	{
		if(ssl)
		{
			SSL_free(ssl);
			ssl = nullptr;
		}
	}
};

class connection
{
	bool const secure;

	socket sock;
	ssl_context ctx;
	ssl_handle ssl;

public:
	connection(connection_type ct, bool secure = false): secure(secure), sock(ct)
	{
		if(secure)
		{
			SSL_load_error_strings();
			SSL_library_init();
		}
	}

	connection(bool secure = false): connection(internet_stream, secure) {}

	void establish_with(std::string const& host, std::string const& port)
	{
		sock.connect(host, port);

		if(secure)
		{
			ctx.init();
			ssl.connect(sock, ctx);
		}
	}

	void establish_with(std::string const& host, std::uint32_t port)
	{
		establish_with(host, std::to_string(port));
	}

	void shutdown()
	{
		SSL_shutdown(ssl.get());
		ssl.clear();
		ctx.clear();
		sock.disconnect();
	}

	// primitives
	// TODO: make these throw

	ssize_t write(char const* buf, std::size_t len)
	{
		if(!len)
			return 0;

		if(secure)
			return SSL_write(ssl.get(), buf, len);

		return ::write(sock.get(), buf, len);
	}

	ssize_t read(char* buf, std::size_t len)
	{
		if(secure)
			return SSL_read(ssl.get(), buf, len);

		return ::read(sock.get(), buf, len);
	}

	// high level

	void send(std::string const& data)
	{
		if(secure)
		{
			int num;
			if((num = SSL_write(ssl.get(), data.data(), data.size())) != (int)data.size())
				throw std::runtime_error(ERR_error_string(SSL_get_error(ssl.get(), num), nullptr));
		}
		else
		{
			if(::write(sock.get(), data.data(), data.size()) != (int)data.size())
				throw std::runtime_error(std::strerror(errno));
		}
	}

	std::string receive()
	{
		std::string data;

		int num;
		char buf[1024];

		if(secure)
		{
			while((num = SSL_read(ssl.get(), buf, sizeof(buf))) > 0)
				data.append(buf, num);

			if(num < 0)
				throw std::runtime_error(ERR_error_string(SSL_get_error(ssl.get(), num), nullptr));
		}
		else
		{
			while((num = ::read(sock.get(), buf, sizeof(buf))) > 0)
				data.append(buf, num);

			if(num < 0)
				throw std::runtime_error(std::strerror(errno));
		}

		return data;
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

	connection conn;
	std::vector<char_type> ibuf;
	std::vector<char_type> obuf;

public:
	basic_socketbuf(bool secure)
	: conn(internet_stream, secure)
	, ibuf(SIZE)
	, obuf(SIZE)
	{
		buf_type::setp(obuf.data(), obuf.data() + obuf.size() - 1);
		buf_type::setg(ibuf.data(), ibuf.data(), ibuf.data());
	}

	basic_socketbuf(basic_socketbuf&& sb)
	: conn(std::move(sb.conn))
	, ibuf(std::move(sb.ibuf))
	, obuf(std::move(sb.obuf))
	{
	}

	basic_socketbuf(basic_socketbuf const&) = delete;

	~basic_socketbuf()
	{
		sync();
	}

	void open(std::string const& host, std::string const& port)
	{
		return conn.establish_with(host, port);
	}

	void close()
	{
		conn.shutdown();
	}

protected:

	int output_buffer()
	{
		int num = buf_type::pptr() - buf_type::pbase();
		conn.write(obuf.data(), num * char_size);
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

		int num = conn.read(ibuf.data(), ibuf.size() * char_size);

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
	basic_socketstream(bool secure = false)
	: stream_type(&buf), buf(secure) {}

	basic_socketstream(basic_socketstream&& ss)
	: buf(std::move(ss.buf)) {}

	basic_socketstream(basic_socketstream const&) = delete;

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
		buf.open(host, port);
	}

	void open(const std::string& host, uint16_t port)
	{
		open(host, std::to_string(port));
	}
};

typedef basic_socketstream<char> socketstream;
typedef basic_socketstream<wchar_t> wsocketstream;

} // net
} // sockets
} // hol

#endif // HOL_SOCKETS_H
