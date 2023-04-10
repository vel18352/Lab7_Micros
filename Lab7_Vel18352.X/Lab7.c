/*
Archivo:        Lab7_Vel18352
Dispositivo:    PIC16F887
Autor:		    Emilio Velasquez 18352
Compilador:	    XC8, MPLABX 5.40
Programa:       Contador binario y display de voltaje por ADC
Hardware:	    2 potenciometros, 8 leds, 4 displays 7 segmentos
Creado:         8/04/2023
Ultima modificacion: 9/04/2023
 */
//******************************************************************************
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
//******************************************************************************

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config MCLRE = OFF      
#pragma config CP = OFF         
#pragma config CPD = OFF        
#pragma config BOREN = OFF      
#pragma config IESO = OFF       
#pragma config FCMEN = OFF      
#pragma config LVP = OFF        
// CONFIG2
#pragma config BOR4V = BOR40V   
#pragma config WRT = OFF        
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//******************************************************************************
// Includes
//******************************************************************************
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>    //Se incluyen las librerias necesarias

#include "ADC.h"
#include "Map.h"
#include "PWM.h"        //Funciones propias
//******************************************************************************
// Definiciones
//******************************************************************************
#define _XTAL_FREQ 4000000  //Se establece el oscilador a 4.0Mhz

//******************************************************************************
// Variables
//******************************************************************************
uint8_t  TMR0_Pre = 250;           //Precarga TMR0
uint8_t  Bandera_ADC = 0;          //Bandera para rotacion de ADC
uint8_t  Contador_PWM = 0;
uint16_t ADC1 = 0;  
uint16_t ADC2 = 0;
uint16_t ADC3 = 0;                 //Variables para almacenar valores ADC
unsigned short Servo1_Val = 0;
unsigned short Servo2_Val = 0;     //Variables para almacenar Duty Cicle
//******************************************************************************
// Interrupciones
//******************************************************************************
void __interrupt() isr(void)
{
    if (ADIF)                                       //Chequea bandera de interrupcion de ADC
    {       
        if (Bandera_ADC == 0)                       //En caso Bandera_ADC 
        {
            ADC1 = ADRESH;                          //Lee ADC y almacena en ADC1 la lectura del primer potenciometro
            Servo1_Val = map(ADC1,0,255,160,650);   //Mapeo para los rangos a los cuales trabaja el servo de PWM
            PWM_duty(1,Servo1_Val);                 //Funcion para seleccionar canal y enviar valor PWM
            Bandera_ADC = 1;                        //Rotar bandera ADC
        }
        else if (Bandera_ADC == 1)
        {
            ADC2 = ADRESH;                          //Lee ADC y almacena en ADC2 la lectura del primer potenciometro
            Servo2_Val = map(ADC2,0,255,160,650);   //Mapeo para los rangos a los cuales trabaja el servo de PWM
            PWM_duty(2,Servo2_Val);                 //Funcion para seleccionar canal y enviar valor PWM
            Bandera_ADC = 2;                        //Rotar bandera ADC
        }    
        else if (Bandera_ADC == 2)
        {
            ADC3 = ADRESH;                          //Lee ADC y almacena en ADC3 la lectura del primer potenciometro
            Bandera_ADC = 0;                        //Rotar bandera ADC 
        }
        ADIF = 0;                                   //Reinicia bandera ADC
    }
    
    if(T0IF)
    {                
        Contador_PWM++;                             //Aumentar contador 
        if (Contador_PWM <= ADC3)                   //Mientras contador este por debajo del valor de ADC3
        {
            PORTCbits.RC3 = 1;                      //Enciende puerto   
        }
        else                                        
        {
            PORTCbits.RC3 = 0;                      //De lo contrario apaga el puerto
        }
        T0IF = 0;                                   //Reiniciar bandera TMR0
        TMR0 = TMR0_Pre;                            //Precarga
    }
    return;      
}

//******************************************************************************
// Configuraciones
//******************************************************************************
void Setup(void)
{
    ANSEL  = 0x07;  //Enciende primeros 2 canales del ADC
    ANSELH = 0x00;   
    TRISA = 0x07;   //Establece primeros dos bits de PORTA como entrada
    PORTA = 0;      //Limpia PORTA
    TRISB = 0;      //Establece PORTB como salida
    PORTB = 0;      //Limpia PORTB
    TRISC = 0;      //Establece PORTC como salida
    PORTC = 0;      //Limpia PORTC
    TRISD = 0;      //Establece PORTD como salida
    PORTD = 0;      //Limpia PORTD
    
}

void Int_Enable(void)
{
    T0IF = 0;
    T0IE = 1;   //Habilitar interrupciones TMR0
    ADIF = 0;
    ADIE = 1;   //Habilitar interrupciones ADC
    PEIE = 1;   //Habilitar interrupciones Perifericas
    GIE = 1;    //Habilitar interrupciones Globales
}

void TMR0_Config(void)
{
    OPTION_REGbits.T0CS = 0;   //Encender TMR0
    OPTION_REGbits.PSA = 0;      
    OPTION_REGbits.PS0 = 0;
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS2 = 0;    // Prescaler 1:1
    T0IF = 0;                  //Limpiar bandera de interrupcion
    TMR0 = TMR0_Pre;           //Precarga
}

//******************************************************************************
// Subrutinas
//******************************************************************************
void ADC_Change(void)                           //Conmutar canal el cual se le hara lectura de ADC para ambos potenciometros
{
    if (ADCON0bits.GO)                          //Se chequea que no este ocurriendo una lectura de ADC
    {           
        return;
    }
    else if (Bandera_ADC == 0)                  //En caso Bandera_ADC sea verdadero 
    {
        ADC_Select(0);                          //Se selecciona el Canal 0 para la lectura de ADC
    }
    else if (Bandera_ADC == 1)
    {
        ADC_Select(1);                          //Se selecciona el Canal 1 para la lectura de ADC
    }        
    else if (Bandera_ADC == 2)
    {
        ADC_Select(2);                           //Se selecciona el Canal 2 para la lectura de ADC
    }
    while(ADCON0bits.GO);                       //Espera a terminar la conversion ADC
    ADCON0bits.GO = 1;                          //Inicia la conversion ADC
}

//******************************************************************************
// Main Loop
//******************************************************************************
void main(void) 
{
    Setup();        
    Int_Enable();
    TMR0_Config();  
    PWM_config(3,4);
    ADC_Int(0);                                  //Llama a las rutinas de configuracion de funciones
    while(1)                                     //Loop principal
    {
        ADC_Change();                           //Llama a la funcion de conmutar el canal de ADC
    }
    return;
}