#include "Tlc5940.h"
#include "tlc_fades.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"

struct config_t
{
int data[5][9];
} configuration;

TLC_CHANNEL_TYPE channel;

int i; // JUST ADDED !!!
int f;

HardwareSerial Uart = HardwareSerial(); // this will change to Serial on production boards

unsigned long ms[5]; // stores timing values for each channel
int rows[5]; // stores what row we use for rainbow flash modes
int states[5]; // stores boolean values for on off states for each channel
int two;
int three;
int four;
int five;

boolean started; // packet state variables
boolean ended;

unsigned long currentMillis;
unsigned long previousMillis;

int row; // give values in setup!
int srow; // CHANGE TO USE rows[1]
int ufo;
int ufoLast;
int rLast;
int gLast;
int bLast;
int ufos[5];

int chan; // current channel (strip)
int rojo; // user defined red value
int gren; // user defined green value
int blue; // user defined blue value
int mode; // user defined mode
int band; // user defined audio frequency band
int sens; // user defined audio sensitivity
int unicorns; // user defined speed setting
int r; // which red channel to output on
int g; // which green channel to output on
int b; // which blue channel to ouptut on
int rr; // random red value
int gg; // random green value
int bb; // random blue value

int rrr; // used for all colors that are not user defined
int ggg;
int bbb;

int pulse; // the value taken from a2d if requested
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 7; // reset is attached to digital pin 3
int spectrumValue[7]; // holds the a2d values

char inData[45];// Buffer for the incoming data
char *inParse[45];// Buffer for the parsed data chunks
char incomingByte;
char buffer[45];
int data[5][8];
int serialIn = 0;
int index = 0;// Incoming data id
int colors[8][3];
int usa[3][3];

void setup()
	{
	Tlc.init();
	Tlc.update();
	Uart.begin(57000);
	colors[0][0] = 255; // red
	colors[0][1] = 0;
	colors[0][2] = 0;
	colors[1][0] = 255;// orange
	colors[1][1] = 30;
	colors[1][2] = 0;
	colors[2][0] = 255;// yellow
	colors[2][1] = 75;
	colors[2][2] = 0;
	colors[3][0] = 0;// green
	colors[3][1] = 255;
	colors[3][2] = 0;
	colors[4][0] = 0;// aqua
	colors[4][1] = 255;
	colors[4][2] = 255;
	colors[5][0] = 0;// blue
	colors[5][1] = 0;
	colors[5][2] = 255;
	colors[6][0] = 100;// indigo
	colors[6][1] = 0;
	colors[6][2] = 255;
	colors[7][0] = 180;// violet
	colors[7][1] = 0;
	colors[7][2] = 255;
	usa[0][0] = 255; // Red
	usa[0][1] = 0;
	usa[0][2] = 0;
	usa[1][0] = 255; // White
	usa[1][1] = 255;
	usa[1][2] = 100;
	usa[2][0] = 0; // Blue
	usa[2][1] = 0;
	usa[2][2] = 255;
	ms[0] = 0;
	ms[1] = 0;
	ms[2] = 0;
	ms[3] = 0;
	ms[4] = 0;
	states[0] = 0;
	states[1] = 0;
	states[2] = 0;
	states[3] = 0;
	states[4] = 0;
	rows[0] = 0;
	rows[1] = 0;
	rows[2] = 0;
	rows[3] = 0;
	rows[4] = 0;
	ufos[0] = 0;
	ufos[1] = 3;
	ufos[2] = 6;
	ufos[3] = 9;
	ufos[4] = 12;
	i = 0; // JUST ADDED!!!
	EEPROM_readAnything(0, configuration);
	}
	
void loop()                     // run over and over again
	{
	if(two > 1)
		{
		two = 0;
		}
	if(three > 2)
		{
		three = 0;
		}
	if(four > 3)
		{
		four = 0;
		}
	if(five > 4)
		{
		five = 0;
		}
	for(f = 0; f < 5; f++) // RESET RAINBOW
		{
		if (rows[f] > 7)
			{
			rows[f] = 0;
			}
		}
	for(int f = 0; f < 5; f++) // RESET UFO STRIPS
		{
		if (ufos[f] > 3)
			{
			ufos[f] = 0;
			}
		}
	Modes();
	char c;
	if (Uart.available())
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
		else
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
			Serial.println("RECEIVED HEADER");
			parseSerialData(); // sorts data based on channel
			}
		serialIn = 0;
		buffer[serialIn] = '\0';
		started = 0;
		ended = 0;
		}
	}

void parseSerialData()
	{
	char *p = buffer; // The data to be parsed
	char *str;   // Temp store for each data chunk
  	int counter = 0; // Id ref for each chunk
    while ((str = strtok_r(p, ":", &p)) != NULL)// Loop through the data and seperate it into chunks at each ":" delimeter
		{
		inParse[counter] = str;// Add chunk to array
        counter++;      // Increment data count
		}
	char *chn = inParse[1];
	char *red = inParse[2];
	char *grn = inParse[3];
	char *blu = inParse[4];
	char *mode = inParse[5];
	char *audB = inParse[6];
	char *audS = inParse[7];
	char *unic = inParse[8];
	char *save = inParse[9];
	sortData(chn,red,grn,blu,mode,audB,audS,unic,save);
	}

void sortData(char *chn, char *red, char *grn, char *blu, char *mode, char *audB, char *audS, char *unic, char *save)
	{
	int chan = atoi(chn);
	int arr = atoi(red);
	int gee = atoi(grn);
	int bee = atoi(blu);
	int emm = atoi(mode);
	int bnd = atoi(audB);
	int ess = atoi(audS);
	int uni = atoi(unic);
	data[chan][0] = chan;
	data[chan][1] = arr; // Convert the characters to integers from serial data (red)
	data[chan][2] = gee; // the green value
	data[chan][3] = bee; // blue
	data[chan][4] = emm; // the modes are commented in at the bottom
	data[chan][5] = bnd; // audio frequency band
	data[chan][6] = ess; // audio sensitivity
	data[chan][7] = uni; //
	int keep = atoi(save); // We should save immediately after this if keep > 1
	if(keep > 0)
		{
		EEPROM_writeAnything(0, configuration);
		keep = 0;
		}
	}

void Modes()
	{
	for (i = 0; i < 5; i++) // starting with channel 0, apply settings to strips, then move to the next channel.
		{
		rojo = data[i][1];
		gren = data[i][2];
		blue = data[i][3];
		mode = data[i][4];
		band = data[i][5];
		sens = data[i][6];
		unicorns = data[i][7];
		switch(ufo)
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
		rLast = ufos[ufoLast];
		gLast = rLast + 1;
		bLast = gLast + 1;
		currentMillis =  millis();
		r = ufos[i]; // I JUST CHANGHED THIS!!!
		g = r + 1;
		b = g + 1;
		rr = random(4095);
		gg = random(4095);
		bb = random(4095);
		int rand = random(9);
		switch(mode)
			{
			case 1: // Set Color
			Tlc.set(r, rojo);
			Tlc.set(g, gren);
			Tlc.set(b, blue);
			Tlc.update();
			break;
			case 2: // Set Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					Tlc.set(r, rojo);
					Tlc.set(g, gren);
					Tlc.set(b, blue);
					Tlc.update();
					}
				else
					{
					states[i] = 0;
					Tlc.set(r, 4095);
					Tlc.set(g, 4095);
					Tlc.set(b, 4095);
					Tlc.update();
					}
				}
			break;
			case 3: // Sync Flash Set Color
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					// Tlc.setAll(4095); // Uncomment to troubleshoot
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
				else
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}
			break;
			case 4: // Rainbow Color Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rrr = colors[row][0];
					ggg = colors[row][1];
					bbb = colors[row][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					row++;
					rows[i] = row;
					Tlc.update();
					}
				else
					{
					Tlc.set(r, 4095);
					Tlc.set(g, 4095);
					Tlc.set(b, 4095);
					Tlc.update();
					}
				}
			case 5: // Synchronized Rainbow Color Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
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
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					row++;
					rows[0] = row;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}
			break;
			case 6: // Random Flash
			if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					Tlc.set(r, rr);
					Tlc.set(g, gg);
					Tlc.set(b, bb);
					Tlc.update();
					}
				else
					{
					states[i] = 0;
					Tlc.set(r, 4095);
					Tlc.set(g, 4095);
					Tlc.set(b, 4095);
					Tlc.update();
					}
				}
			break;
			case 7: // Synchronized Random Flash
			if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					Tlc.set(0, rr);
					Tlc.set(1, gg);
					Tlc.set(2, bb);
					Tlc.set(3, rr);
					Tlc.set(4, gg);
					Tlc.set(5, bb);
					Tlc.set(6, rr);
					Tlc.set(7, gg);
					Tlc.set(8, bb);
					Tlc.set(9, rr);
					Tlc.set(10, gg);
					Tlc.set(11, bb);
					Tlc.set(12, rr);
					Tlc.set(13, gg);
					Tlc.set(14, bb);
					Tlc.update();
					}
				else
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}
			break;
			case 8: // Audio Rainbow Replace
			audIn(band);
			if (pulse > sens)
				{
				rojo = colors[row][0];
				gren = colors[row][1];
				blue = colors[row][2];
				rrr = rrr * 16;
				rrr = 4095 - rrr;
				ggg = ggg * 16;
				ggg = 4095 - ggg;
				bbb = bbb * 16;
				bbb = 4095 - bbb;
				Tlc.set(r, rrr);
				Tlc.set(g, ggg);
				Tlc.set(b, bbb);
				row++;
				rows[i] = row;
				Tlc.update();
				}
			break;
			case 9: // Audio Limited Random
			audIn(band);
			if (pulse > sens)
				{
				rojo = colors[rand][0];
				gren = colors[rand][1];
				blue = colors[rand][2];
				Tlc.set(r, rojo);
				Tlc.set(g, gren);
				Tlc.set(b, blue);
				Tlc.update();
				}
			break;
			case 10: // Audio Full Random
			audIn(band);
			if (pulse > sens)
				{
				Tlc.set(r, rr);
				Tlc.set(g, gg);
				Tlc.set(b, bb);
				Tlc.update();
				}
			break;
			case 11: // Synchronized Audio Replace
			audIn(band);
			if (pulse > sens)
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
				Tlc.set(0, rrr);
				Tlc.set(1, ggg);
				Tlc.set(2, bbb);
				Tlc.set(3, rrr);
				Tlc.set(4, ggg);
				Tlc.set(5, bbb);
				Tlc.set(6, rrr);
				Tlc.set(7, ggg);
				Tlc.set(8, bbb);
				Tlc.set(9, rrr);
				Tlc.set(10, ggg);
				Tlc.set(11, bbb);
				Tlc.set(12, rrr);
				Tlc.set(13, ggg);
				Tlc.set(14, bbb);
				row++;
				rows[0] = row;
				Tlc.update();
				}
			break;
			case 12: // Synchronized Audio Limited Random
			audIn(band);
			if (pulse > sens)
				{
				rrr = colors[rand][0];
				ggg = colors[rand][1];
				bbb = colors[rand][2];
				rrr = rrr * 16;
				rrr = 4095 - rrr;
				ggg = ggg * 16;
				ggg = 4095 - ggg;
				bbb = bbb * 16;
				bbb = 4095 - bbb;
				Tlc.set(0, rrr);
				Tlc.set(1, ggg);
				Tlc.set(2, bbb);
				Tlc.set(3, rrr);
				Tlc.set(4, ggg);
				Tlc.set(5, bbb);
				Tlc.set(6, rrr);
				Tlc.set(7, ggg);
				Tlc.set(8, bbb);
				Tlc.set(9, rrr);
				Tlc.set(10, ggg);
				Tlc.set(11, bbb);
				Tlc.set(12, rrr);
				Tlc.set(13, ggg);
				Tlc.set(14, bbb);
				Tlc.update();
				}
			break;
			case 13: // Synchronized Audio Full Random
			audIn(band);
			if (pulse > sens)
				{
				Tlc.set(0, rr);
				Tlc.set(1, gg);
				Tlc.set(2, bb);
				Tlc.set(3, rr);
				Tlc.set(4, gg);
				Tlc.set(5, bb);
				Tlc.set(6, rr);
				Tlc.set(7, gg);
				Tlc.set(8, bb);
				Tlc.set(9, rr);
				Tlc.set(10, gg);
				Tlc.set(11, bb);
				Tlc.set(12, rr);
				Tlc.set(13, gg);
				Tlc.set(14, bb);
				Tlc.update();
				}
				case 14: // UFO Rainbow Advance Cycles
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
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
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 15:
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
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 16:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE CYCLES
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
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				case 17:
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE NO CYCLES
					{
					ms[0] = currentMillis;
					row = rows[0];
					rojo = colors[row][0];
					gren = colors[row][1];
					blue = colors[row][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					row++;
					rows[0] = row;
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				case 18: // UFO Set Color
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO SET COLOR
					{
					ms[0] = currentMillis;
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rojo);
					Tlc.set(g, gren);
					Tlc.set(b, blue);
					Tlc.set(12, rojo);
					Tlc.set(13, gren);
					Tlc.set(14, blue);
					Tlc.set(rLast, 4095);
					Tlc.set(gLast, 4095);
					Tlc.set(bLast, 4095);
					Tlc.update();
					ufo++;
					}
				break;
				case 19: // UFO SET COLOR AUDIO
				audIn(band);
				if(pulse > sens) // timing check here move to the strips. UFO RAINBOW REPLACE NO CYCLES
					{
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rojo);
					Tlc.set(g, gren);
					Tlc.set(b, blue);
					Tlc.set(12, rojo);
					Tlc.set(13, gren);
					Tlc.set(14, blue);
					Tlc.set(rLast, 4095);
					Tlc.set(gLast, 4095);
					Tlc.set(bLast, 4095);
					Tlc.update();
					ufo++;
					}
				break;
				case 20: // Simple Red
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 21: // Simple Yellow
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 22: // simple Green
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 23: // Simple Aqua
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = pulse * 8;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 24: // Simple Blue
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = 4095;
					bbb = pulse * 8;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 25: // Simple Violet
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095;
					bbb = pulse * 4;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 26: // Simple White
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					ggg = pulse * 8;
					bbb = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095 - ggg;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				else
					{
					rrr = 4095;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.update();
					}
				break;
				case 27: // Sync Simple Red
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095;
					bbb = 4095;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 28: // Sync Simple Yellow
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = 4095;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 29: // Sync Simple Green
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = 4095;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 30: // Sync Simple Aqua
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = pulse * 8;
					ggg = 4095 - ggg;
					bbb = pulse * 8;
					bbb = 4095 - bbb;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 31: // Sync Simple Blue
				audIn(band);
				if(pulse > sens)
					{
					rrr = 4095;
					ggg = 4095;
					bbb = pulse * 8;
					bbb = 4095 - bbb;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 32: // Sync Simple Violet
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095;
					bbb = pulse * 4;
					bbb = 4095 - bbb;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 33: // Sync Simple White
				audIn(band);
				if(pulse > sens)
					{
					rrr = pulse * 8;
					ggg = pulse * 8;
					bbb = pulse * 8;
					rrr = 4095 - rrr;
					ggg = 4095 - ggg;
					bbb = 4095 - bbb;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					}
				else
					{
					Tlc.setAll(4095);
					Tlc.update();
					}
				break;
				case 34: // USA Flash
				if(currentMillis - ms[i] > unicorns) // timing check here move to the strips.
				{
				ms[i] = currentMillis;
				if(states[i] == 0)
					{
					states[i] = 1;
					rrr = usa[three][0];
					ggg = usa[three][1];
					bbb = usa[three][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					three++;
					Tlc.update();
					}
				else
					{
					Tlc.set(r, 4095);
					Tlc.set(g, 4095);
					Tlc.set(b, 4095);
					Tlc.update();
					}
				}
				break;
				case 35: // Sync USA Flash
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rrr = usa[three][0];
					ggg = usa[three][1];
					bbb = usa[three][2];
					rrr = rrr * 16;
					rrr = 4095 - rrr;
					ggg = ggg * 16;
					ggg = 4095 - ggg;
					bbb = bbb * 16;
					bbb = 4095 - bbb;
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					three++;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}				
				break;
				case 36: // Sync Flash 2 Colors
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rrr = data[two][1];
					ggg = data[two][2];
					bbb = data[two][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					two++;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}	
				break;
				case 37: // Sync Flash 3 Colors
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rrr = data[three][1];
					ggg = data[three][2];
					bbb = data[three][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					three++;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}	
				break;
				case 38: // Sync Flash 4 Colors
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rrr = data[four][1];
					ggg = data[four][2];
					bbb = data[four][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					four++;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}	
				break;
				case 39: // Sync Flash 5 Colors
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
				{
				ms[0] = currentMillis;
				if(states[0] == 0)
					{
					states[0] = 1;
					rrr = data[five][1];
					ggg = data[five][2];
					bbb = data[five][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					two++;
					}
				else	
					{
					states[0] = 0;
					Tlc.setAll(4095);
					Tlc.update();
					}
				}	
				break;
				case 40: // Sync Audio Flash 2 Colors
				audIn(band);
				if (pulse > sens)
					{
					rrr = data[two][1];
					ggg = data[two][2];
					bbb = data[two][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					two++;
					Tlc.update();
					}			
				break;
				case 41: // Sync Audio Flash 3 Colors
				audIn(band);
				if (pulse > sens)
					{
					rrr = data[three][1];
					ggg = data[three][2];
					bbb = data[three][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					three++;
					Tlc.update();
					}
				break;
				case 42: // Sync Audio Flash 4 Colors
				audIn(band);
				if (pulse > sens)
					{
					rrr = data[four][1];
					ggg = data[four][2];
					bbb = data[four][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					four++;
					Tlc.update();
					}
				break;
				case 43: // Sync Audio Flash 5 Colors
				audIn(band);
				if (pulse > sens)
					{
					rrr = data[five][1];
					ggg = data[five][2];
					bbb = data[five][3];
					Tlc.set(0, rrr);
					Tlc.set(1, ggg);
					Tlc.set(2, bbb);
					Tlc.set(3, rrr);
					Tlc.set(4, ggg);
					Tlc.set(5, bbb);
					Tlc.set(6, rrr);
					Tlc.set(7, ggg);
					Tlc.set(8, bbb);
					Tlc.set(9, rrr);
					Tlc.set(10, ggg);
					Tlc.set(11, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					five++;
					Tlc.update();
					}
				break;
				case 44: // Sync 2 Color UFO Advance
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[two][1];
						ggg = colors[two][2];
						bbb = colors[two][3];
						two++;
						}
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 45: // Sync 3 Color UFO Advance
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[three][1];
						ggg = colors[three][2];
						bbb = colors[three][3];
						three++;
						}
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 46: // Sync 4 Color UFO Advance
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[four][1];
						ggg = colors[four][2];
						bbb = colors[four][3];
						four++;
						}
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 47: // Sync 5 Color UFO Advance
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips.
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[five][1];
						ggg = colors[five][2];
						bbb = colors[five][3];
						five++;
						}
					r = ufos[ufo];
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
					ufo++;
					}
				break;
				case 48: // Sync 2 Color UFO Replace
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE CYCLES
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[two][1];
						ggg = colors[two][2];
						bbb = colors[two][3];
						two++;
						}
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				case 49: // Sync 3 Color UFO Replace
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE CYCLES
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[three][1];
						ggg = colors[three][2];
						bbb = colors[three][3];
						three++;
						}
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				case 50: // Sync 4 Color UFO Replace
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE CYCLES
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[four][1];
						ggg = colors[four][2];
						bbb = colors[four][3];
						four++;
						}
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				case 51: // Sync 5 Color UFO Replace
				if(currentMillis - ms[0] > unicorns) // timing check here move to the strips. UFO RAINBOW REPLACE CYCLES
					{
					ms[0] = currentMillis;
					if (ufo == 0) // color check here. advance color. after last color, start colors over.
						{
						rrr = colors[five][1];
						ggg = colors[five][2];
						bbb = colors[five][3];
						five++;
						}
					r = ufos[ufo];
					g = r + 1;
					b = g + 1;
					Tlc.set(r, rrr);
					Tlc.set(g, ggg);
					Tlc.set(b, bbb);
					Tlc.set(12, rrr);
					Tlc.set(13, ggg);
					Tlc.set(14, bbb);
					Tlc.update();
					ufo++;
					}
				break;
				}
			}
		}

int audIn(int band)
	{
	digitalWrite(resetPin, HIGH);
	digitalWrite(resetPin, LOW);
	for (int i = 0; i < 7; i++)
		{
		digitalWrite(strobePin, LOW);
		delayMicroseconds(30); // to allow the output to settle
		spectrumValue[i] = analogRead(analogPin);
		digitalWrite(strobePin, HIGH);
		}
	pulse = spectrumValue[band];
	pulse = pulse * 2;
	return pulse;
	}