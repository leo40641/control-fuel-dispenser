/*
*********************************************************************************************************
*                                         FC_Fuel CODE
*
*                             (c) Copyright 2016; Distracom S.A
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              FC_Fuel CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5888LTI-LP097
*
* Filename      : funciones.c
* Version       : V1.00
* Programmer(s) : Leonardo Orjuela   
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <project.h>

extern void Write_EEPROM(uint8 *buffer, uint32 dir, uint8 size);
extern void Read_EEPROM(uint32 dir, uint8 size);

extern uint8 timeout_psoc(uint8 bytes, uint8 time);
extern void call_psoc(uint8 dir);


extern void call_xbee(uint8 cmd);
extern void InitLCD(void);
extern CY_ISR(wait_LCD);
extern uint8 hay_datos_lcd(uint8 lcd);
extern uint8 polling_lcd(uint8 flujo, uint8 dir);
extern void leer_fecha_lcd(uint8 lcd);
extern void write_fecha_lcd(uint8 lcd);
extern uint8 leer_img(uint8 lcd);
extern void cambiar_img(uint8 img, uint8 lcd);
extern void write_vp(uint8 dir, uint16 vp, uint16 valor);
extern void write_easterEgg(uint8 lcd);

extern void limpiarLCD(uint8 dir);
extern uint8 SoyBennett(void);
/* [] END OF FILE */
