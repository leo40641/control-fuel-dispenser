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
* Filename      : lcd.h
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

//{5A}{A5}{06}{83}{00}{00}{01}{00}{01} TRAMA RETURN KEY
//5A A5 04 80 03 00 12 Trama cambio de pantalla
//{5A}{A5}{0C}{83}{02}{00}{04}{00}{00}{00}{00}{00}{23}{CA}{CE} Trama read de vp numero
//{5A}{A5}{0C}{83}{02}{08}{04}{61}{62}{63}{31}{32}{33}{FF}{FF} trama read vp string
//5A A5 03 81 03 02 Trama leer imagen
//5A A5 03 81 20 07 Trama leer hora
//5A A5 0A 80 1F 5A 13 11 08 00 18 56 00 Trama escribir hora
//5A A5 05 82 00 10 00 64  Trama escribir en una vp

#define lcd_write_register 0x80
#define lcd_read_register  0x81
#define lcd_write_vp       0x82
#define lcd_read_vp        0x83
#define lcd_write_curve    0x84

#define vp_basic_touch     0

#define botom_devolver     0xF3
#define botom_enter        0x00F1
#define botom_si           0x5373
#define botom_no           0x4E6E
#define botom_P1           0x2131
#define botom_P2           0x4032
#define botom_P3           0x2333
#define botom_1            0x2131
#define botom_2            0x4032
#define botom_3            0x2333
#define botom_imp_venta    0x5676
#define botom_imp_turno    0x5474
#define botom_Producto1    0x4161
#define botom_Producto2    0x4262
#define botom_Producto3    0x4363
#define botom_Producto4    0x4464
#define botom_cara1        0x2131
#define botom_cara2        0x4032
#define botom_bloquear     0x00F4
#define nobotom_Producto1  0x2131
#define nobotom_Producto2  0x4032
#define nobotom_Producto3  0x2333
#define nobotom_Producto4  0x2434
#define botom_P4           0x2434
#define botom_P5           0x2535


#define b_Efectivo         0xF3
#define b_Acumulacion      0xF4
#define b_FormasPago        0xF5

#define b_Efectivo         0xF3
#define b_Datafono         0xF5
#define b_PuntosCol        0xF4
#define b_Sodexo           0xF6


/*
#define img_menu_inicial   143
#define img_menu_ventas    14
#define img_menu_dvf       18
#define img_teclado_din    22
#define img_teclado_vol    23
#define img_datos_surt     24
#define img_subir_manija   78 
#define img_datos_venta    25
#define img_no_recibos     26
#define img_esperando      82
#define img_sel_id_fd      28
#define img_sel_id_cr      38
#define img_km_venta_id    94
#define img_ibutton        75
#define img_codigo_barras  76
#define img_rfid           77
#define img_sumi_no_auto   50 
#define img_sumi_auto      51
#define img_sin_comuni     70
#define img_error_producto 79
#define img_sel_turno      54
#define img_confir_cerra_t 67
#define img_sel_id_t       58 
#define img_turno_abierto  64 
#define img_canasta        83
#define img_tipo_canasta   47
#define img_momento_corte  72
#define img_reimprimir     69
#define img_abrir_sin_sis  63
#define img_producto_no    88
#define img_exceso_cupo    46
#define img_elegir_cara    80
#define img_turno_cerrado  62
#define img_info_fc        68
#define img_redimir        85
#define img_validar_placa  86
#define img_manija_gas     90
#define img_puntos_vender  48
#define img_sel_id_cr_2    110
#define img_tres_mangueras 107
#define img_dos_mangueras  108
#define img_calibrar       17
#define img_menuventas_2   125
#define img_formas_pago    126
#define img_teclado_fp     131
#define img_valor_boucher  73
#define img_clave_turno    133
#define img_codigo_canasta 134
#define img_lcd_bloqueada  141
#define img_escoja_produ_3 138
#define img_escoja_produ_2 139
#define img_lcd_bloqueada  141
#define img_tipo_vehiculo  142
//#define img_turno_ac       77

*/

#define pantallazo 0
#define img_menu_inicial 	1
#define img_menu_inicial_2 	2
#define img_menu_inicial_3 	3
#define img_menu_inicial_4 	4
#define img_menu_inicial_5 	5
#define img_menu_inicial_6	6
#define img_menu_inicial_7 	7
#define img_menu_inicial_8 	8
#define img_menu_inicial_9 	9
#define img_menu_inicial_10	10
#define img_menu_inicial_1 	11
#define img_menu_ventas  	12
//#define img_calibrar      0x11   	
#define img_menu_dvf     	13
#define img_teclado_din  	14
#define img_teclado_vol  	15
#define img_datos_surt   	16
#define img_datos_venta  	17
#define img_no_recibos   	18
#define img_sel_id_fd    	19
#define img_sel_id_cr    	22
#define img_exceso_cupo  	23
#define img_tipo_canasta 	24
#define img_puntos_vender	25
#define img_sumi_no_auto 	26
#define img_sumi_auto    	27
#define img_sel_turno    	29
#define img_sel_id_t     	30
#define img_turno_cerrado	31
#define img_abrir_sin_sis	32
#define img_turno_abierto	33
#define img_confir_cerra_t	34
#define img_info_fc      	35
#define img_reimprimir   	36
#define img_sin_comuni   	37
#define img_momento_corte	38
#define img_valor_boucher	39
#define img_ibutton      	41
#define img_codigo_barras	42
#define img_rfid         	43
#define img_subir_manija 	44
#define img_error_producto	45
#define img_elegir_cara  	46
#define img_esperando    	47
#define img_canasta      	48
#define img_redimir      	49
#define img_validar_placa	50
#define img_producto_no  	51
#define img_manija_gas   	53
#define img_km_venta_id  	54
#define img_sel_id_cr_2  	56
#define img_tres_mangueras	58
#define img_dos_mangueras	59
#define img_menuventas_2 	62
//#define img_formas_pago  	63 //1.0.3.0
#define img_teclado_fp   	66
#define img_clave_turno  	67
#define img_codigo_canast	68
#define img_escoja_produ_3	70
#define img_escoja_produ_2	71
#define img_lcd_bloqueada	72
#define img_tipo_vehiculo	73

#define img_cedula_fiel     78
#define img_conf_hibr       79
#define img_consignacion    80
#define img_Bandera         81
#define img_datos_venta_2  	82

#define img_ce_pcolombia    83 //1.0.3.2
#define img_con_pcolombia   84 //1.0.3.2
#define img_formas_pago     86 //1.0.3.2
#define img_formas_pago2    85 //1.0.3.2
#define img_mensaje_Pcol    87 //1.0.3.2
#define img_Definir_tRed    88 //1.0.3.2
#define img_Dinero_Pcol     89 //1.0.3.2
#define img_cancelar_espera 90 //1.0.3.2
#define img_TipoDoc_Pcol    91 //1.0.3.2
#define img_HabiliPcol      92 //1.0.3.2

#define img_NoLevBennet     93 //1.0.3.2.1
#define img_TipoClienteCan  94 //1.0.3.2.1

#define img_TipoSurtidor    95

#define vp_valor_dinero    0x200   //va de 0x200 a 0x203
#define vp_valor_vol       0x204   //va de 0x204 a 0x207
#define vp_valor_placa     0x208   //va de 0x208 a 0x213
#define vp_valor_cedula    0x214   //va de 0x214 a 0x223
#define vp_valor_nit       0x224   //va de 0x224 a 0x233
#define vp_valor_km        0x234   //va de 0x234 a 0x243
#define vp_valor_clave_t   0x234
#define vp_valor_code_can  0x234
#define vp_valor_no_fuel   0x244   //va de 0x244 a 0x253
#define vp_msn_autorizo    0x254   //va de 0x254 a 0x2D4
#define vp_producto1       0x2D5   //va de 0x2D5 a 0x2E8
#define vp_producto2       0x2E9   //va de 0x2E9 a 0x2FC
#define vp_producto3       0x2FD   //va de 0x2FD a 0x310
#define vp_producto4       0x311   //va de 0x311 a 0x324
#define vp_cantidad_p1     0x324   
#define vp_cantidad_p2     0x325   
#define vp_cantidad_p3     0x326   
#define vp_cantidad_p4     0x327  
#define vp_valo_p1         0x328 
#define vp_valo_p2         0x332
#define vp_valo_p3         0x336
#define vp_valo_p4         0x340
#define vp_total_c         0x344
#define vp_msn_no_reco     0x348 //va de 0x348
#define vp_placa_gas       0x400 //va de 0x400 - 409
#define vp_vol_redimido    0x410 //va de 410 a 413
#define vp_valor_tapsi     0x414 //va de 414 a 420
#define vp_no_recibos      0x001 
#define vp_pos             0x12A
#define vp_version         0x13A
#define vp_digitos         0x14B
#define vp_valor_consig    0x008
#define vp_valor_datafono  0x421   //va de 0x421 a 0x430
#define vp_valor_fpago     0x431   //va de 0x431 a 0x441
#define vp_valor_baucher   0x441   //va de 0x441 a 0x451
#define vp_combustible_1   0x452   //va de 0x452 a 0x471
#define vp_combustible_2   0x472   //va de 0x472 a 0x491
#define vp_combustible_3   0x492   //va de 0x492 a 0x511 

#define f_lcd_pantallazo   	        0
#define f_lcd_menu_inicial 	        1
#define f_menu_ventas		        2
#define f_menu_dvf			        3
#define f_menu_datos_surt           4
#define f_menu_subir_manija         5
#define f_datos_venta               6
#define f_no_recibos                7
#define f_menu_esperando_reportar   8
#define f_menu_elegir_id_credito    9
#define f_menu_km_venta_id         10
#define f_menu_esperar_auto        11
#define f_lcd_esperar_id           12
#define f_menu_turno               13
#define f_menu_elegir_id_t         14
#define f_menu_cerrar_t            15
#define f_lcd_esperar_id_t         16
#define f_lcd_esperar_turno        17
#define f_lcd_canasta              18
#define f_lcd_tipo_canasta         19
#define f_lcd_esperar_auto_canasta 20
#define f_lcd_esperar_id_c         21
#define f_lcd_esperar_canasta      22
#define f_no_recibos_c             23
#define f_lcd_reimprimir           24
#define f_menu_esperar_reimp       25
#define f_lcd_recuperar_venta      26
#define f_lcd_abrir_sin_sis        27 
#define f_lcd_recuperar_turno      28 
#define f_menu_dvf_cre             29
#define f_menu_cara_reimp          30
#define f_autorizar_gas            31
#define f_validar_placa            32
#define f_lcd_info_fc              33
#define f_lcd_redimir              34
#define f_puntos_vender            35
#define f_esperar_consignacion     36
#define f_formas_pago              37 //1.0.3.2
#define f_valor_formas_pago        38
#define f_boucher_formas_pago      39
#define f_esperar_fp               40
#define f_menu_elegir_manguera     41
#define f_lcd_clave_t              42
#define bloquear_des_lcd           43
#define clave_calibracion          44
#define f_escoger_vehiculo         45
#define f_lcd_esperar_id_fiel      46
#define f_lcd_clave_fiel           47
#define f_lcd_esperar_Arqueo       48
#define f_lcd_clave_Config         49
#define f_lcd_MenuConf             50
#define f_lcd_Consignacion         51 

#define f_menu_elegir_Hibrida      52 //1.0.3.0
#define f_Bandera_estacion         53 //1.0.3.0

#define f_cedula_pcolombia         54 //1.0.3.2
#define f_contra_pcolombia         55 //1.0.3.2
#define f_esperar_cedula_pcol      56 //1.0.3.2
#define f_esperar_tRede            57 //1.0.3.2
#define f_mensaje_Pcol             58 //1.0.3.2
#define f_confirma_Pcol            59 //1.0.3.2
#define f_esperar_fp_pcol          60 //1.0.3.2
#define f_Parcial_RedenPcol        61 //1.0.3.2
#define f_tipodocumenPCol          62 //1.0.3.2 
#define f_HabilitaPcol             63 //1.0.3.2 

#define f_tipoClienteCanasta       64 //1.0.3.2.2

#define f_TipoSurtidorMarca        65 //1.0.3.2.2


/* [] END OF FILE */
