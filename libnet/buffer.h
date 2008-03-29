// 
//   Copyright (C) 2008 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef __BUFFER_H__
#define __BUFFER_H__ 1

#include <boost/cstdint.hpp>
#include <string>

#include "getclocktime.hpp"
#include "network.h"

// _definst_ is the default instance name
namespace gnash
{

class Buffer 
{
public:
    Buffer();
    // Create with a size other than the default
    Buffer(size_t nbytes);
    
    // Delete the allocate memory
    ~Buffer();
    void empty();

    // Resize the buffer that holds the data
    void *resize(size_t nbytes);

    // Put data into the buffer
    void copy(gnash::Network::byte_t *data, size_t nbytes);
    void copy(gnash::Network::byte_t *data) { copy(data, _nbytes); };
    void copy(std::string &str);

    // Find a byte in the buffer
//    Network::byte_t *find(char c);
    Network::byte_t *find(Network::byte_t b);
    Network::byte_t *find(Network::byte_t b, size_t start);
    
    // Drop a byte or range of characters without resizing
//    Network::byte_t *remove(char c);
    Network::byte_t *remove(Network::byte_t c);
    Network::byte_t *remove(int x);
    Network::byte_t *remove(int x, int y);
    
    // Accessors
    Network::byte_t *begin() { return _ptr ; };
    Network::byte_t *end() { return _ptr + _nbytes; };
    Network::byte_t *reference() { return _ptr; }
    size_t size() { return _nbytes; }
    void setSize(size_t nbytes) { _nbytes = nbytes; };
    
    // make ourselves be able to be copied.
    Buffer &operator=(Buffer *buf);
    Buffer &operator=(Buffer &buf);

    // Test against other buffers
    bool operator==(Buffer *buf);
    bool operator==(Buffer &buf);
    Network::byte_t operator[](int x) { return *(_ptr + x); };
    
    // debug stuff, not need for running Cygnal
    void dump();
private:
    void *init(size_t nbytes);
    Network::byte_t *_ptr;
    size_t         _nbytes;
#ifdef USE_STATS_BUFFERS
    struct timespec _stamp;	// used for timing how long data stays in the queue.
#endif
};


} // end of gnash namespace

#endif // end of __BUFFER_H__

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
