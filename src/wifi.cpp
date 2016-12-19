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

#include "ssid_config.h"

SemaphoreHandle_t wifi_alive;

// lifted from https://github.com/SuperHouse/esp-open-rtos/blob/master/examples/mqtt_client/mqtt_client.c
void wifi_task(void *pvParameters)
{
    uint8_t status  = 0;
    uint8_t retries = 30;
    /*
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    }; */
    static struct sdk_station_config config = {
        WIFI_SSID,
        WIFI_PASS
    };


    printf("WiFi: connecting to WiFi\n\r");
    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    while(1)
    {
        while ((status != STATION_GOT_IP) && (retries)){
            status = sdk_wifi_station_get_connect_status();
            printf("%s: status = %d\n\r", __func__, status );
            if( status == STATION_WRONG_PASSWORD ){
                printf("WiFi: wrong password\n\r");
                break;
            } else if( status == STATION_NO_AP_FOUND ) {
                printf("WiFi: AP not found\n\r");
                break;
            } else if( status == STATION_CONNECT_FAIL ) {
                printf("WiFi: connection failed\r\n");
                break;
            }
            vTaskDelay( 1000 / portTICK_PERIOD_MS );
            --retries;
        }
        if (status == STATION_GOT_IP) {
            printf("WiFi: Connected\n\r");
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }

        while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP) {
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }
        printf("WiFi: disconnected\n\r");
        sdk_wifi_station_disconnect();
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
