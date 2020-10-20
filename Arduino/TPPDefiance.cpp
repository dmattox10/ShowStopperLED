/*
This is the Skeleton code for all ATMEGA328P based Designs.
TODO: Reverse UFO Direction? EVEN BETTER!! REVERSE STRIP INSTALLATION DIRECTION
Do we need to change "two" to "two[i]"? same for "three", "four", and "five"?
*/

// Includes
#include "Tlc5940.h" // The library for the TLC5940 PWM IC
#include "tlc_fades.h" // Separate Fading Library for TLC5940
#include <EEPROM.h> // Library for interfacing to on-chip EEPROM memory
#include "EEPROMAnything.h" // Library for writing values larger than a byte (0-256) to EEPROM memory

// EEPROM Data Structure
struct config_t // "a" through "e" are channels 1-5
{
int aChn; // we save channel just to take up space, yup. (0)
int bChn; // this one always equals 1
int cChn; // this one always equals 2
int dChn; // this one always equals 3
int eChn; // this one always equals 4
int aRed; // Channel 1 Red Value (0 - 4095)
int bRed;
int cRed;
int dRed;
int eRed; // Channel 5 Red Value
int aGrn; // Channel 1 Green Value (0 - 4095)
int bGrn;
int cGrn;
int dGrn;
int eGrn; // Channel 5 Green Value
int aBlu; // Channel 1 Blue Value (0 - 4095)
int bBlu;
int cBlu;
int dBlu;
int eBlu; // Channel 5 Blue Value
int aMod; // Channel 1 Mode (0 - 83)
int bMod;
int cMod;
int dMod;
int eMod; // Channel 5 Mode
int aBnd; // Channel 1 Audio Frequency Band (0 - 6)
int bBnd;
int cBnd;
int dBnd;
int eBnd; // Channel 5 Audio Frequency Band
int aSen; // Channel 1 Audio Sensitivity (0 - 31)
int bSen;
int cSen;
int dSen;
int eSen; // Channel 5 Audio Sensitivity
int aUni; // Channel 1 User Speed Setting (0 - 1000) ANYTHING ABOVE(below?) 970(30?) IS DANGEROUS AND PRODUCES UNPREDICTABLE RESULTS!!!!!!!!!!!!!!!
int bUni;
int cUni;
int dUni;
int eUni; // Channel 5 User Speed Setting
} config;

// Pin Definitions, Port Declarations, Library Inclusions
TLC_CHANNEL_TYPE channel;
HardwareSerial Uart = HardwareSerial();
int analogPin = 0; // MSGEQ7 read from using analog input 0
int strobePin = 4; // MSGEQ7 strobe is attached to digital pin 2
int resetPin = 5; // MSGEQ7 reset is attached to digital pin 4
int spectrumValue[7]; // holds the a2d values from input 0

// Uart Data Variable Declarations
char inData[45]; // Buffer for the incoming data
char *inParse[45]; // Buffer for the parsed data chunks
char incomingByte; // One Character at a time
char buffer[45]; // To make room for incoming data, the previous buffer copies to this one?
int serialIn = 0; // ???
int index = 0;// Incoming data id

// Color Variable Declarations
int rojo; // user defined red value
int gren; // user defined green value
int blue; // user defined blue value
int rr; // random red value
int gg; // random green value
int bb; // random blue value
int rrr;
int ggg;
int bbb;
int colors[8][3]; // This matrix stores the color values in RGB values for ROYGBIV
int usa[3][3]; // This matrix stores the color values in RGB values for Red, White, and Blue
int off; // Used to store black instead of typing it out over and over

// Timing Variable Declarations
unsigned long ms[5]; // stores timing values for each channel
int rows[5]; // stores what row we use for rainbow flash modes
int states[5]; // stores boolean values for on off states for each channel
int two;
int twos[5]; // This value keeps track of what step we are one for two step (two color) modes
int three;
int threes[5]; // This value keeps track of what step we are one for three step (three color) modes
int four;
int fours[5]; // This value keeps track of what step we are one for four step (four color) modes
int five;
int fives[5]; // This value keeps track of what step we are one for five step (five color) modes
int fades[5];
boolean started; // packet state variable, toggled after receiving the start-of-packet character and header
boolean ended; // packet state variable, toggle after recieving the end-of-packet character
unsigned long currentMillis; // timing variable, set to the current uptime of the system in milliseconds
unsigned long previousMillis; // timg varaible, stores the last time we used current time for comparison to currentMillis
int row; // give values in setup!
int srow; // CHANGE TO USE rows[1]
int ufo; // stores which strip we should be on
int ufoLast; // the previous strip we were on, needed to move from last strip to first strip
int rLast; // Stores what Red pin we last output on for UFO modes
int gLast; // Stores what Green pin we last output on for UFO modes
int bLast; // Stores what Blue pin we last output on for UFO modes
int strips[5]; // Store what pins we should use (Red pins specifically) for each strip
int unicorns; // user defined speed setting

// Ouput and Mode Variable Declarations
int chan; // current channel (strip) AREN'T WE USING "i" FOR THIS NOW???
int mode; // user defined mode
int band; // user defined audio frequency band
int sens; // user defined audio sensitivity
int pulse;
int r; // which red channel to output on
int g; // which green channel to output on
int b; // which blue channel to ouptut on
int i; // ??? I'm pretty sure we use ONLY this, and "chan" is a waste of space...
int f; // ??? Where the "f" did I use "f" ???
int address;
int data[5][10];

// Custom Function to Load data from EEPROM based on an Address passed by the phone application
void Load(int address) // The maximum memory of 1024 bytes available to us, used by a Save/Load is 180 bytes, so each address is 200 bytes apart
	{
	EEPROM_readAnything(address, config); // Given an address, Load data from EEPROM to RAM
	data[0][1]=config.aChn;
	data[1][1]=config.bChn;
	data[2][1]=config.cChn;
	data[3][1]=config.dChn;
	data[4][1]=config.eChn;
	data[0][2]=config.aRed;
	data[1][2]=config.bRed;
	data[2][2]=config.cRed;
	data[3][2]=config.dRed;
	data[4][2]=config.eRed;
	data[0][3]=config.aGrn;
	data[1][3]=config.bGrn;
	data[2][3]=config.cGrn;
	data[3][3]=config.dGrn;
	data[4][3]=config.eGrn;
	data[0][4]=config.aBlu;
	data[1][4]=config.bBlu;
	data[2][4]=config.cBlu;
	data[3][4]=config.dBlu;
	data[4][4]=config.eBlu;
	data[0][5]=config.aMod;
	data[1][5]=config.bMod;
	data[2][5]=config.cMod;
	data[3][5]=config.dMod;
	data[4][5]=config.eMod;
	data[0][6]=config.aBnd;
	data[1][6]=config.bBnd;
	data[2][6]=config.cBnd;
	data[3][6]=config.dBnd;
	data[4][6]=config.eBnd;
	data[0][7]=config.aSen;
	data[1][7]=config.bSen;
	data[2][7]=config.cSen;
	data[3][7]=config.dSen;
	data[4][7]=config.eSen;
	data[0][8]=config.aUni;
	data[1][8]=config.bUni;
	data[2][8]=config.cUni;
	data[3][8]=config.dUni;
	data[4][8]=config.eUni;
	}
	
void Save(int address)
	{
	config.aChn=data[0][1];
	config.bChn=data[1][1];
	config.cChn=data[2][1];
	config.dChn=data[3][1];
	config.eChn=data[4][1];
	config.aRed=data[0][2];
	config.bRed=data[1][2];
	config.cRed=data[2][2];
	config.dRed=data[3][2];
	config.eRed=data[4][2];
	config.aGrn=data[0][3];
	config.bGrn=data[1][3];
	config.cGrn=data[2][3];
	config.dGrn=data[3][3];
	config.eGrn=data[4][3];
	config.aBlu=data[0][4];
	config.bBlu=data[1][4];
	config.cBlu=data[2][4];
	config.dBlu=data[3][4];
	config.eBlu=data[4][4];
	config.aMod=data[0][5];
	config.bMod=data[1][5];
	config.cMod=data[2][5];
	config.dMod=data[3][5];
	config.eMod=data[4][5];
	config.aBnd=data[0][6];
	config.bBnd=data[1][6];
	config.cBnd=data[2][6];
	config.dBnd=data[3][6];
	config.eBnd=data[4][6];
	config.aSen=data[0][7];
	config.bSen=data[1][7];
	config.cSen=data[2][7];
	config.dSen=data[3][7];
	config.eSen=data[4][7];
	config.aUni=data[0][8];
	config.bUni=data[1][8];
	config.cUni=data[2][8];
	config.dUni=data[3][8];
	config.eUni=data[4][8];
	EEPROM_writeAnything(address, config); // Given an address, Save data from RAM to EEPROM
	}
	
void doConfig(int keep) // Based on  the "keep" variable sent to initiate a  Save or Load, we determine which operation to do
	{
	if(keep < 100) // we Load for keep greater than 100
		{
		switch(keep)
			{
			case 10:
			address = 0; // Set the address where we will Load from
			Load(address); // Load the data at that address
			break;
			case 20:
			address = 200;
			Load(address);
			break;
			case 30:
			address = 400;
			Load(address);
			break;
			case 40:
			address = 600;
			Load(address);
			break;			
			case 50:
			address = 800;
			Load(address);
			break;
			}
		}
	else
		{
		switch(keep)
			{
			case 100:
			address = 0;
			Save(address);
			break;
			case 200:
			address = 200;
			Save(address);
			break;
			case 300:
			address = 400;
			Save(address);
			break;
			case 400:
			address = 600;
			Save(address);
			break;			
			case 500:
			address = 800;
			Save(address);
			break;
			}
		}
	}

// Setup Routine, runs once, each time the MCU first receives power
void setup()
	{
	Uart.begin(38400);
	Serial.begin(38400);
	Tlc.init(4095);
	colors[0][0] = 256; // red
	colors[0][1] = 0;
	colors[0][2] = 0;
	colors[1][0] = 256;// orange
	colors[1][1] = 30;
	colors[1][2] = 0;
	colors[2][0] = 256;// yellow
	colors[2][1] = 75;
	colors[2][2] = 0;
	colors[3][0] = 0;// green
	colors[3][1] = 256;
	colors[3][2] = 0;
	colors[4][0] = 0;// aqua
	colors[4][1] = 256;
	colors[4][2] = 256;
	colors[5][0] = 0;// blue
	colors[5][1] = 0;
	colors[5][2] = 256;
	colors[6][0] = 100;// indigo
	colors[6][1] = 0;
	colors[6][2] = 256;
	colors[7][0] = 180;// violet
	colors[7][1] = 0;
	colors[7][2] = 256;
	usa[0][0] = 256; // Red
	usa[0][1] = 0;
	usa[0][2] = 0;
	usa[1][0] = 256; // White
	usa[1][1] = 256;
	usa[1][2] = 100;
	usa[2][0] = 0; // Blue
	usa[2][1] = 0;
	usa[2][2] = 256;
	ms[0] = 0; // These next five lines set all of the timers to Zero
	ms[1] = 0;
	ms[2] = 0;
	ms[3] = 0;
	ms[4] = 0;
	states[0] = 0; // Set all flashing states to off
	states[1] = 0;
	states[2] = 0;
	states[3] = 0;
	states[4] = 0;
	rows[0] = 0;
	rows[1] = 0;
	rows[2] = 0;
	rows[3] = 0;
	rows[4] = 0;
	strips[0] = 0;
	strips[1] = 3;
	strips[2] = 6;
	strips[3] = 9;
	strips[4] = 12;
	i = 0;
	f = 0;
	off = 4095;
	EEPROM_readAnything(0, config); // Automagically Load the first User saved Configuration
	}
	
// The "Loop" runs over and over again many times per second
void loop()
	{
	Modes(); // Here the program runs the Modes Function where all of the cool stuff actually happens!
	char c;
	if (Uart.available()) // If there is data on the serial port, we've got work to do!
		{
		Serial.println("Uart Data Available");
		c = Uart.read();
		if(c == '<') // Start of packet 
			{
			Serial.println("SOP!");
			started = 1;
			ended = 0;
			}
		else if(c == '>') // End of packet
			{
			Serial.println("EOP!");
			ended = 1;
			}
		else // If we didn't get a packet, it's just noise, empty the buffer!
			{
			Serial.println("Got Garbage.");
			buffer[serialIn] = c; 
			serialIn++;
			buffer[serialIn] = '\0';
			}
		}
	if(started && ended) // complete packet
		{
		if(buffer[0] == 'H' && buffer[1] == 'M' && buffer[2] == 'G') //header
			{
			Serial.println("Got Packet.");
			parseUartData(); // sorts data based on channel
			}
		serialIn = 0;
		buffer[serialIn] = '\0';
		started = 0;
		ended = 0;
		}
	}
	
// The incoming packet we just saved has characters in it to separate the different values from each other, here we remove those.
void parseUartData()
	{
	Serial.println("Parsing.");
	char *p = buffer; // The data to be parsed
	char *str;   // Temp store for each data chunk
  	int counter = 0; // Id ref for each chunk
    while ((str = strtok_r(p, ":", &p)) != NULL)// Loop through the data and seperate it into chunks at each ":" delimeter
		{
		inParse[counter] = str;// Add chunk to array
        counter++;      // Increment data count
		}
	char *chn = inParse[1]; // Channel Value (0 - 4)
	char *red = inParse[2]; // Red Value (0 - 4095) for this channel
	char *grn = inParse[3]; // Green Value (0 - 4095) for this channel
	char *blu = inParse[4]; // Blue Value (0 - 4095) for this channel
	char *mode = inParse[5]; // Mode for this channel
	char *audB = inParse[6]; // Audio Frequency Band for this channel
	char *audS = inParse[7]; // Audio Sensitivity for this channel
	char *unic = inParse[8]; // Unicorns (speed) for this channel
	char *save = inParse[9]; // Save Value
	char *sync = inParse[100;
	sortData(chn,red,grn,blu,mode,audB,audS,unic,save,sync); // Sort the data we now have into a matrix so it is all accesible at once
	}
	
// The Parsed Uart Data needs to be moved to RAM to make room for incoming Uart Data
void sortData(char *chn, char *red, char *grn, char *blu, char *mode, char *audB, char *audS, char *unic, char *save, char *sync)
	{
	Serial.println("Sorting.");
	int chan = atoi(chn); // The index or key we use to know where the rest of the data goes
	int arr = atoi(red);
	int gee = atoi(grn);
	int bee = atoi(blu);
	int emm = atoi(mode);
	int bnd = atoi(audB);
	int ess = atoi(audS);
	int uni = atoi(unic);
	int sync = atoi(sync);
	data[chan][0] = chan;
	data[chan][1] = arr; // Convert the characters to integers from Uart data (red)
	data[chan][2] = gee; // the green value
	data[chan][3] = bee; // blue
	data[chan][4] = emm; // the modes are commented in at the bottom
	data[chan][5] = bnd; // audio frequency band
	data[chan][6] = ess; // audio sensitivity
	data[chan][7] = uni; //
	data[chan][8] = sync;
	int keep = atoi(save); // We save immediately after this if keep > 1
	Serial.println("Channel");
	Serial.println(chan);
	Serial.println();
	Serial.println("RED");
	Serial.println(arr);
	Serial.println("GREEN");
	Serial.println(gee);
	Serial.println("BLUE");
	Serial.println(bee);
	Serial.println("Mode");
	Serial.println(emm);
	Serial.println("Band");
	Serial.println(bnd);
	Serial.println("Sensitivity");
	Serial.println(ess);
	Serial.println("Speed");
	Serial.println(uni);
	Serial.println("Save?");
	Serial.println(keep);
	Serial.println("Sync?");
	Serial.println(sync);
	if(keep > 0)
		{
		doConfig(keep);
		keep = 0; // Reset the value so we don't just keep Saving/Loading over and over and make the pretty lights stop!
		}
	}
	
// As many times per second that loop is run, we apply the modes!
void Modes()
	{
	for (i = 0; i < 5; i++) // starting with channel 0, apply settings to strips, then move to the next channel.
		{
		if (i == 0 && data[i][8] == 1)
			{
			for (int s = 1; s < 5; s++)
				{
				data[s][4] = data[0][4];
				}
			Serial.println("Synchronized.");
			}
		if(twos[i] > 1)
			{
			twos[i] = 0;
			}
		if(threes[i] > 2)
			{		
			threes[i] = 0;
			}
		if(fours[i] > 3)
			{		
			fours[i] = 0;
			}
		if(fives[i] > 4)
			{		
			fives[i] = 0;
			}
		if(rows[i] > 7) // if there are black cycles, each of these must be tested and reset for each channel every loop, outside of Modes()
			{
			rows[i] = 0;
			}
		if(strips[i] > 3)
			{
			strips[i] = 0;
			}
		row = rows[i];
		two = twos[i];
		three = threes[i];
		four = fours[i];
		five = fives[i];
		if(ufo > 3)
			{
			ufo = 0;
			}
		switch(ufo) // depending on what strip we are on, we know where we came from, and where we are going!
			{
			case 0:
			ufoLast = 3;
			break;
			case 1:
			ufoLast = 0;
			break;
			case 2:
			ufoLast = 1;
			break;
			case 3:
			ufoLast = 2;
			break;
			}
		rLast = strips[ufoLast];
		gLast = rLast + 1;
		bLast = gLast + 1;
		rojo = data[i][1]; // Retrieve values from the matrix based on what channel we are working on
		gren = data[i][2];
		blue = data[i][3];
		mode = data[i][4];
		band = data[i][5];
		sens = data[i][6];
		unicorns = data[i][7];
		currentMillis =  millis(); // Every time we run "Modes" we set the "currentMillis" value to the actual curent uptime
		r = strips[i];
		g = r + 1;
		b = g + 1;
		rr = random(off);
		gg = random(off);
		bb = random(off);
		switch(mode)
			{
			case 0: //Set Color
			SetRGB(r, g, b, rojo, gren, blue);
			break;
			case 1: //1 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					SetRGB(r, g, b, rojo, gren, blue);
					}
				else
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 2: //2 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[two][1];
					gren = data[two][2];
					blue = data[two][3];
					SetRGB(r, g, b, rojo, gren, blue);
					two++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 3: //2 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[two][1];
				gren = data[two][2];
				blue = data[two][3];
				SetRGB(r, g, b, rojo, gren, blue);
				two++;
				}
			break;
			case 4: //3 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[three][1];
					gren = data[three][2];
					blue = data[three][3];
					SetRGB(r, g, b, rojo, gren, blue);
					three++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 5: //3 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[three][1];
				gren = data[three][2];
				blue = data[three][3];
				SetRGB(r, g, b, rojo, gren, blue);
				three++;
				}
			break;
			case 6: //4 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[four][1];
					gren = data[four][2];
					blue = data[four][3];
					SetRGB(r, g, b, rojo, gren, blue);
					four++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 7: //4 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[four][1];
				gren = data[four][2];
				blue = data[four][3];
				SetRGB(r, g, b, rojo, gren, blue);
				four++;
				}
			break;
			case 8: //5 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[five][1];
					gren = data[five][2];
					blue = data[five][3];
					SetRGB(r, g, b, rojo, gren, blue);
					five++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 9: //5 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[five][1];
				gren = data[five][2];
				blue = data[five][3];
				SetRGB(r, g, b, rojo, gren, blue);
				five++;
				}
			break;
			case 10: //Red Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 11: //Yellow Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 12: //Green Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = 0;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 13: //Aqua Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 14: //Blue Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 15: //Pink Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = 0;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = 0;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 16: //White Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 17: //Sync Audio Psycho!
			audIn(band);
			if (pulse > sens)
				{
				SyncPsycho();
				}
			break;
			case 18: //Audio 2 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[two][1];
				gren = data[two][2];
				blue = data[two][3];
				SetRGB(r, g, b, rojo, gren, blue);
				two++;
				}
			break;
			case 19: //Audio 3 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[three][1];
				gren = data[three][2];
				blue = data[three][3];
				SetRGB(r, g, b, rojo, gren, blue);
				three++;
				}
			break;
			case 20: //Audio 4 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[four][1];
				gren = data[four][2];
				blue = data[four][3];
				SetRGB(r, g, b, rojo, gren, blue);
				four++;
				}
			break;
			case 21: //Audio 5 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[five][1];
				gren = data[five][2];
				blue = data[five][3];
				SetRGB(r, g, b, rojo, gren, blue);
				five++;
				}
			break;
			case 22: //USA Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = usa[three][0];
					gren = usa[three][1];
					blue = usa[three][2];
					SetRGB(r, g, b, rojo, gren, blue);
					three++;
					}
				else
					{
					states[i] = 0;
					SetOff(r, g, b);	
					}
				}
			break;
			case 23: //USA Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				states[i] = 1;
				rojo = usa[three][0];
				gren = usa[three][1];
				blue = usa[three][2];
				SetRGB(r, g, b, rojo, gren, blue);
				three++;
				}
			break;
			case 24: //Rainbow Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = colors[row][0];
					gren = colors[row][1];
					blue = colors[row][2];
					SetRGB(r, g, b, rojo, gren, blue);
					row++;
					}
				else
					{
					states[i] = 0;
					SetOff(r, g, b);				
					}
				}
			break;
			case 25: //(you're an old man when you get here!) Rainbow Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				rojo = colors[row][0];
				gren = colors[row][1];
				blue = colors[row][2];
				SetRGB(r, g, b, rojo, gren, blue);
				row++;
				}
			break;
			case 26: //Random Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = rr;
					gren = gg;
					blue = bb;
					SetRGB(r, g, b, rojo, gren, blue);
					}
				else
					{
					states[i] = 0;
					SetOff(r, g, b);				
					}
				}
			break;
			case 27: //Random Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				rojo = rr;
				gren = gg;
				blue = bb;
				SetRGB(r, g, b, rojo, gren, blue);
				}
			break;
			case 28: //Audio Rainbow Replace
			audIn(band);
			if (pulse > sens)
				{
				ms[i]= currentMillis;
				rojo = colors[row][0];
				gren = colors[row][1];
				blue = colors[row][2];
				SetRGB(r, g, b, rojo, gren, blue);
				row++;
				}
			break;
			case 29: //Audio Random Replace
			audIn(band);
			if (pulse > sens)
				{
				ms[i]= currentMillis;
				rojo = rr;
				gren = gg;
				blue = bb;
				SetRGB(r, g, b, rojo, gren, blue);
				}
			break;
			case 30: //Audio Red Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = 0;
				blue = 0;
				SetRGB(r, g, b, rojo, gren, blue);
				}
			else
				{
				SetOff(r, g, b);	
				}
			break;
			case 31: //Audio Orange Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = pulse;
				blue = 0;
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 32: //Audio Yellow Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = pulse * 3;
				blue = 0;
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 33: //Audio Green Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = 0;
				gren = pulse * 4;
				blue = 0;
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 34: //Audio Aqua Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = 0;
				gren = pulse * 4;
				blue = pulse * 4;
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 35: //Audio Blue Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = 0;
				gren = 0;
				blue = pulse * 4;
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 36: //Audio Violet Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = 0;
				blue = pulse * 2; // RIGHT HERE!!! IF IT DOESNT LOOK PURPLE ENOUGH CHANGE ME!!!
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 37: //Audio Pink Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = 0;
				blue = pulse * 4; 
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 38: //Audio White Level
			audIn(band);
			if(pulse > sens)
				{
				rojo = pulse * 4;
				gren = pulse * 4;
				blue = pulse * 2; // RIGHT HERE!!! IF IT DOESNT LOOK White ENOUGH CHANGE ME!!!
				SetRGB(r, g, b, rojo, gren, blue);			
				}
			else
				{
				SetOff(r, g, b);
				}
			break;
			case 39: //Sync UFO Set Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO SET COLOR
				{
				ms[0] = currentMillis;
				rrr = data[0][1];
				ggg = data[0][2];
				bbb = data[0][3];
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 40: //Sync UFO 2 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					two = twos[0];
					rrr = data[two][1];
					ggg = data[two][2];
					bbb = data[two][3];
					two++;
					twos[0] = two;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 41: //Sync UFO 3 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					three = threes[0];
					rrr = data[three][1];
					ggg = data[three][2];
					bbb = data[three][3];
					three++;
					threes[0] = three;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 42: //Sync UFO 4 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					four = fours[0];
					rrr = data[four][1];
					ggg = data[four][2];
					bbb = data[four][3];
					four++;
					fours[0] = four;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 43: //Sync UFO 5 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					five = fives[0];
					rrr = data[five][0];
					ggg = data[five][1];
					bbb = data[five][2];
					five++;
					fives[0] = five;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 44: //Sync UFO Rainbow Flash 
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					row++;
					rows[0] = row;
					}
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 45: //Sync UFO Rainbow Flash Cycle (New color each strip)
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				rrr = rrr * 16;
				rrr = 4095 - rrr;
				ggg = ggg * 16;
				ggg = 4095 - ggg;
				bbb = bbb * 16;
				bbb = 4095 - bbb;
				row++;
				rows[0] = row;
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 46: //Sync UFO Rainbow Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					row++;
					rows[0] = row;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 47: //Sync UFO Rainbow Fill Cycle
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				rrr = rrr * 16;
				rrr = 4095 - rrr;
				ggg = ggg * 16;
				ggg = 4095 - ggg;
				bbb = bbb * 16;
				bbb = 4095 - bbb;
				row++;
				rows[0] = row;
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 48: //Sync UFO Random Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					rrr = rr;
					ggg = gg;
					bbb = bb;
					}
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 49: //Sync UFO Random Flash Cycle
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				rrr = rr;
				ggg = gg;
				bbb = bb;
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 50: //Sync UFO Random Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					rrr = rr;
					ggg = gg;
					bbb = bb;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 51: //Sync UFO Random Fill Cycle
			if(currentMillis - ms[0] > unicorns)
				{
				ms[0] = currentMillis;
				rrr = rr;
				ggg = gg;
				bbb = bb;
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 52: //Sync UFO Audio Rainbow Fill
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					row++;
					rows[0] = row;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 53: //Sync UFO Audio Rainbow Cycle
			audIn(band);
			if(pulse > sens)
				{
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				rrr = rrr * 16;
				rrr = 4095 - rrr;
				ggg = ggg * 16;
				ggg = 4095 - ggg;
				bbb = bbb * 16;
				bbb = 4095 - bbb;
				row++;
				rows[0] = row;
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 54: //Sync UFO Audio Set Color
			audIn(band);
			if(pulse > sens)
				{
				rrr = data[0][1];
				ggg = data[0][2];
				bbb = data[0][3];
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 55: //Sync UFO Audio 2 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					two = twos[0];
					rrr = data[two][1];
					ggg = data[two][2];
					bbb = data[two][3];
					two++;
					twos[0] = two;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 56: //Sync UFO Audio 3 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					three = threes[0];
					rrr = data[three][1];
					ggg = data[three][2];
					bbb = data[three][3];
					three++;
					threes[0] = three;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 57: //Sync UFO Audio 4 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					four = fours[0];
					rrr = data[four][1];
					ggg = data[four][2];
					bbb = data[four][3];
					four++;
					fours[0] = four;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 58: //Sync UFO Audio 5 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					five = fives[0];
					rrr = data[five][1];
					ggg = data[five][2];
					bbb = data[five][3];
					five++;
					fives[0] = five;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 59: //Sync Red Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 60: // Sync Yellow Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 61: //Green Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i] = currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 62: //Aqua Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 63: //Blue Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 64: //Pink Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = 0;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = 0;
					blue = fades[i] * 16;
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 65: //White Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 256)
						{
						fades[i] = 256;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i] * 16;
					gren = fades[i] * 16;
					blue = fades[i] * 16;
					SyncRGB(rojo, gren, blue);
					fades[i]--;
					if(fades[i] < 0)
						{
						fades[i] = 0;
						states[i] = 0;
						}
					}
				break;
				}
			break;
			case 66: //Sync Time Psycho!
			if(currentMillis - ms[0] > unicorns)
				{
				SyncPsycho();
				}
			break;
			case 67: //Sync 2 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[two][1];
					gren = data[two][2];
					blue = data[two][3];
					SyncRGB(rojo, gren, blue);
					two++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 68: //Sync 2 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = data[two][1];
				gren = data[two][2];
				blue = data[two][3];
				SyncRGB(rojo, gren, blue);
				two++;
				}
			break;
			case 69: // Haha, 69, hehe, Sync 3 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[three][1];
					gren = data[three][2];
					blue = data[three][3];
					SyncRGB(rojo, gren, blue);
					three++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 70: //Sync 3 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = data[three][1];
				gren = data[three][2];
				blue = data[three][3];
				SyncRGB(rojo, gren, blue);
				three++;
				}
			break;
			case 71: //Sync 4 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[four][1];
					gren = data[four][2];
					blue = data[four][3];
					SyncRGB(rojo, gren, blue);
					four++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 72: //Sync 4 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = data[four][1];
				gren = data[four][2];
				blue = data[four][3];
				SyncRGB(rojo, gren, blue);
				four++;
				}
			break;
			case 73: //Sync 5 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[five][1];
					gren = data[five][2];
					blue = data[five][3];
					SyncRGB(rojo, gren, blue);
					five++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 74: //Sync 5 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = data[five][1];
				gren = data[five][2];
				blue = data[five][3];
				SyncRGB(rojo, gren, blue);
				five++;
				}
			break;
			case 75: //Sync Rainbow Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = colors[row][1];
					gren = colors[row][2];
					blue = colors[row][3];
					SyncRGB(rojo, gren, blue);
					row++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 76: //Sync Rainbow Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = colors[row][1];
				gren = colors[row][2];
				blue = colors[row][3];
				SyncRGB(rojo, gren, blue);
				rows[0]++;
				}
			break;
			case 77: //Sync Random Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = rr;
					gren = gg;
					blue = bb;
					SyncRGB(rojo, gren, blue);
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 78: //Sync Random Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = rr;
				gren = gg;
				blue = bb;
				SyncRGB(rojo, gren, blue);
				}
			break;
			case 79: //Sync Audio Rainbow Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = colors[row][1];
				gren = colors[row][2];
				blue = colors[row][3];
				SyncRGB(rojo, gren, blue);
				row++;
				}
			break;
			case 80: //Sync Audio Random Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = rr;
				gren = gg;
				blue = bb;
				SyncRGB(rojo, gren, blue);
				}
			break;
			}
		rows[i] = row;
		twos[i] = two;
		threes[i] = three;
		fours[i] = four;
		fives[i] = five;
		}
	}
	
int audIn(int band)
	{
	digitalWrite(resetPin, HIGH);
	digitalWrite(resetPin, LOW);
	for (int a = 0; a < 7; a++)
		{
		digitalWrite(strobePin, LOW);
		delayMicroseconds(30); // to allow the output to settle
		spectrumValue[i] = analogRead(analogPin);
		digitalWrite(strobePin, HIGH);
		}
	pulse = spectrumValue[band];
	pulse = pulse * 2; // WE ARE GOING TO END UP MODIFYING THIS!!!!!!!!!!!!!!!
	return pulse;
	}
	
void SetRGB(int r, int g, int b, int rojo, int gren, int blue)
	{
	Tlc.set(r, rojo); // Set the Red pin, to Red value, based on channel
	Tlc.set(g, gren);
	Tlc.set(b, blue);
	Tlc.update(); // Apply the changes
	}

void SetOff(int r, int g, int b)
	{
	Tlc.set(r, off);
	Tlc.set(g, off);
	Tlc.set(b, off);
	Tlc.update();
	}
	
void SyncRGB(int rojo, int gren, int blue)
	{
	Tlc.set(0, rojo);
	Tlc.set(1, gren);
	Tlc.set(2, blue);
	Tlc.set(3, rojo);
	Tlc.set(4, gren);
	Tlc.set(5, blue);
	Tlc.set(6, rojo);
	Tlc.set(7, gren);
	Tlc.set(8, blue);
	Tlc.set(9, rojo);
	Tlc.set(10, gren);
	Tlc.set(11, blue);
	Tlc.set(12, rojo);
	Tlc.set(13, gren);
	Tlc.set(14, blue);
	Tlc.update();
	}
	
void SyncOff()
	{
	Tlc.setAll(off);
	Tlc.update();
	}
	
void SyncPsycho()
	{
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(0, rojo);
	Tlc.set(1, gren);
	Tlc.set(2, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(3, rojo);
	Tlc.set(4, gren);
	Tlc.set(5, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(6, rojo);
	Tlc.set(7, gren);
	Tlc.set(8, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(9, rojo);
	Tlc.set(10, gren);
	Tlc.set(11, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(12, rojo);
	Tlc.set(13, gren);
	Tlc.set(14, blue);
	Tlc.update();
	}
	
void UFOFlash(int ufo, int rrr, int ggg, int bbb, int rLast, int gLast, int bLast)
	{
	r = strips[ufo];
	g = r + 1;
	b = g + 1;
	Tlc.set(r, rrr);
	Tlc.set(g, ggg);
	Tlc.set(b, bbb);
	Tlc.set(12, rrr);
	Tlc.set(13, ggg);
	Tlc.set(14, bbb);
	Tlc.set(rLast, 4095);
	Tlc.set(gLast, 4095);
	Tlc.set(bLast, 4095);
	Tlc.update();
	}
	
void UFOFill(int ufo, int rrr, int ggg, int bbb)
	{
	r = strips[ufo];
	g = r + 1;
	b = g + 1;
	Tlc.set(r, rrr);
	Tlc.set(g, ggg);
	Tlc.set(b, bbb);
	Tlc.set(12, rrr);
	Tlc.set(13, ggg);
	Tlc.set(14, bbb);
	Tlc.update();
	}