
#include "PWM.h"

void PWM_config(char canal, int periodo)
{
    CCP1CON = 0;  
    CCP2CON = 0;                        //Apagar ambios CCP
    
    switch (periodo)
    {
        case 2:
            PR2 = 125;
            break;
        case 3:
            PR2 = 188; 
            break;
        case 4:
            PR2 = 250; 
            break;
        default:
            PR2 = 63;                  
    }                                   //Se selecciona entre 3 casos de 2, 3 y 4 ms para periodo y un default de 1 ms
    
    if (canal == 1)                     //Configuracion CCP1
    {
        CCP1CONbits.P1M = 0;            // Modo sigle output
        CCP1CONbits.CCP1M = 0b1100;     // PWM        
    }
    else if (canal == 2)                //Configuracion CCP2
    {
        CCP2CONbits.CCP2M = 0b1100;     // PWM
    }
    else if (canal == 3)                //Configuracion para encender ambos
    {
        CCP1CONbits.P1M = 0;            // Modo sigle output
        CCP1CONbits.CCP1M = 0b1100;     // PWM
        CCP2CONbits.CCP2M = 0b1100;     // PWM
    }
        PIR1bits.TMR2IF = 0;            // Bandera TMR2    
        T2CONbits.T2CKPS = 0b11;        // Prescaler 1:16
        T2CONbits.TMR2ON = 1;           // Iniciar TMR2
        while (!PIR1bits.TMR2IF);       // Esperar un ciclo del TMR2
        PIR1bits.TMR2IF = 0;
}


void PWM_duty(char canal, unsigned short duty)
{
        if (canal == 1)
        {
            CCPR1L = duty>>2;               //Shift 2 a la derecha duty cicle
            CCP1CONbits.DC1B = duty & 3;    //Realiza un and con los dos bits menos significativos y envia 
        }
        if (canal == 2)
        {
            CCPR2L = (duty>>2);              //Shift 2 a la derecha duty cicle
            CCP2CONbits.DC2B0 = duty & 0b01; 
            CCP2CONbits.DC2B1 = duty & 0b10; //Realiza un and con los dos bits menos significativos y envia 
        }
        return;
}