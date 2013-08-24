./* SeguidorOficial1 - an application for the Pololu 3pi Robot
 *
 * This application uses the Pololu AVR C/C++ Library.  For help, see:
 * -User's guide: http://www.pololu.com/docs/0J20
 * -Command reference: http://www.pololu.com/docs/0J18
 *
 * Created: 9/14/2012 9:02:20 AM
 *  Author: Thelma
 */

#include <pololu/3pi.h>

void initialize()
{
	unsigned int counter; // used as a simple timer
	

	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	
	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_B))
	{
		lcd_goto_xy(0,0);
		print("Press B");
		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);

	// Auto-calibration: turn right and left while calibrating the
	// sensors.
	for(counter=0;counter<80;counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(40,-40);
		else
			set_motors(-40,40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
		calibrate_line_sensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	set_motors(0,0);
}

void meta_ocupada(){
	
	unsigned int dist = 0;
	unsigned int todas = 0;
	
	for (int x = 0; x < 5; x++){
		if (x > 0 && x < 4)
		{
			delay_ms(40);
		}
		dist = analog_read(7);
		todas += dist;
	}
	
	
	if (todas >= 1000)
	{
		return true;
	} 
	else 
	{
		return false;
	}

	//return false;
}

void seguir_linea_instante(int posicion_linea, int m_max, int m_moderada, int umbral_zona_moderada ){

	int medio = 2000;
	
	int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
	
	float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); //
	int b = m_max - pendiente_zona_dastica*medio;
	
	int diferencia_motores = 0; // m1 - m2
	
	int m1 = 0;
	int m2 = 0;
	
	if (posicion_linea_centrada > umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
	}
	else if (posicion_linea_centrada < -umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
	} else {
		double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
		diferencia_motores = m_moderada*x*x*x;
	}
	
	if(diferencia_motores > 0)
	{
		m1 = m_max;
		m2 = m_max - diferencia_motores;
	}
	else if (diferencia_motores < 0)
	{
		m2 = m_max;
		m1 = m_max + diferencia_motores;
	} else
	{
		m1 = m_max;
		m2 = m_max;
	}
	
	clear();
	print_long(m1);
	print("  ");
	print_long(m2);
	lcd_goto_xy(0,1);
	print_long(diferencia_motores);
	
	set_motors(m1,m2);
}

int main()
{

	initialize();

	while(!button_is_pressed(BUTTON_B))
	{
		lcd_goto_xy(0,0);
		print("Press B");
		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);

	 
	unsigned int sensores[5]; 
	int medio = 2000;
	
	int m_max = 200;
	int m_moderada = 190;
	int umbral_zona_moderada = 1200;


	while(1)
	{
		unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
		
		set_motors(m_max,m_max);
		
		int extremos = sensores[0] + sensores[4];
		
		if (extremos < 1700)
		{
			set_motors(0,0);
			delay_ms(2000);
			break;
		}
	
		
	}
	
	while(1)
	{
		
		unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
		
		int todos = 0;
		for (int x = 0; x < 5; x++)
			todos += sensores[x];
		 
		if (todos == 5000)
		{	
			set_motors(0,0);
			delay_ms(1000);
			break;
		}
		
		int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
		
		float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); // 
		int b = m_max - pendiente_zona_dastica*medio;
		
		int diferencia_motores = 0; // m1 - m2
		
		int m1 = 0;
		int m2 = 0;
				 
		if (posicion_linea_centrada > umbral_zona_moderada)
		{
			diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
		}
		else if (posicion_linea_centrada < -umbral_zona_moderada)
		{
			diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
		} else {
			double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
			diferencia_motores = m_moderada*x*x*x;	
		}			
				
		if(diferencia_motores > 0) 
		{
			m1 = m_max;
			m2 = m_max - diferencia_motores;
		}
		else if (diferencia_motores < 0)
		{
			m2 = m_max;
			m1 = m_max + diferencia_motores;
		} else 
		{
			m1 = m_max;
			m2 = m_max;
		}
		
		clear();
		print_long(m1);
		print("  ");
		print_long(m2);
		lcd_goto_xy(0,1);
		print_long(diferencia_motores);
		set_motors(m1,m2);
	}

while(1)
{
	
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
/*	int todos = 0;
	for (int x = 0; x < 5; x++)
	todos += sensores[x];
	
	if (todos < 5000)
	{
		break;
	}*/

	int extremos = sensores[0] + sensores[4];
	
	if (extremos < 2000)
	{
		set_motors(m_max,m_max);
		delay_ms(20);
		break;
	}
	
	int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
	
	float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); //
	int b = m_max - pendiente_zona_dastica*medio;
	
	int diferencia_motores = 0; // m1 - m2
	
	int m1 = 0;
	int m2 = 0;
	
	if (posicion_linea_centrada > umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
	}
	else if (posicion_linea_centrada < -umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
	} else {
		double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
		diferencia_motores = m_moderada*x*x*x;
	}
	
	if(diferencia_motores > 0)
	{
		m1 = m_max;
		m2 = m_max - diferencia_motores;
	}
	else if (diferencia_motores < 0)
	{
		m2 = m_max;
		m1 = m_max + diferencia_motores;
	} else
	{
		m1 = m_max;
		m2 = m_max;
	}
	
	clear();
	print_long(m1);
	print("  ");
	print_long(m2);
	lcd_goto_xy(0,1);
	print_long(diferencia_motores);
	set_motors(m1,m2);
}

while(1)
{
	
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	int extremos = sensores[0] + sensores[4];
	
	if (extremos == 2000)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
	
	int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
	
	float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); //
	int b = m_max - pendiente_zona_dastica*medio;
	
	int diferencia_motores = 0; // m1 - m2
	
	int m1 = 0;
	int m2 = 0;
	
	if (posicion_linea_centrada > umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
	}
	else if (posicion_linea_centrada < -umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
	} else {
		double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
		diferencia_motores = m_moderada*x*x*x;
	}
	
	if(diferencia_motores > 0)
	{
		m1 = m_max;
		m2 = m_max - diferencia_motores;
	}
	else if (diferencia_motores < 0)
	{
		m2 = m_max;
		m1 = m_max + diferencia_motores;
	} else
	{
		m1 = m_max;
		m2 = m_max;
	}
	
	clear();
	print_long(m1);
	print("  ");
	print_long(m2);
	lcd_goto_xy(0,1);
	print_long(diferencia_motores);
	set_motors(m1,m2);
}

while (1)
{
	
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	set_motors(m_max,0);
	if (sensores[0] < 1000)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
}

while (1)
{
	
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	set_motors(m_max,0);
	if (posicion_linea < 2000)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
}


while(1)
{
	
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	int laterales = sensores[0] + sensores[1] + sensores[2];
	
	if (laterales >= 2500)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
	
	int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
	
	float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); //
	int b = m_max - pendiente_zona_dastica*medio;
	
	int diferencia_motores = 0; // m1 - m2
	
	int m1 = 0;
	int m2 = 0;
	
	if (posicion_linea_centrada > umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
	}
	else if (posicion_linea_centrada < -umbral_zona_moderada)
	{
		diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
	} else {
		double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
		diferencia_motores = m_moderada*x*x*x;
	}
	
	if(diferencia_motores > 0)
	{
		m1 = m_max;
		m2 = m_max - diferencia_motores;
	}
	else if (diferencia_motores < 0)
	{
		m2 = m_max;
		m1 = m_max + diferencia_motores;
	} else
	{
		m1 = m_max;
		m2 = m_max;
	}
	
	clear();
	print_long(m1);
	print("  ");
	print_long(m2);
	lcd_goto_xy(0,1);
	print_long(diferencia_motores);
	set_motors(m1,m2);
}

while(1){
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	set_motors(0,m_max);
	if ( (sensores[0] + sensores[1]) < 1000)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
}

while(1){
	unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
	
	set_motors(0,m_max);
	if ( sensores[2] >= 900)
	{
		set_motors(0,0);
		delay_ms(1000);
		break;
	}
}



	
	while(1)
	{
		
		unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
		
		int todos = 0;
		for (int x = 0; x < 5; x++)
		todos += sensores[x];
		
		if (todos == 5000)
		{
			set_motors(0,0);
			delay_ms(1000);
			break;
		}
		
		int posicion_linea_centrada = ((int)posicion_linea) - medio; // Si la linea esta en el centro el valor es 0, en los sensores extremos -2000 o 2000.
		
		float pendiente_zona_dastica = ((float)(m_max - m_moderada)/(medio - umbral_zona_moderada)); //
		int b = m_max - pendiente_zona_dastica*medio;
		
		int diferencia_motores = 0; // m1 - m2
		
		int m1 = 0;
		int m2 = 0;
		
		if (posicion_linea_centrada > umbral_zona_moderada)
		{
			diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada + b;
		}
		else if (posicion_linea_centrada < -umbral_zona_moderada)
		{
			diferencia_motores = pendiente_zona_dastica*posicion_linea_centrada - b;
		} else {
			double x = ((double)posicion_linea_centrada)/(umbral_zona_moderada);
			diferencia_motores = m_moderada*x*x*x;
		}
		
		if(diferencia_motores > 0)
		{
			m1 = m_max;
			m2 = m_max - diferencia_motores;
		}
		else if (diferencia_motores < 0)
		{
			m2 = m_max;
			m1 = m_max + diferencia_motores;
		} else
		{
			m1 = m_max;
			m2 = m_max;
		}
		
		clear();
		print_long(m1);
		print("  ");
		print_long(m2);
		lcd_goto_xy(0,1);
		print_long(diferencia_motores);
		set_motors(m1,m2);
	}

	while(1){
		
			unsigned int posicion_linea = read_line(sensores,IR_EMITTERS_ON);
			
			set_motors(m_max,m_max);
			
				
			if (sensores[2] < 100)
			{
				set_motors(0,0);
				break;
			}
				
	}


}

