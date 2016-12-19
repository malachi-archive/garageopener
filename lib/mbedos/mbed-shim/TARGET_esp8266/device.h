// Cobbling this together from various ATSAMD21GA sources in mbed-os repo
#ifndef MBED_GPIO_OBJECT_H
#define MBED_GPIO_OBJECT_H

#include "mbed_assert.h"
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif


// IO defs lifted from core_cm0plus - smelt very CMSIS related
/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines
    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */

#ifdef __cplusplus
  #define   __IM    volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __IM    volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __OM    volatile             /*!< Defines 'write only' permissions                */
#define     __IOM   volatile             /*!< Defines 'read / write' permissions              */



typedef struct {
    PinName  pin;
    uint32_t mask;
    uint8_t powersave;
    uint8_t mode;
    uint8_t direction;

    __IO uint32_t *OUTCLR;
    __IO uint32_t *OUTSET;
    __I uint32_t *IN;
    __I uint32_t *OUT;
} gpio_t;
}

#endif 
