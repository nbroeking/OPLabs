#include <curl/AsyncCurl.hpp>
#include <log/LogManager.hpp>

using namespace lang;
using namespace os;

namespace curl {

bool AsyncCurl::curl_init = false;

AsyncCurl::AsyncCurl() {
    if( ! curl_init ) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_init = true;
    }
}

void AsyncCurl::sendRequest( Curl& c, CurlObserver* obs, Deallocator<CurlObserver>* dalloc ) {
    enqueue_raw( c.raw, obs, dalloc );
    c.raw = NULL;
}

void AsyncCurl::enqueue_raw( CURL* c, CurlObserver* obs, Deallocator<CurlObserver>* dalloc ){
    Triad* post = new Triad();

    post->request = c;
    post->observer = obs;
    post->dalloc = dalloc;

    requests.push( post );
}

size_t AsyncCurl::__curl_consume_subroutine( void* ptr, size_t size, size_t nmemb, void* obj ) {
    AsyncCurl::Triad* triad = (AsyncCurl::Triad*) obj;

    size_t nlen = size*nmemb;
    byte* bytes = (byte*) ptr;

    triad->observer->read(bytes, nlen);
    return size*nmemb;
}

void AsyncCurl::run() {
    logger::LogContext& log =
        logger::LogManager::instance().getLogContext("AsyncCurl", "run");

    for(;;) {
        log.printfln( DEBUG, "Waiting for requests to send" );
        Triad* tr = requests.front();
        requests.pop();
        
        curl_easy_setopt(tr->request, CURLOPT_WRITEFUNCTION, AsyncCurl::__curl_consume_subroutine);
        curl_easy_setopt(tr->request, CURLOPT_WRITEDATA, tr);

        log.printfln( DEBUG, "Sending request" );
        CURLcode res;
        res = curl_easy_perform(tr->request);

        s32_t http_code = 0;
        if( res != CURLE_OK ) {
            log.printfln( WARN, "Error with curl request" );
            CurlException e(curl_easy_strerror(res), res);
            tr->observer->onException(e);
        } else {
            log.printfln( DEBUG, "Curl request finished" );
            curl_easy_getinfo (tr->request, CURLINFO_RESPONSE_CODE, &http_code);
            tr->observer->onOK(http_code);
        }

        curl_easy_cleanup(tr->request);
        tr->dalloc->deallocate( tr->observer );
        delete tr;
    }
}

}
