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
* Filename      : VariablesG.c
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
//#define fc_version     18
#define si              3     
/********Estados Psoc_p**********/	
#define p_espera 	    1
#define p_listo  	    2	
#define p_surtiendo     3
#define p_venta  	    4
#define p_errormanguera 5
#define p_id			6
/********Estados Psoc_w**********/	
#define w_espera 	        1
#define w_listo  	        2	
#define w_surtiendo         3
#define w_venta  	        4
#define w_pet_autoriza      5
#define w_pet_turno         6
#define w_pproducto_canasta 7
#define w_venta_canasta     8
#define w_cancelada         9
#define w_imp_turno         10
#define w_imp_venta         11
#define w_imp_venta_opues   12
#define w_errormanguera     15
#define w_consignacion      16
#define w_datafono          18
#define w_fpago             19 // 1.0.3.2
#define w_Arqueo            20

#define w_Aut_Pcol          21 // 1.0.3.2
#define w_Con_Pcol          22 // 1.0.3.2
#define w_PetConfinicial    23 // 1.0.3.3 Peticion de COnfig Iniciales
/************Comando Protocolo Psoc*********/
#define p_config_inicial   0xF0 
#define p_config_productos 0xF1
#define p_cambiar_precios  0xF2
#define p_reset_psoc       0xF3
#define p_reporte_venta    0xF4
#define p_autorizar_venta  0xF5
#define p_peticion_id      0xF6
#define p_peticion_totales 0xF7
#define p_peticion_imprmir 0xF8
#define p_envio_id		   0xF9
#define p_peticion_estado  0xFA
#define p_autorizar_cre    0xFB 
#define p_peticion_version 0xFD
#define p_peticion_ppu     0xFE
#define p_printool         0xE0

#define p_Bennett          0xFF
/*********autorizacion de venta********/
#define v_llego_ok 	1
#define v_envio_ok 	2
#define v_auto_ok 	3
#define v_auto_no 	4
#define v_envio_no 	5

#define v_fidel_ok  8
#define v_fidel_no  9

#define ack         3
/***********Tipos de Venta************/
#define venta_contado           0                             
#define venta_Fidelizado        2   
#define venta_credito           1
#define venta_calibracion       5
#define venta_traslado          6
#define venta_consulta_puntos   9
/**************Tipo Id***************/
#define id_ibutton          'I'
#define id_tarjeta          'T'
#define id_cedula           'C'
#define id_placa            'P'
#define id_no_fuel          'F'
#define id_codigo_barras    'B'
#define id_tapsi            'X'
#define id_Puntos_Col_A     'E' // Nuevo
#define id_Puntos_Col_R     'F' // Nuevo
/**************Turno****************/
#define turno_cerrado       0
#define turno_abierto       1
#define turno_aceptado      1
#define turno_no_aceptado   0


/****************************************************************************************************************
						Variables para manejo de datos del surtidor
****************************************************************************************************************/	
extern uint8 	ppux10, placa_obl;
extern uint8 	nombre_pro1[20],nombre_pro2[20],nombre_pro3[20];	//Nombre de cada producto
extern uint8 	nombre_pro4[20],nombre_pro5[20],nombre_pro6[20];	//Nombre de cada producto
extern uint8 	version, decimales_din, decimales_vol, decimales_ppu; //7 - 6
extern uint8    verificar_manija[4][1];
extern uint8   version_fuel[4];
extern uint8 Bandera[2][1];
struct surtidor{
	uint8 dir[4][1];
	uint8 estado[4][1];
	uint8 totales[4][3][24];   	//los 12 primeros son volumen y los segundos pesos
	uint8 ppu[4][3][5];           //Precio de cada manguera
	uint8 grado[4][3][1];			//Contiene el codigo del producto en cada grado
	uint8 mangueras[4][1];           //numero de mangueras 
    uint8 OkCOnfInicial;
    uint8 TipoSurtidor;
};
extern struct surtidor lado;


struct turno{
	uint8 estado[2][1];
	uint8 tipo_id[2][1];                     
	uint8 usuario[2][20];   	       //id de vendedor
	uint8 password[2][11];           //contraseña
    uint8 fecha[2][6];
    uint8 fecha_anterior[2][6];
    uint8 totales[2][174];
    uint8 totales_anteriores[2][174];
    uint8 aceptado[2][1];
    uint8 cedula[2][10];
    uint8 index;
    uint8 turno_acumulado_cargado;
    uint8 pidiendo_id_turno[2][1];
    uint8 peticion_cierre[2][1];
    uint8 Apertura_Fuera_Linea;
};
extern struct turno turno1;
extern uint8 save_turno[193];

extern uint8 Arqueo[2];
extern uint8 CaraTurno;

struct Fiel{
    uint8 Cedula[2][10];
    uint8 Contrasenha[2][4];                     
};
extern struct Fiel Fidelizado;
extern uint8 autoriza_fiel[2];

struct PuntosColombia{ // 1.0.3.2
    uint8 Habilitado;
    uint8 Cedula[2][10];
    uint8 Contra[2][10];
    uint8 ValorV[2][8];
    uint8 FechaV[2][6];
    uint8 Tipo[2][1];
    uint8 Ok[2][1];
    uint8 Disponible[2][8];
    uint8 Redimible[2][8];
    uint8 Dinero[2][8];
    uint8 TotPar[2][1];
    uint8 TipoDoc[2][1];
};
extern struct PuntosColombia PCol; // 1.0.3.2
/****************************************************************************************************************
								Variables para manejo de recibo
****************************************************************************************************************/
extern uint8 preset_rapido[4][1];
extern uint8 nombre[33];          //Nombre de la estacion
extern uint8 nit[15];             
extern uint8 telefono[20];
extern uint8 direccion[33];
extern uint8 lema1[32];
extern uint8 lema2[32];                  
extern uint8 fecha[3];                     		//0=mes 1=dia 2=año
extern uint8 hora[3];                      		//0=min 1=hora
struct recibo{					    		//Datos venta
	uint8  autorizada[4][1];
	uint8  posicion[4][1];                 
	uint8  ppu[4][6];
	uint8  dinero[4][9];	
	uint8  volumen[4][9];
	uint8  producto[4][1];
	uint8  manguera[4][1];
	uint8  preset[4][8]; 					//el bit 0 es el tipo de preset	
	uint8  tipo_id[4][1];
	uint8  id[4][16];
	uint8  km[4][11];
	uint8  placa[4][6];
	uint8  tipo_venta[4][6];		// 0 Contado 1 credito 2 autoservicio
	uint8  totales_ini[4][24]; 		//12 primeros volumen y los segundos pesos
	uint8  totales_fin[4][24]; 		//12 primeros volumen y los segundos pesos
	uint8  fecha_ini[4][6];
	uint8  fecha_fin[4][6];
	uint8  cedula[4][10];
	uint8  nit[4][10];
	uint8  print[4][1];
    uint8  placa_ok[4][1];
    uint8  nit_ok[4][1];
    uint8  cedula_ok[4][1];
    uint8  esCombustible[4][1];
    uint16 ventas_acumuladas[4][1];
    uint8  venta_acumulada_cargada[4][1];
    uint8  trama_auto_cre[4][260];
    uint8  volumen_redimido[4][9];
    uint8  id_manguera[4][1];
    uint8  fecha_consig[4][6];
    uint8  consignacion[4][1];
    uint8  valor_consig[4][7];
    uint8  limite_consig[4][1];
    uint8  forma_pago[4][1];
    uint8  valor_forma_pago[4][7];
    uint8  boucher_forma_pago[4][16];
    uint8  tipo_vehiculo[4][1];
    uint8  convenios[4][1];
};
extern struct recibo rventa;   	//datos de la venta actual lado 
extern uint8 aux_save_venta[149];
extern uint8 save_venta[4][149];


struct canasta{
    uint8  id_cliente[10]; 
    uint8  tipo_id_cliente; 
    uint8  id_producto[4][13];
    uint8  n_producto[4][20];
    uint8  v_producto[4][7];
    uint8  c_producto[4][1];
    uint32 v_intproducto_total[4][1];
    uint8  v_producto_total[4][7];
    uint8  producto_ok;
    uint32 suma_total;
    uint32 saldo_cliente;
};
extern struct canasta rcanasta;   	//datos de la canasta
extern uint8 indice_producto;
extern uint32 v_producto_unitario;

/*struct corte{
    uint8 buffer_corte[300]; 
    uint8 
};*/

/****************************************************************************************************************
								Variables para manejo de Impresora
****************************************************************************************************************/
extern uint8	print1;			    //Puerto de la impresora lado 1
extern uint8	print2;             //Puerto de la impresora lado 2

extern uint8   buffer_rxeeprom[300];

extern uint8 error_producto[2][0];

extern uint8 valor_vp[10];
extern uint8 buffer_lcd[2][200];
extern uint8 bloqueo_lcd[2][1];
extern uint8 msn_ldc[2][110];

extern uint8 reimpresion[4][2]; //Pos 0 activa reimpr Pos 1 guarda si es turno o venta

extern uint8 PosicionTurno[2];

//[] END OF FILE
