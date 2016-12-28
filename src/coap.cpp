extern "C" {
#include "FreeRTOS.h"
}

#define DEBUG

#include <fact/iostream.h>
#include <Tokenizer.h>

using namespace FactUtilEmbedded::std;

#include <coap_lwip.hpp>
#include "main.h"

using namespace yacoap;

// TODO: move this to reused coap.cpp file
const coap_resource_path_t path_well_known_core = {2, {".well-known", "core"}};

extern coap_resource_t resources[];

CoapServer<resources> coapServer;


static const coap_resource_path_t path_light = {1, {"light"}};
static char light = '0';

int handle_get_well_known_core(const coap_resource_t *resource,
                                      const coap_packet_t *inpkt,
                                      coap_packet_t *pkt)
{
    return coapServer.handle_get_well_known_core(resource, inpkt, pkt);
}

static int handle_get_light(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt)
{
    return coap_make_content_response(inpkt, resource,
                              (const uint8_t *)&light, 1,
                              pkt);
}


static int handle_put_light(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt)
{
    printf("handle_put_light\n");
    if (inpkt->payload.len == 0) {
        return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                                  COAP_TYPE_ACK, COAP_RSPCODE_BAD_REQUEST,
                                  NULL, NULL, 0,
                                  pkt);
    }
    if (inpkt->payload.p[0] == '1') {
        light = '1';
        printf("Light ON\n");
    }
    else {
        light = '0';
        printf("Light OFF\n");
    }
    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              COAP_TYPE_ACK, COAP_RSPCODE_CHANGED,
                              resource->content_type,
                              (const uint8_t *)&light, 1,
                              pkt);
}

static const coap_resource_path_t path_ssid = {2, {"wifi", "ssid"}};


class BoundTokenizer : public TokenizerInPlace
{
    size_t len_left;
    
protected:
public:
    BoundTokenizer(char* buffer, const char* delimiters, size_t len) : 
        TokenizerInPlace(buffer, delimiters), len_left(len) {}

    // bounded version, does not presume \0 exists.  Returns 1 char less`
    // than other parseToken since that \0 isn't present
    uint8_t parseToken(size_t len)
    {
      if(bufferPos >= len) return 0;

      while((bufferPos < len) && !parse());
      return bufferPos;
    }
};


bool _isDelimiter(const char* delimiters, char input)
{
    const char* d = delimiters;

    ASSERT(delimiters, "Delimiters cannot be NULL");
    
    if(input == 0) return true;

    while(*d)
    {
        if(input == *d)
            return true;

        d++;
    }

    return false;
}


struct strntok_context
{
    const char* baseline;
    size_t len;

    bool strntok(const char* delimiters, const char** output, size_t* len);
};


// Not working yet "conventional" version
bool strntok_context::strntok(const char* delimiters, const char** output, size_t* len)
{
    if(this->len == 0) return false;
    
    size_t tokenlen = 0;

    while(this->len--)
    {
        if(_isDelimiter(delimiters, baseline[tokenlen]))
        {
            *output = baseline;
            *len = tokenlen;
            baseline += tokenlen + 1;
            return true;
        }
        else
            tokenlen++;
    }

    *output = baseline;
    *len = tokenlen;
    return true;
}


/*
 Getting this result:
 Payload length: 14
 Token found: id=abc
 Sub-Token found: id
 Sub-Token found: abc
 Sub-Token found: abc&pass
 Sub-Token found: f
 Sub-Token found: 
 Sub-Token found: 
 Sub-Token found: 
 Sub-Token found: 
 Sub-Token found: 
 Sub-Token found: 
 Sub-Token found: 

 from ?id=abc&pass=f
 */
template <typename TFunc>
void strntok_callback2(const char* input, size_t len, const char* delimiters,
    TFunc callback)
{
    strntok_context ctx;
    
    ctx.baseline = input;
    ctx.len = len;
    
    const char* buffer;
    size_t buflen;
    
    while(ctx.strntok(delimiters, &buffer, &buflen))
    {
        callback(buffer, buflen);
    }
}

template <typename TFunc>
void strntok_callback(const char* input, size_t len, const char* delimiters,
//    void (*callback)(const char* token, size_t len, void* context), void* context)
    TFunc callback)
{
    const char* baseline = input;
    size_t tokenlen = 0;
    
    while(len--)
    {
        if(_isDelimiter(delimiters, baseline[tokenlen]))
        {
            //callback(baseline, tokenlen, context);
            callback(baseline, tokenlen);
            baseline += tokenlen + 1;
            tokenlen = 0;
        }
        else
            tokenlen++;
    }

    // End of line always counts as a delimiter
    callback(baseline, tokenlen);
}

basic_ostream<char>& delim(basic_ostream<char>& out, const char* str, int len)
{
    while(len--) out << *str++;
    return out;
}

/*
extern "C"
{
#include <string.h>
}*/

struct KeyValuePair
{
    char* key;
    char* value;
};

static int handle_put_ssid(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt)
{
    ASSERT(inpkt->payload.len > 0, "No payload present");
    
    if (inpkt->payload.len == 0)
    {
        return coap_make_badrequest_response(inpkt, pkt);
    }
        
    char* buffer = (char*)inpkt->payload.p;
    auto len = inpkt->payload.len;
    
    clog << "Payload length: " << (uint16_t)len << endl;
    
    // FIX: for now, assume we have the '?'
    buffer++;
    len--;
    
    //char* ctx;
    //rsize_t strmax = len;
    
    
    strntok_callback(buffer, len, "&", [](const char* token, size_t len)  
    {
        clog << "Token found: ";
        delim(clog, token, len) << endl;
        strntok_callback(token, len, "=", [](const char* token, size_t len)
        {
            clog << "Sub-Token found: ";
            delim(clog, token, len) << endl;
        });
    });
    
    
    strntok_callback(buffer, len, "&", [](const char* token, size_t tokenlen)
    {
        static struct sdk_station_config config;
        //KeyValuePair tuples[2];
        //uint8_t tupleCount = 0;
        
        const char* key = NULL;
        const char* value = NULL;
        
        strntok_callback(token, tokenlen, "=", [&](const char* token, size_t len)
        {
            const char** which = key ? &value : &key;
            
            *which = token;
            // FIX: This is a buffer overrun, the [len] will
            // go one byte past the end of the buffer for the last value
            ((char*)token)[len] = 0;
        });
        
        clog << "key: " << key << "=" << value << endl;
        
        if(strcmp(key, "id") == 0) 
        {
            strcpy((char*)config.ssid, value);
        }
        else if(strcmp(key, "pass") == 0)
        {
            strcpy((char*)config.password, value);
        }
    });

    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              COAP_TYPE_ACK, COAP_RSPCODE_CHANGED,
                              resource->content_type,
                              (const uint8_t *)&light, 1,
                              pkt);
}

void coapTask(void *pvParameters)
{
    clog << "COAP waiting for WiFi..." << endl;
    
    wifi_alive.take();
    
    for(;;)
    {
        coapServer.handler();
    }
}


coap_resource_t resources[] =
{
    COAP_RESOURCE_WELLKNOWN(handle_get_well_known_core),
    COAP_RESOURCE_PUT(handle_put_ssid, &path_ssid, COAP_CONTENTTYPE_TXT_PLAIN),
    COAP_RESOURCE_GET(handle_get_light, &path_light, COAP_CONTENTTYPE_TXT_PLAIN),
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)},
    COAP_RESOURCE_NULL
};
