/*
 *   Copyright (C) 2018 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "POCSAGNetwork.h"
#include "Utils.h"
#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>


CPOCSAGNetwork::CPOCSAGNetwork(const std::string& localAddress, unsigned int localPort, const std::string& remoteAddress, unsigned int remotePort, bool debug) :
m_socket(localAddress, localPort),
m_address(),
m_port(remotePort),
m_debug(debug)
{
	assert(!remoteAddress.empty());
	assert(remotePort > 0U);

	m_address = CUDPSocket::lookup(remoteAddress);
}

CPOCSAGNetwork::~CPOCSAGNetwork()
{
}

bool CPOCSAGNetwork::open()
{
	LogMessage("Opening POCSAG network connection");

	return m_socket.open();
}

bool CPOCSAGNetwork::write(CPOCSAGMessage* message)
{
	assert(message != NULL);

	unsigned char data[200U];
	data[0U] = 'P';
	data[1U] = 'O';
	data[2U] = 'C';
	data[3U] = 'S';
	data[4U] = 'A';
	data[5U] = 'G';

	data[6U] = message->m_ric >> 16;
	data[7U] = message->m_ric >> 8;
	data[8U] = message->m_ric >> 0;

	data[9U] = message->m_functional;

	::memcpy(data + 10U, message->m_message, message->m_length);

	if (m_debug)
		CUtils::dump(1U, "POCSAG Network Data Sent", data, message->m_length + 10U);

	return m_socket.write(data, message->m_length + 10U, m_address, m_port);
}

unsigned int CPOCSAGNetwork::read(unsigned char* data)
{
	assert(data != NULL);

	in_addr address;
	unsigned int port;
	int length = m_socket.read(data, 1U, address, port);
	if (length <= 0)
		return 0U;

	if (address.s_addr != m_address.s_addr || port != m_port)
		return 0U;

	if (m_debug)
		CUtils::dump(1U, "POCSAG Network Data Received", data, length);

	return length;
}

void CPOCSAGNetwork::close()
{
	m_socket.close();

	LogMessage("Closing POCSAG network connection");
}
