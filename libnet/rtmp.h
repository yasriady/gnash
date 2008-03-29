// 
//   Copyright (C) 2005, 2006, 2007, 2008 Free Software Foundation, Inc.
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

#ifndef _RTMP_H_
#define _RTMP_H_

#include <boost/cstdint.hpp>
#include <string>
#include <map>

#include "amf.h"
#include "element.h"
#include "handler.h"
#include "network.h"

namespace gnash
{

#define RTMP_HANDSHAKE 0x3
#define RTMP_BODY_SIZE 1536
#define MAX_AMF_INDEXES 64

class DSOEXPORT RTMPproto
{
public:
    typedef enum {
	FROM_CLIENT,                     // Flash player
	FROM_SERVER                      // Flash com server
    } rtmp_source_e;
    typedef enum {
        NONE = 0x0,
        CHUNK_SIZE = 0x1,
        UNKNOWN = 0x2,
        BYTES_READ = 0x3,
        PING = 0x4,
        SERVER = 0x5,
        CLIENT = 0x6,
        UNKNOWN2 = 0x7,
        AUDIO_DATA = 0x8,
        VIDEO_DATA = 0x9,
        UNKNOWN3 = 0xa,
        NOTIFY = 0x12,
        SHARED_OBJ = 0x13,
        INVOKE = 0x14
    } content_types_e;
//     typedef enum {
//         CONNECT = 0x1,
//         DISCONNECT = 0x2,
//         SET_ATTRIBUTE = 0x3,
//         UPDATE_DATA = 0x4,
//         UPDATE_ATTRIBUTE = 0x5,
//         SEND_MESSAGE = 0x6,
//         STATUS = 0x7,
//         CLEAR_DATA = 0x8,
//         DELETE_DATA = 0x9,
//         DELETE_ATTRIBUTE = 0xa,
//         INITIAL_DATA = 0xb
//     } sharedobj_types_e;
    typedef enum {
        RTMP_STATE_HANDSHAKE_SEND,
        RTMP_STATE_HANDSHAKE_RECV,
        RTMP_STATE_HANDSHAKE_ACK,
        RTMP_STATE_CONNECT,
        RTMP_STATE_NETCONNECT,
        RTMP_STATE_NETSTREAM,
        RTMP_STATE_HEADER,
        RTMP_STATE_DONE
    } rtmp_state_t;
    typedef enum {
        RTMP_ERR_UNDEF=0,
        RTMP_ERR_NOTFOUND,
        RTMP_ERR_PERM,
        RTMP_ERR_DISKFULL,
        RTMP_ERR_ILLEGAL,
        RTMP_ERR_UNKNOWNID,
        RTMP_ERR_EXISTS,
        RTMP_ERR_NOSUCHUSER,
        RTMP_ERR_TIMEOUT,
        RTMP_ERR_NORESPONSE
    } rtmp_error_t;

// Each header consists of the following:
//
// * UTF string (including length bytes) - name
// * Boolean - specifies if understanding the header is `required'
// * Long - Length in bytes of header
// * Variable - Actual data (including a type code)
    typedef struct {
        amf::amfutf8_t name;
	boost::uint8_t required;
	boost::uint32_t length;
        void *data;
    } rtmp_head_t;    
    typedef enum {
        HEADER_12 = 0x0,
        HEADER_8  = 0x40,
        HEADER_4  = 0x80,
        HEADER_1  = 0xc0
    } rtmp_headersize_e;    
    
// Each body consists of the following:
//
// * UTF String - Target
// * UTF String - Response
// * Long - Body length in bytes
// * Variable - Actual data (including a type code)
    typedef struct {
        amf::amfutf8_t target;
        amf::amfutf8_t response;
	boost::uint32_t length;
        void *data;
    } rtmp_body_t;
    
    RTMPproto();
    ~RTMPproto();
//     bool handShakeWait();
    bool handShakeRequest();
//    bool handShakeResponse();
    bool clientFinish();
//  bool serverFinish();
    bool packetRequest();
    bool packetSend(Buffer *buf);
    bool packetRead(Buffer *buf);

    void addVariable(amf::Element *el);
    void addVariable(char *name, amf::Element *el);
    void addVariable(std::string &name, amf::Element *el);
    amf::Element *getVariable(const std::string &name);
    void setHandler(Handler *hand) { _handler = hand; };
    int headerSize(gnash::Network::byte_t header);
    Network::byte_t *parseHeader(gnash::Network::byte_t *header);

    int getHeaderSize()         { return _header_size; }; 
    int getTotalSize()          { return _total_size; }; 
    int getPacketSize()         { return _packet_size; };
    int getMysteryWord()        { return _mystery_word; };
    rtmp_source_e getRouting()  { return _src_dest; };
    int getAMFIndex()           { return _amf_index; };

    // 
    bool chunk_size();
    bool bytes_read();
    bool ping();
    bool server();
    bool client();
    bool audio_data();
    bool video_data();
    bool notify();
    bool shared_obj();
    bool invoke();
    void dump();
  protected:
    std::map<const char *, amf::Element *> _variables;
    Buffer		*_handshake;
    Handler		*_handler;
    int                 _amf_index;
    int                 _header_size;
    int                 _total_size;
    int                 _packet_size;
    rtmp_source_e       _src_dest;
    content_types_e     _type;
    int                 _mystery_word;
};

} // end of gnash namespace
// end of _RTMP_H_
#endif

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:

