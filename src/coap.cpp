extern "C" {
#include "FreeRTOS.h"
}

#include <coap_lwip.hpp>
#include <fact/iostream.h>
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
    COAP_RESOURCE_GET(handle_get_light, &path_light, COAP_CONTENTTYPE_TXT_PLAIN),
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)},
    COAP_RESOURCE_NULL
};
