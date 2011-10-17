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

#ifndef WEBSOCKET_SERVER_HPP
#define WEBSOCKET_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <set>

namespace websocketpp {
	class server;
	typedef boost::shared_ptr<server> server_ptr;
}

#include "websocketpp.hpp"
#include "websocket_server_session.hpp"
#include "websocket_connection_handler.hpp"

using boost::asio::ip::tcp;

namespace websocketpp {

class server_error : public std::exception {
public:	
	server_error(const std::string& msg)
		: m_msg(msg) {}
	~server_error() throw() {}
	
	virtual const char* what() const throw() {
		return m_msg.c_str();
	}
private:
	std::string m_msg;
};

class server : public boost::enable_shared_from_this<server> {
	public:
		// System logging levels
		/*static const uint16_t LOG_ALL = 0;
		static const uint16_t LOG_DEBUG = 1;
		static const uint16_t LOG_INFO = 2;
		static const uint16_t LOG_WARN = 3;
		static const uint16_t LOG_ERROR = 4;
		static const uint16_t LOG_FATAL = 5;
		static const uint16_t LOG_OFF = 6;
		
		// Access logging controls
		// Individual bits
		static const uint16_t ALOG_CONNECT = 0x1;
		static const uint16_t ALOG_DISCONNECT = 0x2;
		static const uint16_t ALOG_MISC_CONTROL = 0x4;
		static const uint16_t ALOG_FRAME = 0x8;
		static const uint16_t ALOG_MESSAGE = 0x10;
		static const uint16_t ALOG_INFO = 0x20;
		static const uint16_t ALOG_HANDSHAKE = 0x40;
		// Useful groups
		static const uint16_t ALOG_OFF = 0x0;
		static const uint16_t ALOG_CONTROL = ALOG_CONNECT 
		                                   & ALOG_DISCONNECT 
										   & ALOG_MISC_CONTROL;
		static const uint16_t ALOG_ALL = 0xFFFF;
*/
		server(boost::asio::io_service& io_service, 
			   const tcp::endpoint& endpoint,
			   connection_handler_ptr defc);
		
		// creates a new session object and connects the next websocket
		// connection to it.
		void start_accept();
		
		// INTERFACE FOR LOCAL APPLICATIONS
		
		void set_max_message_size(uint64_t val);
		
		// Test methods determine if a message of the given level should be 
		// written. elog shows all values above the level set. alog shows only
		// the values explicitly set.
		bool test_elog_level(uint16_t level);
		void set_elog_level(uint16_t level);
		
		bool test_alog_level(uint16_t level);
		void set_alog_level(uint16_t level);
		void unset_alog_level(uint16_t level);

		// INTERFACE FOR SESSIONS
		
		// Check if message size is within server's acceptable parameters
		bool validate_message_size(uint64_t val);
		
		// write to the server's logs
		void log(std::string msg,uint16_t level = LOG_ERROR);
		void access_log(std::string msg,uint16_t level);
	private:
		// if no errors starts the session's read loop and returns to the
		// start_accept phase.
		void handle_accept(server_session_ptr session,
			const boost::system::error_code& error);
		
	private:
		uint16_t					m_elog_level;
		uint16_t					m_alog_level;

		uint64_t					m_max_message_size;
		boost::asio::io_service&	m_io_service;
		tcp::acceptor				m_acceptor;
		connection_handler_ptr		m_def_con_handler;
};

}

#endif // WEBSOCKET_SERVER_HPP
