extern "C"
{
  #include "espressif/esp_common.h"
  #include "FreeRTOS.h"
  #include "FreeRTOS_CLI.h"
  #include "task.h"

  #include <stdint.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <string.h>
  #include <esp8266.h>
  #include <esp/uart.h>
  #include <stdio.h>


    #include "lwip/err.h"
    #include "lwip/sockets.h"
    #include "lwip/sys.h"
    #include "lwip/netdb.h"
    #include "lwip/dns.h"
    #include "lwip/api.h"


  extern const char *server_key;
  extern const char *server_cert;
}


// lwip defines, not actually used here and collide with 
// my wrapper (and probably other stuff too)
#undef accept
#undef write

#include "MbedTLS.hpp"

#include "main.h"

using namespace fact::mbedtls;

const char* PORT = "800";

int failures = 0;
int successes = 0;

void serverLoop(SSLContext& ssl)
{
    for(;;)
    {
        while(wifi_alive.take(1000 / portTICK_PERIOD_MS) == pdFALSE)
        {
          puts("Waiting for WiFi");
        }
        
        // these all have to live up here because when C++ uses gotos it is
        // strict about where autos are allocated
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(struct sockaddr_in);
        int len;

        NetContext client, server;
        printf("top of loop, free heap = %u\n", xPortGetFreeHeapSize());

        int ret = server.bind(NULL, PORT, MBEDTLS_NET_PROTO_TCP);
        if(ret != 0)
        {
            printf(" failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
            goto exit;
        }

        printf(" ok\n");

        ret=server.accept(client);

        if( ret != 0 ){
            printf(" Failed to accept connection. Restarting.\n");
            server.free();
            continue;
        }

        ssl.setBIO(client);

        //taskYIELD();
        /*
         * 4. Handshake
         */
        printf("  . Performing the SSL/TLS handshake...");

        while((ret = ssl.handshake()) != 0)
        {
            if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
                goto exit;
            }
        }

        printf(" ok\n");


        /*
         * 3. Write the GET request
         */
        printf("  > Writing status to new client... ");

        getpeername(((mbedtls_net_context&)client).fd, (struct sockaddr *)&peer_addr, &peer_addr_len);
        unsigned char buf[256];
        len = sprintf((char *) buf, "O hai, client %d.%d.%d.%d:%d\nFree heap size is %d bytes\n",
                          ip4_addr1(&peer_addr.sin_addr), ip4_addr2(&peer_addr.sin_addr),
                          ip4_addr3(&peer_addr.sin_addr), ip4_addr4(&peer_addr.sin_addr),
                          peer_addr.sin_port, xPortGetFreeHeapSize());
        while((ret = ssl.write(buf, len)) <= 0)
        {
            if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
                goto exit;
            }
        }

        len = ret;
        printf(" %d bytes written. Closing socket on client.\n\n%s", len, (char *) buf);

        ssl.closeNotify();

    exit:
        ssl.reset();
        client.free();
        server.free();

        if(ret != 0)
        {
            char error_buf[100];
            mbedtls_strerror(ret, error_buf, 100);
            printf("\n\nLast error was: %d - %s\n\n", ret, error_buf);
            failures++;
        } else {
            successes++;
        }

        printf("\n\nsuccesses = %d failures = %d\n", successes, failures);
        printf("Waiting for next client...\n");
    }
}

// shamelessly lifted from https://github.com/SuperHouse/esp-open-rtos/blob/master/examples/tls_server/tls_server.c
void serverTask(void *pvParameters)
{
    const char* pers = "tls_server";

    puts("TLS server task starting...\n");

    EntropyContext entropy;
    SSLContext ssl;
    X509Certificate srvcert;
    PrivateKeyContext pkey;
    SSLConfig config;
    RandomGenerator rg;

    puts("\n  . Seeding the random number generator...");

    int ret;

    if((ret = rg.seed(entropy, pers)) != 0)
    {
        printf(" failed\n  ! mbedtls_ctr_drbg seed returned %d\n", ret);
        abort();
    }

    puts(" ok\n");

    puts("   . Loading the server certificate ...");

    ret = srvcert.parse(server_cert);
    if(ret < 0)
    {
        printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        abort();
    }


    printf(" ok (%d skipped)\n", ret);

    printf("  . Loading the server private key ...");
    ret = pkey.parseKey(server_key);
    if(ret != 0)
    {
        printf(" failed\n ! mbedtls_pk_parse_key returned - 0x%x\n\n", -ret);
        abort();
    }

    printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    printf("  . Setting up the SSL/TLS structure...");

    if((ret = config.defaults(MBEDTLS_SSL_IS_SERVER,
                              MBEDTLS_SSL_TRANSPORT_STREAM,
                              MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);

        ssl.reset();
        return;
    }

    printf(" ok\n");

    mbedtls_x509_crt& _srvcert = srvcert;
    config.caChain(*_srvcert.next);
    //if( ( ret = mbedtls_ssl_conf_own_cert( &config, &srvcert, &pkey ) ) != 0 )
    if( ( ret = config.ownCert(srvcert, pkey) ) != 0 )
    {
        printf( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
        abort();
    }

    config.setRng(rg);

    if((ret = ssl.setup(config)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        ssl.reset();
        return;
    }

    serverLoop(ssl);
}
