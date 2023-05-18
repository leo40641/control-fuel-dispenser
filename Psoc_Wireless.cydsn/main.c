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
* Filename      : main.c
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
#include "Variables.h"
#include "funciones.h"
#include "lcd.h"

void init(){
    uint8 x;
	CyGlobalIntEnable; /* Enable global interrupts. */
    SelectSS_Write(0);
    PSOC_EnableRxInt();
    LCD1_EnableRxInt();
    LCD2_EnableRxInt();
    XBee_EnableRxInt();	
	LCD1_Start();
	LCD2_Start();
    PSOC_Start();
	XBee_Start();
    SPIM_Start();
    EEPROM_Start();
    isr_LCD_StartEx(wait_LCD);
    Timer_LCD_Start();
	/********Inicia EEPROM************/
	SPIM_WriteTxData(0x01);
	while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
	SPIM_WriteTxData(0x00);
	while(!(SPIM_ReadStatus() & SPIM_STS_TX_FIFO_EMPTY));
	CyDelay(1);
	placa_obl = EEPROM_ReadByte(1) & 0x0F;	
    turno1.estado[0][0] = EEPROM_ReadByte(2);
    turno1.estado[1][0] = EEPROM_ReadByte(413);
    turno1.tipo_id[0][0] = EEPROM_ReadByte(3);
    turno1.tipo_id[1][0] = EEPROM_ReadByte(414);
    if((turno1.estado[0][0] != 0) && (turno1.estado[0][0] != 1)){
        turno1.estado[0][0] = turno_cerrado;
    }
    if((turno1.estado[1][0] != 0) && (turno1.estado[1][0] != 1)){
        turno1.estado[1][0] = turno_cerrado;
    }
    for(x=0;x<=5;x++){
        turno1.fecha[0][x] = EEPROM_ReadByte(x+188);    
        turno1.fecha[1][x] = EEPROM_ReadByte(x+599); 
    }   
    for(x=0;x<=19;x++){
        turno1.usuario[0][x] = EEPROM_ReadByte(x+194); 
        turno1.usuario[1][x] = EEPROM_ReadByte(x+605);
    }
    for(x=0;x<=9;x++){
        turno1.password[0][x] = EEPROM_ReadByte(x+4);    
        turno1.password[1][x] = EEPROM_ReadByte(x+415); 
    } 
    if((turno1.password[0][0] < '0') || (turno1.password[0][0] > '9')){
        for(x=0;x<=9;x++){
            turno1.password[0][x] = '0';    
        }        
    }
    if((turno1.password[1][0] < '0') || (turno1.password[1][0] > '9')){
        for(x=0;x<=9;x++){
            turno1.password[1][x] = '0';    
        }        
    }
    for(x=0;x<=9;x++){
        turno1.cedula[0][x] = EEPROM_ReadByte(x+214);  
        turno1.cedula[1][x] = EEPROM_ReadByte(x+625);
    }   
    for(x=0;x<=173;x++){
        turno1.totales[0][x] = EEPROM_ReadByte(x+14); 
        turno1.totales[1][x] = EEPROM_ReadByte(x+425);
    } 
    rventa.ventas_acumuladas[0][0] = (EEPROM_ReadByte(224 << 8)&0xFF00) | EEPROM_ReadByte(225);
    rventa.ventas_acumuladas[1][0] = (EEPROM_ReadByte(226 << 8)&0xFF00) | EEPROM_ReadByte(227);
    cambiar_img(0, 0);
    cambiar_img(0, 1);

}

void init_PSOC(){
    uint8 ok = 0,size;
    CyDelay(1500);
    
    while(ok < 2){
    	PSOC_ClearRxBuffer();
    	PSOC_PutChar(p_peticion_estado);
    	PSOC_PutChar(0x01);
    	timeout_psoc(3,2);
        size = PSOC_GetRxBufferSize();
    	if((PSOC_rxBuffer[0] == 0xEA) && (size == 3)){
    		lado.dir[0][0] = PSOC_rxBuffer[1];
    		lado.estado[0][0] = w_espera;
            ok++;
    	}else{
            ok = 0;
        }
    	PSOC_ClearRxBuffer();
    	PSOC_PutChar(p_peticion_estado);
    	PSOC_PutChar(0x02);
    	timeout_psoc(3,2);
        size = PSOC_GetRxBufferSize();
    	if((PSOC_rxBuffer[0] == 0xEA) && (size == 3)){
    		lado.dir[1][0] = PSOC_rxBuffer[1];
    		lado.estado[1][0] = w_espera;
            ok++;
    	}else{
            ok = 0;
        }
    }
}

void polling_psoc(){
	uint8 x;
	x = Psoc_status_Read();
	if((x & 1) == 1){
		call_psoc(lado.dir[0][0]);
	}else if((x & 2) == 2){
		call_psoc(lado.dir[1][0]);
	}
}

void polling_xbee(){
	uint16 size1, size2, cmd;
	size1 = XBee_GetRxBufferSize();
	if(size1 >= 10){
		CyDelay(2);
		size2 = XBee_GetRxBufferSize();
		if(size2 == size1){
			if((XBee_rxBuffer[0] == 0x7E) && (XBee_rxBuffer[3] == 0x90)){
				cmd = XBee_rxBuffer[16];
				call_xbee(cmd);
				XBee_ClearRxBuffer();
			}else{
				XBee_ClearRxBuffer();
			}
		}
	}
	
}

uint8 SoyBennett(void){
    uint8 ok = 0;
    while(ok < 10){
        PSOC_ClearRxBuffer();
    	PSOC_PutChar(p_Bennett);
        PSOC_PutChar(p_Bennett);
        PSOC_PutChar(p_Bennett);
    	timeout_psoc(3,2);
        uint8 size = PSOC_GetRxBufferSize();
    	if((PSOC_rxBuffer[0] == 0xEA) && (size == 3)){
    		ok = 11;
            lado.TipoSurtidor = 1;
            EEPROM_WriteByte(lado.TipoSurtidor,705);
            PSOC_ClearRxBuffer();
            return 1;
    	}
        else{
            lado.TipoSurtidor = 0;
            EEPROM_WriteByte(lado.TipoSurtidor,705);
            PSOC_ClearRxBuffer();
            return 0;
        }
    }
    return 0;
}

int main()
{
	uint8 f_lcd1 = 0;
	uint8 f_lcd2 = 0;
	init();
    led_Write(0);
    if(!SoyBennett()){
        init_PSOC();
        led_Write(1);
    }
    else{
        lado.dir[0][0] = 0;
        lado.dir[1][0] = 1;
    }
    InitLCD(); //Revisamos Si soy Moto o Automovil
    lado.OkCOnfInicial = 0;
    for(;;)
    {   
		led_Write(1);
		f_lcd1 = polling_lcd(f_lcd1, 0);
		polling_xbee();
		polling_psoc();
		led_Write(0);
		f_lcd2 = polling_lcd(f_lcd2, 1);
		polling_xbee();
		polling_psoc();	
    }
}


/* [] END OF FILE */
