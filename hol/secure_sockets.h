#ifndef HOL_SECURE_SOCKETS_H
#define HOL_SECURE_SOCKETS_H
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

#include "sockets.h"

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

	friend void swap(ssl_context& o1, ssl_context& o2)
	{
		std::swap(o1.ctx, o2.ctx);
	}
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

	friend void swap(ssl_handle& o1, ssl_handle& o2)
	{
		std::swap(o1.ssl, o2.ssl);
	}
};

class ssl_connection
: public connection
{
	tcp_connection tcp;
	ssl_context ctx;
	ssl_handle ssl;

public:
	ssl_connection(connection_type ct): tcp(ct)
	{
		SSL_load_error_strings();
		SSL_library_init();
	}

	ssl_connection(): ssl_connection(internet_stream) {}

	ssl_connection(ssl_connection&& other): ssl_connection()
	{
		using std::swap;
		swap(tcp, other.tcp);
		swap(ctx, other.ctx);
		swap(ssl, other.ssl);
	}

	ssl_connection(ssl_connection const&) = delete;

	ssl_connection& operator=(ssl_connection&& other)
	{
		using std::swap;
		swap(tcp, other.tcp);
		swap(ctx, other.ctx);
		swap(ssl, other.ssl);
		return *this;
	}

	ssl_connection& operator=(ssl_connection const&) = delete;


	void establish_with(std::string const& host, std::string const& port) override
	{
		tcp.establish_with(host, port);
		ctx.init();
		ssl.connect(tcp.socket(), ctx);
	}

	void shutdown() override
	{
		SSL_shutdown(ssl.get());
		ssl.clear();
		ctx.clear();
		tcp.shutdown();
	}

	// primitives
	// TODO: make these throw

	ssize_t write(char const* buf, std::size_t len) override
	{
		if(!len)
			return 0;

		return SSL_write(ssl.get(), buf, len);
	}

	ssize_t read(char* buf, std::size_t len) override
	{
		return SSL_read(ssl.get(), buf, len);
	}
};

} // net
} // sockets
} // hol

#endif // HOL_SECURE_SOCKETS_H
