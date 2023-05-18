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
* Filename      : Variables.c
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

/****************************************************************************************************************
						Variables para manejo de datos del surtidor
****************************************************************************************************************/	
uint8 	ppux10, placa_obl;
uint8 	nombre_pro1[20],nombre_pro2[20],nombre_pro3[20];	//Nombre de cada producto
uint8 	nombre_pro4[20],nombre_pro5[20],nombre_pro6[20];	//Nombre de cada producto
uint8 	version, decimales_din, decimales_vol, decimales_ppu;	//7 - 6
uint8   verificar_manija[4][1];
uint8   version_fuel[4];

struct Fiel Fidelizado;
uint8 autoriza_fiel[2];
struct PuntosColombia PCol; // 1.0.3.2

struct surtidor lado;
struct turno turno1;
uint8 save_turno[193];

/****************************************************************************************************************
								Variables para manejo de recibo
****************************************************************************************************************/
uint8 nombre[33];          //Nombre de la estacion
uint8 nit[15];             
uint8 telefono[20];
uint8 direccion[33];
uint8 lema1[32];
uint8 lema2[32];                  
uint8 fecha[3];                     		//0=mes 1=dia 2=año
uint8 hora[3];                      		//0=min 1=hora

struct recibo rventa;   	//datos de la venta actual lado 1
uint8 aux_save_venta[149];
uint8 save_venta[4][149];
struct canasta rcanasta;   	//datos de la canasta
uint8 indice_producto;
uint32 v_producto_unitario;
uint8 preset_rapido[4][1];
uint8 Arqueo[2];
uint8 CaraTurno;
/****************************************************************************************************************
								Variables para manejo de Impresora
****************************************************************************************************************/
uint8	print1;			    //Puerto de la impresora lado 1
uint8	print2;             //Puerto de la impresora lado 2


/****************************************************************************************************************
								Variables para manejo de EEPROM
****************************************************************************************************************/
uint8   buffer_rxeeprom[300];

/****************************************************************************************************************
								Variables para manejo de LCD
****************************************************************************************************************/
uint8 error_producto[2][0];

uint8 buffer_lcd[2][200];
uint8 msn_ldc[2][110];
uint8 valor_vp[10];
uint8 bloqueo_lcd[2][1];

uint8 reimpresion[4][2];
uint8 PosicionTurno[2];
/* [] END OF FILE */
