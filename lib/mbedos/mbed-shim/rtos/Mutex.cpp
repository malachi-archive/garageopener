#include "Mutex.h"
#include "FreeRTOS.h"

osStatus Mutex::lock(uint32_t millisec)
{
    return osStatus::osOK;
}


osStatus Mutex::unlock()
{
    return osStatus::osOK;
}
