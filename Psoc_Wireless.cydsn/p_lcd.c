#include <project.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Variables.h"
#include "funciones.h"
#include "lcd.h"

uint8 count[2][1];
uint8 prox_img[2][1];
uint8 prox_flujo[2][1];
uint8 timeout_lcd[2][1];
uint8 reverse[7];
uint32 vol_redimir;
uint8 contrasenha_fiel[2][4];

uint8 Identificacion_aux[2][10];
uint8 Tipo_aux[2][1];
uint8 Bandera[2][1];
uint8 Boton_redencion[2];

uint8 Hibrido;
uint8 Hibrido_m;
uint8 Bandera_estacion;
uint8 Step = 0;

uint8 img_autos = 0;


char aux_buffer[7] = {0,0,0,0,0,0,0};
uint8 ok, x, y; 
uint16 pos_vp, lcd_vp;
uint32 valor, preset_cre, ppu_cre;
uint8 img_actual[2];

//1.0.3.2
uint8 Mensaje_err[44] = {'E','l',' ','M','o','n','t','o',' ','a',' ','r','e','d','i','m','i','r',' ','e','s',' ','s','u','p','e','r','i','o','r',' ','a','l',' ','a','u','t','o','r','i','z','a','d','o'};
uint8 ErrPass[26] = {'E','r','r','o','r',',',' ','P','a','s','s','w','o','r','d',' ','I','n','v','a','l','i','d','o','.'};
uint8 ErrCe[53] = {'E','r','r','o','r',' ',',',' ','C','e','d','u','l','a',' ','n','o',' ','a','u','t','o','r','i','z','a','d','a',' ','c','o','n',' ','e','l',' ','c','l','i','e','n','t','e',' ','v','a','l','i','d','a','d','o','.'};
uint8 Mensaje_Ok[17] = {'C','a','n','t','i','d','a','d',' ','A','c','e','p','t','a','d','a'};
uint32 parc = 0;
uint32 red = 0;
uint32 dis = 0;

CY_ISR(wait_LCD){
    Timer_LCD_STATUS;
    count[0][0]++;   
    count[1][0]++;
}

void InitLCD(void){
    Hibrido = EEPROM_ReadByte(700);
    Hibrido_m = EEPROM_ReadByte(701);
    Bandera_estacion = EEPROM_ReadByte(702);
    img_autos = EEPROM_ReadByte(703);
    PCol.Habilitado = EEPROM_ReadByte(704);
    lado.TipoSurtidor = EEPROM_ReadByte(705);
    PosicionTurno[0] = EEPROM_ReadByte(706);
    EEPROM_WriteByte(turno1.Apertura_Fuera_Linea,707);
    
    if ((img_autos > 10) || (img_autos < 1)){
        img_autos = 1;
        EEPROM_WriteByte(img_autos,703);
    }
    
}

void guardar_venta(uint dir){
    uint8 x;
    aux_save_venta[0] = rventa.manguera[dir][0];
    for(x=0;x<=6;x++){
        aux_save_venta[1 + x] = rventa.dinero[dir][x];
        aux_save_venta[8 + x] = rventa.volumen[dir][x];
    }
    for(x=0;x<=4;x++){
        aux_save_venta[15 + x] = rventa.ppu[dir][x];
    }   
    for(x=0;x<=5;x++){
        aux_save_venta[20 + x] = rventa.placa[dir][x];
    }   
    aux_save_venta[26] = rventa.tipo_id[dir][0];
    for(x=0;x<=9;x++){
        aux_save_venta[27 + x] = rventa.id[dir][x];
        aux_save_venta[37 + x] = rventa.km[dir][x];
    }
    for(x=0;x<=5;x++){
        aux_save_venta[47 + x] = rventa.fecha_ini[dir][x];
        aux_save_venta[53 + x] = rventa.fecha_fin[dir][x];
    }
    aux_save_venta[59] = rventa.tipo_venta[dir][0];
    aux_save_venta[60] = rventa.preset[dir][0];
    for(x=1;x<=7;x++){
        aux_save_venta[60 + x] = rventa.preset[dir][x];
    }   
    aux_save_venta[68] = turno1.tipo_id[dir][0];
    for(x=0;x<=9;x++){
        aux_save_venta[69 + x] = turno1.cedula[dir][x];
    }
    for(x=0;x<=23;x++){
        aux_save_venta[79 + x] = rventa.totales_ini[dir][x] & 0x0F;
        aux_save_venta[103 + x] = rventa.totales_fin[dir][x] & 0x0F;
    }
    for(x=0;x<=9;x++){
        aux_save_venta[137 + x] = rventa.nit[dir][x];
        aux_save_venta[127 + x] = rventa.cedula[dir][x];
    }
    aux_save_venta[147] = rventa.print[dir][0] & 0x0F;
    aux_save_venta[148] = 0x7E;
}


void guardar_turno(uint8 dir){
    uint x;
    save_turno[0] = ~turno1.estado[dir][0] & 0x01;      
    save_turno[1] = turno1.tipo_id[dir][0]; 
    for(x=0;x<=9;x++){
        save_turno[2+x] = turno1.cedula[dir][x];
    }
    for(x=0;x<=5;x++){
        save_turno[12+x] = turno1.fecha[dir][x];
    } 
    for(x=0;x<=173;x++){
        save_turno[18+x] = turno1.totales[dir][x] & 0x0F;
    }
    save_turno[192] = 0x7E;
}
uint8 hay_datos_lcd(uint8 lcd){
    uint8 size1, size2, x;
    if(lcd == 0){
        size1 = LCD1_GetRxBufferSize(); 
        if(size1 > 8){
            CyDelay(1);
            size2 = LCD1_GetRxBufferSize(); 
            if(size1 == size2){
                x = 0;
                while(size1 != 0){
                    buffer_lcd[0][x] = LCD1_ReadRxData();
                    x++;
                    size1 = LCD1_GetRxBufferSize();
                }               
                if((buffer_lcd[0][0] == 0x5A) && (buffer_lcd[0][1] == 0xA5)){   
                    LCD1_ClearRxBuffer();
                    return 1;
                }
                LCD1_ClearRxBuffer();
            }
        }
        return 0;
    }else{
        size1 = LCD2_GetRxBufferSize(); 
        if(size1 > 8){
            CyDelay(1);
            size2 = LCD2_GetRxBufferSize(); 
            if(size1 == size2){
                x = 0;
                while(size1 != 0){
                    buffer_lcd[1][x] = LCD2_ReadRxData();
                    x++;
                    size1 = LCD2_GetRxBufferSize();
                }
                if((buffer_lcd[1][0] == 0x5A) && (buffer_lcd[1][1] == 0xA5)){   
                    LCD2_ClearRxBuffer();
                    return 1;
                }
                LCD2_ClearRxBuffer();
            }
        }
        return 0;
    }
}
void read_vp(uint8 lcd, uint16 pos_vp, uint8 size){
    uint8 x;
    if(lcd == 0){
        LCD1_ClearRxBuffer();
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x04);
        LCD1_PutChar(0x83);
        LCD1_PutChar((pos_vp & 0xFF00)>>8);
        LCD1_PutChar(pos_vp & 0x00FF);
        LCD1_PutChar(size); 
        x = LCD1_GetRxBufferSize();
        while(x < (7+(size*2))){
            x = LCD1_GetRxBufferSize();    
        }
        for(x=0;x<(size*2);x++){
            valor_vp[x] = LCD1_rxBuffer[x+7];
        }       
    }else{
        LCD2_ClearRxBuffer();
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x04);
        LCD2_PutChar(0x83);
        LCD2_PutChar((pos_vp & 0xF0)>>8);
        LCD2_PutChar(pos_vp & 0x0F);
        LCD2_PutChar(size); 
        x = LCD2_GetRxBufferSize();
        while(x < (7+(size*2))){
            x = LCD2_GetRxBufferSize();    
        }
        for(x=0;x<(size*2);x++){
            valor_vp[x] = LCD2_rxBuffer[x+7];
        }
    }
}
void write_valor_total(uint8 dir, uint16 vp, uint32 total){
    if(dir == 0){
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x07);
        LCD1_PutChar(0x82);
        LCD1_PutChar((vp & 0xFF00)>>8);
        LCD1_PutChar(vp & 0xFF); 
        LCD1_PutChar((total & 0xFF000000)>>24);
        LCD1_PutChar((total & 0xFF0000)>>16);
        LCD1_PutChar((total & 0xFF00)>>8);
        LCD1_PutChar(total & 0xFF);
    }else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x07);
        LCD2_PutChar(0x82);
        LCD2_PutChar((vp & 0xFF00)>>8);
        LCD2_PutChar(vp & 0xFF); 
        LCD2_PutChar((total & 0xFF000000)>>24);
        LCD2_PutChar((total & 0xFF0000)>>16);
        LCD2_PutChar((total & 0xFF00)>>8);
        LCD2_PutChar(total & 0xFF);    
    }
}
void write_vp(uint8 dir, uint16 vp, uint16 valor){
    if(dir == 0){
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x05);
        LCD1_PutChar(0x82);
        LCD1_PutChar((vp & 0xFF00)>>8);
        LCD1_PutChar(vp & 0xFF); 
        LCD1_PutChar((valor & 0xFF00)>>8);
        LCD1_PutChar(valor & 0xFF);
    }else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x05);
        LCD2_PutChar(0x82);
        LCD2_PutChar((vp & 0xFF00)>>8);
        LCD2_PutChar(vp & 0xFF); 
        LCD2_PutChar((valor & 0xFF00)>>8);
        LCD2_PutChar(valor & 0xFF);    
    }
}
void write_vp_text(uint8 dir, uint16 vp, uint8 *version_f, uint8 size){
    uint8 x=0;
    if(dir == 0){
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(size+3);
        LCD1_PutChar(0x82);
        LCD1_PutChar((vp & 0xFF00)>>8);
        LCD1_PutChar(vp & 0xFF); 
        for(x=0; x<size; x++){
            LCD1_PutChar(version_f[x]);    
        }
    }else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(size+3);
        LCD2_PutChar(0x82);
        LCD2_PutChar((vp & 0xFF00)>>8);
        LCD2_PutChar(vp & 0xFF); 
        for(x=0; x<size; x++){
            LCD2_PutChar(version_f[x]);    
        }    
    }
}
void leer_fecha_lcd(uint8 lcd){
    uint8 x, buffer[6] = {0x5A, 0xA5, 0x03, 0x81, 0x20, 0x07};
    if(lcd == 0){
        LCD1_ClearRxBuffer();
        for(x=0;x<=5;x++){
            LCD1_PutChar(buffer[x]);    
        }
        x = LCD1_GetRxBufferSize();
        while(x < 13){
            x = LCD1_GetRxBufferSize();
        }
        fecha[0] = LCD1_rxBuffer[8];
        fecha[1] = LCD1_rxBuffer[7];
        fecha[2] = LCD1_rxBuffer[6];
        hora[0]  = LCD1_rxBuffer[12];
        hora[1]  = LCD1_rxBuffer[11];
        hora[2]  = LCD1_rxBuffer[10];
    }else{
        LCD2_ClearRxBuffer();
        for(x=0;x<=5;x++){
            LCD2_PutChar(buffer[x]);    
        }
        x = LCD2_GetRxBufferSize();
        while(x < 13){
            x = LCD2_GetRxBufferSize();
        }
        fecha[0] = LCD2_rxBuffer[8];
        fecha[1] = LCD2_rxBuffer[7];
        fecha[2] = LCD2_rxBuffer[6];
        hora[0]  = LCD2_rxBuffer[12];
        hora[1]  = LCD2_rxBuffer[11];
        hora[2]  = LCD2_rxBuffer[10];       
    }
}
void write_fecha_lcd(uint8 lcd){
    uint8 x, buffer[13] = {0x5A, 0xA5, 0x0A, 0x80, 0x1F, 0x5A, 0, 0, 0, 0, 0, 0, 0};
    buffer[6]  = fecha[2];
    buffer[7]  = fecha[1];
    buffer[8]  = fecha[0];
    buffer[10] = hora[2];
    buffer[11] = hora[1];
    buffer[12] = hora[0];
    if(lcd == 0){
        for(x=0;x<=12;x++){
            LCD1_PutChar(buffer[x]);    
        }
    }else{
        for(x=0;x<=12;x++){
            LCD2_PutChar(buffer[x]);    
        }   
    }
}
void borrar_vp_lcd(uint8 lcd){
    uint8 x, buffer[6] = {0x5A, 0xA5, 0x5F, 0x82, 0x02, 0x08};
    if(lcd == 0){
        for(x=0;x<=5;x++){
            LCD1_PutChar(buffer[x]);    
        }
        for(x=0;x<=91;x++){
            LCD1_PutChar(0);
        }
    }else{
        for(x=0;x<=5;x++){
            LCD2_PutChar(buffer[x]);    
        }
        for(x=0;x<=91;x++){
            LCD2_PutChar(0);
        }   
    }
}
uint8 leer_img(uint8 lcd){
    uint8 x, buffer[6] = {0x5A, 0xA5, 0x03, 0x81, 0x03, 0x002};
    if(lcd == 0){
        x = LCD1_GetRxBufferSize();
        if(x == 0){
            for(x=0;x<=5;x++){
                LCD1_PutChar(buffer[x]);    
            }
            x = LCD1_GetRxBufferSize();
            count[lcd][0] = 0;
            while((x < 8) || (count[lcd][0] < 1)){
                x = LCD1_GetRxBufferSize();
            }
            LCD1_ClearRxBuffer();
            return (LCD1_rxBuffer[7]);
        }
        return 0xFF;
    }else{
        x = LCD2_GetRxBufferSize();
        if(x == 0){         
            for(x=0;x<=5;x++){
                LCD2_PutChar(buffer[x]);    
            }
            x = LCD2_GetRxBufferSize(); 
            count[lcd][0] = 0;
            while((x < 8) || (count[lcd][0] < 1)){
                x = LCD2_GetRxBufferSize();
            }
            LCD2_ClearRxBuffer();
            return (LCD2_rxBuffer[7]);  
        }
        return 0xFF;
    }
}
void cambiar_img(uint8 img, uint8 lcd){
    uint8 x, buffer[7] = {0x5A, 0xA5, 0x04, 0x80, 0x03, 0x00, 0x00};
    buffer[6] = img;
    if(lcd == 0){
        for(x=0;x<=6;x++){
            LCD1_PutChar(buffer[x]);    
        }
    }else{
            for(x=0;x<=6;x++){
            LCD2_PutChar(buffer[x]);    
        }   
    }
    
}


void write_producto(uint8 indice, uint8 lcd){
    uint8 x;
    if(lcd == 0){
        switch(indice){
            case 0:
                LCD1_PutChar(0x5A);
                LCD1_PutChar(0xA5);
                LCD1_PutChar(0x17);
                LCD1_PutChar(0x82);
                LCD1_PutChar(0x02);
                LCD1_PutChar(0xD5);
                for(x=0;x<=19;x++){
                  LCD1_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 1:
                LCD1_PutChar(0x5A);
                LCD1_PutChar(0xA5);
                LCD1_PutChar(0x17);
                LCD1_PutChar(0x82);
                LCD1_PutChar(0x02);
                LCD1_PutChar(0xE9);
                for(x=0;x<=19;x++){
                  LCD1_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 2:
                LCD1_PutChar(0x5A);
                LCD1_PutChar(0xA5);
                LCD1_PutChar(0x17);
                LCD1_PutChar(0x82);
                LCD1_PutChar(0x02);
                LCD1_PutChar(0xFD);
                for(x=0;x<=19;x++){
                  LCD1_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 3:
                LCD1_PutChar(0x5A);
                LCD1_PutChar(0xA5);
                LCD1_PutChar(0x17);
                LCD1_PutChar(0x82);
                LCD1_PutChar(0x03);
                LCD1_PutChar(0x11);
                for(x=0;x<=19;x++){
                  LCD1_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;                
        }
    }else{
        switch(indice){
            case 0:
                LCD2_PutChar(0x5A);
                LCD2_PutChar(0xA5);
                LCD2_PutChar(0x17);
                LCD2_PutChar(0x82);
                LCD2_PutChar(0x02);
                LCD2_PutChar(0xD5);
                for(x=0;x<=19;x++){
                  LCD2_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 1:
                LCD2_PutChar(0x5A);
                LCD2_PutChar(0xA5);
                LCD2_PutChar(0x17);
                LCD2_PutChar(0x82);
                LCD2_PutChar(0x02);
                LCD2_PutChar(0xE9);
                for(x=0;x<=19;x++){
                  LCD2_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 2:
                LCD2_PutChar(0x5A);
                LCD2_PutChar(0xA5);
                LCD2_PutChar(0x17);
                LCD2_PutChar(0x82);
                LCD2_PutChar(0x02);
                LCD2_PutChar(0xFD);
                for(x=0;x<=19;x++){
                  LCD2_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;
                
            case 3:
                LCD2_PutChar(0x5A);
                LCD2_PutChar(0xA5);
                LCD2_PutChar(0x17);
                LCD2_PutChar(0x82);
                LCD2_PutChar(0x03);
                LCD2_PutChar(0x11);
                for(x=0;x<=19;x++){
                  LCD2_PutChar(rcanasta.n_producto[indice][x]);  
                }                
            break;     
        }
    }    
}
void write_error(uint8 lcd){
    uint8 dir = lcd;
    uint8 x = 0;
    for(x=0;x<=99;x++){
     msn_ldc[dir][x] = 0x20;  
    }
    
    msn_ldc[dir][0] = 'E';
    msn_ldc[dir][1] = 'r';
    msn_ldc[dir][2] = 'r';
    msn_ldc[dir][3] = 'o';
    msn_ldc[dir][4] = 'r';
    msn_ldc[dir][5] = ' ';
    msn_ldc[dir][6] = 'C';
    msn_ldc[dir][7] = 'o';
    msn_ldc[dir][8] = 'n';
    msn_ldc[dir][9] = 't';
    msn_ldc[dir][10] = 'r';
    msn_ldc[dir][11] = 'a';
    
    
    if(dir == 0){
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x67);
        LCD1_PutChar(0x82);
        LCD1_PutChar(0x02);
        LCD1_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD1_PutChar(msn_ldc[dir][x]);  
        }
    }else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x67);
        LCD2_PutChar(0x82);
        LCD2_PutChar(0x02);
        LCD2_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD2_PutChar(msn_ldc[dir][x]);  
        }                
    }
}
void write_easterEgg(uint8 lcd){
    uint8 dir = lcd;
    uint8 x = 0;
    for(x=0;x<=99;x++){
     msn_ldc[dir][x] = 0x20;  
    }
    uint8 easteregg[25] = {'S','i','s','t','e','m','a','_','p','o','r','_','L','e','T','u','r','t','l','e','B','o','y',':','v'};
    for (x = 0;x<25; x++){
        msn_ldc[dir][x] = easteregg[x];
    }    
    if(dir == 0){
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x67);
        LCD1_PutChar(0x82);
        LCD1_PutChar(0x02);
        LCD1_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD1_PutChar(msn_ldc[dir][x]);  
        }
    }else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x67);
        LCD2_PutChar(0x82);
        LCD2_PutChar(0x02);
        LCD2_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD2_PutChar(msn_ldc[dir][x]);  
        }                
    }
}

void limpiarLCD(uint8 dir){
    if(dir == 0){ // escribo el mensaje en pantalla
        LCD1_PutChar(0x5A);
        LCD1_PutChar(0xA5);
        LCD1_PutChar(0x67);
        LCD1_PutChar(0x82);
        LCD1_PutChar(0x02);
        LCD1_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD1_PutChar(0);  
        }
    }
    else{
        LCD2_PutChar(0x5A);
        LCD2_PutChar(0xA5);
        LCD2_PutChar(0x67);
        LCD2_PutChar(0x82);
        LCD2_PutChar(0x02);
        LCD2_PutChar(0x54);
        for(x=0;x<=99;x++){
          LCD2_PutChar(0);  
        }                
    }
}
//Utilidades
void limpiar_venta(uint8 dir){
    error_producto[dir][0] = 0;
    for(y=0;y<6;y++){                   //inicializar placa, km, nit, cedula
        rventa.placa[dir][y] = 0;
    }
    for(y=0;y<10;y++){
        rventa.cedula[dir][y] = 0;
        rventa.km[dir][y] = 0;
        rventa.nit[dir][y] = 0;
    }
    for(x=0;x<=6;x++){                                   
        aux_buffer[x] = 0;
        reverse[x]    = 0;
    } 
    for(x=0;x<=9;x++){
        rventa.id[dir][x] = 0;    
    }
}
uint8 ProgramarPx(uint8 dir){
    if(Hibrido == 0){
        rventa.tipo_venta[dir][0] = venta_contado;
        rventa.tipo_id[dir][0]    = 0;                                
        lado.estado[dir][0] = w_espera;
        error_producto[dir][0] = 0;                                     
        if((lado.mangueras[dir][0] & 0x0F) == 3){
            cambiar_img(img_escoja_produ_3, dir);      
        }else if((lado.mangueras[dir][0] & 0x0F) == 2){
            cambiar_img(img_escoja_produ_2, dir);      
        }else if((lado.mangueras[dir][0] & 0x0F) == 1){
            rventa.id_manguera[dir][0] = 1;
            x = 0;                                 
            cambiar_img(img_tipo_vehiculo, dir);
            return f_escoger_vehiculo;
        }else if((lado.mangueras[dir][0] & 0x0F) == 0){
            cambiar_img(img_escoja_produ_3, dir);
        }                                   
        return f_menu_elegir_manguera;
    }
    else{
        rventa.tipo_venta[dir][0] = venta_contado;
        rventa.tipo_id[dir][0]    = 0;                                
        lado.estado[dir][0] = w_espera;
        error_producto[dir][0] = 0;                                     
        rventa.id_manguera[dir][0]  = Hibrido_m;
        rventa.tipo_vehiculo[dir][0] = 0x6D;
        x = 0;
        while(x != 3){
            PSOC_ClearRxBuffer();                            
            PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
            PSOC_PutChar(dir + lado.dir[0][0]); //dir
            PSOC_PutChar(2);                    //tipo preset            
            PSOC_PutChar(0xF0 | preset_rapido[dir][0]);
            PSOC_PutChar(0);
            PSOC_PutChar(9);
            PSOC_PutChar(9);
            PSOC_PutChar(9);
            PSOC_PutChar(9);
            PSOC_PutChar(9);
            PSOC_PutChar(Hibrido_m);                    //producto
            PSOC_PutChar('N');                  //precio
            PSOC_PutChar(0);
            PSOC_PutChar(0);
            PSOC_PutChar(0);
            PSOC_PutChar(0);
            x = timeout_psoc(3, 2);
        } 
        rventa.preset[dir][0] = 3;
        rventa.preset[dir][1] = 0;
        rventa.preset[dir][2] = 0;
        rventa.preset[dir][3] = 9;
        rventa.preset[dir][4] = 9;
        rventa.preset[dir][5] = 9;
        rventa.preset[dir][6] = 9;
        rventa.preset[dir][7] = 9;
        count[dir][0] = 0;
        error_producto[dir][0] = 0;
        cambiar_img(img_subir_manija, dir);
        return f_menu_subir_manija;   
    }
}

// Flujos Personalizados
uint8 FlujoPantallazo(uint8 dir){
    ok = hay_datos_lcd(dir);
    if(ok == 1){
        if(buffer_lcd[dir][3] == lcd_read_vp){
            pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
            lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
            if((pos_vp == 0) && (lcd_vp == 7)){
                count[dir][0] = 0;
                rventa.id_manguera[dir][0] = 0;
                Boton_redencion[dir] = 0;
                if(bloqueo_lcd[dir][0] == 0){
                    if(Hibrido == 1){
                        cambiar_img(11, dir); 
                    }else{
                        cambiar_img( img_autos, dir); 
                    }
                }else{
                    cambiar_img(img_lcd_bloqueada, dir);
                }
                return f_lcd_menu_inicial;
            }
        }
    }else 
    if(rventa.ventas_acumuladas[dir][0] >= 1){
        if(lado.estado[dir][0] == w_espera){
            Read_EEPROM(((10000 + (dir*120000)) * rventa.ventas_acumuladas[dir][0]),149);
            for(x=0;x<149;x++){
                save_venta[dir][x] = buffer_rxeeprom[x];
            }
            rventa.venta_acumulada_cargada[dir][0] = 1;
            lado.estado[dir][0] = w_venta;
        }
    }
    if(img_actual[dir] != 0){
        img_actual[dir] = leer_img(dir);
        cambiar_img(0, dir);    
    }

    return f_lcd_pantallazo;
    
}

void imprimir_venta_fuera (uint8 dir){
	for(uint8 x = 0; x<1; x++){
	    PSOC_PutChar(p_printool);//Peticion de Imprimir
		PSOC_PutChar(dir);//Bandera Autorización
	    PSOC_PutChar(rventa.fecha_fin[dir][2]);
	    PSOC_PutChar(rventa.fecha_fin[dir][1]);
	    PSOC_PutChar(rventa.fecha_fin[dir][0]);
	    PSOC_PutChar(rventa.fecha_fin[dir][3]);
	    PSOC_PutChar(rventa.fecha_fin[dir][4]);
	    for(x=0;x<9;x++){
	        PSOC_PutChar(rventa.dinero[dir][x]);
	    }
	    for(x=0;x<9;x++){
	        PSOC_PutChar(rventa.volumen[dir][x]);
	    }
	    PSOC_ClearTxBuffer();
	    CyDelay(100);
	}
}

uint8 polling_lcd(uint8 flujo, uint8 dir){
    
    if((Bandera[0][0] == 3)||(Bandera[1][0]== 3)){  // Nos Aseguramos que cuando se cierre turno
        cambiar_img(0, dir);                        // se lleve el sistema a FCS en ambas caras.
        Bandera[dir][0] = 0;
        return f_lcd_pantallazo;
    }
    switch(flujo){
        case f_lcd_pantallazo:
            return FlujoPantallazo(dir);
        break;

        case f_lcd_menu_inicial:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_bloquear:                                
                                cambiar_img(img_clave_turno, dir);  
                                return bloquear_des_lcd;        
                            break;
                            
                            case img_menu_ventas:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                    for(y=0;y<10;y++){
                                        rventa.cedula[dir][y] = 0;
                                        rventa.km[dir][y] = 0;
                                        rventa.nit[dir][y] = 0;
                                        rventa.placa[dir][y%6] = 0;
                                        error_producto[dir][0] = 0;
                                    }
                                    lado.estado[dir][0] = w_espera;
                                    cambiar_img(img_menuventas_2, dir); 
                                    return f_menu_ventas;
                                }    
                            break;
                                
                            case img_sel_turno:
                                if(((lado.estado[1][0] != w_surtiendo) && (dir == 0)) || ((lado.estado[0][0] != w_surtiendo) && (dir == 1))){
                                    turno1.turno_acumulado_cargado = 0;
                                    lado.estado[dir][0] = w_espera;
                                    cambiar_img(img_sel_turno, dir);    
                                    return f_menu_turno;
                                }
                            break;    
                                
                            case img_tipo_canasta:
                                if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                    lado.estado[dir][0] = w_espera;

                                    for(y=0;y<=3;y++){
                                        for(x=0;x<=19;x++){
                                            rcanasta.n_producto[y][x] = 0;       
                                        }
                                        for(x=0;x<=6;x++){
                                            rcanasta.v_producto[y][x] = 0;
                                            rcanasta.v_producto_total[y][x] = 0;
                                        } 
                                        for(x=0;x<=12;x++){
                                            rcanasta.id_producto[y][x] = 0;       
                                        }                                         
                                        rcanasta.c_producto[y][0] = 0;
                                    }
                                    for(x=0;x<=9;x++){
                                        rcanasta.id_cliente[x] = 0;
                                    }
                                    rcanasta.tipo_id_cliente = 0;                                    
                                    indice_producto = 0;
                                    
                                    if(dir == 0){
                                        LCD1_PutChar(0x5A);
                                        LCD1_PutChar(0xA5);
                                        LCD1_PutChar(0xE9);
                                        LCD1_PutChar(0x82);
                                        LCD1_PutChar(0x02);
                                        LCD1_PutChar(0xD5);
                                        for(x=0;x<=229;x++){
                                          LCD1_PutChar(0);  
                                        } 
                                    }else{
                                        LCD2_PutChar(0x5A);
                                        LCD2_PutChar(0xA5);
                                        LCD2_PutChar(0xE9);
                                        LCD2_PutChar(0x82);
                                        LCD2_PutChar(0x02);
                                        LCD2_PutChar(0xD5);
                                        for(x=0;x<=229;x++){
                                          LCD2_PutChar(0);  
                                        }                                    
                                    }
                                    rcanasta.v_intproducto_total[0][0] = 0;
                                    rcanasta.v_intproducto_total[1][0] = 0;
                                    rcanasta.v_intproducto_total[2][0] = 0;
                                    rcanasta.v_intproducto_total[3][0] = 0;
                                    rcanasta.suma_total = 0;
                                    cambiar_img(img_tipo_canasta, dir); 
                                    return f_lcd_tipo_canasta;
                                }
                            break;
                                
                            case 0x11:
                                if(1){
                                uint8 Step = turno1.peticion_cierre[0][0]+ turno1.peticion_cierre[1][0];
                                if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&(Step == 0)) {
                                    error_producto[dir][0] = 0;
                                    for(x=0;x<=6;x++){                                   
                                        aux_buffer[x] = 0;
                                        reverse[x]    = 0;
                                    } 
                                    for(x=0;x<=9;x++){
                                        rventa.id[dir][x] = 0;   
                                        rventa.km[dir][x] = 0;   
                                        rventa.nit[dir][x] = 0;   
                                        rventa.cedula[dir][x] = 0;
                                        rventa.placa[dir][x%6] = 0;   
                                    }
                                    error_producto[dir][0] = 0;
                                    rventa.esCombustible[dir][0] = 1;
                                    rventa.tipo_venta[dir][0] = venta_calibracion;
                                    rventa.tipo_id[dir][0]    = 'W';
                                    if((lado.mangueras[dir][0] & 0x0F) == 3){
                                    cambiar_img(img_escoja_produ_3, dir);      
                                    }else if((lado.mangueras[dir][0] & 0x0F) == 2){
                                        cambiar_img(img_escoja_produ_2, dir);      
                                    }else if((lado.mangueras[dir][0] & 0x0F) == 1){
                                        rventa.id_manguera[dir][0] = 1;
                                        count[dir][0] = 0;
                                        cambiar_img(img_clave_turno, dir);  
                                        return clave_calibracion;
                                    }else if((lado.mangueras[dir][0] & 0x0F) == 0){
                                        cambiar_img(img_escoja_produ_3, dir);
                                    }                                   
                                    return f_menu_elegir_manguera;                                    
                            }}
                            break;
 
                            case botom_P1:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                preset_rapido[dir][0] = 1;
                                limpiar_venta(dir);
                                return ProgramarPx(dir);
                                }
                            break;

                            case botom_P2:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                preset_rapido[dir][0] = 2;
                                limpiar_venta(dir);
                                return ProgramarPx(dir);
                                }
                            break;

                            case botom_P3:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                preset_rapido[dir][0] = 3;
                                limpiar_venta(dir);
                                return ProgramarPx(dir);
                                }
                            break;
                            //Casos existentes solo en motos
                            case botom_P4:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                preset_rapido[dir][0] = 4;
                                limpiar_venta(dir);
                                rventa.tipo_venta[dir][0] = venta_contado;
                                rventa.tipo_id[dir][0]    = 0;                                
                                lado.estado[dir][0] = w_espera;
                                error_producto[dir][0] = 0;                                     
                                rventa.id_manguera[dir][0]  = Hibrido_m;
                                rventa.tipo_vehiculo[dir][0] = 0x6D; 
                                reverse[0] = 0x30;
                                reverse[1] = 0x30;
                                reverse[2] = 0x30;
                                reverse[3] = 0x35;
                                reverse[4] = 0x00;
                                reverse[5] = 0x00;
                                reverse[6] = 0x00;
                                
                                
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                    PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                    PSOC_PutChar(2);                    //tipo preset            
                                    PSOC_PutChar(reverse[0] & 0x0F);
                                    PSOC_PutChar(reverse[1] & 0x0F);
                                    PSOC_PutChar(reverse[2] & 0x0F);
                                    PSOC_PutChar(reverse[3] & 0x0F);
                                    PSOC_PutChar(reverse[4] & 0x0F);
                                    PSOC_PutChar(reverse[5] & 0x0F);
                                    PSOC_PutChar(reverse[6] & 0x0F);
                                    PSOC_PutChar(rventa.id_manguera[dir][0]);                    //producto
                                    PSOC_PutChar('N');                  //precio
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    x = timeout_psoc(3, 2);
                                }    
                                rventa.preset[dir][0] = 2;
                                rventa.preset[dir][1] = reverse[0] & 0x0F;
                                rventa.preset[dir][2] = reverse[1] & 0x0F;
                                rventa.preset[dir][3] = reverse[2] & 0x0F;
                                rventa.preset[dir][4] = reverse[3] & 0x0F;
                                rventa.preset[dir][5] = reverse[4] & 0x0F;
                                rventa.preset[dir][6] = reverse[5] & 0x0F;
                                rventa.preset[dir][7] = reverse[6] & 0x0F;
                                count[dir][0] = 0;
                                error_producto[dir][0] = 0;
                                cambiar_img(img_subir_manija, dir);
                                return f_menu_subir_manija;
                                }
                            break;

                            case botom_P5:
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                preset_rapido[dir][0] = 5;
                                limpiar_venta(dir);
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                rventa.tipo_venta[dir][0] = venta_contado;
                                rventa.tipo_id[dir][0]    = 0;                                
                                lado.estado[dir][0] = w_espera;
                                error_producto[dir][0] = 0;                                     
                                rventa.id_manguera[dir][0]  = Hibrido_m;
                                rventa.tipo_vehiculo[dir][0] = 0x6D; 
                                reverse[0] = 0x30;
                                reverse[1] = 0x30;
                                reverse[2] = 0x30;
                                reverse[3] = 0x30;
                                reverse[4] = 0x31;
                                reverse[5] = 0x00;
                                reverse[6] = 0x00;
                                
                                if((turno1.estado[dir][0] == turno_abierto)&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                    PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                    PSOC_PutChar(2);                    //tipo preset            
                                    PSOC_PutChar(reverse[0] & 0x0F);
                                    PSOC_PutChar(reverse[1] & 0x0F);
                                    PSOC_PutChar(reverse[2] & 0x0F);
                                    PSOC_PutChar(reverse[3] & 0x0F);
                                    PSOC_PutChar(reverse[4] & 0x0F);
                                    PSOC_PutChar(reverse[5] & 0x0F);
                                    PSOC_PutChar(reverse[6] & 0x0F);
                                    PSOC_PutChar(Hibrido_m);                    //producto
                                    PSOC_PutChar('N');                  //precio
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    x = timeout_psoc(3, 2);
                                }    
                                rventa.preset[dir][0] = 2;
                                rventa.preset[dir][1] = reverse[0] & 0x0F;
                                rventa.preset[dir][2] = reverse[1] & 0x0F;
                                rventa.preset[dir][3] = reverse[2] & 0x0F;
                                rventa.preset[dir][4] = reverse[3] & 0x0F;
                                rventa.preset[dir][5] = reverse[4] & 0x0F;
                                rventa.preset[dir][6] = reverse[5] & 0x0F;
                                rventa.preset[dir][7] = reverse[6] & 0x0F;
                                count[dir][0] = 0;
                                error_producto[dir][0] = 0;
                                cambiar_img(img_subir_manija, dir);
                                return f_menu_subir_manija; 
                            }
                            break;


                                
                            case img_info_fc:
                                x = 0;
                                uint8 MFC_Name1[16];
                                while(x != 20){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_peticion_version);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    x = timeout_psoc(20, 6);
                                }
                                for(x=0; x<=3; x++){
                                    version_fuel[x] = PSOC_rxBuffer[x];
                                }
                                for(x=0; x<=15; x++){
                                    MFC_Name1[x] = PSOC_rxBuffer[x+5];
                                }
                                write_vp_text(dir, 0x0250 , MFC_Name1, 32);
                                CyDelay(5);
                                write_vp_text(dir, 0x014A , version_fuel, 4);
                                CyDelay(5);
                                write_vp(dir,vp_pos,lado.dir[dir][0]);
                                CyDelay(5);
                                uint8 fc_version = 27;
                                write_vp(dir,vp_version,fc_version);
                                CyDelay(50);
                                cambiar_img(img_info_fc, dir);  
                                return f_lcd_info_fc;                                
                            break;   
                                

                            case img_reimprimir:
                                cambiar_img(img_reimprimir, dir);   
                                return f_lcd_reimprimir;                               
                            break;    
                                
                            case 0x66: //Consignación
                                if((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06)){
                                    cambiar_img(img_consignacion, dir);    
                                    return f_lcd_Consignacion;
                                    count[dir][0] = 0;
                                }
                            break;
    
                        }
                    }
                                break;
                }}   
            }}
            else if(count[dir][0] >= 40){
                cambiar_img(0, dir);
                return f_lcd_pantallazo;
            }           
        }
        break;
        
        case bloquear_des_lcd:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_clave_t:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            if(y == 4){
                                for(x=0;x<y;x++){
                                    if(turno1.password[dir][x] != buffer_lcd[dir][7+x]){
                                         cambiar_img(75, dir);
                                        CyDelay(800);
                                        cambiar_img(0, dir);    
                                        return f_lcd_pantallazo;    
                                    }
                                }
                            }else{
                                cambiar_img(75, dir);
                                CyDelay(800);
                                cambiar_img(0, dir);    
                                return f_lcd_pantallazo;
                            }
                            if(bloqueo_lcd[dir][0] == 0){
                                bloqueo_lcd[dir][0] = 1;
                                cambiar_img(img_lcd_bloqueada, dir);    
                                return f_lcd_menu_inicial;
                            }else{                                
                                bloqueo_lcd[dir][0] = 0;
                                if(Hibrido == 1){
                                    cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;
                            }
                        break;                             
                    }
                }
            }
        }
        break; 
        ///////////////////////////////////////////////////////////////
        //CONSIGNACION FLUJO
        case f_lcd_Consignacion:
            if(1){
                ok = hay_datos_lcd(dir);
                if(ok == 1){
                    if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case 0x0432: //Dirección de la consignación
                            y = (buffer_lcd[dir][6] * 2)-2;
                            uint8 Dinero[7] = {0,0,0,0,0,0,0};
                            
                            for(x=0;x<y;x++){
                                Dinero[x] = buffer_lcd[dir][7+x];
                            }
                            valor = 0;
                            for(x=0;x<7;x++){
                                if(Dinero[x] != 0 && Dinero[x] != 0xFF){
                                    valor = valor*10 + (Dinero[x]-48);
                                }
                            }
                            
                            if (valor == 0){
                                cambiar_img(pantallazo,dir);
                                return f_lcd_pantallazo;
                            }
                            itoa(valor,aux_buffer,10);
                            for(x=0;x<=6;x++){
                                rventa.valor_consig[dir][x] = 0;
                                rventa.valor_consig[dir][x] = aux_buffer[x];
                            }                          
                            CyDelay(200);
                            
                            leer_fecha_lcd(dir);
                            rventa.fecha_consig[dir][0] = fecha[0];
                            rventa.fecha_consig[dir][1] = fecha[1];
                            rventa.fecha_consig[dir][2] = fecha[2];
                            rventa.fecha_consig[dir][3] = hora[2];
                            rventa.fecha_consig[dir][4] = hora[1];
                            rventa.fecha_consig[dir][5] = hora[0];
                            lado.estado[dir][0] = w_consignacion; 
                            rventa.consignacion[dir][0] = 1;
                            count[dir][0] = 0;
                            cambiar_img(img_esperando, dir);
                            return f_esperar_consignacion;
                            break;
                        }
                    }
                }
                if(count[dir][0] >= 60){                
                    cambiar_img(img_sin_comuni, dir);
                    CyDelay(2000);
                    cambiar_img(0, dir);
                    return f_lcd_pantallazo;      
                }
            }
        break;
        
        case f_esperar_consignacion:  
        if(1){
            if(rventa.consignacion[dir][0] == 0){
                cambiar_img(0, dir);
                return f_lcd_pantallazo;                
            }else if(count[dir][0] >= 60){
                lado.estado[dir][0] =  w_espera;
                cambiar_img(img_sin_comuni, dir);
                CyDelay(2000);
                cambiar_img(0, dir);
                return f_lcd_pantallazo;      
            }
        }
        break; 
        ///////////////////////////////////////////////////////////////
        //VENTAS FLUJO
        case f_menu_ventas:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case img_menu_dvf:              //CONTADO
                                for(x=0;x<=6;x++){                                   
                                    aux_buffer[x] = 0;
                                    reverse[x]    = 0;
                                } 
                                for(x=0;x<=9;x++){
                                    rventa.id[dir][x] = 0;    
                                }                                   
                                rventa.tipo_venta[dir][0] = venta_contado;
                                rventa.tipo_id[dir][0]    = 0;
                                if((lado.mangueras[dir][0] & 0x0F) == 3){
                                    cambiar_img(img_escoja_produ_3, dir);      
                                }else if((lado.mangueras[dir][0] & 0x0F) == 2){
                                    cambiar_img(img_escoja_produ_2, dir);      
                                }else if((lado.mangueras[dir][0] & 0x0F) == 1){
                                    rventa.id_manguera[dir][0] = 1;
                                    cambiar_img(img_menu_dvf, dir);
                                    count[dir][0] = 0;
                                    return f_menu_dvf;
                                }else if((lado.mangueras[dir][0] & 0x0F) == 0){
                                    cambiar_img(img_escoja_produ_3, dir);
                                }                                   
                                return f_menu_elegir_manguera;
                            break;
                                
                            case img_sel_id_cr:             //Credito
                                turno1.pidiendo_id_turno[dir][0] = 0;
                                rventa.autorizada[dir][0] = 0;
                                rventa.tipo_venta[dir][0] = venta_credito;
                                rventa.esCombustible[dir][0] = 1;
                                if((lado.mangueras[dir][0] & 0x0F) == 3){
                                    cambiar_img(img_escoja_produ_3, dir);      
                                }else if((lado.mangueras[dir][0] & 0x0F) == 2){
                                    cambiar_img(img_escoja_produ_2, dir);      
                                }else if((lado.mangueras[dir][0] & 0x0F) == 1){
                                    rventa.id_manguera[dir][0] = 1;
                                    cambiar_img(img_sel_id_cr_2, dir);  
                                    rventa.tipo_id[dir][0] = 0;
                                    for(x=0;x<=9;x++){
                                        rventa.id[dir][x] = 0;    
                                    }  
                                    x= 0;
                                    return f_menu_elegir_id_credito;
                                }else if((lado.mangueras[dir][0] & 0x0F) == 0){
                                    cambiar_img(img_escoja_produ_3, dir);
                                } 
                                for(x=0;x<=9;x++){
                                    rventa.id[dir][x] = 0;    
                                }                                                                   
                                return f_menu_elegir_manguera;                                    
                            break; 
                                
                            case img_sel_id_fd:             //fidelidad
                                for(x=0;x<=6;x++){                                   
                                    aux_buffer[x] = 0;
                                    reverse[x]    = 0;
                                }                                 
                                for(x=0;x<=9;x++){
                                    rventa.id[dir][x] = 0;    
                                }
                                turno1.pidiendo_id_turno[dir][0] = 0;
                                rventa.autorizada[dir][0] = 0;
                                rventa.tipo_venta[dir][0] = venta_Fidelizado;
                                rventa.esCombustible[dir][0] = 1;
                                cambiar_img(img_puntos_vender, dir);    
                                return f_puntos_vender;                                    
                            break;  
                               
                            case 0x3F: //NUEVO 
                                for(x=0;x<=6;x++){
                                    rventa.valor_forma_pago[dir][x] = 0;
                                }
                                for(x=0;x<=15;x++){
                                    rventa.boucher_forma_pago[dir][x] = 0;
                                }
                                cambiar_img(img_formas_pago2, dir);	
								return f_formas_pago;
                            break; 
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img(img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                               
                            break;    
                        }
                    }
                }   
            }           }
        break; 
        
        case f_menu_elegir_id_credito:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
            	for(x=0;x<16;x++){
                    rventa.id[dir][x] = 0;
                }
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;
                                
                            case img_tres_mangueras:
                                rventa.convenios[dir][0] = 'Z';
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){  
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('I');
                                    cambiar_img(img_ibutton, dir);
                                    x = timeout_psoc(3, 2);
                                }
                                return f_lcd_esperar_id;                                                                           
                            break;    
                                
                            case img_ibutton:
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){  
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('I');
                                    cambiar_img(img_ibutton, dir);
                                    x = timeout_psoc(3, 2);
                                }

                                return f_lcd_esperar_id;    
                            break;    
                                
                            case img_rfid:
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('T');
                                    x = timeout_psoc(3, 2);
                                }
                                cambiar_img(img_rfid, dir); 
                                return f_lcd_esperar_id;                                
                            break;   
                                
                            case img_codigo_barras:
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }                                    
                                cambiar_img(img_codigo_barras, dir);    
                                return f_lcd_esperar_id;    
                            break;                                 
                                
                        }
                    }else{        
                        switch(pos_vp){                            
                            case vp_valor_placa: 
                                y = (buffer_lcd[dir][6] * 2)-2;
                                for(x=0;x<y;x++){
                                    if(buffer_lcd[dir][7+x] != 0xFF){
                                        rventa.id[dir][x] = buffer_lcd[dir][7+x];
                                        if(x <= 6){
                                            rventa.placa[dir][x] = buffer_lcd[dir][7+x];
                                        }
                                    }
                                }
                                rventa.tipo_id[dir][0] = id_placa;
                                if(rventa.esCombustible[dir][0] == 1){
                                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                                        count[dir][0] = 0;                            
                                        lado.estado[dir][0] = w_pet_autoriza;
                                        cambiar_img(img_esperando, dir);    
                                        return f_menu_esperar_auto;                                        
                                    }else{
                                        cambiar_img(img_km_venta_id, dir); 
                                        return f_menu_km_venta_id;
                                    }
                                }else{
                                    count[dir][0] = 0;
                                    lado.estado[dir][0] = w_pet_autoriza;                                    
                                    cambiar_img(img_esperando, dir); 
                                    return f_menu_esperar_auto;                                    
                                }
                            break;  
                                
                            case vp_valor_cedula:
                                y = (buffer_lcd[dir][6] * 2)-2;
                                for(x=0;x<y;x++){
                                    if(buffer_lcd[dir][7+x] != 0xFF){
                                        rventa.id[dir][x] = buffer_lcd[dir][7+x];
                                    }
                                }
                                rventa.tipo_id[dir][0] = id_cedula; 
                                if(rventa.esCombustible[dir][0] == 1){
                                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                                        count[dir][0] = 0;                            
                                        lado.estado[dir][0] = w_pet_autoriza;
                                        cambiar_img(img_esperando, dir);    
                                        return f_menu_esperar_auto;                                        
                                    }else{
                                        cambiar_img(img_km_venta_id, dir); 
                                        return f_menu_km_venta_id;
                                    }
                                }else{
                                    count[dir][0] = 0;
                                    lado.estado[dir][0] = w_pet_autoriza;                                    
                                    cambiar_img(img_esperando, dir); 
                                    return f_menu_esperar_auto;                                    
                                }
                            break;  
                            
                            case vp_valor_no_fuel:
                                y = (buffer_lcd[dir][6] * 2)-2;
                                for(x=0;x<y;x++){
                                    if(buffer_lcd[dir][7+x] != 0xFF){
                                        rventa.id[dir][x] = buffer_lcd[dir][7+x];
                                    }
                                }
                                rventa.tipo_id[dir][0] = id_no_fuel; 
                                if(rventa.esCombustible[dir][0] == 1){
                                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                                        count[dir][0] = 0;                            
                                        lado.estado[dir][0] = w_pet_autoriza;
                                        cambiar_img(img_esperando, dir);    
                                        return f_menu_esperar_auto;                                        
                                    }else{
                                        cambiar_img(img_km_venta_id, dir); 
                                        return f_menu_km_venta_id;
                                    }
                                }else{
                                    count[dir][0] = 0;
                                    lado.estado[dir][0] = w_pet_autoriza;                                    
                                    cambiar_img(img_esperando, dir); 
                                    return f_menu_esperar_auto;                                    
                                }
                            break; 
                                
                            case vp_valor_tapsi:
                                y = (buffer_lcd[dir][6] * 2)-2;
                                for(x=0;x<y;x++){
                                    if(buffer_lcd[dir][7+x] != 0xFF){
                                        rventa.id[dir][x] = buffer_lcd[dir][7+x];
                                    }
                                }
                                rventa.tipo_id[dir][0] = id_tapsi; 
                                if(rventa.esCombustible[dir][0] == 1){
                                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                                        count[dir][0] = 0;                            
                                        lado.estado[dir][0] = w_pet_autoriza;
                                        cambiar_img(img_esperando, dir);    
                                        return f_menu_esperar_auto;                                        
                                    }else{
                                        cambiar_img(img_km_venta_id, dir); 
                                        return f_menu_km_venta_id;
                                    }
                                }else{
                                    count[dir][0] = 0;
                                    lado.estado[dir][0] = w_pet_autoriza;                                    
                                    cambiar_img(img_esperando, dir); 
                                    return f_menu_esperar_auto;                                    
                                }
                            break;                                
                                                                
                        }
                    }
                }   
            }                }
        break; 
        
        case f_menu_km_venta_id:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8]; 
                    switch(pos_vp){                                                          
                        case vp_valor_km:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.km[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }
                            count[dir][0] = 0;                            
                            lado.estado[dir][0] = w_pet_autoriza;
                            cambiar_img(img_esperando, dir);    
                            return f_menu_esperar_auto;
                        break;                                                        
                    }
                    if(botom_devolver == lcd_vp){
                        if(Hibrido == 1){
                                cambiar_img(11, dir); 
                        }else{
                            cambiar_img( img_autos, dir); 

                        } 
                        return f_lcd_menu_inicial;                            
                    } 
                }   
            }            
        }
        break;
        
        case f_menu_elegir_manguera:
        if(1){
            if(lado.OkCOnfInicial == 0){
                lado.estado[dir][0] = w_PetConfinicial;
            }
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;
                                
                            case botom_1:
                                rventa.id_manguera[dir][0] = 1;  
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;                                 
                                cambiar_img(img_tipo_vehiculo, dir);
                                return f_escoger_vehiculo;
                            break;    

                            case botom_2:
                                rventa.id_manguera[dir][0] = 2;  
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;  
                                cambiar_img(img_tipo_vehiculo, dir);
                                return f_escoger_vehiculo;                                 
                            break; 
                                
                            case botom_3:
                                rventa.id_manguera[dir][0] = 3;  
                                rventa.tipo_id[dir][0] = 0;
                                x = 0; 
                                cambiar_img(img_tipo_vehiculo, dir);
                                return f_escoger_vehiculo;                                 
                            break;                                                                                                                                                                 
                        }
                    }
                }   
            }               
        }
        break; 
        
        case f_escoger_vehiculo:
        if(1){
            ok = hay_datos_lcd(dir);
            if(rventa.tipo_venta[dir][0] == venta_calibracion){
                rventa.tipo_vehiculo[dir][0] = 0;
                rventa.tipo_id[dir][0] = 'W';
                cambiar_img(img_clave_turno, dir);  
                return clave_calibracion;
            }
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        if(lcd_vp == botom_devolver){
                            cambiar_img(img_menuventas_2, dir); 
                            return f_menu_ventas;    
                        }else{
                            rventa.tipo_vehiculo[dir][0] = lcd_vp;
                            if(rventa.tipo_venta[dir][0] == venta_calibracion){
                                rventa.tipo_id[dir][0] = 'W';
                                cambiar_img(img_clave_turno, dir);  
                                return clave_calibracion;
                            }
                            if(rventa.tipo_venta[dir][0] != venta_contado){
                                cambiar_img(img_sel_id_cr_2,dir);
                                return f_menu_elegir_id_credito;
                            }else{
                                if(preset_rapido[dir][0] != 0){
                                    if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                    x = 0;
                                    while(x != 3){
                                        PSOC_ClearRxBuffer();                            
                                        PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                        PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                        PSOC_PutChar(2);                    //tipo preset            
                                        PSOC_PutChar(0xF0 | preset_rapido[dir][0]);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(9);
                                        PSOC_PutChar(9);
                                        PSOC_PutChar(9);
                                        PSOC_PutChar(9);
                                        PSOC_PutChar(9);
                                        PSOC_PutChar(rventa.id_manguera[dir][0]);//producto
                                        PSOC_PutChar('N');                  //precio
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        x = timeout_psoc(3, 2);
                                    } 
                                    rventa.preset[dir][0] = 3;
                                    rventa.preset[dir][1] = 0;
                                    rventa.preset[dir][2] = 0;
                                    rventa.preset[dir][3] = 9;
                                    rventa.preset[dir][4] = 9;
                                    rventa.preset[dir][5] = 9;
                                    rventa.preset[dir][6] = 9;
                                    rventa.preset[dir][7] = 9;
                                    count[dir][0] = 0;
                                    preset_rapido[dir][0] = 0; 
                                    error_producto[dir][0] = 0;
                                    cambiar_img(img_subir_manija, dir);
                                    return f_menu_subir_manija;
                                    }else{
                                        cambiar_img(0,dir);
                                        return f_lcd_pantallazo;
                                    }
                                }                                
                                cambiar_img(img_menu_dvf, dir);
                                count[dir][0] = 0;
                                return f_menu_dvf;
                            }
                        }
                    }   
                }   
            }                
        }
        break; 
        
        case f_menu_dvf:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){    //Full
                        if(lcd_vp == img_subir_manija){
                            x = 0;
                            if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                            
                                    PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                    PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                    PSOC_PutChar(3);                    //tipo preset            
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(9);
                                    PSOC_PutChar(9);
                                    PSOC_PutChar(9);
                                    PSOC_PutChar(9);
                                    PSOC_PutChar(9);
                                    PSOC_PutChar(rventa.id_manguera[dir][0]);                    //producto
                                    PSOC_PutChar('N');                  //precio
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    PSOC_PutChar(0);
                                    x = timeout_psoc(3, 2);
                                }                             
                                rventa.preset[dir][0] = 3;
                                rventa.preset[dir][1] = 0;
                                rventa.preset[dir][2] = 0;
                                rventa.preset[dir][3] = 9;
                                rventa.preset[dir][4] = 9;
                                rventa.preset[dir][5] = 9;
                                rventa.preset[dir][6] = 9;
                                rventa.preset[dir][7] = 9; 
                                count[dir][0] = 0;
                                cambiar_img(img_subir_manija, dir);
                                return f_menu_subir_manija;
                            }else{
                                cambiar_img(0,dir);
                                return f_lcd_pantallazo;
                            }
                        }else 
                        if(lcd_vp == botom_devolver){
                            if(rventa.tipo_venta[dir][0] == venta_contado){
                                cambiar_img(img_menuventas_2, dir); 
                                return f_menu_ventas;
                            }else
                            if (rventa.tipo_venta[dir][0] == venta_calibracion){
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;
                            }
                        }    
                    }else{        
                        switch(pos_vp){                            
                            case vp_valor_dinero: 
                                if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                    for(x=0;x<=6;x++){                                   
                                        aux_buffer[x] = 0;
                                        reverse[x]    = 0;
                                    }                            
                                    valor = (buffer_lcd[dir][12] << 16) | (buffer_lcd[dir][13] << 8) | buffer_lcd[dir][14]; 
                                    itoa(valor,aux_buffer,10);
                                    y = 0;
                                    for(x=6;x>=1;x--){
                                        if((aux_buffer[x]>=0x30) && (aux_buffer[x]<=0x39)){
                                            reverse[y] = aux_buffer[x];
                                            y++;
                                        }    
                                    }
                                    reverse[y] = aux_buffer[0];
                                    x = 0;
                                    while(x != 3){
                                        PSOC_ClearRxBuffer();                                 
                                        PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                        PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                        PSOC_PutChar(2);                    //tipo preset            
                                        PSOC_PutChar(reverse[0] & 0x0F);
                                        PSOC_PutChar(reverse[1] & 0x0F);
                                        PSOC_PutChar(reverse[2] & 0x0F);
                                        PSOC_PutChar(reverse[3] & 0x0F);
                                        PSOC_PutChar(reverse[4] & 0x0F);
                                        PSOC_PutChar(reverse[5] & 0x0F);
                                        PSOC_PutChar(reverse[6] & 0x0F);
                                        PSOC_PutChar(rventa.id_manguera[dir][0]);                    //producto
                                        PSOC_PutChar('N');                  //precio
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        x = timeout_psoc(3, 2);
                                    }    
                                    rventa.preset[dir][0] = 2;
                                    rventa.preset[dir][1] = reverse[0] & 0x0F;
                                    rventa.preset[dir][2] = reverse[1] & 0x0F;
                                    rventa.preset[dir][3] = reverse[2] & 0x0F;
                                    rventa.preset[dir][4] = reverse[3] & 0x0F;
                                    rventa.preset[dir][5] = reverse[4] & 0x0F;
                                    rventa.preset[dir][6] = reverse[5] & 0x0F;
                                    rventa.preset[dir][7] = reverse[6] & 0x0F;
                                    count[dir][0] = 0;
                                    error_producto[dir][0] = 0;
                                    cambiar_img(img_subir_manija, dir);
                                    return f_menu_subir_manija;  
                                }
                                else
                                    {
                                        cambiar_img(0,dir);
                                        return f_lcd_pantallazo;
                                    }
                            break;  
                                
                            case vp_valor_vol:
                                if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                    for(x=0;x<=6;x++){                                   
                                        aux_buffer[x] = 0;
                                        reverse[x]    = 0;
                                    }                                
                                    valor = (buffer_lcd[dir][12] << 16) | (buffer_lcd[dir][13] << 8) | buffer_lcd[dir][14]; 
                                    itoa(valor,aux_buffer,10);
                                    y = 0;
                                    for(x=6;x>=1;x--){
                                        if((aux_buffer[x]>=0x30) && (aux_buffer[x]<=0x39)){
                                            reverse[y] = aux_buffer[x];
                                            y++;
                                        }    
                                    }  
                                    reverse[y] = aux_buffer[0];
                                    x = 0;
                                    while(x != 3){
                                        PSOC_ClearRxBuffer();                                
                                        PSOC_PutChar(p_autorizar_venta);    //Envio trama de autorizar
                                        PSOC_PutChar(dir + lado.dir[0][0]); //dir
                                        PSOC_PutChar(1);                    //tipo preset            
                                        PSOC_PutChar(reverse[0] & 0x0F);
                                        PSOC_PutChar(reverse[1] & 0x0F);
                                        PSOC_PutChar(reverse[2] & 0x0F);
                                        PSOC_PutChar(reverse[3] & 0x0F);
                                        PSOC_PutChar(reverse[4] & 0x0F);
                                        PSOC_PutChar(reverse[5] & 0x0F);
                                        PSOC_PutChar(reverse[6] & 0x0F);
                                        PSOC_PutChar(rventa.id_manguera[dir][0]);                    //producto
                                        PSOC_PutChar('N');                  //precio
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        PSOC_PutChar(0);
                                        x = timeout_psoc(3, 2);
                                    }                                    
                                    rventa.preset[dir][0] = 1;
                                    rventa.preset[dir][1] = reverse[0] & 0x0F;
                                    rventa.preset[dir][2] = reverse[1] & 0x0F;
                                    rventa.preset[dir][3] = reverse[2] & 0x0F;
                                    rventa.preset[dir][4] = reverse[3] & 0x0F;
                                    rventa.preset[dir][5] = reverse[4] & 0x0F;
                                    rventa.preset[dir][6] = reverse[5] & 0x0F;
                                    rventa.preset[dir][7] = reverse[6] & 0x0F; 
                                    count[dir][0] = 0;
                                    error_producto[dir][0] = 0;
                                    cambiar_img(img_subir_manija, dir);
                                    return f_menu_subir_manija; 
                                }
                                else
                                    {
                                        cambiar_img(0,dir);
                                        return f_lcd_pantallazo;
                                    }
                            break;                                
                        }
                    }
                }   
            }
            if((count[dir][0] > 40)&&(leer_img(dir) != img_menu_dvf)){
            	count[dir][0] = 0;
            	cambiar_img(img_menu_dvf,dir);
			}   
        }
        break;
        
        case f_menu_subir_manija:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
            if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        if(lcd_vp == botom_devolver){
                            if(rventa.tipo_venta[dir][0] != venta_contado){
                                lado.estado[dir][0] = w_cancelada;                   
                            }
                            x = 0;
                            while(x != 3){
                                PSOC_ClearRxBuffer();                                 
                                PSOC_PutChar(p_reset_psoc);
                                PSOC_PutChar(dir + lado.dir[0][0]);
                                PSOC_PutChar(2);
                                x = timeout_psoc(3, 2);
                            }                
                            cambiar_img(0, dir);    
                            return f_lcd_pantallazo;    
                        }
                    }
                }
            } 
            else 
            if(lado.estado[dir][0] == w_surtiendo){
                rventa.autorizada[dir][0] = 0;
                borrar_vp_lcd(dir);
                CyDelay(200);
                leer_fecha_lcd(dir);
                rventa.fecha_ini[dir][0] = fecha[0];
                rventa.fecha_ini[dir][1] = fecha[1];
                rventa.fecha_ini[dir][2] = fecha[2];
                rventa.fecha_ini[dir][3] = hora[2];
                rventa.fecha_ini[dir][4] = hora[1];
                rventa.fecha_ini[dir][5] = hora[0]; 
                if(rventa.tipo_venta[dir][0] == venta_contado){    
                    if((Hibrido == 1)&&(preset_rapido[dir][0] != 0)){
                        cambiar_img(img_subir_manija, dir);
                    }
                    else
                    {   
                        cambiar_img(img_datos_surt, dir);
                    }
                }else{
                    cambiar_img(img_subir_manija, dir);
                }
                return f_menu_datos_surt;               
            }else 
            if(error_producto[dir][0] == 1){
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                lado.estado[dir][0] = w_cancelada;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 6;
                cambiar_img(img_error_producto, dir);   
                return f_menu_esperando_reportar;                 
            }else 
            if(count[dir][0] >= 40){
                if(rventa.tipo_venta[dir][0] != venta_contado){
                    lado.estado[dir][0] = w_cancelada;                   
                } 
                x = 0;
                while(x != 3){
                    PSOC_ClearRxBuffer();                                 
                    PSOC_PutChar(p_reset_psoc);
                    PSOC_PutChar(dir + lado.dir[0][0]);
                    PSOC_PutChar(2);
                    x = timeout_psoc(3, 2);
                }                
                cambiar_img(0, dir);    
                return f_lcd_pantallazo;                 
            }
            }
        break;
        
        case f_menu_datos_surt:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                            
                        case vp_valor_placa: 
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.placa[dir][x] = buffer_lcd[dir][7+x];
                                }    
                            }
                            cambiar_img(img_datos_surt, dir);                                
                        break;  
                            
                        case vp_valor_cedula:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.cedula[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }
                            cambiar_img(img_datos_surt, dir);                                        
                        break;   
                            
                        case vp_valor_nit:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.nit[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }
                            cambiar_img(img_datos_surt, dir);                                        
                        break;    
                            
                        case vp_valor_km:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.km[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }
                            cambiar_img(img_datos_surt, dir);                                        
                        break;
                            
                        default:
                            cambiar_img(img_datos_surt, dir);
                        break;
                    }
                }   
            }
            else if(lado.estado[dir][0] == w_espera){
                if(rventa.tipo_venta[dir][0] != venta_contado){
                    lado.estado[dir][0] = w_cancelada;
                    cambiar_img(0, dir);    
                    return f_lcd_pantallazo;                    
                }                
                img_actual[dir] = leer_img(dir);
                CyDelay(200);
                if(img_actual[dir] == img_datos_surt){                  
                    cambiar_img(0, dir);    
                    return f_lcd_pantallazo;
                }
                if(img_actual[dir] == img_subir_manija){                  
                    cambiar_img(0, dir);    
                    return f_lcd_pantallazo;
                } 
            }
            else if(rventa.autorizada[dir][0] == v_llego_ok){
                CyDelay(200);
                img_actual[dir] = leer_img(dir);
                if(img_actual[dir] == 0x18){
                    cambiar_img(img_datos_surt, dir);
                }
                else {
                    if(img_actual[dir] == 40){
                      img_actual[dir] = img_datos_surt;
                    }
                    if((img_actual[dir] == img_datos_surt) || (img_actual[dir] == img_subir_manija)){
                            if ((((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado)&&(rventa.tipo_venta[dir][0] == venta_contado)){
                                cambiar_img(img_datos_venta_2, dir);
                            }
                            else{
                                cambiar_img(img_datos_venta, dir);
                            }
                            x = f_datos_venta;
                            count[dir][0] = 0;
                        if((rventa.tipo_venta[dir][0] == venta_Fidelizado) && (vol_redimir > 0)){
                            cambiar_img(img_redimir, dir);
                            autoriza_fiel[dir] = 0;
                            x = f_lcd_redimir;
                        }else{
                            autoriza_fiel[dir] = 0;
                        }

                        CyDelay(100);
                        leer_fecha_lcd(dir);
                        rventa.fecha_fin[dir][0] = fecha[0];
                        rventa.fecha_fin[dir][1] = fecha[1];
                        rventa.fecha_fin[dir][2] = fecha[2];
                        rventa.fecha_fin[dir][3] = hora[2];
                        rventa.fecha_fin[dir][4] = hora[1];
                        rventa.fecha_fin[dir][5] = hora[0];                    
                        rventa.autorizada[dir][0] = 0;
                        return x;
                    }
                }
            }
        }
        break;
        
        case f_datos_venta:
        if(1){
            if (Hibrido == 1){
                if (preset_rapido[dir][0] != 0){
                    preset_rapido[dir][0] = 0; 
                    rventa.venta_acumulada_cargada[dir][0] = 0;
                    lado.estado[dir][0] = w_venta;
                    count[dir][0] = 0;
                    prox_flujo[dir][0] = f_lcd_pantallazo;
                    prox_img[dir][0] = 0;
                    timeout_lcd[dir][0] = 80;
                    rventa.print[dir][0] = 0;
                    rventa.ventas_acumuladas[dir][0]++;
                    rventa.autorizada[dir][0] = v_envio_no;
                    cambiar_img(img_esperando, dir);    
                    return f_menu_esperando_reportar;                                 
                }
            }
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    //Pasa a imprimir
                        if(lcd_vp == b_Efectivo){
                            if((rventa.tipo_venta[dir][0] == venta_contado) || (rventa.tipo_venta[dir][0] == venta_Fidelizado)){
                                count[dir][0] = 0;
                                cambiar_img(img_no_recibos, dir);   
                                return f_no_recibos;
                            }
                            else{ //Credito
                                rventa.venta_acumulada_cargada[dir][0] = 0;
                                lado.estado[dir][0] = w_venta;
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 80;
                                rventa.print[dir][0] = 2;
                                rventa.ventas_acumuladas[dir][0]++;
                                rventa.autorizada[dir][0] = v_envio_no;
                                cambiar_img(img_esperando, dir);    
                                return f_menu_esperando_reportar;                                 
                            }
                        } 
                        else if (lcd_vp == b_Acumulacion){ 
                            if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){ // La acumulación de puntos colombia solo es para Esso y Mobil
                                if(rventa.tipo_venta[dir][0] == venta_contado){ // La acumulación de puntos colombia solo es para contados
                                    count[dir][0] = 0;
                                    PCol.Tipo[dir][0] = 'A'; // Tipo transacción Acumulación
                                    cambiar_img(img_TipoDoc_Pcol, dir);   
                                    return f_tipodocumenPCol;
                                }
                                else{
                                    break; // me salgo no Esso Mobil
                                }
                            }
                            else{
                                break; // me salgo no Contado
                            }
                            
                        }
                        else if (lcd_vp == b_FormasPago){
                            if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){ // Si es o no Esso Mobil Esta o no Puntos Colombia
                                count[dir][0] = 0;
                                PCol.Tipo[dir][0] = 'R'; // Redención
                                cambiar_img(img_formas_pago2, dir);
                                return f_formas_pago;
                            }
                            else{
                                count[dir][0] = 0;
                                cambiar_img(img_formas_pago, dir);   
                                return f_formas_pago;
                            }
                                  
                        }
                    }
                    else{        
                            switch(pos_vp){                            
                                case vp_valor_placa: 
                                    y = (buffer_lcd[dir][6] * 2)-2;
                                    for(x=0;x<y;x++){
                                        if(buffer_lcd[dir][7+x] != 0xFF){
                                            rventa.placa[dir][x] = buffer_lcd[dir][7+x];
                                        }
                                    }
                                    if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){
                                        cambiar_img(img_datos_venta_2, dir);
                                    }
                                    else{
                                        cambiar_img(img_datos_venta, dir);
                                    }                               
                                break;  
                                    
                                case vp_valor_cedula:
                                    y = (buffer_lcd[dir][6] * 2)-2;
                                    for(x=0;x<y;x++){
                                        if(buffer_lcd[dir][7+x] != 0xFF){
                                            rventa.cedula[dir][x] = buffer_lcd[dir][7+x];
                                        }
                                    }
                                    if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){
                                        cambiar_img(img_datos_venta_2, dir);
                                    }
                                    else{
                                        cambiar_img(img_datos_venta, dir);
                                    } 
                                break;   
                                    
                                case vp_valor_nit:
                                    y = (buffer_lcd[dir][6] * 2)-2;
                                    for(x=0;x<y;x++){
                                        if(buffer_lcd[dir][7+x] != 0xFF){
                                            rventa.nit[dir][x] = buffer_lcd[dir][7+x];
                                        }
                                    }
                                    cambiar_img(img_datos_venta, dir);
                                break;    
                                    
                                case vp_valor_km:
                                    y = (buffer_lcd[dir][6] * 2)-2;
                                    for(x=0;x<y;x++){
                                        if(buffer_lcd[dir][7+x] != 0xFF){
                                            rventa.km[dir][x] = buffer_lcd[dir][7+x];
                                        }
                                    }
                                    if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){
                                        cambiar_img(img_datos_venta_2, dir);
                                    }
                                    else{
                                        cambiar_img(img_datos_venta, dir);
                                    } 
                                break;                                
                            }
                        }
                    }   
                }
            else 
            if(rventa.tipo_venta[dir][0] == venta_contado){
                if(count[dir][0] >= 120){
                    read_vp(dir, vp_valor_placa, 3);
                    for(x=0;x<=5;x++){
                        if((valor_vp[x] == 0xFF) || (valor_vp[x] == 0)){
                            break;
                        }
                        rventa.placa[dir][x] = valor_vp[x];
                    }
                    rventa.print[dir][0] = '0';
                    rventa.venta_acumulada_cargada[dir][0] = 0;                        
                    lado.estado[dir][0] = w_venta;
                    count[dir][0] = 0;
                    prox_flujo[dir][0] = f_lcd_pantallazo;
                    prox_img[dir][0] = 0;
                    timeout_lcd[dir][0] = 80;
                    rventa.ventas_acumuladas[dir][0]++;
                    rventa.autorizada[dir][0] = v_envio_no;
                    cambiar_img(img_esperando, dir);    
                    return f_menu_esperando_reportar;                                  
                }
                else{
                    //2 minutos de espera para la venta credito
                    if(count[dir][0] >= 240){
                        read_vp(dir, vp_valor_placa, 3);
                        for(x=0;x<=5;x++){
                            if((valor_vp[x] == 0xFF) || (valor_vp[x] == 0)){
                                break;
                            }
                        rventa.placa[dir][x] = valor_vp[x];
                        }
                         if (Hibrido == 1){
                            rventa.print[dir][0] = '0';
                        }
                        else{
                            rventa.print[dir][0] = '2';
                        }
                        rventa.venta_acumulada_cargada[dir][0] = 0;                        
                        lado.estado[dir][0] = w_venta;
                        count[dir][0] = 0;
                        prox_flujo[dir][0] = f_lcd_pantallazo;
                        prox_img[dir][0] = 0;
                        timeout_lcd[dir][0] = 80;
                        rventa.ventas_acumuladas[dir][0]++;
                        rventa.autorizada[dir][0] = v_envio_no;
                        cambiar_img(img_esperando, dir);    
                        return f_menu_esperando_reportar;
                    }
                }
            }
        }
        break;
            
        // Formas de pago 1.0.3.2
        
        case f_formas_pago:
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        if(lcd_vp == b_Efectivo){ // 1.0.3.2 Efectivo OK
                            if((rventa.tipo_venta[dir][0] == venta_contado) || (rventa.tipo_venta[dir][0] == venta_Fidelizado)){
                                count[dir][0] = 0;
                                rventa.forma_pago[dir][0] = 6; 
                                cambiar_img(img_no_recibos, dir);   
                                return f_no_recibos;
                            }
                            else{
                                rventa.venta_acumulada_cargada[dir][0] = 0;
                                rventa.forma_pago[dir][0] = 6; 
                                lado.estado[dir][0] = w_venta;
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 80;
                                rventa.print[dir][0] = 2;
                                rventa.ventas_acumuladas[dir][0]++;
                                rventa.autorizada[dir][0] = v_envio_no;
                                cambiar_img(img_esperando, dir);    
                                return f_menu_esperando_reportar;                                 
                            }
                        }
                        if(lcd_vp == b_Datafono){ // 1.0.3.2
                        	rventa.forma_pago[dir][0] = 0; 
                            break;
                        }
                        if(lcd_vp == b_PuntosCol){ // 1.0.3.2
                            if(rventa.tipo_venta[dir][0] == venta_contado){ // La acumulación de puntos colombia solo es para contados
                                count[dir][0] = 0;
                                rventa.forma_pago[dir][0] = 27; 
                                PCol.Tipo[dir][0] = 'R'; // Tipo transacción Acumulación
                                cambiar_img(img_TipoDoc_Pcol, dir);   
                                return f_tipodocumenPCol;
                            }
                        }
                        if(lcd_vp == b_Sodexo){
                        	rventa.forma_pago[dir][0] = 5; 
                            break;
                        }
                    }
                }
            }
        break;
        
        case f_tipodocumenPCol:
            
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    //Pasa a imprimir
                        if(lcd_vp == 0xCC){
                            PCol.TipoDoc[dir][0] = 'C';
                            cambiar_img(img_ce_pcolombia, dir);   
                            return f_cedula_pcolombia; 
                        }
                        if(lcd_vp == 0xCE){
                            PCol.TipoDoc[dir][0] = 'E';
                            cambiar_img(img_ce_pcolombia, dir);   
                            return f_cedula_pcolombia; 
                        }
                    }
                }
            }
               
        break;
            
        case f_cedula_pcolombia: // 1.0.3.2
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case 0x524: // Cedula Puntos Colombia
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<10;x++){
                                PCol.Cedula[dir][x] = 0; // Limpio antes de meter alguna cosa  
                                if(PCol.Cedula[dir][x] == 0xFF){
                                    PCol.Cedula[dir][x] = 0;
                                }
                            }
                            if (y<=10){
                                for(x=0;x<y;x++){
                                    PCol.Cedula[dir][x] = buffer_lcd[dir][7+x];
                                    if(PCol.Cedula[dir][x] == 0xFF){
                                        PCol.Cedula[dir][x] = 0;
                                    }
                                    rventa.id[dir][x]=PCol.Cedula[dir][x];
                                }
                                PCol.Ok[dir][0]= 0;
                                lado.estado[dir][0] = w_Aut_Pcol;
                                cambiar_img(img_cancelar_espera,dir);
                                count[dir][0] = 0;
                                timeout_lcd[dir][0] = 0xFF;
                                return f_esperar_cedula_pcol;
                                
                            }
                        }
                    }
                }   
        }
        break;
        
        case f_esperar_cedula_pcol: // 1.0.3.2
            if(1){
               if(PCol.Ok[dir][0] != 0){
                    limpiarLCD(dir);
                    if(dir == 0){
                        LCD1_PutChar(0x5A);
                        LCD1_PutChar(0xA5);
                        LCD1_PutChar(0x67);
                        LCD1_PutChar(0x82);
                        LCD1_PutChar(0x02);
                        LCD1_PutChar(0x54);
                        for(x=0;x<=99;x++){
                          LCD1_PutChar(msn_ldc[dir][x]);  
                        }
                    }else{
                        LCD2_PutChar(0x5A);
                        LCD2_PutChar(0xA5);
                        LCD2_PutChar(0x67);
                        LCD2_PutChar(0x82);
                        LCD2_PutChar(0x02);
                        LCD2_PutChar(0x54);
                        for(x=0;x<=99;x++){
                          LCD2_PutChar(msn_ldc[dir][x]);  
                        }                
                    }
                    count[dir][0] = 0;
                    
                    if(PCol.Tipo[dir][0] == 'R'){
                        timeout_lcd[dir][0] =  10;
                    }
                    else {
                        timeout_lcd[dir][0] =  5;
                    }
                    cambiar_img(img_mensaje_Pcol,dir);
                    return f_mensaje_Pcol;
                }
                ok = hay_datos_lcd(dir);
                if(ok == 1){
                    if(buffer_lcd[dir][3] == lcd_read_vp){
                        pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                        lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                        if(lcd_vp == 0xF4){
                			count[dir][0] = 0;
                            cambiar_img(img_datos_venta_2, dir);
                            return f_datos_venta;                            
                        }
                    }
                }
                if (count[dir][0]> timeout_lcd[dir][0]){
                    count[dir][0] = 0;
                    cambiar_img(img_datos_venta_2, dir);
                    return f_datos_venta; 
                }
            }
        break;
            
        case f_mensaje_Pcol: // 1.0.3.2
            if(1){
                if (count[dir][0]> timeout_lcd[dir][0]){ // Tiempo para mostrar lo que tengo en pantalla Oks
                    if(PCol.Ok[dir][0]== 1){ //Aproved
                        if (PCol.Tipo[dir][0] == 'A'){
                            cambiar_img(img_formas_pago,dir);
                            rventa.tipo_id[dir][0] = 'E';
                            for( x =0; x<10;x++){
                                rventa.nit[dir][x] = PCol.Cedula[dir][x];
                            }
                            return f_formas_pago;
                        }
                        if (PCol.Tipo[dir][0] == 'R'){ // Ir a Contraseña
                            for( x =0; x<10;x++){
                                rventa.nit[dir][x] = PCol.Cedula[dir][x];
                            }
                            rventa.tipo_id[dir][0] = 'F';
                            cambiar_img(img_con_pcolombia,dir);
                            PCol.Ok[dir][0] = 0;
                            return f_contra_pcolombia;
                        }
                        break;
                    }
                    if(PCol.Ok[dir][0]== 2){ //Not Aproved
                        cambiar_img(img_datos_venta_2,dir);
                        return f_datos_venta;
                    }
                }
            }
        break;
        
        case f_contra_pcolombia: // 1.0.3.2
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case 0x52E: // Cedula Puntos Colombia
                            y = (buffer_lcd[dir][6] * 2)-2;
                            if (y<=10){
                                for (x=0;x<10;x++){
                                    PCol.Contra[dir][x] = 0;
                                }
                                for(x=0;x<y;x++){
                                    PCol.Contra[dir][x] = buffer_lcd[dir][7+x];
                                    if(PCol.Contra[dir][x] == 0xFF){
                                        PCol.Contra[dir][x] = 0;
                                    }
                                }
                                
                            for (x=0;x<=15;x++){
                                rventa.boucher_forma_pago[dir][x]=0;
                            }
                            for (x=0;x<10;x++){
                                write_vp(dir,1326+x,0);
                                rventa.boucher_forma_pago[dir][x] = PCol.Contra[dir][x];
                            }
                            
                            ///////////////////////////////////////////////////////////////////////
                            // Escribir el Valor Disponible y el de Compra en 
                            uint8 DineroMensaje[9] = {' ',' ',' ',' ',' ',' ',' ',' ',' '};
                            write_vp_text(dir,0x0338,DineroMensaje,9);
                            write_vp_text(dir,0x0328,DineroMensaje,9);
                            
                            //Dinero Venta
                            for (x=0;x<9;x++){
                                if(rventa.dinero[dir][x]<0x30){
                                    DineroMensaje[x] = rventa.dinero[dir][8-x] + 0x30;
                                }
                                else{
                                    DineroMensaje[x] = rventa.dinero[dir][8-x];
                                }
                            }
                            //Procedimiento Dinero Humano
                            if(1){
                                uint8 Presetmsn[13] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
                                uint8 bandvalor = 0;
                                uint8 flag = 0;
                                for (x=0;x<=7;x++){
                                    if((DineroMensaje[x] != '0') && (flag  == 0)){
                                        bandvalor = 9-x;
                                        flag = 1;
                                    }
                                    if (flag == 0){
                                        DineroMensaje[x] = ' ';
                                    }
                                }
                                
                                Presetmsn[0] = '$';
                                Presetmsn[1] = ' ';
                                Presetmsn[2] = DineroMensaje[0];
                                Presetmsn[3] = DineroMensaje[1];
                                Presetmsn[4] = DineroMensaje[2];
                                Presetmsn[5] = ' ';
                                Presetmsn[6] = DineroMensaje[3];
                                Presetmsn[7] = DineroMensaje[4];
                                Presetmsn[8] = DineroMensaje[5];
                                Presetmsn[9] = ' ';
                                Presetmsn[10] = DineroMensaje[6];
                                Presetmsn[11] = DineroMensaje[7];
                                Presetmsn[12] = DineroMensaje[8];
     
                                if (bandvalor > 6){
                                    Presetmsn[5] = '.';
                                    Presetmsn[9] = '.';
                                    }
                                else if (bandvalor > 3){
                                    Presetmsn[9] = '.';
                                }
                                for (x=1;x<12;x++){
                                    if(Presetmsn[2] != ' '){
                                        x = 100;
                                    }
                                    else {
                                        Presetmsn[2]  = Presetmsn[3];
                                        Presetmsn[3]  = Presetmsn[4];
                                        Presetmsn[4]  = Presetmsn[5];
                                        Presetmsn[5]  = Presetmsn[6];
                                        Presetmsn[6]  = Presetmsn[7];
                                        Presetmsn[7]  = Presetmsn[8];
                                        Presetmsn[8]  = Presetmsn[9];
                                        Presetmsn[9]  = Presetmsn[10];
                                        Presetmsn[10] = Presetmsn[11];
                                        Presetmsn[11] = Presetmsn[12]; 
                                        Presetmsn[12] = ' ';
                                    }
                                }
                            
                            write_vp_text(dir,0x0338,Presetmsn,13);
                            }
                            //Dinero Disponible
                            for (x=0;x<8;x++){
                                    DineroMensaje[x] = PCol.Disponible[dir][7-x];
                            }
                            //Procedimiento Dinero Humano
                            if(1){
                                uint8 Presetmsn[13] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
                                uint8 bandvalor = 0;
                                uint8 flag = 0;
                                for (x=0;x<=7;x++){
                                    if((DineroMensaje[x] != '0') && (flag  == 0)){
                                        bandvalor = 8-x;
                                        flag = 1;
                                    }
                                    if (flag == 0){
                                        DineroMensaje[x] = ' ';
                                    }
                                }
                                
                                Presetmsn[0] = '$';
                                Presetmsn[1] = ' ';
                                Presetmsn[2] = ' ';
                                Presetmsn[3] = DineroMensaje[0];
                                Presetmsn[4] = DineroMensaje[1];
                                Presetmsn[5] = ' ';
                                Presetmsn[6] = DineroMensaje[2];
                                Presetmsn[7] = DineroMensaje[3];
                                Presetmsn[8] = DineroMensaje[4];
                                Presetmsn[9] = ' ';
                                Presetmsn[10] = DineroMensaje[5];
                                Presetmsn[11] = DineroMensaje[6];
                                Presetmsn[12] = DineroMensaje[7];
     
                                if (bandvalor > 6){
                                    Presetmsn[5] = '.';
                                    Presetmsn[9] = '.';
                                    }
                                else if (bandvalor > 3){
                                    Presetmsn[9] = '.';
                                }
                                for (x=1;x<12;x++){
                                    if(Presetmsn[2] != ' '){
                                        x = 100;
                                    }
                                    else {
                                        Presetmsn[2]  = Presetmsn[3];
                                        Presetmsn[3]  = Presetmsn[4];
                                        Presetmsn[4]  = Presetmsn[5];
                                        Presetmsn[5]  = Presetmsn[6];
                                        Presetmsn[6]  = Presetmsn[7];
                                        Presetmsn[7]  = Presetmsn[8];
                                        Presetmsn[8]  = Presetmsn[9];
                                        Presetmsn[9]  = Presetmsn[10];
                                        Presetmsn[10] = Presetmsn[11];
                                        Presetmsn[11] = Presetmsn[12]; 
                                        Presetmsn[12] = ' ';
                                    }
                                }
                            
                            write_vp_text(dir,0x0328,Presetmsn,13);
                            }
                            
                            ///////////////////////////////////////////////////////////////////////
                            cambiar_img(img_Definir_tRed,dir);
                            return f_esperar_tRede;
                            
                        }
                    }
                }
            }
        }
        break;
        
        case f_esperar_tRede: // 1.0.3.2
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(lcd_vp){
                        case 0x5474:
                            // Hacer las comparaciones del dinero
                            //rventa.dinero[dir][x];
                            //PCol.Disponible[dir][x];
                            for(x=0;x<8;x++){
                                PCol.Redimible[dir][x] = rventa.dinero[dir][7-x];
                            }
                            // Traduccion a enteros
                            PCol.TotPar[dir][0] = 0;
                            red = PCol.Redimible[dir][7]+(PCol.Redimible[dir][6]*10)+(PCol.Redimible[dir][5]*100)+(PCol.Redimible[dir][4]*1000)+(PCol.Redimible[dir][3]*10000)+(PCol.Redimible[dir][2]*100000)+(PCol.Redimible[dir][1]*1000000)+(PCol.Redimible[dir][0]*10000000);
                            dis = (PCol.Disponible[dir][0]-0x30) +((PCol.Disponible[dir][1]-0x30)*10) +((PCol.Disponible[dir][2]-0x30)*100) +((PCol.Disponible[dir][3]-0x30)*1000) +((PCol.Disponible[dir][4]-0x30)*10000) +((PCol.Disponible[dir][5]-0x30)*100000) +((PCol.Disponible[dir][6]-0x30)*1000000) +((PCol.Disponible[dir][7]-0x30)*10000000);//realizar comparacion
                            
                            if(dis >= red){ // Si si todo bien
                                PCol.Ok[dir][0] = 1;
                                timeout_lcd[dir][0] = 10;
                                for (x=0;x<100;x++){
                                    if(x<17){
                                        msn_ldc[dir][x] = Mensaje_Ok[x];
                                    }
                                    else {
                                        msn_ldc[dir][x] = ' ';
                                    }
                                }            
                                for(x=0;x<7;x++){
                    			   rventa.valor_forma_pago[dir][x] = rventa.dinero[dir][6-x] + 0x30;
                                }
                            }
                            else{ // si no no autorizo
                                PCol.Ok[dir][0] = 2;
                                timeout_lcd[dir][0] = 20;
                                // Mensaje Error
                                for (x=0;x<100;x++){
                                    if(x<44){
                                        msn_ldc[dir][x] = Mensaje_err[x];
                                    }
                                    else {
                                        msn_ldc[dir][x] = ' ';
                                    }
                                }
                                
                                
                            }
                            count[dir][0] = 0; // contador en 0
                            limpiarLCD(dir);
                            if(dir == 0){ // escribo el mensaje en pantalla
                                LCD1_PutChar(0x5A);
                                LCD1_PutChar(0xA5);
                                LCD1_PutChar(0x67);
                                LCD1_PutChar(0x82);
                                LCD1_PutChar(0x02);
                                LCD1_PutChar(0x54);
                                for(x=0;x<=99;x++){
                                  LCD1_PutChar(msn_ldc[dir][x]);  
                                }
                            }
                            else{
                                LCD2_PutChar(0x5A);
                                LCD2_PutChar(0xA5);
                                LCD2_PutChar(0x67);
                                LCD2_PutChar(0x82);
                                LCD2_PutChar(0x02);
                                LCD2_PutChar(0x54);
                                for(x=0;x<=99;x++){
                                  LCD2_PutChar(msn_ldc[dir][x]);  
                                }                
                            }
                            //mostrar pantalla de mensajee ir al menu indicado.
                            cambiar_img(img_mensaje_Pcol,dir);
                            return f_confirma_Pcol;
                        break;  
                        
                        case 0x5070:
                            cambiar_img(img_Dinero_Pcol,dir);
                            return f_Parcial_RedenPcol;
                        break;
                            
                        }
                    }
                }
            }
        
        break;
        
        case f_confirma_Pcol: // 1.0.3.2
            if(1){
                
                if (count[dir][0]> timeout_lcd[dir][0]){ // Tiempo para mostrar lo que tengo en pantalla Oks
                    if(PCol.Ok[dir][0]== 1){ //Aproved
                        lado.estado[dir][0] = w_fpago;
                        cambiar_img(img_esperando, dir);    
                        return f_esperar_fp_pcol;
                    }
                    if(PCol.Ok[dir][0]== 2){ //Not Aproved
                        cambiar_img(img_Definir_tRed,dir);
                        return f_esperar_tRede;
                    }
                    if(PCol.Ok[dir][0]== 3){ //Not Aproved
                        
                        count[dir][0] = 0;
                        cambiar_img(img_datos_venta_2,dir);
                        return f_datos_venta;
                    }
                }
            }
        break;
        
        case f_Parcial_RedenPcol:
            if(1){
                ok = hay_datos_lcd(dir);
                if(ok == 1){
                    if(buffer_lcd[dir][3] == lcd_read_vp){
                        pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                        lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                        switch(pos_vp){                                                          
                            case 0x52E: // Cedula Puntos Colombia
                                y = (buffer_lcd[dir][6] * 2)-2;
                                if (y<=10){
                                    for (x=0;x<8;x++){
                                        PCol.Dinero[dir][x] = 0;
                                    }
                                    for(x=0;x<y;x++){
                                        PCol.Dinero[dir][x] = buffer_lcd[dir][7+x];
                                        if(PCol.Dinero[dir][x] == 0xFF){
                                            PCol.Dinero[dir][x] = 0;
                                        }
                                    }
                                    for(x = 0; x<8;x++){
                                        if (PCol.Dinero[dir][7] == 0){
                                            PCol.Dinero[dir][7] = PCol.Dinero[dir][6];
                                            PCol.Dinero[dir][6] = PCol.Dinero[dir][5];
                                            PCol.Dinero[dir][5] = PCol.Dinero[dir][4];
                                            PCol.Dinero[dir][4] = PCol.Dinero[dir][3];
                                            PCol.Dinero[dir][3] = PCol.Dinero[dir][2];
                                            PCol.Dinero[dir][2] = PCol.Dinero[dir][1];
                                            PCol.Dinero[dir][1] = PCol.Dinero[dir][0];
                                            PCol.Dinero[dir][0] = 0x30;
                                        }
                                    }
                                    
                                    for(x=0;x<8;x++){
                                        PCol.Redimible[dir][x] = rventa.dinero[dir][7-x];
                                    }
                                    // Traduccion a enteros
                                    parc = (PCol.Dinero[dir][7]-0x30) + (PCol.Dinero[dir][6]-0x30)*10 + (PCol.Dinero[dir][5]-0x30)*100 + (PCol.Dinero[dir][4]-0x30)*1000 + (PCol.Dinero[dir][3]-0x30)*10000 + (PCol.Dinero[dir][2]-0x30)*100000 +(PCol.Dinero[dir][1]-0x30)*1000000+ (PCol.Dinero[dir][0]-0x30)*1000000;
                                    red = PCol.Redimible[dir][7]+(PCol.Redimible[dir][6]*10)+(PCol.Redimible[dir][5]*100)+(PCol.Redimible[dir][4]*1000)+(PCol.Redimible[dir][3]*10000)+(PCol.Redimible[dir][2]*100000)+(PCol.Redimible[dir][1]*1000000)+(PCol.Redimible[dir][0]*10000000);
                                    dis = (PCol.Disponible[dir][0]-0x30) +((PCol.Disponible[dir][1]-0x30)*10) +((PCol.Disponible[dir][2]-0x30)*100) +((PCol.Disponible[dir][3]-0x30)*1000) +((PCol.Disponible[dir][4]-0x30)*10000) +((PCol.Disponible[dir][5]-0x30)*100000) +((PCol.Disponible[dir][6]-0x30)*1000000) +((PCol.Disponible[dir][7]-0x30)*10000000);//realizar comparacion
                                    PCol.TotPar[dir][0] = 1;
                                    //Comparo con lo que se puede
                                    
                                    if((dis >= parc) && (parc <= red)){ // Si si todo bien
                                        PCol.Ok[dir][0] = 1;
                                        timeout_lcd[dir][0] = 10;
                                        for (x=0;x<100;x++){
                                            if(x<17){
                                                msn_ldc[dir][x] = Mensaje_Ok[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        for(x=0;x<=6;x++){
                            			   rventa.valor_forma_pago[dir][x] = PCol.Dinero[dir][x+1];
                                        }
                                    }
                                    else{ // si no no autorizo
                                        PCol.Ok[dir][0] = 2;
                                        timeout_lcd[dir][0] = 20;
                                        // Mensaje Error
                                        for (x=0;x<100;x++){
                                            if(x<44){
                                                msn_ldc[dir][x] = Mensaje_err[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        
                                        
                                    }
                                    count[dir][0] = 0; // contador en 0
                                    limpiarLCD(dir);
                                    if(dir == 0){ // escribo el mensaje en pantalla
                                        LCD1_PutChar(0x5A);
                                        LCD1_PutChar(0xA5);
                                        LCD1_PutChar(0x67);
                                        LCD1_PutChar(0x82);
                                        LCD1_PutChar(0x02);
                                        LCD1_PutChar(0x54);
                                        for(x=0;x<=99;x++){
                                          LCD1_PutChar(msn_ldc[dir][x]);  
                                        }
                                    }
                                    else{
                                        LCD2_PutChar(0x5A);
                                        LCD2_PutChar(0xA5);
                                        LCD2_PutChar(0x67);
                                        LCD2_PutChar(0x82);
                                        LCD2_PutChar(0x02);
                                        LCD2_PutChar(0x54);
                                        for(x=0;x<=99;x++){
                                          LCD2_PutChar(msn_ldc[dir][x]);  
                                        }                
                                    }
                                    //mostrar pantalla de mensajee ir al menu indicado.
                                    timeout_lcd[dir][0] = 10;
                                    cambiar_img(img_mensaje_Pcol,dir);
                                    return f_confirma_Pcol;
                                    //
                                }
                            break;
                        }
                    }
                }
            }
        break;
            
            
        case f_esperar_fp_pcol: // 1.0.3.2
        if(1){
            if(rventa.forma_pago[dir][0] == 0xFF){ // Forma Pago Autorizada Bro!          
                if(PCol.TotPar[dir][0] == 0){
                    cambiar_img(img_no_recibos,dir);
                    count[dir][0]=0;
                    return f_no_recibos;
                }
                else{
                    if(red == parc){
                        cambiar_img(img_no_recibos,dir);
                        count[dir][0]=0;
                        return f_no_recibos;
                    }
                    else{
                        cambiar_img(img_formas_pago,dir);
                        count[dir][0]=0;
                        return f_formas_pago;
                    }
                
                }
                
            }else 
            if(count[dir][0] >= 120){               // se acabo el tiempo de espera Bro!
                lado.estado[dir][0] = w_surtiendo;
                count[dir][0] = 0;
                cambiar_img(img_datos_venta_2,dir);
                return f_datos_venta;
            }else
            if(rventa.forma_pago[dir][0] == 0xFE){ // Forma Pago no Autorizada Bro!
                lado.estado[dir][0] = w_surtiendo;
                limpiarLCD(dir);
                if(dir == 0){ // escribo el mensaje en pantalla
                    LCD1_PutChar(0x5A);
                    LCD1_PutChar(0xA5);
                    LCD1_PutChar(0x67);
                    LCD1_PutChar(0x82);
                    LCD1_PutChar(0x02);
                    LCD1_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD1_PutChar(msn_ldc[dir][x]);  
                    }
                }
                else{
                    LCD2_PutChar(0x5A);
                    LCD2_PutChar(0xA5);
                    LCD2_PutChar(0x67);
                    LCD2_PutChar(0x82);
                    LCD2_PutChar(0x02);
                    LCD2_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD2_PutChar(msn_ldc[dir][x]);  
                    }                
                }
                //mostrar pantalla de mensajee ir al menu indicado.
                count[dir][0] = 0;
                timeout_lcd[dir][0] = 10;
                PCol.Ok[dir][0] = 3;
                cambiar_img(img_mensaje_Pcol,dir);
                return f_confirma_Pcol;
                
            }
        }
        break;
        
        // Fin de Venta
        case f_no_recibos:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 1){
                        read_vp(dir, vp_valor_placa, 3);
                        for(x=0;x<=5;x++){
                            if((valor_vp[x] == 0xFF) || (valor_vp[x] == 0)){
                                break;
                            }
                            rventa.placa[dir][x] = valor_vp[x];
                        }
                        rventa.print[dir][0] = buffer_lcd[dir][8];
                        rventa.venta_acumulada_cargada[dir][0] = 0;                        
                        lado.estado[dir][0] = w_venta;
                        count[dir][0] = 0;
                        prox_flujo[dir][0] = f_lcd_pantallazo;
                        prox_img[dir][0] = 0;
                        timeout_lcd[dir][0] = 80;
                        rventa.ventas_acumuladas[dir][0]++;
                        rventa.autorizada[dir][0] = v_envio_no;
                        cambiar_img(img_esperando, dir);    
                        return f_menu_esperando_reportar;                            
                    }
                }   
            }else 
            if(count[dir][0] >= 20){
                read_vp(dir, vp_valor_placa, 3);
                for(x=0;x<=5;x++){
                    if((valor_vp[x] == 0xFF) || (valor_vp[x] == 0)){
                        break;
                    }
                    rventa.placa[dir][x] = valor_vp[x];
                }
                rventa.print[dir][0] = '0';
                rventa.venta_acumulada_cargada[dir][0] = 0;                        
                lado.estado[dir][0] = w_venta;
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 80;
                rventa.ventas_acumuladas[dir][0]++;
                rventa.autorizada[dir][0] = v_envio_no;
                cambiar_img(img_esperando, dir);    
                return f_menu_esperando_reportar;                                  
            }                       
        }
        break;
            
        case f_menu_esperando_reportar:
        if(1){
            if((count[dir][0] >= timeout_lcd[dir][0]) || (rventa.autorizada[dir][0] == v_envio_ok)){
                if(rventa.autorizada[dir][0] == v_envio_no){
                    rventa.autorizada[dir][0] = 0;
                    x = ((EEPROM_ReadByte(224 + (dir*2))<<8)&0xFF00) | EEPROM_ReadByte(225 + (dir*2));
                    if(x < rventa.ventas_acumuladas[dir][0]){
                        EEPROM_WriteByte((rventa.ventas_acumuladas[dir][0]>>8),(224 + (dir*2)));
                        EEPROM_WriteByte((rventa.ventas_acumuladas[dir][0] & 0xFF),(225 + (dir*2)));
                        guardar_venta(dir);
                        imprimir_venta_fuera(dir);
                        Write_EEPROM(aux_save_venta,((10000+(dir*120000))*rventa.ventas_acumuladas[dir][0]),149);
                        img_actual[dir] = leer_img(dir);
                        return f_lcd_pantallazo; 
                    }
                }
                if((rventa.tipo_venta[dir][0] == venta_consulta_puntos) &&( rventa.autorizada[dir][0] == 8)){
                    lado.estado[dir][0] = w_cancelada;                   
                    x = 0;
                    while(x != 3){
                        PSOC_ClearRxBuffer();                                 
                        PSOC_PutChar(p_reset_psoc);
                        PSOC_PutChar(dir + lado.dir[0][0]);
                        PSOC_PutChar(2);
                        x = timeout_psoc(3, 2);
                    }
                }
                rventa.autorizada[dir][0] = 0;
                Bandera[dir][0] = 0;
                cambiar_img(prox_img[dir][0], dir);
                return prox_flujo[dir][0];                
            }                    
        }
        break; 
        
        //CALIBRACION

        case clave_calibracion:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_clave_t:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                rventa.id[dir][x] = buffer_lcd[dir][7+x];
                            } 
                            count[dir][0] = 0;                            
                            lado.estado[dir][0] = w_pet_autoriza;
                            cambiar_img(img_esperando, dir);    
                            return f_menu_esperar_auto;
                        break;                                                              
                    }
                    if(lcd_vp == botom_devolver){
                        if(Hibrido == 1){
                                cambiar_img(11, dir); 
                        }else{
                            cambiar_img( img_autos, dir); 

                        } 
                        return f_lcd_menu_inicial;                            
                    }
                }                  
            }    }
        break;   
        
        //////////////////////////////////////////////////////////////
            

        case f_lcd_redimir:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_si:
                                if (Bandera[dir][0] == 0){
                                    for(uint8 x = 0;x<10;x++){
                                    Identificacion_aux[dir][x] = rventa.id[dir][x];
                                    }
                                    Tipo_aux[dir][0] = rventa.tipo_id[dir][0];
                                    Bandera[dir][0] = 1;
                                }
                                
                                rventa.volumen_redimido[dir][0] = (((((vol_redimir%1000000)%100000)%10000)%1000)%100)%10;  
                                rventa.volumen_redimido[dir][1] = (((((vol_redimir%1000000)%100000)%10000)%1000)%100)/10; 
                                rventa.volumen_redimido[dir][2] = ((((vol_redimir%1000000)%100000)%10000)%1000)/100; 
                                rventa.volumen_redimido[dir][3] = (((vol_redimir%1000000)%100000)%10000)/1000;
                                rventa.volumen_redimido[dir][4] = ((vol_redimir%1000000)%100000)/10000; 
                                rventa.volumen_redimido[dir][5] = (vol_redimir%1000000)/100000; 
                                rventa.volumen_redimido[dir][6] = vol_redimir/1000000;
                                count[dir][0] = 0;
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }  
                                count[dir][0] = 0;
                                cambiar_img(img_cedula_fiel, dir);
                                Boton_redencion[dir] = 1;
                                return f_lcd_esperar_id_fiel;
                            break;
                                
                            case botom_no:
                                rventa.volumen_redimido[dir][0] = 0;  
                                rventa.volumen_redimido[dir][1] = 0; 
                                rventa.volumen_redimido[dir][2] = 0; 
                                rventa.volumen_redimido[dir][3] = 0;
                                rventa.volumen_redimido[dir][4] = 0; 
                                rventa.volumen_redimido[dir][5] = 0; 
                                rventa.volumen_redimido[dir][6] = 0; 
                                count[dir][0] = 0;
                                if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){
                                    cambiar_img(img_datos_venta_2, dir);
                                }
                                else{
                                    cambiar_img(img_datos_venta, dir);
                                }
                                return f_datos_venta;                                
                            break;    
                        }
                    }
                    else{
                        if(pos_vp == vp_vol_redimido){
                            while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }  
                            if (Bandera[dir][0] == 0){
                                for(uint8 x = 0;x<10;x++){
                                    Identificacion_aux[dir][x] = rventa.id[dir][x];
                                }
                                Tipo_aux[dir][0] = rventa.tipo_id[dir][0];
                                Bandera[dir][0] = 1;
                            }
                            valor = (buffer_lcd[dir][12] << 16) | (buffer_lcd[dir][13] << 8) | buffer_lcd[dir][14]; 
                            if(valor <= vol_redimir) {
                                rventa.volumen_redimido[dir][0] = (((((valor%1000000)%100000)%10000)%1000)%100)%10;  
                                rventa.volumen_redimido[dir][1] = (((((valor%1000000)%100000)%10000)%1000)%100)/10; 
                                rventa.volumen_redimido[dir][2] = ((((valor%1000000)%100000)%10000)%1000)/100; 
                                rventa.volumen_redimido[dir][3] = (((valor%1000000)%100000)%10000)/1000;
                                rventa.volumen_redimido[dir][4] = ((valor%1000000)%100000)/10000; 
                                rventa.volumen_redimido[dir][5] = (valor%1000000)/100000; 
                                rventa.volumen_redimido[dir][6] = valor/1000000;
                                count[dir][0] = 0;
                                Boton_redencion[dir] = 2;
                                count[dir][0] = 0;
                                cambiar_img(img_cedula_fiel, dir);

                                return f_lcd_esperar_id_fiel;
                            }else{
                                Boton_redencion[dir] = 2;
                                cambiar_img(img_exceso_cupo, dir); 
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_redimir;
                                prox_img[dir][0] = img_redimir;
                                timeout_lcd[dir][0] = 10;   
                                return f_menu_esperando_reportar;                                 
                            }
                        }
                        else{
                            
                            x = 0;
                            while(x != 3){
                                PSOC_ClearRxBuffer();                                
                                PSOC_PutChar(p_peticion_id);
                                PSOC_PutChar(dir + lado.dir[0][0]);
                                PSOC_PutChar('B');
                                x = timeout_psoc(3, 2);
                            }
                            count[dir][0] = 0;
                            cambiar_img(img_cedula_fiel,dir);
                            return f_lcd_esperar_id_fiel;
                        }    
                    }
                }   
            }
        }
        break;    
 
        case f_puntos_vender:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];                   
                    switch(lcd_vp){
                        case botom_devolver:
                            cambiar_img(img_menuventas_2, dir); 
                            return f_menu_ventas;                            
                        break;    
                            
                        case img_sel_id_cr:
                            if((lado.mangueras[dir][0] & 0x0F) == 3){
                                cambiar_img(img_escoja_produ_3, dir);      
                            }else if((lado.mangueras[dir][0] & 0x0F) == 2){
                                cambiar_img(img_escoja_produ_2, dir);      
                            }else if((lado.mangueras[dir][0] & 0x0F) == 1){
                                rventa.id_manguera[dir][0] = 1;
                                cambiar_img(img_sel_id_cr_2, dir);
                                return f_menu_elegir_id_credito;
                            }else if((lado.mangueras[dir][0] & 0x0F) == 0){
                                cambiar_img(img_escoja_produ_3, dir);
                            } 
                            for(x=0;x<=9;x++){
                                rventa.id[dir][x] = 0;    
                            }                                                                   
                            return f_menu_elegir_manguera;                                                          
                        break;   
                            
                        case img_sumi_auto:
                            rventa.tipo_venta[dir][0] = venta_consulta_puntos;
                            rventa.id_manguera[dir][0] = 1; 
                            cambiar_img(img_sel_id_cr_2, dir);  
                            return f_menu_elegir_id_credito; 
                        break;                                      
                    } 
                }        
            }        }
        break;
        
        case f_lcd_esperar_id_fiel:
        if(1){
            if(rventa.tipo_id[dir][0] == 'K'){
                rventa.tipo_id[dir][0] = id_cedula;
                cambiar_img(77, dir);
                count[dir][0] = 0;
                return f_lcd_clave_fiel;
            }
        }
        break;

        case f_lcd_clave_fiel:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                rventa.autorizada[dir][0] = 8;
                uint8 Cifrado[4] = {'*','*','*','*'};
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_clave_t:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            if(y == 4){
                                for(x=0;x<y;x++){
                                    contrasenha_fiel[dir][x] = buffer_lcd[dir][7+x];
                                    }
                                    write_vp_text(dir, 0x0234,Cifrado, 4);
                                    CyDelay(50);  
                                    autoriza_fiel[dir] = 1;                                
                                }
                            else{
                                for (x=0;x<100;x++){
                                    if(x<53){
                                        msn_ldc[dir][x] = ErrPass[x];
                                    }
                                    else {
                                        msn_ldc[dir][x] = ' ';
                                    }
                                }
                                count[dir][0] = 0; // contador en 0
                                limpiarLCD(dir);
                                if(dir == 0){ // escribo el mensaje en pantalla
                                    LCD1_PutChar(0x5A);
                                    LCD1_PutChar(0xA5);
                                    LCD1_PutChar(0x67);
                                    LCD1_PutChar(0x82);
                                    LCD1_PutChar(0x02);
                                    LCD1_PutChar(0x54);
                                    for(x=0;x<=99;x++){
                                      LCD1_PutChar(msn_ldc[dir][x]);  
                                    }
                                }
                                else{
                                    LCD2_PutChar(0x5A);
                                    LCD2_PutChar(0xA5);
                                    LCD2_PutChar(0x67);
                                    LCD2_PutChar(0x82);
                                    LCD2_PutChar(0x02);
                                    LCD2_PutChar(0x54);
                                    for(x=0;x<=99;x++){
                                      LCD2_PutChar(msn_ldc[dir][x]);  
                                    }
                                }
                                autoriza_fiel[dir] = 0;
                                cambiar_img(img_sumi_no_auto, dir); 
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 25;   
                                return f_menu_esperando_reportar;   
                            }
                            if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                                for(uint8 h = 0; h<4; h++){
                                    if(Fidelizado.Contrasenha[dir][h] != contrasenha_fiel[dir][h]){
                                        h = 4;
                                        autoriza_fiel[dir] = 0;
                                        for (x=0;x<100;x++){
                                            if(x<53){
                                                msn_ldc[dir][x] = ErrPass[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        count[dir][0] = 0; // contador en 0
                                        limpiarLCD(dir);
                                        if(dir == 0){ // escribo el mensaje en pantalla
                                            LCD1_PutChar(0x5A);
                                            LCD1_PutChar(0xA5);
                                            LCD1_PutChar(0x67);
                                            LCD1_PutChar(0x82);
                                            LCD1_PutChar(0x02);
                                            LCD1_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD1_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        else{
                                            LCD2_PutChar(0x5A);
                                            LCD2_PutChar(0xA5);
                                            LCD2_PutChar(0x67);
                                            LCD2_PutChar(0x82);
                                            LCD2_PutChar(0x02);
                                            LCD2_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD2_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        autoriza_fiel[dir] = 0;
                                        cambiar_img(img_sumi_no_auto, dir); 
                                        count[dir][0] = 0;
                                        prox_flujo[dir][0] = f_lcd_pantallazo;
                                        prox_img[dir][0] = 0;
                                        timeout_lcd[dir][0] = 25;   
                                        return f_menu_esperando_reportar;
                                    }
                                }
                                for(uint8 h = 0; h<10; h++){
                                    if(Fidelizado.Cedula[dir][h] != rventa.id[dir][h]){
                                        h = 10;
                                        autoriza_fiel[dir] = 0;
                                        for (x=0;x<100;x++){
                                            if(x<53){
                                                msn_ldc[dir][x] = ErrCe[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        
                                        count[dir][0] = 0; // contador en 0
                                        limpiarLCD(dir);
                                        if(dir == 0){ // escribo el mensaje en pantalla
                                            LCD1_PutChar(0x5A);
                                            LCD1_PutChar(0xA5);
                                            LCD1_PutChar(0x67);
                                            LCD1_PutChar(0x82);
                                            LCD1_PutChar(0x02);
                                            LCD1_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD1_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        else{
                                            LCD2_PutChar(0x5A);
                                            LCD2_PutChar(0xA5);
                                            LCD2_PutChar(0x67);
                                            LCD2_PutChar(0x82);
                                            LCD2_PutChar(0x02);
                                            LCD2_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD2_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        autoriza_fiel[dir] = 0;
                                        cambiar_img(img_sumi_no_auto, dir); 
                                        count[dir][0] = 0;
                                        prox_flujo[dir][0] = f_lcd_pantallazo;
                                        prox_img[dir][0] = 0;
                                        timeout_lcd[dir][0] = 25;   
                                        return f_menu_esperando_reportar;
                                    }
                                }
                                if(autoriza_fiel[dir] == 1){
                                    cambiar_img(img_sumi_auto, dir); 
                                    count[dir][0] = 0;
                                    prox_flujo[dir][0] = f_lcd_pantallazo;
                                    prox_img[dir][0] = 0;
                                    timeout_lcd[dir][0] = 25;   
                                    return f_menu_esperando_reportar;
                                }
                            }
                            else{
                                for(uint8 h = 0; h<4; h++){
                                    if(Fidelizado.Contrasenha[dir][h] != contrasenha_fiel[dir][h]){
                                        h = 4;
                                        //Agregar
                                        autoriza_fiel[dir] = 0;
                                        for (x=0;x<100;x++){
                                            if(x<53){
                                                msn_ldc[dir][x] = ErrPass[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        count[dir][0] = 0; // contador en 0
                                        limpiarLCD(dir);
                                        if(dir == 0){ // escribo el mensaje en pantalla
                                            LCD1_PutChar(0x5A);
                                            LCD1_PutChar(0xA5);
                                            LCD1_PutChar(0x67);
                                            LCD1_PutChar(0x82);
                                            LCD1_PutChar(0x02);
                                            LCD1_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD1_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        else{
                                            LCD2_PutChar(0x5A);
                                            LCD2_PutChar(0xA5);
                                            LCD2_PutChar(0x67);
                                            LCD2_PutChar(0x82);
                                            LCD2_PutChar(0x02);
                                            LCD2_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD2_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        autoriza_fiel[dir] = 0;
                                        cambiar_img(img_redimir, dir); 
                                        return f_lcd_redimir;

                                        //End
                                    }
                                }
                                for(uint8 h = 0; h<10; h++){
                                    if(Fidelizado.Cedula[dir][h] != rventa.id[dir][h]){
                                        h = 10;
                                        //Agregar
                                        autoriza_fiel[dir] = 0;
                                        for (x=0;x<100;x++){
                                            if(x<53){
                                                msn_ldc[dir][x] = ErrPass[x];
                                            }
                                            else {
                                                msn_ldc[dir][x] = ' ';
                                            }
                                        }
                                        count[dir][0] = 0; // contador en 0
                                        limpiarLCD(dir);
                                        if(dir == 0){ // escribo el mensaje en pantalla
                                            LCD1_PutChar(0x5A);
                                            LCD1_PutChar(0xA5);
                                            LCD1_PutChar(0x67);
                                            LCD1_PutChar(0x82);
                                            LCD1_PutChar(0x02);
                                            LCD1_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD1_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        else{
                                            LCD2_PutChar(0x5A);
                                            LCD2_PutChar(0xA5);
                                            LCD2_PutChar(0x67);
                                            LCD2_PutChar(0x82);
                                            LCD2_PutChar(0x02);
                                            LCD2_PutChar(0x54);
                                            for(x=0;x<=99;x++){
                                              LCD2_PutChar(msn_ldc[dir][x]);  
                                            }
                                        }
                                        autoriza_fiel[dir] = 0;
                                        cambiar_img(img_redimir, dir); 
                                        return f_lcd_redimir;
                                        //End
                                    }
                                }
                                if(autoriza_fiel[dir] == 1){
                                    if(Bandera[dir][0] == 1){
                                        for(uint8 x = 0;x<10;x++){
                                            rventa.id[dir][x] = Identificacion_aux[dir][x];
                                        }
                                        rventa.tipo_id[dir][0] = Tipo_aux[dir][0];
                                        Bandera[dir][0] = 0;
                                    }
                                    if (((img_autos == 4)||(img_autos == 5))&&PCol.Habilitado){
                                        cambiar_img(img_datos_venta_2, dir);
                                    }
                                    else{
                                        cambiar_img(img_datos_venta, dir);
                                    }
                                    
                                    return f_datos_venta;
                                }
                                
                            }
                        break; 
                        }                              
                    }
                }   
        }
        break;
        
        case f_menu_esperar_auto:
        if(1){
            if(rventa.autorizada[dir][0] == v_auto_ok){
                limpiarLCD(dir);
                if(dir == 0){
                    LCD1_PutChar(0x5A);
                    LCD1_PutChar(0xA5);
                    LCD1_PutChar(0x67);
                    LCD1_PutChar(0x82);
                    LCD1_PutChar(0x02);
                    LCD1_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD1_PutChar(msn_ldc[dir][x]);  
                    }
                }else{
                    LCD2_PutChar(0x5A);
                    LCD2_PutChar(0xA5);
                    LCD2_PutChar(0x67);
                    LCD2_PutChar(0x82);
                    LCD2_PutChar(0x02);
                    LCD2_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD2_PutChar(msn_ldc[dir][x]);  
                    }                
                }
                count[dir][0] = 0; 
                if(rventa.esCombustible[dir][0] == 1){
                    cambiar_img(img_sumi_auto, dir); 
                    count[dir][0] = 0;
                    prox_flujo[dir][0] = f_menu_dvf_cre;
                    if(rventa.tipo_venta[dir][0] == venta_Fidelizado){
                        for(x=0;x<=6;x++){                                   
                            aux_buffer[x] = 0;
                            reverse[x]    = 0;
                        }
                        count[dir][0] = 0;
                        prox_flujo[dir][0] = f_menu_dvf_cre;
                        vol_redimir = (rventa.trama_auto_cre[dir][3] & 0x0F)+((rventa.trama_auto_cre[dir][4] & 0x0F)*10)+((rventa.trama_auto_cre[dir][5] & 0x0F)*100)+((rventa.trama_auto_cre[dir][6] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][7] & 0x0F)*10000)+((rventa.trama_auto_cre[dir][8] & 0x0F)*100000)+((rventa.trama_auto_cre[dir][9] & 0x0F)*1000000);

                    }        
                    prox_img[dir][0] = img_menu_dvf;
                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                        prox_flujo[dir][0] = f_lcd_pantallazo;
                        prox_img[dir][0] = 0;                        
                    }
                    count[dir][0] = 0;
                    timeout_lcd[dir][0] = 10;                    
                    return f_menu_esperando_reportar;

                }else{
                    cambiar_img(img_sumi_auto, dir);
                    for(x=0;x<=9;x++){
                        rcanasta.id_cliente[x] = rventa.id[dir][x];
                    }
                    rcanasta.tipo_id_cliente = rventa.tipo_id[dir][0];
                    count[dir][0] = 0;
                    prox_flujo[dir][0] = f_lcd_canasta;
                    prox_img[dir][0] = img_canasta;
                    timeout_lcd[dir][0] = 15;   
                    return f_menu_esperando_reportar;                                        
                }
            }else if(rventa.autorizada[dir][0] == v_auto_no){
                limpiarLCD(dir);
                if(dir == 0){

                    LCD1_PutChar(0x5A);
                    LCD1_PutChar(0xA5);
                    LCD1_PutChar(0x67);
                    LCD1_PutChar(0x82);
                    LCD1_PutChar(0x02);
                    LCD1_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD1_PutChar(msn_ldc[dir][x]);  
                    }
                }else{

                    LCD2_PutChar(0x5A);
                    LCD2_PutChar(0xA5);
                    LCD2_PutChar(0x67);
                    LCD2_PutChar(0x82);
                    LCD2_PutChar(0x02);
                    LCD2_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD2_PutChar(msn_ldc[dir][x]);  
                    }                
                }
                cambiar_img(img_sumi_no_auto, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 15;   
                return f_menu_esperando_reportar;                
            }else
            if(rventa.autorizada[dir][0] == v_fidel_ok){
                limpiarLCD(dir);
                if(dir == 0){
                    LCD1_PutChar(0x5A);
                    LCD1_PutChar(0xA5);
                    LCD1_PutChar(0x67);
                    LCD1_PutChar(0x82);
                    LCD1_PutChar(0x02);
                    LCD1_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD1_PutChar(msn_ldc[dir][x]);  
                    }
                }else{
                    LCD2_PutChar(0x5A);
                    LCD2_PutChar(0xA5);
                    LCD2_PutChar(0x67);
                    LCD2_PutChar(0x82);
                    LCD2_PutChar(0x02);
                    LCD2_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD2_PutChar(msn_ldc[dir][x]);  
                    }                
                }
                count[dir][0] = 0;
                if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                    rventa.tipo_id[dir][0] = 0;
                    x = 0;
                    while(x != 3){
                        PSOC_ClearRxBuffer();                                
                        PSOC_PutChar(p_peticion_id);
                        PSOC_PutChar(dir + lado.dir[0][0]);
                        PSOC_PutChar('B');
                        x = timeout_psoc(3, 2);
                    }
                    count[dir][0] = 0;
                    cambiar_img(img_cedula_fiel, dir);
                    return f_lcd_esperar_id_fiel;
                }
                else{
                count[dir][0] = 0; 
                if(rventa.esCombustible[dir][0] == 1){
                    cambiar_img(img_sumi_auto, dir); 
                    count[dir][0] = 0;
                    prox_flujo[dir][0] = f_menu_dvf_cre;
                    if(rventa.tipo_venta[dir][0] == venta_Fidelizado){
                        for(x=0;x<=6;x++){                                   
                            aux_buffer[x] = 0;
                            reverse[x]    = 0;
                        }                        
                        prox_flujo[dir][0] = f_menu_dvf_cre;
                        vol_redimir = (rventa.trama_auto_cre[dir][3] & 0x0F)+((rventa.trama_auto_cre[dir][4] & 0x0F)*10)+((rventa.trama_auto_cre[dir][5] & 0x0F)*100)+((rventa.trama_auto_cre[dir][6] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][7] & 0x0F)*10000)+((rventa.trama_auto_cre[dir][8] & 0x0F)*100000)+((rventa.trama_auto_cre[dir][9] & 0x0F)*1000000);

                    }        
                    prox_img[dir][0] = img_menu_dvf;
                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                        prox_flujo[dir][0] = f_lcd_pantallazo;
                        prox_img[dir][0] = 0;                        
                    }
                    timeout_lcd[dir][0] = 10;                    
                    return f_menu_esperando_reportar;
                }                       
                timeout_lcd[dir][0] = 10;                    
                return f_menu_esperando_reportar;
            }
            }else 
            if(count[dir][0] >= 70){
                cambiar_img(img_sin_comuni, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;
                lado.estado[dir][0] = w_espera;
                return f_menu_esperando_reportar;               
            }
        }
        break;

        case f_lcd_esperar_id:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_reset_psoc);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar(1);
                                    x = timeout_psoc(3, 2);
                                }                                
                                cambiar_img(img_sel_id_cr_2, dir);  
                                return f_menu_elegir_id_credito;                            
                            break;
                        }
                    }
                }
            }    
            if((rventa.tipo_id[dir][0] == id_ibutton)||(rventa.tipo_id[dir][0] == id_codigo_barras)||(rventa.tipo_id[dir][0] == id_tarjeta)){
                if(rventa.esCombustible[dir][0] == 1){
                    if(rventa.tipo_venta[dir][0] == venta_consulta_puntos){
                        count[dir][0] = 0;                            
                        lado.estado[dir][0] = w_pet_autoriza;
                        cambiar_img(img_esperando, dir);    
                        return f_menu_esperar_auto;                                        
                    }else{
                        if(rventa.convenios[dir][0] == 'Z'){
                            rventa.tipo_id[dir][0] = 'Z';
                            rventa.convenios[dir][0] = 0;
                        }
                        cambiar_img(img_km_venta_id, dir); 
                        return f_menu_km_venta_id;
                    }
                }else{
                    count[dir][0] = 0;
                    lado.estado[dir][0] = w_pet_autoriza;
                    cambiar_img(img_esperando, dir); 
                    return f_menu_esperar_auto;                                    
                }                    
            }}
        break; 
            
        case f_menu_dvf_cre:
        if(1){
            ok = hay_datos_lcd(dir);
            if(lado.TipoSurtidor == 1){
                if(lado.estado[dir][0] != w_espera){
                    cambiar_img(93,dir);
                    img_actual[dir] = leer_img(dir);
                    count[dir][0] = 30;
                    //Me aseguro que la manguera este colgada con un cliente credito.
                }
                else{
                    if(img_actual[dir] == 93){
                        cambiar_img(img_menu_dvf,dir);
                        img_actual[dir] = leer_img(dir);
                    }
                }
            }
            if(ok == 1){
                uint8 Combustible = 0, delta = 0;
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){    //Full
                        if(lcd_vp == img_subir_manija){
                            if(rventa.tipo_venta[dir][0] == venta_Fidelizado){
                                rventa.trama_auto_cre[dir][2] = 2;
                                rventa.trama_auto_cre[dir][3] = 0;
                                rventa.trama_auto_cre[dir][4] = 0;
                                rventa.trama_auto_cre[dir][5] = 0;
                                rventa.trama_auto_cre[dir][6] = 9;
                                rventa.trama_auto_cre[dir][7] = 9;
                                rventa.trama_auto_cre[dir][8] = 9;
                                rventa.trama_auto_cre[dir][9] = 9;                                
                            }
                            x = 0;
                            if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                            
                                    PSOC_PutChar(p_autorizar_cre);    //Envio trama de autorizar
                                    for(x=1;x<=rventa.trama_auto_cre[dir][0];x++){
                                        PSOC_PutChar(rventa.trama_auto_cre[dir][x]);        
                                    }    
                                    x = timeout_psoc(3, 2);
                                }                             
                                rventa.preset[dir][0] = 3;
                                rventa.preset[dir][1] = 0;
                                rventa.preset[dir][2] = 0;
                                rventa.preset[dir][3] = 9;
                                rventa.preset[dir][4] = 9;
                                rventa.preset[dir][5] = 9;
                                rventa.preset[dir][6] = 9;
                                rventa.preset[dir][7] = 9;  
                                count[dir][0] = 0;
                                error_producto[dir][0] = 0;
                                cambiar_img(img_subir_manija, dir);
                                return f_menu_subir_manija;
                            }
                            else{
                                cambiar_img(0,dir);
                                return f_lcd_pantallazo;
                            }
                        }  
                    }else{        
                        switch(pos_vp){                            
                            case vp_valor_dinero: 
                                valor = (buffer_lcd[dir][12] << 16) | (buffer_lcd[dir][13] << 8) | buffer_lcd[dir][14];
                                preset_cre = (rventa.trama_auto_cre[dir][3] & 0x0F)+((rventa.trama_auto_cre[dir][4] & 0x0F)*10)+((rventa.trama_auto_cre[dir][5] & 0x0F)*100)+((rventa.trama_auto_cre[dir][6] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][7] & 0x0F)*10000)+((rventa.trama_auto_cre[dir][8] & 0x0F)*100000)+((rventa.trama_auto_cre[dir][9] & 0x0F)*1000000);
                                if(rventa.trama_auto_cre[dir][2] == 1){
                                    Combustible = rventa.trama_auto_cre[dir][10] + rventa.trama_auto_cre[dir][21] + rventa.trama_auto_cre[dir][32];  
                                    switch(Combustible){
                                        case 1:
                                            delta = 0;
                                        break;
                                        
                                        case 2:
                                            delta = 11;
                                        break;
                                    
                                        case 3:
                                            delta = 22;
                                        break;
                                    }
                                    ppu_cre = (rventa.trama_auto_cre[dir][11+delta] & 0x0F)+((rventa.trama_auto_cre[dir][12+delta] & 0x0F)*10)+((rventa.trama_auto_cre[dir][13+delta] & 0x0F)*100)+((rventa.trama_auto_cre[dir][14+delta] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][15+delta] & 0x0F)*10000);
                                    preset_cre = (((preset_cre/1000) * ppu_cre)*90)/100;    
                                }
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;
                                    reverse[x] = 0;
                                }
                                if((valor < preset_cre) || (rventa.tipo_venta[dir][0] == venta_Fidelizado)){
                                    itoa(valor,aux_buffer,10);
                                    y = 0;
                                    for(x=6;x>=1;x--){
                                        if((aux_buffer[x]>=0x30) && (aux_buffer[x]<=0x39)){
                                            reverse[y] = aux_buffer[x];
                                            y++;
                                        }    
                                    }
                                    reverse[y] = aux_buffer[0];
                                    rventa.trama_auto_cre[dir][2] = 2;
                                    rventa.trama_auto_cre[dir][3] = reverse[0] & 0x0F;
                                    rventa.trama_auto_cre[dir][4] = reverse[1] & 0x0F;
                                    rventa.trama_auto_cre[dir][5] = reverse[2] & 0x0F;
                                    rventa.trama_auto_cre[dir][6] = reverse[3] & 0x0F;
                                    rventa.trama_auto_cre[dir][7] = reverse[4] & 0x0F;
                                    rventa.trama_auto_cre[dir][8] = reverse[5] & 0x0F;
                                    rventa.trama_auto_cre[dir][9] = reverse[6] & 0x0F;
                                    x = 0;
                                    if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                        while(x != 3){
                                            PSOC_ClearRxBuffer();                                 
                                            PSOC_PutChar(p_autorizar_cre);    //Envio trama de autorizar
                                            for(x=1;x<=rventa.trama_auto_cre[dir][0];x++){
                                                PSOC_PutChar(rventa.trama_auto_cre[dir][x]);        
                                            } 
                                            x = timeout_psoc(3, 2);
                                        }
                                        rventa.preset[dir][0] = 2;
                                        rventa.preset[dir][1] = reverse[0] & 0x0F;
                                        rventa.preset[dir][2] = reverse[1] & 0x0F;
                                        rventa.preset[dir][3] = reverse[2] & 0x0F;
                                        rventa.preset[dir][4] = reverse[3] & 0x0F;
                                        rventa.preset[dir][5] = reverse[4] & 0x0F;
                                        rventa.preset[dir][6] = reverse[5] & 0x0F;
                                        rventa.preset[dir][7] = reverse[6] & 0x0F;
                                        count[dir][0] = 0;
                                        error_producto[dir][0] = 0;
                                        cambiar_img(img_subir_manija, dir);
                                        return f_menu_subir_manija;
                                }
                                    else{
                                        cambiar_img(0,dir);
                                        return f_lcd_pantallazo;

                                    }
                                    cambiar_img(img_exceso_cupo, dir); 
                                    count[dir][0] = 0;
                                    prox_flujo[dir][0] = f_menu_dvf_cre;
                                    prox_img[dir][0] = img_menu_dvf;
                                    timeout_lcd[dir][0] = 10;   
                                    return f_menu_esperando_reportar;                                
                                }
                            break;  
                                
                            case vp_valor_vol: 
                                valor = (buffer_lcd[dir][12] << 16) | (buffer_lcd[dir][13] << 8) | buffer_lcd[dir][14];
                                preset_cre = (rventa.trama_auto_cre[dir][3] & 0x0F)+((rventa.trama_auto_cre[dir][4] & 0x0F)*10)+((rventa.trama_auto_cre[dir][5] & 0x0F)*100)+((rventa.trama_auto_cre[dir][6] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][7] & 0x0F)*10000)+((rventa.trama_auto_cre[dir][8] & 0x0F)*100000)+((rventa.trama_auto_cre[dir][9] & 0x0F)*1000000);
                                if(rventa.trama_auto_cre[dir][2] == 2){
                                    Combustible = rventa.trama_auto_cre[dir][10] + rventa.trama_auto_cre[dir][21] + rventa.trama_auto_cre[dir][32];  
                                    switch(Combustible){
                                        case 1:
                                            delta = 0;
                                        break;
                                        
                                        case 2:
                                            delta = 11;
                                        break;
                                    
                                        case 3:
                                            delta = 22;
                                        break;
                                    }
                                    ppu_cre = (rventa.trama_auto_cre[dir][11+delta] & 0x0F)+((rventa.trama_auto_cre[dir][12+delta] & 0x0F)*10)+((rventa.trama_auto_cre[dir][13+delta] & 0x0F)*100)+((rventa.trama_auto_cre[dir][14+delta] & 0x0F)*1000)+((rventa.trama_auto_cre[dir][15+delta] & 0x0F)*10000);
                                    preset_cre = (((preset_cre / ppu_cre)*90)/100) * 1000;    
                                }
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;
                                    reverse[x] = 0;
                                }
                                if((valor < preset_cre) || (rventa.tipo_venta[dir][0] == venta_Fidelizado)){
                                    itoa(valor,aux_buffer,10);
                                    y = 0;
                                    for(x=6;x>=1;x--){
                                        if((aux_buffer[x]>=0x30) && (aux_buffer[x]<=0x39)){
                                            reverse[y] = aux_buffer[x];
                                            y++;
                                        }    
                                    }
                                    reverse[y] = aux_buffer[0];
                                    rventa.trama_auto_cre[dir][2] = 1;
                                    rventa.trama_auto_cre[dir][3] = reverse[0] & 0x0F;
                                    rventa.trama_auto_cre[dir][4] = reverse[1] & 0x0F;
                                    rventa.trama_auto_cre[dir][5] = reverse[2] & 0x0F;
                                    rventa.trama_auto_cre[dir][6] = reverse[3] & 0x0F;
                                    rventa.trama_auto_cre[dir][7] = reverse[4] & 0x0F;
                                    rventa.trama_auto_cre[dir][8] = reverse[5] & 0x0F;
                                    rventa.trama_auto_cre[dir][9] = reverse[6] & 0x0F;
                                    x = 0;
                                    
                                    if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {

                                        while(x != 3){
                                            PSOC_ClearRxBuffer();                                 
                                            PSOC_PutChar(p_autorizar_cre);    //Envio trama de autorizar
                                            for(x=1;x<=rventa.trama_auto_cre[dir][0];x++){
                                                PSOC_PutChar(rventa.trama_auto_cre[dir][x]);        
                                            } 
                                            x = timeout_psoc(3, 2);
                                        }    
                                        rventa.preset[dir][0] = 1;
                                        rventa.preset[dir][1] = reverse[0] & 0x0F;
                                        rventa.preset[dir][2] = reverse[1] & 0x0F;
                                        rventa.preset[dir][3] = reverse[2] & 0x0F;
                                        rventa.preset[dir][4] = reverse[3] & 0x0F;
                                        rventa.preset[dir][5] = reverse[4] & 0x0F;
                                        rventa.preset[dir][6] = reverse[5] & 0x0F;
                                        rventa.preset[dir][7] = reverse[6] & 0x0F;
                                        count[dir][0] = 0;
                                        error_producto[dir][0] = 0;
                                        cambiar_img(img_subir_manija, dir);
                                        return f_menu_subir_manija;
                                        }
                                    else{
                                            cambiar_img(0,dir);
                                            return f_lcd_pantallazo;
                                        }
                                }else{
                                    cambiar_img(img_exceso_cupo, dir); 
                                    count[dir][0] = 0;
                                    prox_flujo[dir][0] = f_menu_dvf_cre;
                                    prox_img[dir][0] = img_menu_dvf;
                                    timeout_lcd[dir][0] = 10;   
                                    return f_menu_esperando_reportar;                                
                                }                               
                            break;                                
                        }
                    }
                }   
            }
            if((count[dir][0] > 40)&&(leer_img(dir) != img_menu_dvf)){
            	count[dir][0] = 0;
            	cambiar_img(img_menu_dvf,dir);
			}
        }
        break;    
            
        case f_menu_turno:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case img_sel_id_t:               //Abrir Turno
                                if(turno1.estado[dir][0] == turno_cerrado){
                                    turno1.tipo_id[dir][0] = 0;
                                    for(x=0;x<=9;x++){
                                        turno1.cedula[dir][x] = 0;
                                    }
                                    turno1.pidiendo_id_turno[dir][0] = 1;
                                    cambiar_img(img_sel_id_t, dir); 
                                    return f_menu_elegir_id_t;
                                }
                            break;
                                
                            case img_confir_cerra_t:        //Cerrar Turno
                                if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                                    cambiar_img(img_confir_cerra_t, dir);   
                                    return f_menu_cerrar_t; 
                                }
                            break;
                                
                            case img_momento_corte:
                                count[dir][0] = 0;
                                lado.estado[dir][0] = w_Arqueo;
                                cambiar_img(img_esperando, dir);    
                                return f_lcd_esperar_Arqueo;
                                 
                            break;                                
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;                                
                        }
                    }
                }   
            }                }
        break;
        
        case f_lcd_esperar_Arqueo:
        //Aquí esperamos el arqueo
            if(Arqueo[dir] == 1){
                Arqueo[dir] = 0;
                ok = 1;
                lado.estado[dir][0] = w_espera;
                if(Hibrido == 1){
                        cambiar_img(11, dir); 
                }else{
                    cambiar_img( img_autos, dir); 

                }                                    
                return f_lcd_menu_inicial;
            }                               
            if(count[dir][0] >= 40){
                cambiar_img(img_momento_corte, dir);
                leer_fecha_lcd(dir);
                x = 0;
                lado.estado[dir][0] = w_espera;
                while(x != 2){
                    PSOC_ClearRxBuffer(); 
                    PSOC_PutChar(0xFC);
                    PSOC_PutChar(dir);
                    for(x=0;x<=5;x++){
                        PSOC_PutChar(turno1.fecha[CaraTurno][x]);    
                    }
                    for(x=0;x<=2;x++){
                        PSOC_PutChar(fecha[CaraTurno]);    
                    } 
                    for(x=0;x<=2;x++){
                        PSOC_PutChar(hora[x]);    
                    } 
                    for(x=0;x<=19;x++){
                        PSOC_PutChar(turno1.usuario[CaraTurno][x]);    
                    }  
                    for(x=0;x<=9;x++){
                        PSOC_PutChar(turno1.cedula[CaraTurno][x]);    
                    }  
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+5]);    
                    }  
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+34]);    
                    }
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+63]);    
                    }  
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+92]);    
                    }  
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+121]);    
                    }
                    for(x=0;x<=23;x++){
                        PSOC_PutChar(turno1.totales[CaraTurno][x+150]);    
                    } 
                    x = timeout_psoc(2, 4);
                }
                if(Hibrido == 1){
                        cambiar_img(11, dir); 
                }else{
                    cambiar_img( img_autos, dir); 

                }                                    
                return f_lcd_menu_inicial;
            }                       
        break;
        
        case f_menu_elegir_id_t:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                cambiar_img(img_sel_turno, dir);    
                                return f_menu_turno;                            
                            break;
                                
                            case img_ibutton:
                                turno1.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('I');
                                    x = timeout_psoc(3, 2);
                                } 
                                cambiar_img(img_ibutton, dir);  
                                return f_lcd_esperar_id_t;    
                            break;    
                                
                            case img_rfid:
                                turno1.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                 
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('T');
                                    x = timeout_psoc(3, 2);
                                }                                
                                cambiar_img(img_rfid, dir); 
                                return f_lcd_esperar_id_t;    
                            break;                                   
                                
                        }
                    }else{        
                        switch(pos_vp){                                                           
                            case vp_valor_cedula:
                                y = (buffer_lcd[dir][6] * 2)-2;
                                for(x=0;x<y;x++){
                                    if(buffer_lcd[dir][7+x] != 0xFF){
                                        turno1.cedula[dir][x] = buffer_lcd[dir][7+x];
                                    }
                                }
                                leer_fecha_lcd(dir);
                                turno1.fecha[dir][0] = fecha[0];
                                turno1.fecha[dir][1] = fecha[1];
                                turno1.fecha[dir][2] = fecha[2];
                                turno1.fecha[dir][3] = hora[2];
                                turno1.fecha[dir][4] = hora[1];
                                turno1.fecha[dir][5] = hora[0];
                                count[dir][0] = 0;
                                x = 0;
                                while(x != 176){                               
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(0xF7);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    x = timeout_psoc(176, 2);
                                }                                     
                                for(x=0;x<=173;x++){
                                    //turno1.totales[dir][x] = PSOC_rxBuffer[x+2];
                                    turno1.totales[0][x] = PSOC_rxBuffer[x+2];   
                                    turno1.totales[1][x] = PSOC_rxBuffer[x+2]; 
                                }
                                turno1.tipo_id[dir][0] = id_cedula;  
                                cambiar_img(img_clave_turno, dir);  
                                return f_lcd_clave_t;
                            break;                                          
                        }
                    }
                }   
            }                   }
        break;
            
        case f_lcd_esperar_id_t:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                x = 0;
                                while(x != 3){                               
                                    PSOC_ClearRxBuffer();                            
                                    PSOC_PutChar(p_reset_psoc);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar(1);
                                    x = timeout_psoc(3, 2);
                                }                               
                                cambiar_img(img_sel_id_t, dir); 
                                return f_menu_elegir_id_t;                            
                            break;
                        }
                    }
                }
            }    
            if((turno1.tipo_id[dir][0] == id_ibutton)||(turno1.tipo_id[dir][0] == id_tarjeta)){
                for(x=0;x<=9;x++){
                    turno1.cedula[0][x] = rventa.id[dir][x];
                    turno1.cedula[1][x] = rventa.id[dir][x];
                }
                leer_fecha_lcd(dir);
                for(x=0; x<2; x++){
                    turno1.fecha[x][0] = fecha[0];
                    turno1.fecha[x][1] = fecha[1];
                    turno1.fecha[x][2] = fecha[2];
                    turno1.fecha[x][3] = hora[2];
                    turno1.fecha[x][4] = hora[1];
                    turno1.fecha[x][5] = hora[0];                
                    count[dir][0] = 0;
                }
                x = 0;
                while(x != 176){                               
                    PSOC_ClearRxBuffer();
                    PSOC_PutChar(0xF7);
                    PSOC_PutChar(dir + lado.dir[0][0]);
                    x = timeout_psoc(176, 2);
                }  
                for(x=0;x<=173;x++){
                    turno1.totales[0][x] = PSOC_rxBuffer[x+2];   
                    turno1.totales[1][x] = PSOC_rxBuffer[x+2]; 
                } 
                cambiar_img(img_clave_turno, dir);  
                return f_lcd_clave_t;                    
            }            
        }
        break; 
            
        case f_lcd_clave_t:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_clave_t:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    turno1.password[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }
                            turno1.aceptado[dir][0] = 3;
                            lado.estado[dir][0] = w_pet_turno;
                            cambiar_img(img_esperando, dir);    
                            return f_lcd_esperar_turno; 
                        break;                                                              
                    }
                }   
            }            
        }
        break;            
            
        case  f_lcd_esperar_turno:
        if(1){
            if(turno1.aceptado[dir][0] == turno_no_aceptado){
                cambiar_img(75, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;   
                return f_menu_esperando_reportar;                    
            }else 
            if(turno1.aceptado[dir][0] == turno_aceptado){
                if(turno1.estado[dir][0] == turno_cerrado){ 
                    cambiar_img(img_turno_cerrado, dir);
                    Bandera[0][0] = 3;
                    Bandera[1][0] = 3;
                }else if(((turno1.estado[dir][0] == turno_abierto)&&(lado.estado[dir][0] != 0x06))&&((turno1.peticion_cierre[0][0]+turno1.peticion_cierre[1][0]) == 0)) {
                    cambiar_img(img_turno_abierto, dir);
                    CaraTurno = dir;
                }
                
                if(dir == 0){
                    EEPROM_WriteByte(turno1.estado[dir][0], 2);
                    EEPROM_WriteByte(turno1.tipo_id[dir][0], 3);
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.password[dir][x], (4+x));    
                    }
                    for(x=0;x<=5;x++){
                        EEPROM_WriteByte(turno1.fecha[dir][x], (188 + x));    
                    } 
                    for(x=0;x<=19;x++){
                        EEPROM_WriteByte(turno1.usuario[dir][x], (194 + x));    
                    }  
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.cedula[dir][x], (214 + x));    
                    }                  
                    for(x=0;x<=173;x++){
                        EEPROM_WriteByte(turno1.totales[dir][x], (14 + x));    
                    }
                }else 
                if(dir == 1){
                    EEPROM_WriteByte(turno1.estado[dir][0], 413);
                    EEPROM_WriteByte(turno1.tipo_id[dir][0], 414);
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.password[dir][x], (415+x));    
                    }
                    for(x=0;x<=5;x++){
                        EEPROM_WriteByte(turno1.fecha[dir][x], (599 + x));    
                    } 
                    for(x=0;x<=19;x++){
                        EEPROM_WriteByte(turno1.usuario[dir][x], (605 + x));    
                    }  
                    for(x=0;x<=9;x++){
                        EEPROM_WriteByte(turno1.cedula[dir][x], (625 + x));    
                    }                  
                    for(x=0;x<=173;x++){
                        EEPROM_WriteByte(turno1.totales[dir][x], (425 + x));    
                    }    
                }   
                
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;               
                return f_menu_esperando_reportar; 
            }else 
            if(count[dir][0] >= 120){
                if(turno1.estado[dir][0] == turno_cerrado){
                    cambiar_img(img_abrir_sin_sis, dir);                
                    return f_lcd_abrir_sin_sis;                    
                }else{
                    guardar_turno(dir);                    
                    turno1.estado[0][0] = turno_cerrado;
                    turno1.estado[1][0] = turno_cerrado;
                    leer_fecha_lcd(dir);
                    x = 0;                    
                    while(x != 2){
                        PSOC_ClearRxBuffer(); 
                        PSOC_PutChar(0xFC);
                        for(x=0;x<=5;x++){
                            PSOC_PutChar(turno1.fecha_anterior[dir][x]);    
                        }
                        for(x=0;x<=2;x++){
                            PSOC_PutChar(fecha[x]);    
                        } 
                        for(x=0;x<=2;x++){
                            PSOC_PutChar(hora[x]);    
                        } 
                        for(x=0;x<=19;x++){
                            PSOC_PutChar(turno1.usuario[dir][x]);    
                        }  
                        for(x=0;x<=9;x++){
                            PSOC_PutChar(turno1.cedula[dir][x]);    
                        }  
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+5]);    
                        }  
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+34]);    
                        }
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+63]);    
                        }  
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+92]);    
                        }  
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+121]);    
                        }
                        for(x=0;x<=23;x++){
                            PSOC_PutChar(turno1.totales_anteriores[dir][x+150]);    
                        } 
                        x = timeout_psoc(2, 4);
                    }
                    if(dir == 0){
                        EEPROM_WriteByte(turno1.estado[dir][0], 2);
                        EEPROM_WriteByte(turno1.tipo_id[dir][0], 3);                    
                        for(x=0;x<=9;x++){
                            EEPROM_WriteByte(turno1.password[dir][x], (4+x));    
                        }                   
                        for(x=0;x<=5;x++){
                            EEPROM_WriteByte(turno1.fecha[dir][x], (188 + x));    
                        } 
                        for(x=0;x<=19;x++){
                            EEPROM_WriteByte(turno1.usuario[dir][x], (194 + x));    
                        }  
                        for(x=0;x<=9;x++){
                            EEPROM_WriteByte(turno1.cedula[dir][x], (214 + x));    
                        }                  
                        for(x=0;x<=173;x++){
                            EEPROM_WriteByte(turno1.totales[dir][x], (14 + x));    
                        }
                    }else if(dir == 1){
                        EEPROM_WriteByte(turno1.estado[dir][0], 413);
                        EEPROM_WriteByte(turno1.tipo_id[dir][0], 414);                    
                        for(x=0;x<=9;x++){
                            EEPROM_WriteByte(turno1.password[dir][x], (415 + x));    
                        }                   
                        for(x=0;x<=5;x++){
                            EEPROM_WriteByte(turno1.fecha[dir][x], (599 + x));    
                        } 
                        for(x=0;x<=19;x++){
                            EEPROM_WriteByte(turno1.usuario[dir][x], (605 + x));    
                        }  
                        for(x=0;x<=9;x++){
                            EEPROM_WriteByte(turno1.cedula[dir][x], (625 + x));    
                        }                  
                        for(x=0;x<=173;x++){
                            EEPROM_WriteByte(turno1.totales[dir][x], (425 + x));    
                        }
                    }    
                }
                lado.estado[dir][0] = w_espera;
                cambiar_img(img_sin_comuni, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;   
                return f_menu_esperando_reportar;               
            }            }
        break; 
            
        case f_menu_cerrar_t:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_si:
                            if (lado.estado[0][0] == w_espera && lado.estado[1][0] == w_espera){
                                turno1.peticion_cierre[dir][0] = 1;
                                if(dir == 0){
                                    turno1.tipo_id[dir][0] = EEPROM_ReadByte(3);  
                                    for(x=0;x<=9;x++){
                                        turno1.cedula[dir][x] = EEPROM_ReadByte(214+x); 
                                    }
                                }else 
                                if(dir == 1){
                                    turno1.tipo_id[dir][0] = EEPROM_ReadByte(414);  
                                    for(x=0;x<=9;x++){
                                        turno1.cedula[dir][x] = EEPROM_ReadByte(625+x); 
                                    }
                                }    
                                for(x=0;x<=5;x++){
                                    turno1.fecha_anterior[dir][x] = turno1.fecha[dir][x];  
                                }  
                                for(x=0;x<=173;x++){
                                    turno1.totales_anteriores[dir][x] = turno1.totales[dir][x]; 
                                }                                
                                leer_fecha_lcd(dir);
                                turno1.fecha[dir][0] = fecha[0];
                                turno1.fecha[dir][1] = fecha[1];
                                turno1.fecha[dir][2] = fecha[2];
                                turno1.fecha[dir][3] = hora[2];
                                turno1.fecha[dir][4] = hora[1];
                                turno1.fecha[dir][5] = hora[0]; 
                                count[dir][0] = 0;
                                x = 0;
                                while(x != 176){                               
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(0xF7);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    x = timeout_psoc(176, 2);
                                }  
                                for(x=0;x<=173;x++){
                                    turno1.totales[0][x] = PSOC_rxBuffer[x+2];  
                                } 
                                turno1.aceptado[dir][0] = 3;
                                lado.estado[dir][0] = w_pet_turno;
                                cambiar_img(img_esperando, dir);    
                                return f_lcd_esperar_turno;  
                            }
                            break;
                                
                            case botom_no:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial; 
                            break;
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                               
                            break;    
                        }
                    }
                }   
            }            }
        break;
        
        case f_lcd_abrir_sin_sis:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_si:
                                cambiar_img(66, dir);
                                guardar_turno(dir);  
                                turno1.estado[0][0] = turno_abierto;
                                turno1.estado[1][0] = turno_abierto;
                                
                                //////////////////////////////////////////////////////////////////
                                //Le pido Totales al lado Par
                                while(x != 176){                               
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(0xF7);
                                    PSOC_PutChar(lado.dir[0][0]);
                                    x = timeout_psoc(176, 2);
                                }
                                
                                EEPROM_WriteByte(turno1.estado[0][0], 2);
                                EEPROM_WriteByte(turno1.tipo_id[dir][0], 3);
                                for(x=0;x<=9;x++){
                                    EEPROM_WriteByte(turno1.password[dir][x], (4+x));    
                                }  
                                for(x=0;x<=9;x++){
                                    turno1.password[0][x] = EEPROM_ReadByte(4+x);   
                                }                                 
                                for(x=0;x<=5;x++){
                                    EEPROM_WriteByte(turno1.fecha[dir][x], (188 + x));    
                                } 
                                for(x=0;x<=19;x++){
                                    EEPROM_WriteByte(0, (194 + x));    
                                }  
                                for(x=0;x<=9;x++){
                                    EEPROM_WriteByte(turno1.cedula[dir][x], (214 + x));    
                                }                  
                                for(x=0;x<=173;x++){
                                    EEPROM_WriteByte(turno1.totales[0][x], (14 + x));    
                                } 
                                //////////////////////////////////////////////////////////////////
                                //Le pido Totales al lado Impar
                                while(x != 176){                               
                                    PSOC_ClearRxBuffer();
                                    PSOC_PutChar(0xF7);
                                    PSOC_PutChar(lado.dir[0][0]+1);
                                    x = timeout_psoc(176, 2);
                                }
                                EEPROM_WriteByte(turno1.estado[1][0], 413);
                                EEPROM_WriteByte(turno1.tipo_id[dir][0], 414);
                                for(x=0;x<=9;x++){
                                    EEPROM_WriteByte(turno1.password[dir][x], (415+x));    
                                }  
                                for(x=0;x<=9;x++){
                                    turno1.password[0][x] = EEPROM_ReadByte(415+x);   
                                }                                 
                                for(x=0;x<=5;x++){
                                    EEPROM_WriteByte(turno1.fecha[dir][x], (599 + x));    
                                } 
                                for(x=0;x<=19;x++){
                                    EEPROM_WriteByte(0, (605 + x));    
                                }  
                                for(x=0;x<=9;x++){
                                    EEPROM_WriteByte(turno1.cedula[dir][x], (625 + x));    
                                }                  
                                for(x=0;x<=173;x++){
                                    EEPROM_WriteByte(turno1.totales[1][x], (425 + x));    
                                }    
                                //////////////////////////////////////////////////////////////////
                                turno1.Apertura_Fuera_Linea = 1;
                                EEPROM_WriteByte(turno1.Apertura_Fuera_Linea,707);
                                lado.estado[dir][0] = w_espera;
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 6;    
                                return f_menu_esperando_reportar;                             
                            break;
                                
                            case botom_no:
                                cambiar_img(0, dir);    
                                return f_lcd_pantallazo; 
                            break;    
                        }
                    }
                }   
            }             
        }
        break;    
            
        case f_lcd_canasta:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                }                
                                return f_lcd_menu_inicial;                            
                            break;
                                
                            case botom_enter:
                                if(((rcanasta.c_producto[0][0] >= 1)||(rcanasta.c_producto[1][0] >= 1))||((rcanasta.c_producto[2][0] >= 1)||(rcanasta.c_producto[3][0] >= 1))){
                                    leer_fecha_lcd(dir);
                                    cambiar_img(img_no_recibos, dir);   
                                    return f_no_recibos_c;
                                }                                                               
                            break;    

                            case botom_Producto1:                    //Producto 1
                                indice_producto = 0;
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }                                    
                                cambiar_img(img_codigo_barras, dir);    
                                return f_lcd_esperar_id_c;                                                            
                            break;
                                
                            case nobotom_Producto1:                    //Producto 1
                                rcanasta.c_producto[0][0] = 0;
                                v_producto_unitario = 0;
                                rcanasta.v_intproducto_total[0][0] = 0;
                                write_valor_total(dir, 0x328, rcanasta.v_intproducto_total[0][0]);
                                write_vp(dir,0x324,0);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                for(x=0;x<=6;x++){
                                        rcanasta.v_producto_total[0][x] = 0;    
                                }
                                rcanasta.suma_total = rcanasta.v_intproducto_total[3][0] + rcanasta.v_intproducto_total[2][0] + rcanasta.v_intproducto_total[1][0] + rcanasta.v_intproducto_total[0][0];
                                write_valor_total(dir,0x0344,rcanasta.suma_total);
                            break;

                            case botom_Producto2:                    //Producto 2
                                indice_producto = 1;
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }                                    
                                cambiar_img(img_codigo_barras, dir);    
                                return f_lcd_esperar_id_c;                                                            
                            break;
                                
                            case nobotom_Producto2:                    //Producto 1
                                rcanasta.c_producto[1][0] = 0;
                                v_producto_unitario = 0;
                                rcanasta.v_intproducto_total[1][0] = 0;
                                write_valor_total(dir, 0x332, rcanasta.v_intproducto_total[1][0]);
                                write_vp(dir,0x325,0);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                for(x=0;x<=6;x++){
                                        rcanasta.v_producto_total[1][x] = 0;    
                                }
                                rcanasta.suma_total = rcanasta.v_intproducto_total[3][0] + rcanasta.v_intproducto_total[2][0] + rcanasta.v_intproducto_total[1][0] + rcanasta.v_intproducto_total[0][0];
                                write_valor_total(dir,0x0344,rcanasta.suma_total);
                            break;
                                
                            case botom_Producto3:                    //Producto 3
                                indice_producto = 2;
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }                                    
                                cambiar_img(img_codigo_barras, dir);    
                                return f_lcd_esperar_id_c;                                                            
                            break;
                                
                            case nobotom_Producto3:                    //Producto 1
                                rcanasta.c_producto[2][0] = 0;
                                v_producto_unitario = 0;
                                rcanasta.v_intproducto_total[2][0] = 0;
                                write_valor_total(dir, 0x336, rcanasta.v_intproducto_total[2][0]);
                                write_vp(dir,0x326,0);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                for(x=0;x<=6;x++){
                                        rcanasta.v_producto_total[2][x] = 0;    
                                }
                                rcanasta.suma_total = rcanasta.v_intproducto_total[3][0] + rcanasta.v_intproducto_total[2][0] + rcanasta.v_intproducto_total[1][0] + rcanasta.v_intproducto_total[0][0];
                                write_valor_total(dir,0x0344,rcanasta.suma_total);
                            break; 
                                
                            case botom_Producto4:                    //Producto 4
                                indice_producto = 3;
                                rventa.tipo_id[dir][0] = 0;
                                x = 0;
                                while(x != 3){
                                    PSOC_ClearRxBuffer();                                
                                    PSOC_PutChar(p_peticion_id);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar('B');
                                    x = timeout_psoc(3, 2);
                                }                                    
                                cambiar_img(img_codigo_barras, dir);    
                                return f_lcd_esperar_id_c;                                                            
                            break; 
                            
                            case nobotom_Producto4:                    //Producto 1
                                rcanasta.c_producto[3][0] = 0;
                                v_producto_unitario = 0;
                                rcanasta.v_intproducto_total[3][0] = 0;
                                write_valor_total(dir, 0x340, rcanasta.v_intproducto_total[3][0]);
                                write_vp(dir,0x327,0);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                for(x=0;x<=6;x++){
                                        rcanasta.v_producto_total[3][x] = 0;    
                                }
                                rcanasta.suma_total = rcanasta.v_intproducto_total[3][0] + rcanasta.v_intproducto_total[2][0] + rcanasta.v_intproducto_total[1][0] + rcanasta.v_intproducto_total[0][0];
                                write_valor_total(dir,0x0344,rcanasta.suma_total);
                            break; 
                                                                   
                                
                        }
                    }else{        
                        switch(pos_vp){                                                           
                            case vp_cantidad_p1:
                                rcanasta.c_producto[0][0] = lcd_vp;
                                v_producto_unitario = rcanasta.v_producto[0][6] + (rcanasta.v_producto[0][5] * 10) + (rcanasta.v_producto[0][4] * 100) + (rcanasta.v_producto[0][3] * 1000) + (rcanasta.v_producto[0][2] * 10000) + (rcanasta.v_producto[0][1] * 100000)+ (rcanasta.v_producto[0][0] * 1000000);
                                rcanasta.v_intproducto_total[0][0] = lcd_vp * v_producto_unitario;
                                write_valor_total(dir, 0x328, rcanasta.v_intproducto_total[0][0]);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                itoa(rcanasta.v_intproducto_total[0][0],aux_buffer,10);
                                for(x=0;x<=6;x++){
                                    if((aux_buffer[x]>='0') && (aux_buffer[x]<='9')){
                                        rcanasta.v_producto_total[0][x] = aux_buffer[x];
                                    }else{
                                        rcanasta.v_producto_total[0][x] = 0;    
                                    }
                                }
                            break;
                                
                            case vp_cantidad_p2:
                                rcanasta.c_producto[1][0] = lcd_vp;
                                v_producto_unitario = rcanasta.v_producto[1][6] + (rcanasta.v_producto[1][5] * 10) + (rcanasta.v_producto[1][4] * 100) + (rcanasta.v_producto[1][3] * 1000) + (rcanasta.v_producto[1][2] * 10000) + (rcanasta.v_producto[1][1] * 100000)+ (rcanasta.v_producto[1][0] * 1000000);
                                rcanasta.v_intproducto_total[1][0] = lcd_vp * v_producto_unitario;
                                write_valor_total(dir, 0x332, rcanasta.v_intproducto_total[1][0]);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                itoa(rcanasta.v_intproducto_total[1][0],aux_buffer,10);
                                for(x=0;x<=6;x++){
                                    if((aux_buffer[x]>='0') && (aux_buffer[x]<='9')){
                                        rcanasta.v_producto_total[1][x] = aux_buffer[x];
                                    }else{
                                        rcanasta.v_producto_total[1][x] = 0;    
                                    }
                                }                                
                            break;   
                                
                            case vp_cantidad_p3:
                                rcanasta.c_producto[2][0] = lcd_vp;
                                v_producto_unitario = rcanasta.v_producto[2][6] + (rcanasta.v_producto[2][5] * 10) + (rcanasta.v_producto[2][4] * 100) + (rcanasta.v_producto[2][3] * 1000) + (rcanasta.v_producto[2][2] * 10000) + (rcanasta.v_producto[2][1] * 100000)+ (rcanasta.v_producto[2][0] * 1000000);
                                rcanasta.v_intproducto_total[2][0] = lcd_vp * v_producto_unitario;
                                write_valor_total(dir, 0x336, rcanasta.v_intproducto_total[2][0]); 
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                itoa(rcanasta.v_intproducto_total[2][0],aux_buffer,10);
                                for(x=0;x<=6;x++){
                                    if((aux_buffer[x]>='0') && (aux_buffer[x]<='9')){
                                        rcanasta.v_producto_total[2][x] = aux_buffer[x];
                                    }else{
                                        rcanasta.v_producto_total[2][x] = 0;    
                                    }
                                }                                
                            break; 
                                
                            case vp_cantidad_p4:
                                rcanasta.c_producto[3][0] = lcd_vp;
                                v_producto_unitario = rcanasta.v_producto[3][6] + (rcanasta.v_producto[3][5] * 10) + (rcanasta.v_producto[3][4] * 100) + (rcanasta.v_producto[3][3] * 1000) + (rcanasta.v_producto[3][2] * 10000) + (rcanasta.v_producto[3][1] * 100000)+ (rcanasta.v_producto[3][0] * 1000000);
                                rcanasta.v_intproducto_total[3][0] = lcd_vp * v_producto_unitario;
                                write_valor_total(dir, 0x340, rcanasta.v_intproducto_total[3][0]);
                                for(x=0;x<=6;x++){
                                    aux_buffer[x] = 0;    
                                }
                                itoa(rcanasta.v_intproducto_total[3][0],aux_buffer,10);
                                for(x=0;x<=6;x++){
                                    if((aux_buffer[x]>='0') && (aux_buffer[x]<='9')){
                                        rcanasta.v_producto_total[3][x] = aux_buffer[x];
                                    }else{
                                        rcanasta.v_producto_total[3][x] = 0;    
                                    }
                                }                                
                            break;                                 
                        }
                        rcanasta.suma_total = rcanasta.v_intproducto_total[3][0] + rcanasta.v_intproducto_total[2][0] + rcanasta.v_intproducto_total[1][0] + rcanasta.v_intproducto_total[0][0];
                        write_valor_total(dir,0x0344,rcanasta.suma_total);
                    }
                }   
            }             }
        break; 
            
        case f_lcd_tipo_canasta:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case img_canasta:
                                rventa.esCombustible[dir][0] = 0;
                                rventa.tipo_venta[dir][0] = venta_contado;
                                cambiar_img(img_canasta, dir);  
                                return f_lcd_canasta;
                            break;
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                }                
                                return f_lcd_menu_inicial;                            
                            break;                                
                                
                            case img_sel_id_cr:
                                turno1.pidiendo_id_turno[dir][0] = 0;
                                rventa.autorizada[dir][0] = 0;
                                rventa.tipo_venta[dir][0] = venta_credito;
                                rventa.esCombustible[dir][0] = 0;
                                cambiar_img(img_TipoClienteCan, dir);                
                                return f_tipoClienteCanasta; 
                            break;    
                                                                 
                        }
                    }
                }   
            }                
        }
        break; 
        
        case f_tipoClienteCanasta:
            ok = hay_datos_lcd(dir);
                if(ok == 1){
                    if(buffer_lcd[dir][3] == lcd_read_vp){
                        pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                        lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                        if(pos_vp == 0){
                            switch(lcd_vp){
                                case botom_devolver:
                                    if(Hibrido == 1){
                                            cambiar_img(11, dir); 
                                    }else{
                                        cambiar_img( img_autos, dir); 

                                    }                
                                    return f_lcd_menu_inicial;                            
                                break;  
                                
                                case img_sel_id_cr:
                                    turno1.pidiendo_id_turno[dir][0] = 0;
                                    rventa.autorizada[dir][0] = 0;
                                    rventa.tipo_venta[dir][0] = venta_credito;
                                    rventa.esCombustible[dir][0] = 0;
                                    cambiar_img(img_sel_id_cr_2, dir);                
                                    return f_menu_elegir_id_credito;                            
                                break;  
                                    
                                case img_sel_id_fd:
                                    turno1.pidiendo_id_turno[dir][0] = 0;
                                    rventa.autorizada[dir][0] = 0;
                                    rventa.tipo_venta[dir][0] = venta_Fidelizado;
                                    rventa.esCombustible[dir][0] = 0;
                                    cambiar_img(img_sel_id_cr_2, dir);                
                                    return f_menu_elegir_id_credito;                            
                                break;  
                            }}}}
        break;
        
        case f_lcd_esperar_id_c:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){
                            case botom_devolver:
                                x = 0;
                                while(x != 3){                               
                                    PSOC_ClearRxBuffer();                            
                                    PSOC_PutChar(p_reset_psoc);
                                    PSOC_PutChar(dir + lado.dir[0][0]);
                                    PSOC_PutChar(1);
                                    x = timeout_psoc(3, 2);
                                }                               
                                cambiar_img(img_canasta, dir);  
                                return f_lcd_canasta;                            
                            break;
                        }
                    }else if(pos_vp == vp_valor_code_can){
                        y = (buffer_lcd[dir][6] * 2)-2;
                        for(x=0;x<=12;x++){
                            rventa.id[dir][x] = 0;    
                        }
                        for(x=0;x<y;x++){
                            if(buffer_lcd[dir][7+x] != 0xFF){
                                rcanasta.id_producto[indice_producto][x] = buffer_lcd[dir][7+x];
                                rventa.id[dir][x] = rcanasta.id_producto[indice_producto][x];
                            }
                        }
                        lado.estado[dir][0] = w_pproducto_canasta;                  
                        rcanasta.producto_ok = 0;
                        count[dir][0] = 0;
                        cambiar_img(img_esperando, dir);
                        return f_lcd_esperar_canasta;
                    }
                }
            }    
            if(rventa.tipo_id[dir][0] == id_codigo_barras){
                for(x=0;x<=12;x++){
                    rcanasta.id_producto[indice_producto][x] = rventa.id[dir][x]; 
                }
                lado.estado[dir][0] = w_pproducto_canasta;                  
                rcanasta.producto_ok = 0;
                count[dir][0] = 0;
                cambiar_img(img_esperando, dir);
                return f_lcd_esperar_canasta; 
            }                    
        }
        break; 
            
        case f_lcd_esperar_canasta:
        if(1){
            if(rcanasta.producto_ok == 2){                  //Producto reconocido
                write_producto(indice_producto, dir);
                cambiar_img(img_canasta, dir);
                return f_lcd_canasta;
            }else if(rcanasta.producto_ok == 1){            //Producto NO reconocido
                cambiar_img(img_producto_no, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_canasta;
                prox_img[dir][0] = img_canasta;
                timeout_lcd[dir][0] = 10;   
                return f_menu_esperando_reportar;             
            }
            else if(count[dir][0] >= 40){
                cambiar_img(img_sin_comuni, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;   
                return f_menu_esperando_reportar;               
            }          
        }
        break; 
            
        case f_no_recibos_c:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 1){
                        rventa.print[dir][0] = buffer_lcd[dir][8];
                        lado.estado[dir][0] = w_venta_canasta;
                        count[dir][0] = 0;
                        prox_flujo[dir][0] = f_lcd_pantallazo;
                        prox_img[dir][0] = 0;
                        timeout_lcd[dir][0] = 40;
                        cambiar_img(img_esperando, dir);    
                        return f_menu_esperando_reportar;                            
                    }
                }   
            }                       
        }
        break;
            
        case f_lcd_reimprimir:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_imp_turno:               
                                lado.estado[dir][0] = w_imp_turno;
                                count[dir][0] = 0;
                                reimpresion[dir][0] = 0;
                                cambiar_img(img_esperando, dir);    
                                return f_menu_esperar_reimp;                              
                            break;
                                
                            case botom_imp_venta:        
                                lado.estado[dir][0] = w_imp_venta;
                                count[dir][0] = 0;
                                reimpresion[dir][0] = 0;
                                cambiar_img(img_elegir_cara, dir);    
                                return f_menu_cara_reimp;
                            break; 
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;                                
                        }
                    }
                }   
            }                 }
        break; 
        
        case f_menu_cara_reimp:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_cara1:               
                                lado.estado[dir][0] = w_imp_venta;
                                count[dir][0] = 0;
                                reimpresion[dir][0] = 0;
                                cambiar_img(img_esperando, dir);    
                                return f_menu_esperar_reimp;
                            break;
                                
                            case botom_cara2:  
                                if(lado.estado[(~dir & 1)][0] == w_espera){
                                    lado.estado[dir][0] = w_imp_venta_opues;
                                    count[dir][0] = 0;
                                    reimpresion[dir][0] = 0;
                                    cambiar_img(img_esperando, dir);    
                                    return f_menu_esperar_reimp;
                                }
                            break; 
                                
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;                                
                        }
                    }
                }   
            }                        }
        break; 
            
        case f_menu_esperar_reimp:
        if(1){
            if(reimpresion[dir][0] == 1){
                if(Hibrido == 1){
                        cambiar_img(11, dir); 
                }else{
                    cambiar_img( img_autos, dir); 

                } 
                return f_lcd_menu_inicial;                
            }else if(count[dir][0] >= 80){
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 6;
                cambiar_img(img_esperando, dir);    
                return f_menu_esperando_reportar;                
            }}
        break; 
            
        case f_lcd_info_fc:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                Hibrido = EEPROM_ReadByte(700);
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_devolver:
                                if(Hibrido == 1){
                                        cambiar_img(11, dir); 
                                }else{
                                    cambiar_img( img_autos, dir); 

                                } 
                                return f_lcd_menu_inicial;                            
                            break;
                            case 0x002C: //Configuración del surtidor
                                cambiar_img(77, dir); 
                                return f_lcd_clave_Config;                            
                            break;
                        }
                    }
                }   
            }            
        }
        break;
       

        case f_lcd_clave_Config:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                uint8 Cifrado[6] = {'*','*','*','*','*','*'};
                uint8 PwordLCD[6] = {0,0,0,0,0,0};
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_clave_t:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            if(y == 6){
                                for(x=0;x<y;x++){
                                    PwordLCD[x] = buffer_lcd[dir][7+x];
                                    }
                                    write_vp_text(dir, 0x0432,Cifrado, 6);
                                    CyDelay(50); 
                                if (PwordLCD[0] == PwordLCD[4]){
                                    if(PwordLCD[1] == PwordLCD[3]){
                                        if(PwordLCD[2] == '8' && PwordLCD[5]==0xFF){
                                            write_easterEgg(dir);
                                            cambiar_img(img_sumi_no_auto, dir); 
                                            count[dir][0] = 0;
                                            prox_flujo[dir][0] = f_lcd_pantallazo;
                                            prox_img[dir][0] = 0;
                                            timeout_lcd[dir][0] = 15;   
                                            return f_menu_esperando_reportar;
                                        }
                                    }
                                
                                }
                                }

                            else{
                                write_error(dir);
                                cambiar_img(img_sumi_no_auto, dir); 
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 15;   
                                return f_menu_esperando_reportar;
                            }
                            leer_fecha_lcd(dir);
                            uint8 Pword[6] = {0,0,0,0,0,0};
                            unsigned long long Pass = 0;
                            Pass = fecha[0]*fecha[1]*fecha[2];
                            Pass = Pass*lado.dir[dir][0]+(fecha[(fecha[0]*lado.dir[dir][0])%3]*fecha[(fecha[0]*lado.dir[dir][0])%2]*fecha[(fecha[0]*lado.dir[dir][0])%1]);
                            Pass = Pass + fecha[0]*lado.dir[dir][0];
                            Pass = Pass*lado.dir[dir][0];
                            Pass = Pass*((Pass%lado.dir[dir][0])+1);
                            Pass = (Pass%1000000);
                            if (Pass<1000){
                                Pass = Pass*159357;
                            }
                            Pword[0] = (Pass/100000)%10;
                            Pword[1] = (Pass/10000)%10;
                            Pword[2] = (Pass/1000)%10;
                            Pword[3] = (Pass/100)%10;
                            Pword[4] = (Pass/10)%10;
                            Pword[5] = Pass%10;
                            for(x = 0; x<=5; x++){
                                if ((Pword[x]+48) != (PwordLCD[x])){
                                    x = 10;   
                                }
                            }
                            
                            if (x !=  6){
                                write_error(dir);
                                cambiar_img(img_sumi_no_auto, dir); 
                                count[dir][0] = 0;
                                prox_flujo[dir][0] = f_lcd_pantallazo;
                                prox_img[dir][0] = 0;
                                timeout_lcd[dir][0] = 5;   
                                return f_menu_esperando_reportar;
                            }
                            else{
                                cambiar_img(img_conf_hibr, dir);
                                return f_lcd_MenuConf;
                            }
                        break; 
                        }                              
                    }
                }
            break;
        }
        
        case f_lcd_MenuConf:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        if(lcd_vp == 0x01){
                            Hibrido = 1;
                            cambiar_img(img_escoja_produ_3, dir);
                            x = f_menu_elegir_Hibrida;
                                
                        }else{
                            Hibrido = 0;
                            cambiar_img( img_Bandera, dir); 
                            x = f_Bandera_estacion;
                        }
                        EEPROM_WriteByte(Hibrido,700);
                        return x;
                    }   
                }
            }
        }
        break;
        
        case f_Bandera_estacion:
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    switch(lcd_vp){                    
                        case 0x2131:
                            img_autos = 1;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x4032:
                            img_autos = 2;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2333:
                            img_autos = 3;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2434:
                            img_autos = 4; 
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_HabiliPcol,dir);
                            return f_HabilitaPcol;
                            
                        break;

                        case 0x2535:
                            img_autos = 5;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_HabiliPcol,dir);
                            return f_HabilitaPcol;
                            
                        break;

                        case 0x5E36:
                            img_autos = 6;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2637:
                            img_autos = 7;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2A38:
                            img_autos = 8;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2930:
                            img_autos = 9;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        case 0x2839:
                            img_autos = 10;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                        default:
                            img_autos = 1;  
                            EEPROM_WriteByte(img_autos,703);
                            cambiar_img(img_autos, dir);
                            Bandera[0][0] = 3;
                            Bandera[1][0] = 3;
                            return f_lcd_menu_inicial;
                        break;

                    }
                }   
            }
        break;
        
        case f_HabilitaPcol:
            if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){
                        switch(lcd_vp){
                            case botom_si:
                                PCol.Habilitado = 1;
                                EEPROM_WriteByte(PCol.Habilitado,704);
                                cambiar_img(img_autos, dir);
                                Bandera[0][0] = 3;
                                Bandera[1][0] = 3;
                                return f_lcd_menu_inicial;                 
                            break;
                                
                            case botom_no:
                                PCol.Habilitado = 0;
                                EEPROM_WriteByte(PCol.Habilitado,704);
                                cambiar_img(img_autos, dir);
                                Bandera[0][0] = 3;
                                Bandera[1][0] = 3;
                                return f_lcd_menu_inicial;
                            break;    
                        }
                    }
                }   
            }             
        }
            break;
        
        case f_menu_elegir_Hibrida:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];
                    if(pos_vp == 0){                    
                        switch(lcd_vp){                                
                            case botom_1:
                                Hibrido_m = 1;  
                            break;    

                            case botom_2:
                                Hibrido_m = 2;                              
                            break; 
                                
                            case botom_3:
                                Hibrido_m = 3;                           
                            break;                                                                                                                                                                 
                        }
                        EEPROM_WriteByte(Hibrido_m,701);
                        cambiar_img(11, dir);
                        Bandera[0][0] = 3;
                        Bandera[1][0] = 3;
                        return f_lcd_menu_inicial;
                        
                        
                    }
                }   
            }               
        }
        break;

      
            
        case f_valor_formas_pago:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_fpago:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.valor_forma_pago[dir][x] = buffer_lcd[dir][7+x];
                                }
                            }                           
                            cambiar_img(img_valor_boucher, dir);    
                            return f_boucher_formas_pago;
                        break;                                                              
                    }
                    if(lcd_vp == botom_devolver){
                        if(Hibrido == 1){
                                cambiar_img(11, dir); 
                        }else{
                            cambiar_img( img_autos, dir); 

                        } 
                        return f_lcd_menu_inicial;                            
                    }
                }   
            }                 }
        break; 
            
        case f_boucher_formas_pago:
        if(1){
            ok = hay_datos_lcd(dir);
            if(ok == 1){
                if(buffer_lcd[dir][3] == lcd_read_vp){
                    pos_vp = (buffer_lcd[dir][4] << 8) | buffer_lcd[dir][5];
                    lcd_vp = (buffer_lcd[dir][7] << 8) | buffer_lcd[dir][8];        
                    switch(pos_vp){                                                          
                        case vp_valor_baucher:
                            y = (buffer_lcd[dir][6] * 2)-2;
                            for(x=0;x<y;x++){
                                if(buffer_lcd[dir][7+x] != 0xFF){
                                    rventa.boucher_forma_pago[dir][x] = buffer_lcd[dir][7+x];
                                }
                            } 
                            count[dir][0] = 0;
                            lado.estado[dir][0] = w_fpago;
                            cambiar_img(img_esperando, dir);    
                            return f_esperar_fp;
                        break;                                                              
                    }
                    if(lcd_vp == botom_devolver){
                        if(Hibrido == 1){
                                cambiar_img(11, dir); 
                        }else{
                            cambiar_img( img_autos, dir); 

                        } 
                        return f_lcd_menu_inicial;                            
                    }
                }   
            }                 }
        break; 
            
        case f_esperar_fp:
        if(1){
            if(rventa.forma_pago[dir][0] == 0xFF){
                if(Hibrido == 1){
                        cambiar_img(11, dir); 
                }else{
                    cambiar_img( img_autos, dir); 

                } 
                return f_lcd_menu_inicial;                
            }else 
            if(count[dir][0] >= 120){
                lado.estado[dir][0] = w_espera;
                 cambiar_img(75, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 10;   
                return f_menu_esperando_reportar;               
            }else 
            if(rventa.forma_pago[dir][0] == 0xFE){
                limpiarLCD(dir);
                if(dir == 0){
                    LCD1_PutChar(0x5A);
                    LCD1_PutChar(0xA5);
                    LCD1_PutChar(0x67);
                    LCD1_PutChar(0x82);
                    LCD1_PutChar(0x02);
                    LCD1_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD1_PutChar(msn_ldc[dir][x]);  
                    }
                }else{
                    LCD2_PutChar(0x5A);
                    LCD2_PutChar(0xA5);
                    LCD2_PutChar(0x67);
                    LCD2_PutChar(0x82);
                    LCD2_PutChar(0x02);
                    LCD2_PutChar(0x54);
                    for(x=0;x<=99;x++){
                      LCD2_PutChar(msn_ldc[dir][x]);  
                    }                
                }
                cambiar_img(img_sumi_no_auto, dir); 
                count[dir][0] = 0;
                prox_flujo[dir][0] = f_lcd_pantallazo;
                prox_img[dir][0] = 0;
                timeout_lcd[dir][0] = 15;   
                return f_menu_esperando_reportar;               
            }           
        }
        break;    
    }
    return flujo;
}

/* [] END OF FILE */
