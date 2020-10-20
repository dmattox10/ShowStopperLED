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
int fChn; // we save channel just to take up space, yup. (0)
int gChn; // this one always equals 1
int hChn; // this one always equals 2
int iChn; // this one always equals 3
int jChn; // this one always equals 4
int aRed; // Channel 1 Red Value (0 - 4095)
int bRed;
int cRed;
int dRed;
int eRed; // Channel 5 Red Value
int fRed; // Channel 1 Red Value (0 - 4095)
int gRed;
int hRed;
int iRed;
int jRed; // Channel 5 Red Value
int aGrn; // Channel 1 Green Value (0 - 4095)
int bGrn;
int cGrn;
int dGrn;
int eGrn; // Channel 5 Green Value
int fGrn; // Channel 1 Green Value (0 - 4095)
int gGrn;
int hGrn;
int iGrn;
int jGrn; // Channel 5 Green Value
int aBlu; // Channel 1 Blue Value (0 - 4095)
int bBlu;
int cBlu;
int dBlu;
int eBlu; // Channel 5 Blue Value
int fBlu; // Channel 1 Blue Value (0 - 4095)
int gBlu;
int hBlu;
int iBlu;
int jBlu; // Channel 5 Blue Value
int aMod; // Channel 1 Mode (0 - 83)
int bMod;
int cMod;
int dMod;
int eMod; // Channel 5 Mode
int fMod; // Channel 1 Mode (0 - 83)
int gMod;
int hMod;
int iMod;
int jMod; // Channel 5 Mode
int aBnd; // Channel 1 Audio Frequency Band (0 - 6)
int bBnd;
int cBnd;
int dBnd;
int eBnd; // Channel 5 Audio Frequency Band
int fBnd; // Channel 1 Audio Frequency Band (0 - 6)
int gBnd;
int hBnd;
int iBnd;
int jBnd; // Channel 5 Audio Frequency Band
int aSen; // Channel 1 Audio Sensitivity (0 - 31)
int bSen;
int cSen;
int dSen;
int eSen; // Channel 5 Audio Sensitivity
int fSen; // Channel 1 Audio Sensitivity (0 - 31)
int gSen;
int hSen;
int iSen;
int jSen; // Channel 5 Audio Sensitivity
int aUni; // Channel 1 User Speed Setting (0 - 1000) ANYTHING ABOVE(below?) 970(30?) IS DANGEROUS AND PRODUCES UNPREDICTABLE RESULTS!!!!!!!!!!!!!!!
int bUni;
int cUni;
int dUni;
int eUni; // Channel 5 User Speed Setting
int fUni; // Channel 1 User Speed Setting (0 - 1000) ANYTHING ABOVE(below?) 970(30?) IS DANGEROUS AND PRODUCES UNPREDICTABLE RESULTS!!!!!!!!!!!!!!!
int gUni;
int hUni;
int iUni;
int jUni; // Channel 5 User Speed Setting
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
char *inParse[11]; // Buffer for the parsed data chunks
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
unsigned long ms[10]; // stores timing values for each channel
int rows[10]; // stores what row we use for rainbow flash modes
int states[10]; // stores boolean values for on off states for each channel
int two;
int twos[10]; // This value keeps track of what step we are one for two step (two color) modes
int three;
int threes[10]; // This value keeps track of what step we are one for three step (three color) modes
int four;
int fours[10]; // This value keeps track of what step we are one for four step (four color) modes
int five;
int fives[10]; // This value keeps track of what step we are one for five step (five color) modes
int fades[10];
int six;
int sixes[10];
int seven;
int sevens[10];
int eight;
int eights[10];
int nine;
int nines[10];
int ten;
int tens[10];
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
int strips[10]; // Store what pins we should use (Red pins specifically) for each strip
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
int data[10][10];
int Sync;

// Custom Function to Load data from EEPROM based on an Address passed by the phone application
void Load(int address) // The maximum memory of 1024 bytes available to us, used by a Save/Load is 180 bytes, so each address is 200 bytes apart
	{
	EEPROM_readAnything(address, config); // Given an address, Load data from EEPROM to RAM
	data[0][1]=config.aChn;
	data[1][1]=config.bChn;
	data[2][1]=config.cChn;
	data[3][1]=config.dChn;
	data[4][1]=config.eChn;
	data[5][1]=config.fChn;
	data[6][1]=config.gChn;
	data[7][1]=config.hChn;
	data[8][1]=config.iChn;
	data[9][1]=config.jChn;
	data[0][2]=config.aRed;
	data[1][2]=config.bRed;
	data[2][2]=config.cRed;
	data[3][2]=config.dRed;
	data[4][2]=config.eRed;
	data[5][2]=config.fRed;
	data[6][2]=config.gRed;
	data[7][2]=config.hRed;
	data[8][2]=config.iRed;
	data[9][2]=config.jRed;
	data[0][3]=config.aGrn;
	data[1][3]=config.bGrn;
	data[2][3]=config.cGrn;
	data[3][3]=config.dGrn;
	data[4][3]=config.eGrn;
	data[5][3]=config.fGrn;
	data[6][3]=config.gGrn;
	data[7][3]=config.hGrn;
	data[8][3]=config.iGrn;
	data[9][3]=config.jGrn;
	data[0][4]=config.aBlu;
	data[1][4]=config.bBlu;
	data[2][4]=config.cBlu;
	data[3][4]=config.dBlu;
	data[4][4]=config.eBlu;
	data[5][4]=config.fBlu;
	data[6][4]=config.gBlu;
	data[7][4]=config.hBlu;
	data[8][4]=config.iBlu;
	data[9][4]=config.jBlu;
	data[0][5]=config.aMod;
	data[1][5]=config.bMod;
	data[2][5]=config.cMod;
	data[3][5]=config.dMod;
	data[4][5]=config.eMod;
	data[5][5]=config.fMod;
	data[6][5]=config.gMod;
	data[7][5]=config.hMod;
	data[8][5]=config.iMod;
	data[9][5]=config.jMod;
	data[0][6]=config.aBnd;
	data[1][6]=config.bBnd;
	data[2][6]=config.cBnd;
	data[3][6]=config.dBnd;
	data[4][6]=config.eBnd;
	data[5][6]=config.fBnd;
	data[6][6]=config.gBnd;
	data[7][6]=config.hBnd;
	data[8][6]=config.iBnd;
	data[9][6]=config.jBnd;
	data[0][7]=config.aSen;
	data[1][7]=config.bSen;
	data[2][7]=config.cSen;
	data[3][7]=config.dSen;
	data[4][7]=config.eSen;
	data[5][7]=config.fSen;
	data[6][7]=config.gSen;
	data[7][7]=config.hSen;
	data[8][7]=config.iSen;
	data[9][7]=config.jSen;
	data[0][8]=config.aUni;
	data[1][8]=config.bUni;
	data[2][8]=config.cUni;
	data[3][8]=config.dUni;
	data[4][8]=config.eUni;
	data[5][8]=config.fUni;
	data[6][8]=config.gUni;
	data[7][8]=config.hUni;
	data[8][8]=config.iUni;
	data[9][8]=config.jUni;
	}
	
void Save(int address)
	{
	config.aChn=data[0][1];
	config.bChn=data[1][1];
	config.cChn=data[2][1];
	config.dChn=data[3][1];
	config.eChn=data[4][1];
	config.fChn=data[5][1];
	config.gChn=data[6][1];
	config.hChn=data[7][1];
	config.iChn=data[8][1];
	config.jChn=data[9][1];
	config.aRed=data[0][2];
	config.bRed=data[1][2];
	config.cRed=data[2][2];
	config.dRed=data[3][2];
	config.eRed=data[4][2];
	config.fRed=data[5][2];
	config.gRed=data[6][2];
	config.hRed=data[7][2];
	config.iRed=data[8][2];
	config.jRed=data[9][2];
	config.aGrn=data[0][3];
	config.bGrn=data[1][3];
	config.cGrn=data[2][3];
	config.dGrn=data[3][3];
	config.eGrn=data[4][3];
	config.fGrn=data[5][3];
	config.gGrn=data[6][3];
	config.hGrn=data[7][3];
	config.iGrn=data[8][3];
	config.jGrn=data[9][3];
	config.aBlu=data[0][4];
	config.bBlu=data[1][4];
	config.cBlu=data[2][4];
	config.dBlu=data[3][4];
	config.eBlu=data[4][4];
	config.fBlu=data[5][4];
	config.gBlu=data[6][4];
	config.hBlu=data[7][4];
	config.iBlu=data[8][4];
	config.jBlu=data[9][4];
	config.aMod=data[0][5];
	config.bMod=data[1][5];
	config.cMod=data[2][5];
	config.dMod=data[3][5];
	config.eMod=data[4][5];
	config.fMod=data[5][5];
	config.gMod=data[6][5];
	config.hMod=data[7][5];
	config.iMod=data[8][5];
	config.jMod=data[9][5];
	config.aBnd=data[0][6];
	config.bBnd=data[1][6];
	config.cBnd=data[2][6];
	config.dBnd=data[3][6];
	config.eBnd=data[4][6];
	config.fBnd=data[5][6];
	config.gBnd=data[6][6];
	config.hBnd=data[7][6];
	config.iBnd=data[8][6];
	config.jBnd=data[9][6];
	config.aSen=data[0][7];
	config.bSen=data[1][7];
	config.cSen=data[2][7];
	config.dSen=data[3][7];
	config.eSen=data[4][7];
	config.fSen=data[5][7];
	config.gSen=data[6][7];
	config.hSen=data[7][7];
	config.iSen=data[8][7];
	config.jSen=data[9][7];
	config.aUni=data[0][8];
	config.bUni=data[1][8];
	config.cUni=data[2][8];
	config.dUni=data[3][8];
	config.eUni=data[4][8];	
	config.fUni=data[5][8];
	config.gUni=data[6][8];
	config.hUni=data[7][8];
	config.iUni=data[8][8];
	config.jUni=data[9][8];
	EEPROM_writeAnything(address, config); // Given an address, Save data from RAM to EEPROM
	}
	
void doConfig(int keep) // Based on  the "keep" variable sent to initiate a  Save or Load, we determine which operation to do
	{
	if(keep < 10) // we Load for keep greater than 100
		{
		switch(keep)
			{
			case 1:
			address = 0; // Set the address where we will Load from
			Load(address); // Load the data at that address
			break;
			case 2:
			address = 500;
			Load(address);
			break;
			case 3:
			address = 1000;
			Load(address);
			break;
			case 4:
			address = 1500;
			Load(address);
			break;			
			case 5:
			address = 2000;
			Load(address);
			break;
			}
		}
	else
		{
		switch(keep)
			{
			case 10:
			address = 0;
			Save(address);
			break;
			case 20:
			address = 500;
			Save(address);
			break;
			case 30:
			address = 1000;
			Save(address);
			break;
			case 40:
			address = 1500;
			Save(address);
			break;			
			case 50:
			address = 2000;
			Save(address);
			break;
			}
		}
	}

// Setup Routine, runs once, each time the MCU first receives power
void setup()
	{
	Uart.begin(57000);
	Tlc.init();
	colors[0][0] = 4095; // red
	colors[0][1] = 0;
	colors[0][2] = 0;
	colors[1][0] = 4095;// orange
	colors[1][1] = 500;
	colors[1][2] = 0;
	colors[2][0] = 4095;// yellow
	colors[2][1] = 2250;
	colors[2][2] = 0;
	colors[3][0] = 0;// green
	colors[3][1] = 4095;
	colors[3][2] = 0;
	colors[4][0] = 0;// aqua
	colors[4][1] = 4095;
	colors[4][2] = 4095;
	colors[5][0] = 0;// blue
	colors[5][1] = 0;
	colors[5][2] = 4095;
	colors[6][0] = 1600;// indigo
	colors[6][1] = 0;
	colors[6][2] = 4095;
	colors[7][0] = 2880;// violet
	colors[7][1] = 0;
	colors[7][2] = 4095;
	usa[0][0] = 4095; // Red
	usa[0][1] = 0;
	usa[0][2] = 0;
	usa[1][0] = 4095; // White
	usa[1][1] = 4095;
	usa[1][2] = 1600;
	usa[2][0] = 0; // Blue
	usa[2][1] = 0;
	usa[2][2] = 4095;
	ms[0] = 0; // These next five lines set all of the timers to Zero
	ms[1] = 0;
	ms[2] = 0;
	ms[3] = 0;
	ms[4] = 0;
	ms[5] = 0; // These next five lines set all of the timers to Zero
	ms[6] = 0;
	ms[7] = 0;
	ms[8] = 0;
	ms[9] = 0;
	states[0] = 0; // Set all flashing states to off
	states[1] = 0;
	states[2] = 0;
	states[3] = 0;
	states[4] = 0;
	states[5] = 0;
	states[6] = 0;
	states[7] = 0;
	states[8] = 0;
	states[9] = 0;
	rows[0] = 0;
	rows[1] = 0;
	rows[2] = 0;
	rows[3] = 0;
	rows[4] = 0;
	rows[5] = 0;
	rows[6] = 0;
	rows[7] = 0;
	rows[8] = 0;
	rows[9] = 0;
	strips[0] = 0;
	strips[1] = 3;
	strips[2] = 6;
	strips[3] = 9;
	strips[4] = 12;
	strips[5] = 15;
	strips[6] = 18;
	strips[7] = 21;
	strips[8] = 24;
	strips[9] = 27;
	i = 0;
	f = 0;
	off = 4095;
	EEPROM_readAnything(0, config); // Automagically Load the first User saved Configuration
	}
	
// The "Loop" runs over and over again many times per second
void loop()
	{
	for(int m = 0; m < 10; m++)
		{
		if(twos[m] > 1)
			{
			twos[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(threes[m] > 2)
			{		
			threes[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(fours[m] > 3)
			{		
			fours[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(fives[m] > 4)
			{		
			fives[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(sixes[m] > 5)
			{		
			sixes[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(sevens[m] > 6)
			{		
			sevens[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(eights[m] > 7)
			{		
			eights[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(nines[m] > 8)
			{		
			nines[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(tens[m] > 9)
			{		
			tens[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{	
		if(rows[m] > 8) // if there are black cycles, each of these must be tested and reset for each channel every loop, outside of Modes()
			{
			rows[m] = 0;
			}
		}
	for(int m = 0; m < 10; m++)
		{
		if(strips[m] > 8)
			{
			strips[m] = 0;
			}
		}
	Modes(); // Here the program runs the Modes Function where all of the MAGIC happens!
	char c;
	if (Uart.available()) // If there is data on the serial port, we've got work to do!
		{
		c = Uart.read();
		if(c == '<') // Start of packet 
			{
			started = 1;
			ended = 0;
			}
		else if(c == '>') // End of packet
			{
			ended = 1;
			}
		else // If we didn't get a packet, it's just noise, empty the buffer!
			{
			buffer[serialIn] = c; 
			serialIn++;
			buffer[serialIn] = '\0';
			}
		}
	if(started && ended) // complete packet
		{
		if(buffer[0] == 'H' && buffer[1] == 'M' && buffer[2] == 'G') //header
			{
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
	char *sync = inParse[10]; // Syncronizations Value
	sortData(chn,red,grn,blu,mode,audB,audS,unic,save,sync); // Sort the data we now have into a matrix so it is all accesible at once
	}
	
// The Parsed Uart Data needs to be moved to RAM to make room for incoming Uart Data
void sortData(char *chn, char *red, char *grn, char *blu, char *mode, char *audB, char *audS, char *unic, char *save, char *sync)
	{
	int chan = atoi(chn); // The index or key we use to know where the rest of the data goes
	int arr = atoi(red);
	int gee = atoi(grn);
	int bee = atoi(blu);
	int emm = atoi(mode);
	int bnd = atoi(audB);
	int ess = atoi(audS);
	int uni = atoi(unic);
	int syn = atoi(sync);
	data[chan][0] = chan; // if nothing responds CHECK HERE!!!!
	data[chan][1] = arr; // Convert the characters to integers from Uart data (red)
	data[chan][2] = gee; // the green value
	data[chan][3] = bee; // blue
	data[chan][4] = emm; // the modes are commented in at the bottom
	data[chan][5] = bnd; // audio frequency band
	data[chan][6] = ess; // audio sensitivity
	data[chan][7] = uni; //
	data[chan][8] = syn;
	int keep = atoi(save); // We save immediately after this if keep > 1
	if(keep > 0)
		{
		doConfig(keep);
		keep = 0; // Reset the value so we don't just keep Saving/Loading over and over and make the pretty lights stop!
		}
	}
	
// As many times per second that loop is run, we apply the modes!
void Modes()
	{
	for (i = 0; i < 10; i++) // starting with channel 0, apply settings to strips, then move to the next channel.
		{
		if (i == 0 && data[i][8] == 1)
			{
			for (int s = 1; s < 10; s++)
				{
				data[s][4] = data[0][4];
				}
			}
		row = rows[i];
		two = twos[i];
		three = threes[i];
		four = fours[i];
		five = fives[i];
		six = sixes[i];
		seven = sevens[i];
		eight = eights[i];
		nine = nines[i];
		ten = tens[i];
		if(ufo > 7)
			{
			ufo = 0;
			}
		switch(ufo) // depending on what strip we are on, we know where we came from, and where we are going!
			{
			case 0:
			ufoLast = 7;
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
			case 4:
			ufoLast = 3;
			break;
			case 5:
			ufoLast = 4;
			break;
			case 6:
			ufoLast = 5;
			break;
			case 7:
			ufoLast = 6;
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
			case 10: //6 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[six][1];
					gren = data[six][2];
					blue = data[six][3];
					SetRGB(r, g, b, rojo, gren, blue);
					six++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 11: //6 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[six][1];
				gren = data[six][2];
				blue = data[six][3];
				SetRGB(r, g, b, rojo, gren, blue);
				six++;
				}
			break;
			case 12: //7 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[seven][1];
					gren = data[seven][2];
					blue = data[seven][3];
					SetRGB(r, g, b, rojo, gren, blue);
					seven++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 13: //7 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[seven][1];
				gren = data[seven][2];
				blue = data[seven][3];
				SetRGB(r, g, b, rojo, gren, blue);
				seven++;
				}
			break;
			case 14: //8 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[eight][1];
					gren = data[eight][2];
					blue = data[eight][3];
					SetRGB(r, g, b, rojo, gren, blue);
					eight++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 15: //8 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[eight][1];
				gren = data[eight][2];
				blue = data[eight][3];
				SetRGB(r, g, b, rojo, gren, blue);
				eight++;
				}
			break;
			case 16: //9 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[nine][1];
					gren = data[nine][2];
					blue = data[nine][3];
					SetRGB(r, g, b, rojo, gren, blue);
					nine++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 17: //9 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[nine][1];
				gren = data[nine][2];
				blue = data[nine][3];
				SetRGB(r, g, b, rojo, gren, blue);
				nine++;
				}
			break;
			case 18: //10 Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rojo = data[ten][1];
					gren = data[ten][2];
					blue = data[ten][3];
					SetRGB(r, g, b, rojo, gren, blue);
					ten++;
					}
				else	
					{
					states[i] = 0;
					SetOff(r, g, b);
					}
				}
			break;
			case 19: //10 Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				rojo = data[ten][1];
				gren = data[ten][2];
				blue = data[ten][3];
				SetRGB(r, g, b, rojo, gren, blue);
				five++;
				}
			break;
			case 20: //Red Fade
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
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
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
			case 21: //Yellow Fade
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
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
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
			case 22: //Green Fade
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
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
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
			case 23: //Aqua Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = fades[i];
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = fades[i];
					blue = fades[i];
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
			case 24: //Blue Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[i];
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
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
					blue = fades[i];
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
			case 25: //Pink Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = fades[i];
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i];
					gren = 0;
					blue = fades[i];
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
			case 26: //White Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = fades[i];
					SetRGB(r, g, b, rojo, gren, blue);
					fades[i]++;
					if(fades[i] > 4095)
						{
						fades[i] = 4095;
						states[i] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
					{
					ms[i]= currentMillis;
					rojo = fades[i];
					gren = fades[i];
					blue = fades[i];
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
			case 27: //Sync Audio Psycho!
			audIn(band);
			if (pulse > sens)
				{
				SyncPsycho();
				}
			break;
			case 28: //Audio 2 Color Replace
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
			case 29: //Audio 3 Color Replace
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
			case 30: //Audio 4 Color Replace
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
			case 31: //Audio 5 Color Replace
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
			case 32: //Audio 6 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[six][1];
				gren = data[six][2];
				blue = data[six][3];
				SetRGB(r, g, b, rojo, gren, blue);
				six++;
				}
			break;
			case 33: //Audio 7 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[seven][1];
				gren = data[seven][2];
				blue = data[seven][3];
				SetRGB(r, g, b, rojo, gren, blue);
				seven++;
				}
			break;
			case 34: //Audio 8 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[eight][1];
				gren = data[eight][2];
				blue = data[eight][3];
				SetRGB(r, g, b, rojo, gren, blue);
				eight++;
				}
			break;
			case 35: //Audio 9 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[nine][1];
				gren = data[nine][2];
				blue = data[nine][3];
				SetRGB(r, g, b, rojo, gren, blue);
				nine++;
				}
			break;
			case 36: //Audio 10 Color Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = data[ten][1];
				gren = data[ten][2];
				blue = data[ten][3];
				SetRGB(r, g, b, rojo, gren, blue);
				ten++;
				}
			break;
			case 37: //USA Flash
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
			case 38: //USA Replace
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
			case 39: //Rainbow Flash
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
			case 40: //(you're an old man when you get here!) Rainbow Replace
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
			case 41: //Random Color Flash
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
			case 42: //Random Color Replace
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i]= currentMillis;
				rojo = rr;
				gren = gg;
				blue = bb;
				SetRGB(r, g, b, rojo, gren, blue);
				}
			break;
			case 43: //Audio Rainbow Replace
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
			case 44: //Audio Random Replace
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
			case 45: //Audio Red Level
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
			case 46: //Audio Orange Level
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
			case 47: //Audio Yellow Level
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
			case 48: //Audio Green Level
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
			case 49: //Audio Aqua Level
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
			case 50: //Audio Blue Level
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
			case 51: //Audio Violet Level
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
			case 52: //Audio Pink Level
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
			case 53: //Audio White Level
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
			case 54: //Sync UFO Set Color Flash
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
			case 55: //Sync UFO 2 Color Fill
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
			case 56: //Sync UFO 3 Color Fill
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
			case 57: //Sync UFO 4 Color Fill
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
			case 58: //Sync UFO 5 Color Fill
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
			case 59: //Sync UFO 6 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					six = sixes[0];
					rrr = data[six][0];
					ggg = data[six][1];
					bbb = data[six][2];
					six++;
					sixes[0] = six;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 60: //Sync UFO 7 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					seven = sevens[0];
					rrr = data[seven][0];
					ggg = data[seven][1];
					bbb = data[seven][2];
					seven++;
					sevens[0] = seven;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 61: //Sync UFO 8 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					eight = eights[0];
					rrr = data[eight][0];
					ggg = data[eight][1];
					bbb = data[eight][2];
					eight++;
					eights[0] = eight;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 62: //Sync UFO 9 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					nine = nines[0];
					rrr = data[nine][0];
					ggg = data[nine][1];
					bbb = data[nine][2];
					nine++;
					nines[0] = nine;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 63: //Sync UFO 10 Color Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					ten = tens[0];
					rrr = data[ten][0];
					ggg = data[ten][1];
					bbb = data[ten][2];
					ten++;
					tens[0] = ten;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 64: //Sync UFO Rainbow Flash 
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					row++;
					rows[0] = row;
					}
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 65: //Sync UFO Rainbow Flash Cycle (New color each strip)
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				row++;
				rows[0] = row;
				UFOFlash(ufo, rrr, ggg, bbb, rLast, gLast, bLast);
				ufo++;
				}
			break;
			case 66: //Sync UFO Rainbow Fill
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					row++;
					rows[0] = row;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 67: //Sync UFO Rainbow Fill Cycle
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW ADVANCE NO CYCLES
				{
				ms[0] = currentMillis;
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				row++;
				rows[0] = row;
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 68: //Sync UFO Random Flash
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
			case 69: //Sync UFO Random Flash Cycle
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
			case 70: //Sync UFO Random Fill
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
			case 71: //Sync UFO Random Fill Cycle
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
			case 72: //Sync UFO Audio Rainbow Fill
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					row = rows[0];
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					row++;
					rows[0] = row;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 73: //Sync UFO Audio Rainbow Cycle
			audIn(band);
			if(pulse > sens)
				{
				row = rows[0];
				rrr = colors[row][0];
				ggg = colors[row][1];
				bbb = colors[row][2];
				row++;
				rows[0] = row;
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 74: //Sync UFO Audio Set Color
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
			case 75: //Sync UFO Audio 2 Color
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
			case 76: //Sync UFO Audio 3 Color
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
			case 77: //Sync UFO Audio 4 Color
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
			case 79: //Sync UFO Audio 5 Color
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
			case 80: //Sync UFO Audio 6 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					six = sixes[0];
					rrr = data[six][1];
					ggg = data[six][2];
					bbb = data[six][3];
					six++;
					sixes[0] = six;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 81: //Sync UFO Audio 7 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					seven = sevens[0];
					rrr = data[seven][1];
					ggg = data[seven][2];
					bbb = data[seven][3];
					seven++;
					sevens[0] = seven;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 82: //Sync UFO Audio 8 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					eight = eights[0];
					rrr = data[eight][1];
					ggg = data[eight][2];
					bbb = data[eight][3];
					eight++;
					eights[0] = eight;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 83: //Sync UFO Audio 9 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					nine = nines[0];
					rrr = data[nine][1];
					ggg = data[nine][2];
					bbb = data[nine][3];
					nine++;
					nines[0] = nine;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 84: //Sync UFO Audio 10 Color
			audIn(band);
			if(pulse > sens)
				{
				if (ufo == 0) // color check here. advance color. after last color, start colors over.
					{
					ten = tens[0];
					rrr = data[ten][1];
					ggg = data[ten][2];
					bbb = data[ten][3];
					ten++;
					tens[0] = ten;
					}
				UFOFill(ufo, rrr, ggg, bbb);
				ufo++;
				}
			break;
			case 85: //Sync Red Fade
			switch(states[i])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = fades[0];
					gren = 0;
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = fades[0];
					gren = 0;
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 86: // Sync Yellow Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = fades[0];
					gren = fades[0];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = fades[0];
					gren = fades[0];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 87: //Green Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = 0;
					gren = fades[0];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					rojo = 0;
					gren = fades[0];
					blue = 0;
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 88: //Aqua Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = 0;
					gren = fades[0];
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = 0;
					gren = fades[0];
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 89: //Blue Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = 0;
					gren = 0;
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 90: //Pink Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = fades[0];
					gren = 0;
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = fades[0];
					gren = 0;
					blue = fades[0];
					SetRGB(r, g, b, rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 91: //White Fade
			switch(states[0])
				{
				case 0: // fading in
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = fades[0];
					gren = fades[0];
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]++;
					if(fades[0] > 4095)
						{
						fades[0] = 4095;
						states[0] = 1;
						}
					}
				break;
				case 1:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0]= currentMillis;
					rojo = fades[0];
					gren = fades[0];
					blue = fades[0];
					SyncRGB(rojo, gren, blue);
					fades[0]--;
					if(fades[0] < 0)
						{
						fades[0] = 0;
						states[0] = 0;
						}
					}
				break;
				}
			break;
			case 92: //Sync Time Psycho!
			if(currentMillis - ms[0] > unicorns)
				{
				SyncPsycho();
				}
			break;
			case 93: //Sync 2 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					two = twos[0];
					rojo = data[two][1];
					gren = data[two][2];
					blue = data[two][3];
					SyncRGB(rojo, gren, blue);
					two++;
					twos[0] = two;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 94: //Sync 2 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				two = twos[0];
				rojo = data[two][1];
				gren = data[two][2];
				blue = data[two][3];
				SyncRGB(rojo, gren, blue);
				two++;
				twos[0] = two;
				}
			break;
			case 95: // Haha, 69, hehe, Sync 3 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					three = threes[0];
					states[0] = 1;
					rojo = data[three][1];
					gren = data[three][2];
					blue = data[three][3];
					SyncRGB(rojo, gren, blue);
					three++;
					threes[0] = three;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 96: //Sync 3 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				three = threes[0];
				ms[0] = currentMillis;
				rojo = data[three][1];
				gren = data[three][2];
				blue = data[three][3];
				SyncRGB(rojo, gren, blue);
				three++;
				threes[0] = three;
				}
			break;
			case 97: //Sync 4 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					four = fours[0];
					rojo = data[four][1];
					gren = data[four][2];
					blue = data[four][3];
					SyncRGB(rojo, gren, blue);
					four++;
					fours[0] = four;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 98: //Sync 4 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				four = fours[0];
				ms[0] = currentMillis;
				rojo = data[four][1];
				gren = data[four][2];
				blue = data[four][3];
				SyncRGB(rojo, gren, blue);
				four++;
				fours[0] = four;
				}
			break;
			case 99: //Sync 5 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					five = fives[0];
					rojo = data[five][1];
					gren = data[five][2];
					blue = data[five][3];
					SyncRGB(rojo, gren, blue);
					five++;
					fives[0] = five;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 100: //Sync 5 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				five = fives[0];
				ms[0] = currentMillis;
				rojo = data[five][1];
				gren = data[five][2];
				blue = data[five][3];
				SyncRGB(rojo, gren, blue);
				five++;
				fives[0] = five;
				}
			break;
			case 101: //Sync 6 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					six = sixes[0];
					rojo = data[six][1];
					gren = data[six][2];
					blue = data[six][3];
					SyncRGB(rojo, gren, blue);
					six++;
					sixes[0] = six;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 102: //Sync 6 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				six = sixes[0];
				ms[0] = currentMillis;
				rojo = data[six][1];
				gren = data[six][2];
				blue = data[six][3];
				SyncRGB(rojo, gren, blue);
				six++;
				sixes[0] = six;
				}
			break;
			case 103: //Sync 7 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				seven = sevens[0];
				ms[0 ] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[seven][1];
					gren = data[seven][2];
					blue = data[seven][3];
					SyncRGB(rojo, gren, blue);
					five++;
					sevens[0] = seven;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 104: //Sync 7 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				seven = sevens[0];
				ms[0] = currentMillis;
				rojo = data[seven][1];
				gren = data[seven][2];
				blue = data[seven][3];
				SyncRGB(rojo, gren, blue);
				seven++;
				sevens[0] = seven;
				}
			break;
			case 105: //Sync 8 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					eight = eights[0];
					rojo = data[eight][1];
					gren = data[eight][2];
					blue = data[eight][3];
					SyncRGB(rojo, gren, blue);
					eight++;
					eights[0] = eight;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 106: //Sync 8 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				eight = eights[0];
				ms[0] = currentMillis;
				rojo = data[eight][1];
				gren = data[eight][2];
				blue = data[eight][3];
				SyncRGB(rojo, gren, blue);
				eight++;
				eights[0] = eight;
				}
			break;
			case 107: //Sync 9 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				nine = nines[0];
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[nine][1];
					gren = data[nine][2];
					blue = data[nine][3];
					SyncRGB(rojo, gren, blue);
					nine++;
					nines[0] = nine;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 108: //Sync 9 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				nine = nines[0];
				ms[0] = currentMillis;
				rojo = data[nine][1];
				gren = data[nine][2];
				blue = data[nine][3];
				SyncRGB(rojo, gren, blue);
				nine++;
				nines[0] = nine;
				}
			break;
			case 109: //Sync 10 Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ten = tens[0];
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rojo = data[ten][1];
					gren = data[ten][2];
					blue = data[ten][3];
					SyncRGB(rojo, gren, blue);
					ten++;
					tens[0] = ten;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 110: //Sync 10 Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ten = tens[0];
				ms[0] = currentMillis;
				rojo = data[ten][1];
				gren = data[ten][2];
				blue = data[ten][3];
				SyncRGB(rojo, gren, blue);
				ten++;
				tens[0] = ten;
				}
			break;
			case 111: //Sync Rainbow Flash
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
					rows[0]++;
					}
				else	
					{
					states[0] = 0;
					SyncOff();
					}
				}
			break;
			case 112: //Sync Rainbow Replace
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
			case 113: //Sync Random Color Flash
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
			case 114: //Sync Random Color Replace
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				rojo = rr;
				gren = gg;
				blue = bb;
				SyncRGB(rojo, gren, blue);
				}
			break;
			case 115: //Sync Audio Rainbow Replace
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
			case 116: //Sync Audio Random Replace
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
		sixes[i] = six;
		sevens[i] = seven;
		eights[i] = eight;
		nines[i] = nine;
		tens[i] = ten;
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
	rojo = 4095 - rojo;
	gren = 4095 - gren;
	blue = 4095 - blue;
	Tlc.set(r, rojo); // Set the Red pin, to Red value, based on channel
	Tlc.set(g, gren);
	Tlc.set(b, blue);
	Tlc.update(); // Apply the changes
	}

void SetOff(int r, int g, int b)
	{
	rojo = 4095 - rojo;
	gren = 4095 - gren;
	blue = 4095 - blue;
	Tlc.set(r, off);
	Tlc.set(g, off);
	Tlc.set(b, off);
	Tlc.update();
	}
	
void SyncRGB(int rojo, int gren, int blue)
	{
	rojo = 4095 - rojo;
	gren = 4095 - gren;
	blue = 4095 - blue;
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
	Tlc.set(15, rojo);
	Tlc.set(16, gren);
	Tlc.set(17, blue);
	Tlc.set(18, rojo);
	Tlc.set(19, gren);
	Tlc.set(20, blue);
	Tlc.set(21, rojo);
	Tlc.set(22, gren);
	Tlc.set(23, blue);
	Tlc.set(24, rojo);
	Tlc.set(25, gren);
	Tlc.set(26, blue);
	Tlc.set(27, rojo);
	Tlc.set(28, gren);
	Tlc.set(29, blue);
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
	rojo = 4095 - rojo;
	gren = 4095 - gren;
	blue = 4095 - blue;
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
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(15, rojo);
	Tlc.set(16, gren);
	Tlc.set(17, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(18, rojo);
	Tlc.set(19, gren);
	Tlc.set(20, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(21, rojo);
	Tlc.set(22, gren);
	Tlc.set(23, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(24, rojo);
	Tlc.set(25, gren);
	Tlc.set(26, blue);
	rojo = random(off);
	gren = random(off);
	blue = random(off);
	Tlc.set(27, rojo);
	Tlc.set(28, gren);
	Tlc.set(29, blue);
	Tlc.update();
	}
	
void UFOFlash(int ufo, int rrr, int ggg, int bbb, int rLast, int gLast, int bLast)
	{
	r = strips[ufo];
	g = r + 1;
	b = g + 1;
	rrr = 4095 - rrr;
	ggg = 4095 - ggg;
	bbb = 4095 - bbb;
	Tlc.set(r, rrr);
	Tlc.set(g, ggg);
	Tlc.set(b, bbb);
	Tlc.set(24, rrr);
	Tlc.set(25, ggg);
	Tlc.set(26, bbb);
	Tlc.set(27, rrr);
	Tlc.set(28, ggg);
	Tlc.set(29, bbb);
	Tlc.set(rLast, 4095);
	Tlc.set(gLast, 4095);
	Tlc.set(bLast, 4095);
	}
	
void UFOFill(int ufo, int rrr, int ggg, int bbb)
	{
	r = strips[ufo];
	g = r + 1;
	b = g + 1;
	rrr = 4095 - rrr;
	ggg = 4095 - ggg;
	bbb = 4095 - bbb;
	Tlc.set(r, rrr);
	Tlc.set(g, ggg);
	Tlc.set(b, bbb);
	Tlc.set(24, rrr);
	Tlc.set(25, ggg);
	Tlc.set(26, bbb);
	Tlc.set(27, rrr);
	Tlc.set(28, ggg);
	Tlc.set(29, bbb);
	}