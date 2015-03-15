#ifndef ICMPSOCKET_HPP_
#define ICMPSOCKET_HPP_

/*
 * Author: jrahm
 * created: 2015/01/14
 * ICMPSocket.hpp: <description>
 */

#include <io/binary/BufferPutter.hpp>
#include <io/binary/BufferGetter.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#include <types.h>
#include <os/Time.hpp>
    
#include <algorithm>
#include <io/SocketAddress.hpp>
#include <io/ICMPHeader.hpp>
#include <io/HasRawFd.hpp>

#include <log/LogManager.hpp>

namespace io {

/**
 * @brief This class describes a packet in the icmp protocol
 */
class ICMPPacket {
public:
    ICMPPacket():msg(NULL){}
    /**
     * @brief Set the ICMP header of the packet 
     * 
     * @param header The new header to set
     */
    inline void setHeader( const ICMPHeader& header ){
        this->hdr = header;
    }

    /** Take a message and deserialize it */
    void deserialize( const byte* message, size_t len ) {
        BufferGetter getter(message, len);
        getObject( getter, this->hdr );
        const byte* next = getter.rest(len);
        setMessage(next, len);
    }

    /**
     * @brief Set the message in this ICMP packet
     * 
     * @param mesg The message in bytes
     * @param len The length of the message
     */
    inline void setMessage( const byte* mesg, size_t len ) {
        if( this->msg ) delete[] this->msg;

        this->msg = new byte[len];
        std::copy(mesg, mesg + len, this->msg);
        this->len = len;
    }

    /**
     * @brief Return the size of the serialized packet
     * 
     * @return The size of the serialized packet
     */
    inline size_t serializeSize() const {
        return len + ICMPHEADER_SIZE;
    }

    /**
     * @brief Serialize this packet into an array of bytes
     * 
     * @param out The output of the serialize function
     * @param len The length of the output
     * 
     * @return 0 on success, -1 if the serialize size is too small
     */
    inline int serialize( byte* out, size_t len ) const {
        if( len < this->serializeSize() ) {
            return -1;
        }
        ICMPHeader lhdr = hdr;
        lhdr.setChecksum(0);
		BufferPutter putter(out, len);

		putObject( putter, lhdr );
		putter.putBytes(msg, this->len);

        checksum(out, len);
        logger::LogManager::instance().getLogContext("IO","ICMP").
            printHex(TRACE, out, len);
        return 0;
    }

    inline void checksum(byte* out, size_t len) const {
        uint16_t* summer;
        size_t sum = 0;

        for ( summer = (uint16_t*)out; (byte*)summer < out + len ; ++ summer ){
            sum += * summer;
        }
        if( len & 1 ) {
            sum += msg[len-1];
        }

        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);

        ((u16_t*)out)[1] = ~sum;
    }
        
    /**
     * @brief Delete this ICMPPacket
     */
    inline ~ICMPPacket() {
        delete[] msg;
    }

    inline byte* getMessage() {
        return msg;
    }

    inline size_t getMessgaeLength() {
        return len;
    }
private:
    ICMPHeader hdr;
    size_t len;
    byte* msg;
};

/**
 * @brief A socket class for ICMP packets 
 */
class ICMPSocket: public HasRawFd {
public:
    ICMPSocket() ;

    /**
     * @brief initialize this ICMP socket will return 0
     *         on success otherwise an error code.
     */
    int init();

    /**
     * @brief Set this socket to be non blocking
     * 
     * @param yes True if the socket should be set to be non blocking
     * 
     * @return 0 on success, error code otherwise
     */
    int setNonBlocking( bool yes = true );

    /**
     * @brief Get a message from icmp
     */
    int receive(ICMPPacket& pkt, SocketAddress*& from_address);

    /**
     * @brief Send a packet to the address given
     * 
     * @param pkt The packet to send
     * @param addr The address to send the packet to
     * @param socklen The socklen argument of the address
     * 
     * @return True on success, false otherwise
     */
    ssize_t send(const ICMPPacket& pkt, const SocketAddress& to_addr);

    /**
     * @brief Set the timeout value of this socket
     * @param mics The number of microseconds for the timeout
     */
    void setTimeout( os::micros_t mics );

    int getRawFd() {
        return m_fd;
    }

private:
    int m_fd ;
    int m_seq ;
};

}

#endif /* ICMPSOCKET_HPP_ */
