/*******************************************************************************
 * Author  : Stefan Werntges
 * License : CC
 * Project : https://github.com/Jormuntgant/ArduinoAirsoft.git
 * Desciption:
 * 
 * Using a Piezo sound transducer to disguise your Enemy? Why the hell not?
 * This is the Code for the Attiny85 (Receiver + Soundoutput)
 * 
********************************************************************************
 * Credits and rights for:
 * VirtualWire (library for Communication) : Copyright (C) 2008 Mike McCauley
********************************************************************************
 * ToDo:
 * no reported bugs
********************************************************************************
 * USE:
 * Switch Power on 				-> The Test LED should start to blink
 * Send the same name 3 times 	-> LED stays on
 * 
 * Send the name 				-> LED on + Piezo on this Frequenz
 * Send the name again	 		-> Deactivate the Sound, ready for new frequenz
 * 
 * commands:
 * <name>:<XXXXX> 	-> <name> is the Name -> :F -> <XXXXX> is the tone value
 * <name>:noTone	-> <name> is the Name -> :noTone disables the sound
*******************************************************************************/

#include <VirtualWire.h>

#define VW_RECV_PIN 2
#define SOUND_PIN 3
#define LED_TEST_PIN 4

//#define debug

String myName = "";
int mySound = LOW;

uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;

void setup()
{
	pinMode(VW_RECV_PIN,INPUT);
	pinMode(SOUND_PIN,OUTPUT);
	pinMode(LED_TEST_PIN,OUTPUT);
	
	vw_set_ptt_inverted(true);
	vw_set_rx_pin(VW_RECV_PIN);
	vw_setup(1000);
	vw_rx_start();
	
#ifdef debug
	Serial.begin(9600);
	Serial.println("SoundGrenade debug mode");
#endif
	
	digitalWrite(LED_TEST_PIN,HIGH);
	
	int count=0;
	unsigned long t = millis();
	int stat = HIGH;
	
	String testName = "";
	
	while(count<2)
	{
		if(t+500<millis())
		{
			if(stat == HIGH)
				stat=LOW;
			else
				stat=HIGH;
			digitalWrite(LED_TEST_PIN,stat);
			t=millis();
		}
		
		if(vw_get_message(buf,&buflen))
		{
			for(int i=0;i<buflen;i++)
			{
				testName.concat(buf[i]);
			}
			if(myName != testName)
			{
				myName = testName;
				count=0;
			}
			else
				count++;
			testName="";
			
#ifdef debug
	Serial.print("myName: ");
	Serial.println(myName);
	Serial.print("testName: ");
	Serial.println(testName);
	Serial.print("count: ");
	Serial.println(count);
#endif
		}
		

	}
	digitalWrite(LED_TEST_PIN,HIGH);
	
}

void loop()
{
	if(vw_get_message(buf,&buflen))
	{
		String cmd="";
		for(int i=0;i<buflen;i++)
		{
			cmd.concat(buf[i]);
		}

#ifdef debug
	Serial.print("myName: ");
	Serial.println(myName);
	Serial.print("command: ");
	Serial.println(cmd);
#endif
		
		if(cmd == myName)
		{
			
#ifdef debug
	Serial.println("cmd == myName");
	Serial.print("oldState");
	Serial.println(mySound);
#endif
			digitalWrite(LED_TEST_PIN,mySound);

			if(mySound == LOW)
				mySound=HIGH;
			else
				mySound=LOW;
#ifdef debug
	Serial.print("newState");
	Serial.println(mySound);
#endif				
			digitalWrite(SOUND_PIN,mySound);
		}
#ifdef debug
	else
		Serial.println("cmd != myName");
#endif
	}
}
