#ifndef ICMPSOCKET_HPP_
#define ICMPSOCKET_HPP_

/*
 * Author: jrahm
 * created: 2015/01/14
 * ICMPSocket.hpp: <description>
 */

#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <types.h>
#include <os/Time.hpp>
    
#include <algorithm>

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
    inline void setHeader( const icmphdr& header ){
        this->hdr = header;
    }

    /** Take a message and deserialize it */
    void deserialize( const byte* message, size_t len ) {
        std::copy( message, message + sizeof(hdr), (byte*)&hdr );
        setMessage(message + sizeof(hdr), len - sizeof(hdr));
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
        return len + sizeof(struct icmphdr);
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
        icmphdr lhdr = hdr;
        checksum(lhdr);

        std::copy((byte*)&lhdr, (byte*)&lhdr + sizeof(struct icmphdr), out);
        std::copy(msg, msg + len, out + sizeof(struct icmphdr));

        return 0;
    }

    inline void checksum(icmphdr& hdr) const {
        hdr.checksum = 0;
        uint16_t* summer = (uint16_t*)&hdr;

        size_t sum = 0;
        size_t res; 
        
        for( ; (byte*)summer < (byte*)&hdr + sizeof(hdr) ; summer ++ ) {
            sum += * summer;
        }
        for ( summer = (uint16_t*)msg; (byte*)summer < msg + len ; ++ summer ){
            sum += * summer;
        }
        if( len & 1 ) {
            sum += msg[len-1];
        }

        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);

        res = ~sum;
        hdr.checksum = res;
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
    struct icmphdr hdr;
    size_t len;
    byte* msg;
};

/**
 * @brief A socket class for ICMP packets 
 */
class ICMPSocket {
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
    int receive(ICMPPacket& pkt, struct sockaddr* ptr, socklen_t& socklen);

    /**
     * @brief Send a packet to the address given
     * 
     * @param pkt The packet to send
     * @param addr The address to send the packet to
     * @param socklen The socklen argument of the address
     * 
     * @return True on success, false otherwise
     */
    ssize_t send(const ICMPPacket& pkt, struct sockaddr* addr, socklen_t socklen);

    /**
     * @brief Set the timeout value of this socket
     * @param mics The number of microseconds for the timeout
     */
    void setTimeout( os::micros_t mics );

private:
    int m_fd ;
    int m_seq ;
    struct icmphdr m_packet_header;
};

}

#endif /* ICMPSOCKET_HPP_ */
