#ifndef ASYNCCURL_HPP_
#define ASYNCCURL_HPP_

/*
 * Author: jrahm
 * created: 2015/02/13
 * AsyncCurl.hpp: <description>
 */

#include <curl/curl.h>

#include <os/Runnable.hpp>
#include <containers/BlockingQueue.hpp>
#include <lang/Deallocator.hpp>

namespace curl {

class CurlException: public CException {
public:
    CurlException(const char* err, int code):
        CException(err, code){}
};

typedef s32_t http_response_code_t;
class CurlObserver {
public:
    virtual void onOK(http_response_code_t status_code) = 0 ;
    virtual void onException(CurlException& exc) = 0 ;
    virtual void read(const byte* bytes, size_t size) = 0 ;
    virtual inline ~CurlObserver(){}
};

class AsyncCurl;

class Curl {
public:
    friend class AsyncCurl;
    inline Curl() {
        raw = curl_easy_init();
        m_post_fields = NULL;
    }

    inline void setSSLPeerVerifyEnabled( bool enabled ) {
        curl_easy_setopt(raw, CURLOPT_SSL_VERIFYPEER, enabled);
    }

    inline void setSSLHostVerifyEnabled( bool enabled ) {
        curl_easy_setopt(raw, CURLOPT_SSL_VERIFYHOST, enabled);
    }

    inline void setURL( const char* url ) {
        curl_easy_setopt(raw, CURLOPT_URL, url);
    }

    inline void setFollowLocation( bool follow ) {
        curl_easy_setopt(raw, CURLOPT_FOLLOWLOCATION, follow);
    }
    
    inline void setPostFields( const char* data ) {
        if(m_post_fields)
            delete[] m_post_fields;
        m_post_fields = strdup(data);
        curl_easy_setopt(raw, CURLOPT_POSTFIELDS, m_post_fields);
    }

private:
    CURL* raw;
    char* m_post_fields;
};

class AsyncCurl: public os::ManagedRunnable {
public:
    AsyncCurl();

    /* Send a curl request */
    void sendRequest( Curl& c, CurlObserver* observer,
        lang::Deallocator<CurlObserver>* dalloc=NULL );

    void run();

    void stop();

private:
    static size_t __curl_consume_subroutine( void*, size_t, size_t, void* );
    static bool curl_init ;
    
    class Triad {
    public:
        CURL* request;
        CurlObserver* observer;
        lang::Deallocator<CurlObserver>* dalloc;
    };

    void enqueue_raw( CURL* c,
        CurlObserver* obs,
        lang::Deallocator<CurlObserver>* dealloc );

    containers::BlockingQueue<Triad*> requests;
};

}

#endif /* ASYNCCURL_HPP_ */
