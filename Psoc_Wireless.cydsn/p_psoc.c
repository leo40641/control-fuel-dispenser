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
* Filename      : p_psoc.c
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

uint8 contador;

/*
*********************************************************************************************************
*                                          CY_ISR(wait_psoc)
*********************************************************************************************************
*/
CY_ISR(wait_psoc){
	Timer_psoc_STATUS;
	contador++;
}

/*
*********************************************************************************************************
*                                void timeout_psoc(uint8 bytes, uint8 time)
*********************************************************************************************************
*/
uint8 timeout_psoc(uint8 bytes, uint8 time){
	uint8 size;
	Timer_psoc_Start();
	isr_psoc_StartEx(wait_psoc);
	contador = 0;
	size = PSOC_GetRxBufferSize();
	while((size < bytes) && (contador <= time)){
		size = PSOC_GetRxBufferSize();
	}
    return size;
	Timer_psoc_Stop();
	isr_psoc_Stop();    
}

/*
*********************************************************************************************************
*                               void call_psoc(uint8 dir)
*********************************************************************************************************
*/
void call_psoc(uint8 dir){
	uint8 x, dir_a, cmd;
	dir_a = dir - lado.dir[0][0];
	PSOC_ClearRxBuffer();
	PSOC_PutChar(p_peticion_estado);
	PSOC_PutChar(dir);
	timeout_psoc(3,2);
	if(PSOC_rxBuffer[0] == 0xEA){
		cmd = PSOC_rxBuffer[2]; 
		switch(cmd){
			case p_espera:
                if((lado.estado[dir_a][0] == w_listo) || (lado.estado[dir_a][0] == w_surtiendo)){
    				lado.estado[dir_a][0] = w_espera;
                }    
			break;
				
			case p_surtiendo:           
				lado.estado[dir_a][0] = w_surtiendo;
			break;	
				
			case p_listo:
                if(lado.estado[dir_a][0] == w_espera){
    				lado.estado[dir_a][0] = w_listo;
                }
			break;
			
			case p_errormanguera:
				error_producto[dir_a][0] = 1;
			break;
				
			case p_id:
				PSOC_ClearRxBuffer();
				PSOC_PutChar(p_envio_id);
				PSOC_PutChar(dir);
				timeout_psoc(16,2);
				if(PSOC_rxBuffer[0] == 0xE9){
					rventa.tipo_id[dir_a][0] = PSOC_rxBuffer[2];
                    if(turno1.pidiendo_id_turno[dir_a][0] == 1){
                        turno1.tipo_id[dir_a][0] = PSOC_rxBuffer[2];
                        turno1.pidiendo_id_turno[dir_a][0] = 0;
                    }
					rventa.id[dir_a][0] = PSOC_rxBuffer[3];
					rventa.id[dir_a][1] = PSOC_rxBuffer[4];
					rventa.id[dir_a][2] = PSOC_rxBuffer[5];
					rventa.id[dir_a][3] = PSOC_rxBuffer[6];
					rventa.id[dir_a][4] = PSOC_rxBuffer[7];
					rventa.id[dir_a][5] = PSOC_rxBuffer[8];
					rventa.id[dir_a][6] = PSOC_rxBuffer[9];
					rventa.id[dir_a][7] = PSOC_rxBuffer[10];
					rventa.id[dir_a][8] = PSOC_rxBuffer[11];
					rventa.id[dir_a][9] = PSOC_rxBuffer[12];
					rventa.id[dir_a][10] = PSOC_rxBuffer[13];
					rventa.id[dir_a][11] = PSOC_rxBuffer[14];
					rventa.id[dir_a][12] = PSOC_rxBuffer[15];                                     
				}
			break;
				
			case p_venta:
				PSOC_ClearRxBuffer();
				PSOC_PutChar(p_reporte_venta);
				PSOC_PutChar(dir);
				timeout_psoc(70,2);
				rventa.manguera[dir_a][0] = PSOC_rxBuffer[2];
				for(x=0;x<=6;x++){
					rventa.dinero[dir_a][x] = PSOC_rxBuffer[3+x];
					rventa.volumen[dir_a][x] = PSOC_rxBuffer[10+x];
				}
				for(x=0;x<=4;x++){
					rventa.ppu[dir_a][x] = PSOC_rxBuffer[17+x];
				}				
				for(x=0;x<=23;x++){
					rventa.totales_ini[dir_a][x] = PSOC_rxBuffer[22+x];
					rventa.totales_fin[dir_a][x] = PSOC_rxBuffer[46+x];
				}
				rventa.autorizada[dir_a][0] = v_llego_ok;
			break;			
		}			
	}
}

/* [] END OF FILE */
