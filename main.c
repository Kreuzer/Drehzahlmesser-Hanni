/**
 * Drehzahlmesser für die Hannibal Pumpe
 *
 * Abgriff der Drehzahl am "W"-Ausgang der Lichtmaschine.
 *
 * Funktionen:
 * 	- Anzeige der Drehzahl
 * 	-
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"


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



ISR( TIMER1_CAPT_vect )
{
  static unsigned char ErsteFlanke = TRUE;

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


ISR( TIMER1_OVF_vect )
{
  NrOverflows++;
}

int main()
{
  char lcdCounterString[8];
  double Erg = 0.0;

  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

  TCCR1B = (1<<ICES1)  | (1<<CS10); // Input Capture Edge, kein PreScale
  TIMSK1 = (1<<ICIE1) | (1<<TOIE1); // Interrupts akivieren, Capture + Overflow

  sei();


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

      //
      // Das Ergebnis fuer die Anzeige aufbereiten ...
      //
      dtostrf( Erg, 5, 3, lcdCounterString );  // 3 Nachkommastellen

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
  }
}
