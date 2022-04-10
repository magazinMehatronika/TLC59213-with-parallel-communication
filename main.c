//  MSP430FR2311 paralelna komunikacija sa Ti TLC59213 LED 8 kanalnim drajverom
//  sa darlington izlazima do 500mA po kanalu
//  Implementirane funkcije:
//  setLED (buffer, duration)
//                   - buffer: 0b******** - pozicija bita pali jedan od 8 kanala
//                   - duration: dužina trajanja sijanja LED kanala
//
//                  MSP430FR2311
//                -----------------
//            /|\|                 |
//             | |                 |
//             --|RST              |
//               |         P1.0...7|-> Led kanal 0...7
//               |             P2.0|-> CLK
//               |             P2.2|-> ~CLR
//          BTN->|P2.1             |
//
// OPIS: Klikom na prekidač BTN, MSP izlazi iz sleep moda i
//           interapt rutinom menja šemu sijanja LED kanala
//
//
//   magazinMehatronika @ eduIoT
//   April 2020
//******************************************************************************
#include <msp430.h> 

unsigned int m=0x00;


void setLED (unsigned int buffer, unsigned long duration)   // SetLED (LED output, duration) function
{

    P2OUT |= BIT2;                          // CLR high
    __delay_cycles(50);
    P1OUT |= buffer;                        // Data set
    __delay_cycles(50);
    P2OUT |= BIT0;                          // CLK high
    __delay_cycles(50);


    P2OUT &= ~BIT0;                          // CLK low
    __delay_cycles(50);
    P1OUT = 0x00;                          // Data clear
    __delay_cycles(50);

       while ((duration--)>0)
        __delay_cycles(1);

    P2OUT &= ~BIT2;                          // CLR low
   __delay_cycles(50);
}


/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;                // Stop watchdog timer

    P1DIR |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;                  // P1.0...7 as out
    P1OUT &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);                // P1.0...7 low


    P2DIR |= BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0;                  // P2.0...7 as output
    P2OUT &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);               // P2.0...7 high


    P2DIR &= ~BIT1;                                             // P2.1 as input
    P2OUT |= BIT1;                                              // Configure P2.1 as pulled-up
    P2REN |= BIT1;                                              // P2.1 pull-up register enable
    P2IES |= BIT1;                                              // P2.1 Hi/Low edge
    P2IE  |= BIT1;

    PM5CTL0 &= ~LOCKLPM5;                                       // Disable the GPIO power-on default high-impedance mode
                                                                // to activate previously configured port settings
    P2IFG &= ~BIT1;                                             // P2.1 IFG cleared

 while (1)
     {

        __bis_SR_register(LPM3_bits | GIE);                // Enter LPM0,enable interrupts
        __no_operation();                                  // For debug,Remain in LPM0

         }
}



#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)

{
    switch (m) {
        case 0:
        {
            m=1;
            setLED (0xff, 65000);
            }
            break;
        case 1:
        {
            m=2;
            setLED (0x44, 65000);
            }
            break;
        case 2:
        {
            m=0;
            setLED (0x88, 65000);
            }
            break;
        default:
            break;
    }

    P2IFG &= ~BIT1;                                            // Clear P2.1 IFG
    __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3
}



// UNUSED_HWI_ISR()
#pragma vector=UNMI_VECTOR
#pragma vector=WDT_VECTOR
#pragma vector=TIMER1_B0_VECTOR
#pragma vector=TIMER1_B1_VECTOR
#pragma vector=TIMER0_B0_VECTOR
#pragma vector=TIMER0_B1_VECTOR
#pragma vector=PORT1_VECTOR
// #pragma vector=PORT2_VECTOR
#pragma vector=ECOMP0_VECTOR
#pragma vector=ADC_VECTOR
#pragma vector=EUSCI_B0_VECTOR
#pragma vector=EUSCI_A0_VECTOR
#pragma vector=RTC_VECTOR
#pragma vector=SYSNMI_VECTOR
#pragma vector=RESET_VECTOR

__interrupt void UNUSED_HWI_ISR (void)
{
__no_operation();
}
