#include "ADC.h"

void ADC_Int(uint8_t ADFM_Select)
{   
    ADCON0 = 0b10000001;                //Enciende ADC y FOSC/32 como ADCS
    ADCON1bits.ADFM  = ADFM_Select;     //Justificado a la izquierda
    ADCON1bits.VCFG1 = 0;   
    ADCON1bits.VCFG0 = 0;               //Voltajes de referencia como VDD y VSS
}

void ADC_Select(int channel)
{
    ADCON0bits.CHS0 = (channel >> 0) & 0x01;
    ADCON0bits.CHS1 = (channel >> 1) & 0x01;
    ADCON0bits.CHS2 = (channel >> 2) & 0x01;
    ADCON0bits.CHS3 = (channel >> 3) & 0x01;    
}