/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef WEBSOCKET_SERVER_SESSION_HPP
#define WEBSOCKET_SERVER_SESSION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#if defined(WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace websocketpp {
	class server_session;
	typedef boost::shared_ptr<server_session> server_session_ptr;
}

#include "websocket_session.hpp"
#include "websocket_server.hpp"

using boost::asio::ip::tcp;

namespace websocketpp {

class server_session : public session {
public:
	server_session (server_ptr s,
			 boost::asio::io_service& io_service,
			 connection_handler_ptr defc,
		     uint64_t buf_size);

	/*** SERVER INTERFACE ***/
	
	// This function is called when a connection to a new client has been
	// established and the server is ready to read the client handshake.
	void on_connect();
	
	/*** HANDSHAKE INTERFACE ***/
	
	// Call this from your on_client_connect handler to initialize this
	// connection as websocket and perform the websocket handshake
	void start_websocket();

	// Set an HTTP header for the outgoing server handshake response.
	void set_header(const std::string& key, const std::string& val);

	// Call this from your on_client_connect handler to initialize this
	// connection as HTTP and send the HTTP headers (set with set_header)
	void start_http(int http_code = 200, const std::string& http_body = "", bool done=true);
	virtual void http_write(const std::string& body, bool done=false);
	virtual void http_write_async_send();
	bool m_http_done;


	// Asynchronously get the body of a POST request
	void read_http_post_body(boost::function<void(std::string)> callback);
	
	// Selects a subprotocol for the connection to use. val must be a value
	// present in the client's opening handshake or the empty string for null.
	void select_subprotocol(const std::string& val);
	
	// Selects an extension from the list offered by the client. Each extension
	// selected must have been offered by the client. Extensions will be used
	// in the order that they were selected here.
	void select_extension(const std::string& val);
	
	/*** SESSION INTERFACE ***/
	// see session
	virtual bool is_server() const { return true;}

	void log(const std::string& msg, uint16_t level) const;
	void access_log(const std::string& msg, uint16_t level) const;
protected:
	// Opening handshake processors and callbacks. These need to be defined in
	virtual void write_handshake();
	virtual void handle_write_handshake(const boost::system::error_code& e);
	virtual void read_handshake();
	virtual void handle_read_handshake(const boost::system::error_code& e,
	                                   std::size_t bytes_transferred);
	void process_response_headers();
	virtual void handle_write_http_response(const boost::system::error_code& error, boost::shared_ptr<std::vector<unsigned char> > buf);
	virtual void handle_read_http_post_body(const boost::system::error_code& e,
	                 std::size_t bytes_transferred, boost::function<void(std::string)> callback);
	virtual void handle_http_read_for_eof(const boost::system::error_code& e);
private:	
	
protected:
	// connection resources
	server_ptr	m_server;	
private:
	
};

}

#endif // WEBSOCKET_SERVER_SESSION_HPP
