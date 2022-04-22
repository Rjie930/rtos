#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"
#include "sys.h"

#define LED0 PCout(13)
#define WK_UP       PAin(0) //WKUP°´¼üPA0

#ifdef __cplusplus
extern "C" {
#endif




void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif

