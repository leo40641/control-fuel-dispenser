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
* Filename      : p_xbee.c
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
/********Comandos del cda***********/
#define xconfiguraciones_iniciales  0xC0
#define xpeticion_estado            0xC1
#define xconfigurar_productos       0xC2
#define xcambiar_precios            0xC3
#define xactualizar_hora            0xC4
#define xreset                      0xC5
#define xreporte_venta              0xC6
#define xpet_producto_canasta       0xC7
#define ximprimir                   0xCC
#define xpautorizarventa            0xC8
#define xautorizarventa             0xC9
#define xdatosturno                 0xCA
#define xdatosproductocanasta       0xCB
#define xventacanasta               0xCD
#define xautorizaventacanasta       0xCE
#define xconfirmacionturno          0xCF
#define xpet_cerrar_turno           0xD0
#define xconsignacion               0xD2
#define xdatos_formapago            0xD4
#define xnombre_productos           0xD5
#define xobtener_informacion        0xD6

#define XDataPCol                   0xD7 // 1.0.3.2
#define XRDataPCol                  0xD8 // 1.0.3.2

/********Comandos hacia el cda***********/
#define wconfiguraciones_iniciales  0xA0
#define wpeticion_estado            0xA1
#define wconfigurar_productos       0xA2
#define wcambiar_precios            0xA3
#define wactualizar_hora            0xA4
#define wreset                      0xA5
#define wreporte_venta              0xA6
#define wpet_producto_canasta       0xA7
#define wimprimir                   0xAC
#define wpautorizarventa            0xA8
#define wautorizarventa             0xA9
#define wdatosturno                 0xAA
#define wdatosproductocanasta       0xAB
#define wventacanasta               0xAD
#define wautorizaventacanasta       0xAE
#define wconfirmacionturno          0xAF
#define wpet_cerrar_turno           0xB0
#define wconsignacion               0xB1
#define wdatos_formapago            0xB3
#define wnombre_productos           0xB5
#define wEnvioPcolC                 0xB7 // 1.0.3.2
/************Reset al FC*****************/
#define reset_venta                 1
#define reset_venta_canasta         2   
#define reset_cancelada             3
#define reset_reimpresion           4

#define reset_consig                6
#define obligar_consignar           7
#define reset_consignar             8
#define reset_fpagos                9
/*
*********************************************************************************************************
*                                       void call_xbee(uint8 cmd)
*********************************************************************************************************
*/

uint8 Bandera[2][1];


void call_xbee(uint8 cmd){
	uint16 size, x, y;
	uint8 dir_a, respuesta[300], cheksum, tx_ok;
	tx_ok = 0;
	dir_a = XBee_rxBuffer[15] - lado.dir[0][0];
	respuesta[0]  = 0x7E;
	respuesta[3]  = 0x10;
	respuesta[4]  = 0x01;
	respuesta[5]  = XBee_rxBuffer[4];
	respuesta[6]  = XBee_rxBuffer[5];
	respuesta[7]  = XBee_rxBuffer[6];
	respuesta[8]  = XBee_rxBuffer[7];
	respuesta[9]  = XBee_rxBuffer[8];
	respuesta[10] = XBee_rxBuffer[9];
	respuesta[11] = XBee_rxBuffer[10];
	respuesta[12] = XBee_rxBuffer[11];
	respuesta[13] = 0xFF;
	respuesta[14] = 0xFE;
	respuesta[15] = 0x00;
	respuesta[16] = 0x00;	
	respuesta[17] = XBee_rxBuffer[15];
	switch(cmd){
	    case xconfiguraciones_iniciales:
        if(1){
			fecha[2] = XBee_rxBuffer[17];
			fecha[1] = XBee_rxBuffer[18];
			fecha[0] = XBee_rxBuffer[19];
			hora[2]  = XBee_rxBuffer[20];
			hora[1]  = XBee_rxBuffer[21];
			hora[0]  = XBee_rxBuffer[22];
            if(lado.OkCOnfInicial == 0){
                lado.estado[dir_a][0] = w_espera;
                lado.OkCOnfInicial = 1;
            }
            lado.mangueras[0][0] = (XBee_rxBuffer[24] & 0x0F)/2;     //Mangueras    
            lado.mangueras[1][0] = lado.mangueras[0][0];            
			write_fecha_lcd(0);
			write_fecha_lcd(1);
			placa_obl = XBee_rxBuffer[26];
			EEPROM_WriteByte(placa_obl,1);
			PSOC_PutChar(p_config_inicial);
			size = ((XBee_rxBuffer[1] << 8) | XBee_rxBuffer[2]) + 3;
			for(x=23;x<size;x++){
				PSOC_PutChar(XBee_rxBuffer[x]);
			}
            
            // Información de la estación
            
            for(x=0;x<=29;x++){						//Nombre Estacion
    			nombre[x]=XBee_rxBuffer[x+8+22];
    			EEPROM_WriteByte(nombre[x],x+4+22); 
    		} 
    		for(x=0;x<=29;x++){						//Direccion
    			direccion[x]=XBee_rxBuffer[x+38+22];
    			EEPROM_WriteByte(direccion[x],x+64+22); 
    		} 
    		for(x=0;x<=29;x++){						//Lema1
    			lema1[x]=XBee_rxBuffer[x+68+22];
    			EEPROM_WriteByte(lema1[x],x+194+22); 
    		} 
    		for(x=0;x<=29;x++){						//Lema2
    			lema2[x]=XBee_rxBuffer[x+98+22];
    			EEPROM_WriteByte(lema2[x],x+224+22); 
    		}
            
            // Fin de la informaicón
                    
                    
			size = 21;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wconfiguraciones_iniciales;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;
        }
		break;

        case xobtener_informacion:
            if(1){
			size = 216;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0xD4;
			respuesta[18] = wdatosturno;		//Comando 
            respuesta[19] = ~turno1.estado[dir_a][0] & 0x01;		
            respuesta[20] = 0;
            ////////////////////////////////
            x = 0;
            while(x != 20){
                PSOC_ClearRxBuffer();                                 
                PSOC_PutChar(p_peticion_version);
                PSOC_PutChar(dir_a + lado.dir[0][0]);
                x = timeout_psoc(20, 6);
            }
            for(x=0; x<=3; x++){
                version_fuel[x] = PSOC_rxBuffer[x];
            }
            respuesta[21] = version_fuel[0];
            respuesta[22] = version_fuel[1];
            respuesta[23] = version_fuel[2];
            respuesta[24] = version_fuel[3];
            respuesta[25] = '|';
            respuesta[26] = '2';
            respuesta[27] = '.';
            respuesta[28] = '7';
            respuesta[29] = 'W';
            respuesta[30] = 'i';
            ////////////////////////////////
            for(x=0;x<=2;x++){
                respuesta[31+x] = fecha[x];
            }
            for(x=0;x<=2;x++){
                respuesta[34+x] = hora[x];
            }  
            ///////////////////////////////
            x = 0;
            while(x != 176){                               
                PSOC_ClearRxBuffer();
                PSOC_PutChar(p_peticion_ppu); // Peticion solo de PPUs
                PSOC_PutChar(dir_a + lado.dir[0][0]);
                x = timeout_psoc(176, 3);
            }
            uint8 delta = 0;
            for(uint8 i = 0; i< 5 ;i++){
                if( PSOC_rxBuffer[i] == 0xE7){
                    if (PSOC_rxBuffer[i+1] == 0xFF){
                        delta = i+2;
                        i = 10;
                    }
                }
            }
            
            for(x=0;x<=173;x++){
               respuesta[37+x] = PSOC_rxBuffer[x+delta] & 0x0F; 
            }
            
            /////////////////////////////
            
            for(x=0;x<=3;x++){
                respuesta[211+x] = 0;
            }
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[215] = cheksum;
            for(x = 216;x<300;x++){
                respuesta[x] = 0;
            }
			tx_ok = 1;          
            }
        break;
			
		case xpeticion_estado:
        if(1){
			size = 21;
			respuesta[1]  = 0;		
			respuesta[2]  = 0x11;
			respuesta[18] = wpeticion_estado;		
			respuesta[19] = lado.estado[dir_a][0];
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;
        }
		break;
        
        case XDataPCol: // 1.0.3.2
            if(1){
                size = 64;//Tamaño
                respuesta[1]  = 0;		//Lenght
    			respuesta[2]  = 0x3C;
    			respuesta[18] = wEnvioPcolC;		//Comando
    			respuesta[19] = PCol.Tipo[dir_a][0]; // 'A' o 'R'
                for(x=0; x<=9; x++){
                    respuesta[x+20] = PCol.Cedula[dir_a][x]; // La Cedula
                }
                for(x=0; x<9; x++){
                    respuesta[x+30] = rventa.dinero[dir_a][x];; // La Cedula
                }
                respuesta[39] = rventa.id_manguera[dir_a][0];
                for(x=0; x<6; x++){
                    respuesta[x+40] = rventa.fecha_ini[dir_a][x]; // La Cedula
                }
                respuesta[46] = PCol.TipoDoc[dir_a][0];
                for(x=0; x<6; x++){
                    respuesta[x+47] = rventa.fecha_fin[dir_a][x];; // La Cedula
                }
                for(x=0; x<10; x++){
                    respuesta[x+53] = turno1.cedula[dir_a][x];; // La Cedula
                }
    			cheksum = 0;
    			for(x=3;x<(size-1);x++){
    				cheksum += respuesta[x]; 
    			}
    			cheksum = 0xFF - (cheksum & 0xFF);
    			respuesta[63] = cheksum;
    			tx_ok = 1;
            }
        break;

        
        case XRDataPCol: // 1.0.3.2
            if(1){
            size = ((XBee_rxBuffer[1] << 8) | XBee_rxBuffer[2]) + 3;
            for ( x = 0; x<= 7; x++){
                PCol.Disponible[dir_a][x] = 0;
            }
            if(XBee_rxBuffer[17] == 1){
    			for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			}
                //Aqui recibo eldinero que tiene autorizada la persona
                for ( x = 0; x<= 7; x++){
                    PCol.Disponible[dir_a][x] =  XBee_rxBuffer[x+118];
                }
                lado.estado[dir_a][0] = w_surtiendo;
                PCol.Ok[dir_a][0] = 1;
            }else 
            if(XBee_rxBuffer[17] == 2){
                for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			} 
                lado.estado[dir_a][0] = w_surtiendo;
                PCol.Ok[dir_a][0] = 2;
            }
            
            
			size = 21;
			respuesta[1]  = 0;		        //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wEnvioPcolC;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1; 
        }
        break;  
            
            
		case xconfigurar_productos:
        if(1){
            for(x=0; x<=19; x++){
                nombre_pro1[x] = ' ';
                nombre_pro2[x] = ' ';
                nombre_pro3[x] = ' ';
            }
            x = 0;
            y = 0;
            while(XBee_rxBuffer[17+x] != ';'){
                nombre_pro1[x] = XBee_rxBuffer[17+x];
                x++;
                y++;
            }
            x=0;
            y++;
            while(XBee_rxBuffer[17+y] != ';'){
                nombre_pro2[x] = XBee_rxBuffer[17+y];
                x++;
                y++;
            }
            x=0;
            y++;
            while(XBee_rxBuffer[17+y] != ';'){
                nombre_pro3[x] = XBee_rxBuffer[17+y];
                x++;
                y++;
            }
            y++;
            PosicionTurno[dir_a] = XBee_rxBuffer[17+y];
            EEPROM_WriteByte(PosicionTurno[0],0x0706);
            
			LCD1_PutChar(0x5A);
            LCD1_PutChar(0xA5);
            LCD1_PutChar(0x17);
            LCD1_PutChar(0x82);
            LCD1_PutChar(0x04);
            LCD1_PutChar(0x52);
            for(x=0;x<=19;x++){
              LCD1_PutChar(nombre_pro1[x]);  
            }            
            LCD2_PutChar(0x5A);
            LCD2_PutChar(0xA5);
            LCD2_PutChar(0x17);
            LCD2_PutChar(0x82);
            LCD2_PutChar(0x04);
            LCD2_PutChar(0x52);
            for(x=0;x<=19;x++){
              LCD2_PutChar(nombre_pro1[x]);  
            }
            
			LCD1_PutChar(0x5A);
            LCD1_PutChar(0xA5);
            LCD1_PutChar(0x17);
            LCD1_PutChar(0x82);
            LCD1_PutChar(0x04);
            LCD1_PutChar(0x72);
            for(x=0;x<=19;x++){
              LCD1_PutChar(nombre_pro2[x]);  
            }            
            LCD2_PutChar(0x5A);
            LCD2_PutChar(0xA5);
            LCD2_PutChar(0x17);
            LCD2_PutChar(0x82);
            LCD2_PutChar(0x04);
            LCD2_PutChar(0x72);
            for(x=0;x<=19;x++){
              LCD2_PutChar(nombre_pro2[x]);  
            }  
            
            LCD1_PutChar(0x5A);
            LCD1_PutChar(0xA5);
            LCD1_PutChar(0x17);
            LCD1_PutChar(0x82);
            LCD1_PutChar(0x04);
            LCD1_PutChar(0x92);
            for(x=0;x<=19;x++){
              LCD1_PutChar(nombre_pro3[x]);  
            }            
            LCD2_PutChar(0x5A);
            LCD2_PutChar(0xA5);
            LCD2_PutChar(0x17);
            LCD2_PutChar(0x82);
            LCD2_PutChar(0x04);
            LCD2_PutChar(0x92);
            for(x=0;x<=19;x++){
              LCD2_PutChar(nombre_pro3[x]);  
            } 
            
			size = 21;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wconfigurar_productos;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;	
        }
		break;
	
		case xcambiar_precios:
        if(1){
			PSOC_PutChar(p_cambiar_precios);
			PSOC_PutChar(XBee_rxBuffer[15]);
			for(x=17;x<32;x++){
				PSOC_PutChar(XBee_rxBuffer[x]);	
			}
			size = 21;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wcambiar_precios;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;	
        }
		break;			
		
		case xactualizar_hora:
        if(1){
			fecha[0] = XBee_rxBuffer[17];
			fecha[1] = XBee_rxBuffer[18];
			fecha[2] = XBee_rxBuffer[19];
			hora[0]  = XBee_rxBuffer[20];
			hora[1]  = XBee_rxBuffer[21];
			hora[2]  = XBee_rxBuffer[22];
			//escribir fecha y hora lcd
			size = 21;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wactualizar_hora;		//Comando
			respuesta[19] = lado.estado[dir_a][0];
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;
        }
		break;	
			
		case xreset:
        if(1){
			switch(XBee_rxBuffer[17]){
				case reset_venta:
					if(lado.estado[dir_a][0] == w_venta){
						lado.estado[dir_a][0] = w_espera;
                        rventa.autorizada[dir_a][0] = v_envio_ok;
                        if(rventa.ventas_acumuladas[dir_a][0] >= 1){
                            rventa.ventas_acumuladas[dir_a][0]--;
                            EEPROM_WriteByte((rventa.ventas_acumuladas[dir_a][0]>>8),(224 + (dir_a*2)));
                            EEPROM_WriteByte((rventa.ventas_acumuladas[dir_a][0] & 0xFF),(225 + (dir_a*2)));                            
                        }
                        rventa.venta_acumulada_cargada[dir_a][0] = 0;
					}
				break;
                    
				case reset_venta_canasta:
					if(lado.estado[dir_a][0] == w_venta_canasta){
						lado.estado[dir_a][0] = w_espera;
                        rventa.autorizada[dir_a][0] = v_envio_ok;
					}
				break;  
                    
				case reset_cancelada:
					if(lado.estado[dir_a][0] == w_cancelada){
						lado.estado[dir_a][0] = w_espera;
                        rventa.autorizada[dir_a][0] = v_envio_ok;
					}
				break;

				case reset_reimpresion:
					if((lado.estado[dir_a][0] == w_imp_turno) || (lado.estado[dir_a][0] == w_imp_venta) || (lado.estado[dir_a][0] == w_imp_venta_opues)){
						lado.estado[dir_a][0] = w_espera;
                        reimpresion[dir_a][0] = 1;
					}
                    if(lado.estado[dir_a][0] == w_Arqueo){
                        lado.estado[dir_a][0] = w_espera;
                        Arqueo[dir_a] = 1;
					}

				break; 
                    
				case reset_consig:
					if(rventa.consignacion[dir_a][0] == 1){
						lado.estado[dir_a][0] = w_espera;
                        rventa.consignacion[dir_a][0] = 0;
					}
				break; 
 
				case obligar_consignar:
					rventa.limite_consig[0][0] = 1;
                    rventa.limite_consig[1][0] = 1;
				break; 
                
				case reset_consignar:
					rventa.limite_consig[0][0] = 0;
                    rventa.limite_consig[1][0] = 0;
				break;
                    
                case reset_fpagos:
                    rventa.forma_pago[dir_a][0] = 0xFF;  
                    lado.estado[dir_a][0] = w_surtiendo; // 1.0.3.2
                break;    
  
			}
			size = 21;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wreset;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;	
        }
		break;

		case xreporte_venta:
            if(1){
			size = 176;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0xAC;
			respuesta[18] = wreporte_venta;		//Comando
			respuesta[19] = rventa.manguera[dir_a][0];
			for(x=0;x<=6;x++){
				respuesta[20 + x] = rventa.dinero[dir_a][x];
				respuesta[27 + x] = rventa.volumen[dir_a][x];
			}
			for(x=0;x<=4;x++){
				respuesta[34 + x] = rventa.ppu[dir_a][x];
			}	
			for(x=0;x<=5;x++){
				respuesta[39 + x] = rventa.placa[dir_a][x];
			}	
			respuesta[45] = rventa.tipo_id[dir_a][0];
			for(x=0;x<=9;x++){
				respuesta[46 + x] = rventa.id[dir_a][x];
				respuesta[56 + x] = rventa.km[dir_a][x];
			}
			for(x=0;x<=5;x++){
				respuesta[66 + x] = rventa.fecha_ini[dir_a][x];
				respuesta[72 + x] = rventa.fecha_fin[dir_a][x];
			}
			respuesta[78] = rventa.tipo_venta[dir_a][0];
			respuesta[79] = rventa.preset[dir_a][0];
			for(x=1;x<=7;x++){
				respuesta[79 + x] = rventa.preset[dir_a][x];
			}	
			respuesta[87] = turno1.tipo_id[dir_a][0];
			for(x=0;x<=9;x++){
				respuesta[88 + x] = turno1.cedula[dir_a][x];
			}
			for(x=0;x<=23;x++){
				respuesta[98 + x] = rventa.totales_ini[dir_a][x] & 0x0F;
				respuesta[122 + x] = rventa.totales_fin[dir_a][x] & 0x0F;
			}
			for(x=0;x<=9;x++){
				respuesta[156 + x] = rventa.nit[dir_a][x];
				respuesta[146 + x] = rventa.cedula[dir_a][x];
			}
			respuesta[166] = rventa.print[dir_a][0] & 0x0F;			
			for(x=0;x<=6;x++){
				respuesta[167 + x] = rventa.volumen_redimido[dir_a][x];
			}        
            
            respuesta[174] = rventa.tipo_vehiculo[dir_a][0];
            if(rventa.venta_acumulada_cargada[dir_a][0] == 1){
                for(x=0;x<148;x++){
                    respuesta[x+19] = save_venta[dir_a][x];
                }
    			for(x=0;x<=6;x++){
    				respuesta[167 + x] = 0;
    			}
                respuesta[174] = 'R';
            }
            
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[175] = cheksum;
			tx_ok = 1;
            }
		break;
            
        case xconsignacion:
            if(1){
			size = 44;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x28;
			respuesta[18] = wconsignacion;		//Comando
			respuesta[19] = rventa.valor_consig[dir_a][0]; 
            respuesta[20] = rventa.valor_consig[dir_a][1];
            respuesta[21] = rventa.valor_consig[dir_a][2];
            respuesta[22] = rventa.valor_consig[dir_a][3];
            respuesta[23] = rventa.valor_consig[dir_a][4];
            respuesta[24] = rventa.valor_consig[dir_a][5];
            respuesta[25] = rventa.valor_consig[dir_a][6];
            respuesta[26] = rventa.fecha_consig[dir_a][0];
            respuesta[27] = rventa.fecha_consig[dir_a][1];
            respuesta[28] = rventa.fecha_consig[dir_a][2];
            respuesta[29] = rventa.fecha_consig[dir_a][3];
            respuesta[30] = rventa.fecha_consig[dir_a][4];
            respuesta[31] = rventa.fecha_consig[dir_a][5];
            respuesta[32] = turno1.tipo_id[dir_a][0];	
            for(x=0;x<=9;x++){
                respuesta[33+x] = turno1.cedula[dir_a][x];
            }
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[43] = cheksum;
			tx_ok = 1;
            }
        break; 
			
		case ximprimir:
            if(1){
			PSOC_PutChar(p_peticion_imprmir);
			PSOC_PutChar(XBee_rxBuffer[15]);
			size = ((XBee_rxBuffer[1] << 8) | XBee_rxBuffer[2]) + 3;
			for(x=17;x<size;x++){
				PSOC_PutChar(XBee_rxBuffer[x]);	
			}
			size = 21;
			respuesta[1]  = 0;		        //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wimprimir;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;		
            }
		break;
            
        case xpautorizarventa:
            if(1){
			size = 55;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x33;
			respuesta[18] = wpautorizarventa;		            //Comando
            respuesta[19] = rventa.tipo_id[dir_a][0];
            for(x=0;x<=9;x++){
                respuesta[20+x] = rventa.id[dir_a][x];    
            }
            for(x=0;x<=9;x++){
                respuesta[30+x] = rventa.km[dir_a][x];    
            }
            respuesta[40] = rventa.tipo_venta[dir_a][0];
            respuesta[41] = rventa.esCombustible[dir_a][0];
            respuesta[42] = 'I';
            for(x=0;x<=9;x++){
                respuesta[43+x] = 0;    
            } 
            respuesta[53] = rventa.id_manguera[dir_a][0];
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[54] = cheksum;
			tx_ok = 1;
    }
        break;
            
        case xautorizarventa:
            if(1){
            if(XBee_rxBuffer[17] == 1){
                size = ((XBee_rxBuffer[1] << 8) | XBee_rxBuffer[2]) + 3;
                rventa.trama_auto_cre[dir_a][1] = XBee_rxBuffer[15];
                rventa.trama_auto_cre[dir_a][2] = XBee_rxBuffer[18];
                y=2;
    			for(x=119;x<size;x++){
    				rventa.trama_auto_cre[dir_a][(x-119)+3] = XBee_rxBuffer[x];
                    y++;
    			} 
                rventa.trama_auto_cre[dir_a][0] = y;
    			for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			} 
                lado.estado[dir_a][0] = w_espera;
                rventa.autorizada[dir_a][0] = v_auto_ok;
            }else if(XBee_rxBuffer[17] == 2){
                lado.estado[dir_a][0] = w_espera;
                rventa.autorizada[dir_a][0] = v_auto_no;
    			for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			}                 
            }else if(XBee_rxBuffer[17] == 3){
                lado.estado[dir_a][0] = w_espera;
                rventa.forma_pago[dir_a][0] = 0xFE;
    			for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			}                 
            }else if(XBee_rxBuffer[17] == 4){
                rventa.autorizada[dir_a][0] = v_fidel_ok;
                size = ((XBee_rxBuffer[1] << 8) | XBee_rxBuffer[2]) + 3;
                rventa.trama_auto_cre[dir_a][1] = XBee_rxBuffer[15];
                rventa.trama_auto_cre[dir_a][2] = XBee_rxBuffer[18];
                y=2;
    			for(x=119;x<size;x++){
    				rventa.trama_auto_cre[dir_a][(x-119)+3] = XBee_rxBuffer[x];
                    y++;
    			} 
                rventa.trama_auto_cre[dir_a][0] = y;
                lado.estado[dir_a][0] = w_espera;
                for(x=0;x<99;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+19];	
    			}
                
                for(x=159;x<170;x++){
    				Fidelizado.Cedula[dir_a][x-159] = XBee_rxBuffer[x];
    			}
                for(x=0;x<4;x++){ 
    				Fidelizado.Contrasenha[dir_a][x] = XBee_rxBuffer[x+170];
    			}
            }
			size = 21;
			respuesta[1]  = 0;		        //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wautorizarventa;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1; 
        }
        break;
            
        case xdatosturno:
            if(1){
			size = 216;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0xD4;
			respuesta[18] = wdatosturno;		//Comando 
            respuesta[19] = ~turno1.estado[dir_a][0] & 0x01;		
            respuesta[20] = turno1.tipo_id[dir_a][0];	
            for(x=0;x<=9;x++){
                respuesta[21+x] = turno1.cedula[dir_a][x];
            }
            for(x=0;x<=5;x++){
                respuesta[31+x] = turno1.fecha[dir_a][x];
            } 
            x = 0;
            while(x != 176){                               
                PSOC_ClearRxBuffer();
                PSOC_PutChar(0xF7);
                PSOC_PutChar(dir_a + lado.dir[0][0]);
                x = timeout_psoc(176, 2);
            }                                     
            for(x=0;x<=173;x++){
                turno1.totales[dir_a][x] = PSOC_rxBuffer[x+2];  
            }            
            for(x=0;x<=173;x++){
                respuesta[37+x] = turno1.totales[dir_a][x] & 0x0F;
            } 
            for(x=0;x<=3;x++){
                respuesta[211+x] = turno1.password[dir_a][x];
            }
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[215] = cheksum;
			tx_ok = 1;
            }
        break;
            

            
        case xpet_producto_canasta:
            if(1){
			size = 34;
			respuesta[1]  = 0;		                    //Lenght
			respuesta[2]  = 0x1E;
			respuesta[18] = wpet_producto_canasta;		//Comando
            for(x=0;x<=12;x++){
			    respuesta[19+x] = rventa.id[dir_a][x];
            }
            respuesta[32] = rventa.tipo_venta[dir_a][0];
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[33] = cheksum;
			tx_ok = 1; 
            }
        break;
            
        case xdatosproductocanasta:
            if(1){
            if(XBee_rxBuffer[17] == 1){
                rcanasta.producto_ok = 2;
                for(x=0;x<=19;x++){
                    rcanasta.n_producto[indice_producto][x] = XBee_rxBuffer[18+x];
                }
                for(x=0;x<=6;x++){
                    rcanasta.v_producto[indice_producto][x] = XBee_rxBuffer[38+x] & 0x0F;
                }               
            }else{
                rcanasta.producto_ok = 1;
            }
            lado.estado[dir_a][0] = w_espera;
			size = 21;
			respuesta[1]  = 0;		        //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wdatosproductocanasta;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;   
            }
        break;
            
        case xventacanasta:
            if(1){
			size = 134;
			respuesta[1]  = 0;		//Lenght
			respuesta[2]  = 0x82;
			respuesta[18] = wventacanasta;		//Comando
			respuesta[19] = fecha[0];
            respuesta[20] = fecha[1];
            respuesta[21] = fecha[2];
			respuesta[22] = hora[2];
            respuesta[23] = hora[1];
            respuesta[24] = hora[0];            
			for(x=0;x<=12;x++){                                 //producto1
				respuesta[25 + x] = rcanasta.id_producto[0][x];
			}
            respuesta[38] = rcanasta.c_producto[0][0];
			for(x=0;x<=6;x++){
				respuesta[39 + x] = rcanasta.v_producto_total[0][x];
			}            
			for(x=0;x<=12;x++){                                 //Producto 2
				respuesta[46 + x] = rcanasta.id_producto[1][x];
			}
            respuesta[59] = rcanasta.c_producto[1][0];
			for(x=0;x<=6;x++){
				respuesta[60 + x] = rcanasta.v_producto_total[1][x];
			}            
			for(x=0;x<=12;x++){                                 //Producto 3
				respuesta[67 + x] = rcanasta.id_producto[2][x];
			}
            respuesta[80] = rcanasta.c_producto[2][0];
			for(x=0;x<=6;x++){
				respuesta[81 + x] = rcanasta.v_producto_total[2][x];
			}            
			for(x=0;x<=12;x++){                                 //Producto 4
				respuesta[88 + x] = rcanasta.id_producto[3][x];
			}
            respuesta[101] = rcanasta.c_producto[3][0];
			for(x=0;x<=6;x++){
				respuesta[102 + x] = rcanasta.v_producto_total[3][x];
			}             
            respuesta[109] = rventa.tipo_venta[dir_a][0];
			respuesta[110] = rcanasta.tipo_id_cliente;
			for(x=0;x<=9;x++){
				respuesta[111 + x] = rcanasta.id_cliente[x];
			}	
			respuesta[121] = turno1.tipo_id[dir_a][0];
			for(x=0;x<=9;x++){
				respuesta[122 + x] = turno1.cedula[dir_a][x];
			}
			respuesta[132] = rventa.print[dir_a][0] & 0x0F;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[133] = cheksum;
			tx_ok = 1;	 
            }
        break;
            
        case xautorizaventacanasta:
            if(1){
            if(XBee_rxBuffer[17] == 1){
    			for(x=0;x<69;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+18];	
    			} 
                if(rventa.esCombustible[dir_a][0] == 0){
                    rcanasta.saldo_cliente = (XBee_rxBuffer[97] & 0x0F) + ((XBee_rxBuffer[96] & 0x0F)*10) + ((XBee_rxBuffer[95] & 0x0F)*100) + ((XBee_rxBuffer[94] & 0x0F)*1000) + ((XBee_rxBuffer[93] & 0x0F)*10000) + ((XBee_rxBuffer[92] & 0x0F)*100000) + ((XBee_rxBuffer[91] & 0x0F)*1000000) + ((XBee_rxBuffer[90] & 0x0F)*10000000); 
                }
                lado.estado[dir_a][0] = w_espera;
                rventa.autorizada[dir_a][0] = v_auto_ok;
            }else{
                lado.estado[dir_a][0] = w_espera;
                rventa.autorizada[dir_a][0] = v_auto_no;
    			for(x=0;x<69;x++){
    				msn_ldc[dir_a][x] = XBee_rxBuffer[x+18];	
    			}                 
            }            
			size = 21;
			respuesta[1]  = 0;		                    //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wautorizaventacanasta;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1; 
            }
        break;
            
        case xconfirmacionturno: 
            if(1){
            turno1.peticion_cierre[0][0] = 0;
            turno1.peticion_cierre[1][0] = 0;
            if(XBee_rxBuffer[17] == 1){
                if(turno1.estado[dir_a][0] == turno_cerrado){
                    for(x=0;x<=19;x++){
                        turno1.usuario[dir_a][x] = XBee_rxBuffer[18+x];
                    }
                    for(x=0;x<=9;x++){
                        turno1.cedula[dir_a][x] = XBee_rxBuffer[38+x];
                    }
                    for(x=0;x<=3;x++){
                        turno1.password[dir_a][x] = XBee_rxBuffer[48+x];
                    }                   
                }
                if((XBee_rxBuffer[52] & 0x0F) != turno1.estado[dir_a][0]){
                    if(turno1.estado[dir_a][0] == turno_cerrado){
                        turno1.estado[dir_a][0] = turno_abierto;  
                    }else if(turno1.estado[dir_a][0] == turno_abierto){
                        turno1.estado[dir_a][0] = turno_cerrado;
                        Bandera[0][0] = 3;
                        Bandera[1][0] = 3;
                    }
                }
                if(dir_a == 0){
                    EEPROM_WriteByte(turno1.estado[dir_a][0], 2);
                    EEPROM_WriteByte(turno1.tipo_id[dir_a][0], 3);
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.password[dir_a][x], (4+x));    
                    }
                    for(x=0;x<=5;x++){
                        EEPROM_WriteByte(turno1.fecha[dir_a][x], (188 + x));    
                    } 
                    for(x=0;x<=19;x++){
                        EEPROM_WriteByte(turno1.usuario[dir_a][x], (194 + x));    
                    }  
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.cedula[dir_a][x], (214 + x));    
                    }                  
                    for(x=0;x<=173;x++){
                        EEPROM_WriteByte(turno1.totales[dir_a][x], (14 + x));    
                    }
                }else 
                if(dir_a == 1){
                    EEPROM_WriteByte(turno1.estado[dir_a][0], 413);
                    EEPROM_WriteByte(turno1.tipo_id[dir_a][0], 414);
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.password[dir_a][x], (415+x));    
                    }
                    for(x=0;x<=5;x++){
                        EEPROM_WriteByte(turno1.fecha[dir_a][x], (599 + x));    
                    } 
                    for(x=0;x<=19;x++){
                        EEPROM_WriteByte(turno1.usuario[dir_a][x], (605 + x));    
                    }  
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.cedula[dir_a][x], (625 + x));    
                    }                  
                    for(x=0;x<=173;x++){
                        EEPROM_WriteByte(turno1.totales[dir_a][x], (425 + x));    
                    }    
                }
                
                turno1.aceptado[dir_a][0] = turno_aceptado;
                lado.estado[dir_a][0]  = w_espera;
            }else{
                turno1.aceptado[dir_a][0] = turno_no_aceptado;
                lado.estado[dir_a][0]  = w_espera;                
            }
			size = 21;
			respuesta[1]  = 0;		                    //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wconfirmacionturno;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;  
            }
        break;
            
        case xpet_cerrar_turno:
            if(1){
            turno1.tipo_id[0][0] = EEPROM_ReadByte(3);  
            turno1.tipo_id[1][0] = EEPROM_ReadByte(3); 
            for(x=0;x<=9;x++){
                turno1.cedula[0][x] = EEPROM_ReadByte(4+x);   
                turno1.cedula[1][x] = EEPROM_ReadByte(4+x); 
            }
            for(x=0;x<=5;x++){
                turno1.fecha_anterior[0][x] = turno1.fecha[dir_a][x];  
                turno1.fecha_anterior[1][x] = turno1.fecha[dir_a][x];
            }  
            for(x=0;x<=173;x++){
                turno1.totales_anteriores[0][x] = turno1.totales[dir_a][x];     
                turno1.totales_anteriores[1][x] = turno1.totales[dir_a][x];
            }                                
            leer_fecha_lcd(dir_a);
            for(x=0; x<2; x++){
                turno1.fecha[x][0] = fecha[0];
                turno1.fecha[x][1] = fecha[1];
                turno1.fecha[x][2] = fecha[2];
                turno1.fecha[x][3] = hora[2];
                turno1.fecha[x][4] = hora[1];
                turno1.fecha[x][5] = hora[0]; 
            }
            x = 0;
            while(x != 176){                               
                PSOC_ClearRxBuffer();
                PSOC_PutChar(0xF7);
                PSOC_PutChar(dir_a + lado.dir[0][0]);
                x = timeout_psoc(176, 2);
            }  
            for(x=0;x<=173;x++){
                turno1.totales[0][x] = PSOC_rxBuffer[x+2];    
            }
            for(x=0;x<=173;x++){
                turno1.totales[1][x] = PSOC_rxBuffer[x+2];    
            }
			size = 21;
			respuesta[1]  = 0;		                    //Lenght
			respuesta[2]  = 0x11;
			respuesta[18] = wpet_cerrar_turno;		//Comando
			respuesta[19] = ack;
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[20] = cheksum;
			tx_ok = 1;  
            lado.estado[dir_a][0] = w_pet_turno;
            }
        break; 
            
        case xdatos_formapago:
            if(1){
            size = 76;
			respuesta[1]  = 0;		                    //Lenght
			respuesta[2]  = 0x48;
			respuesta[18] = wdatos_formapago;		//Comando
            respuesta[19] = rventa.forma_pago[dir_a][0];
            for(x=0;x<=6;x++){
			    respuesta[20+x] = rventa.valor_forma_pago[dir_a][x];
            }
            for(x=0;x<=15;x++){// los primeros 6 caracteres del boucher 
			    respuesta[27+x] = rventa.boucher_forma_pago[dir_a][x]; // Llenar todo de 0 y llenarlo con el password de PuntosColombia
            }
            
            //Agregar 
            respuesta[43] = 1;  //Liquido o Canasta
            for(x=0;x<=5;x++){
				respuesta[44 + x] = rventa.fecha_ini[dir_a][x];
            }
            respuesta[50] = rventa.id_manguera[dir_a][0];
            
            for (x=0;x<8;x++){
                respuesta[51+x] = rventa.dinero[dir_a][7-x] + 0x30;
            }
            for(x=0; x<6; x++){
                    respuesta[x+59] = rventa.fecha_fin[dir_a][x];; // La Cedula
            }
            for(x=0; x<10; x++){
                respuesta[x+65] = turno1.cedula[dir_a][x];; // La Cedula
            }
			cheksum = 0;
			for(x=3;x<(size-1);x++){
				cheksum += respuesta[x]; 
			}
			cheksum = 0xFF - (cheksum & 0xFF);
			respuesta[75] = cheksum;
			tx_ok = 1; 
            }   
        break;    
			
	}
	if(tx_ok == 1){
		for(x=0;x<size;x++){
			XBee_PutChar(respuesta[x]);
		}
	}
}
/* [] END OF FILE */
