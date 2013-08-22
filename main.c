/**
 * Drehzahlmesser für die Hannibal Pumpe
 *
 * Abgriff der Drehzahl am "W"-Ausgang der Lichtmaschine.
 *
 * Funktionen:
 * 	- Anzeige der Drehzahl
 * 	- Akkuspannung ?
 * 	-
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "lcd.h"
#include "i2cmaster.h"
#include "FloatingAverage.h"


#define UART_BAUD_RATE      9600

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


volatile unsigned char NrOverflows = 0; // Anzahl der Timer Overflows die während
                                        // der Messung passiert sind
volatile unsigned int  StartTime = 0;   // ICR-Wert bei 1.High-Flanke speichern
volatile unsigned int  EndTime = 0;     // ICR-Wert bei 2.High-Flanke speichern
volatile unsigned char UpdateDisplay;   // Job Flag

volatile unsigned char flag_1s = 0;		// Wird jede sec durch ISR auf 1 gesetzt
volatile unsigned char signal_ok = 0; 	// Frequenz vorhanden, wenn 1 oder 2
volatile unsigned int counter_ms = 0;

ISR( TIMER1_CAPT_vect )
{
  static unsigned char ErsteFlanke = TRUE;
  signal_ok = 1;

  if( UpdateDisplay )          // Das Display wurde mit den Ergebnissen der vorhergehenden
    return;                    // Messung noch nicht upgedated. Die naechste Messung
                               // verzögern, bis die Start und EndTime Variablen wieder
                               // gefahrlos beschrieben werden koennen

  //
  // Bei der ersten Flanke beginnt die Messung, es wird der momentane
  // Timer beim Input Capture als Startwert gesichert
  //
  if( ErsteFlanke )
  {
    StartTime = ICR1;
    NrOverflows = 0;
    ErsteFlanke = FALSE;       // Die naechste Flanke ist das Ende der Messung
  }

  //
  // das ist die zweite Flanke im Messzyklus. Die Messung wird gestoppt
  //
  else
  {
    EndTime = ICR1;
    UpdateDisplay = TRUE;      // Eine vollständige Messung. Sie kann ausgewertet werden
    ErsteFlanke = TRUE;        // Bei der naechsten Flanke beginnt der naechste Messzyklus
  }
}

/**
 * Timerüberläufe werden gespeichert
 */
ISR( TIMER1_OVF_vect )
{
  NrOverflows++;
}

/**
 * Löst jede ms aus.
 */
ISR( TIMER0_COMPA_vect ){


	if (counter_ms < 1000){
		counter_ms++;
	}
	else{
		flag_1s = 1;
		if(signal_ok == 1)
			signal_ok = 2;
		else
			signal_ok = 0;
		counter_ms = 0;
	}
}

int main()
{
  char lcdCounterString[8];
  double Erg = 0.0;

  i2c_init();
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  lcd_init(LCD_DISP_ON);

  // init Timer 0 -> Zeitbasis (unabhängig von der Messung
  TCCR0A = (1<<WGM01);				// CTC-Modus
  TCCR0B = (1<<CS01) | (1<<CS00); 	// Prescaller = 64
  OCR0A = 125; 						// -> Interrupt alle 1ms
  TIMSK0 = (1<<OCIE0A); 			// Interrupt aktivieren


  // init Timer 1 -> Messung des Signals
  TCCR1B = (1<<ICES1) | (1<<ICNC1) | (1<<CS10); // Input Capture Edge, Input Cataputure Noise Canceler, kein PreScale
  TIMSK1 = (1<<ICIE1) | (1<<TOIE1); // Interrupts akivieren, Capture + Overflow

  sei();

  lcd_clrscr();
  lcd_puts("Drehzahlmesser");

  // Gleitender Mittelwert
  // Datenstruktur anlegen:
  tFloatAvgFilter filterFrequency;

  // initialisieren und mit 0 fuellen
  InitFloatAvg(&filterFrequency, 0);


  while(1)
  {
    //
    // liegt eine vollständige Messung vor?
    //
    if( UpdateDisplay )
    {

      //
      // Die Zeitdauer zwischen den Flanken bestimmen
      // Da EndTime und StartTime unsigned sind, braucht nicht
      // darauf Ruecksicht genommen werden, dass EndTime auch
      // kleiner als StartTime sein kann. Es kommt trotzdem
      // das richtige Ergebnis raus.
      // Allerdings muss die Anzahl der Overflows in der Messperiode
      // beruecksichtigt werden
      //
      // Die Zeitdauer wird als Anzahl der Taktzyklen zwischen den
      // beiden gemessenen Flanken berechnet ...
      Erg = (NrOverflows * 65536) + EndTime - StartTime;

      // ... mit der bekannten Taktfrequenz ergibt sich dann die Signalfrequenz
      Erg = F_CPU / Erg;       // f = 1 / t

      // der Mittelwerts berrechnung zuführen
      AddToFloatAvg(&filterFrequency, Erg);

      //
      // Das Ergebnis fuer die Anzeige aufbereiten ...
      //
      dtostrf( GetOutputValue(&filterFrequency), 5, 1, lcdCounterString );  // 3 Nachkommastellen
//      dtostrf( Erg, 5, 3, lcdCounterString );  // 3 Nachkommastellen

      //
      // ... und ausgeben
      //
      uart_puts(lcdCounterString);
      uart_putc('\n');

      //
      // Das wars: Display ist wieder up to date
      // die naechste Messung kann starten
      //
      UpdateDisplay = FALSE;
    }

    //
	// jede Sekunde auf dem Display ausgeben
	if(flag_1s){
	  flag_1s = 0;
	  lcd_clrscr();

	  // Signal vorhanden, dann Ausgeben
	  if(signal_ok){
		lcd_puts("F=");
		lcd_puts(lcdCounterString);
		lcd_puts("Hz");
	  }
	  else{
		  lcd_puts("kein Signal");
	  }

	}
  }
}
