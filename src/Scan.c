/* Copyright (c) 1997 Hand Held Products, All Right Reserved. */

#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <dolphin.h>

#define ESC       0x1B	 /* 27  */
#define NOECHO	  0x01   /* echo characters */

int sgetch(void);

struct Symbology
{
	int  symbology;
	char *name;
	int  enabled;
} symbologies[] =
{
	{ CODE128,  "Code 128" },
	{ CODE25IL, "I 2 of 5" },
	{ CODE39,   "Code 3 of 9" },
	{ CODABAR,  "Codabar" },
	{ UPC_EAN,  "UPC" },
};

int find_symbology(int symbology)
{
	int i;
	if (symbology < 0)
		symbology = -symbology;
	
	for (i = 0; i < sizeof(symbologies) / sizeof(symbologies[0]); i++)
	{
		if (symbology == symbologies[i].symbology)
			return i;
	}
	
	return -1;
}

void display_decode(int symbology, char* symb)
{
	int i;
	
	sound(2800);
	set_led(get_led() | LED_GREEN);

	i = find_symbology(symbology);

	if (i != -1)
		sprintf(symb,"%s", symbologies[i].name);
	else
		sprintf(symb,"id: %d", symbology);

	delay(100);
	nosound();
}
	
int scan(char* label, int* length, int* direction)
{
	int status;
	set_barcode_options(30, 0, 0);
	
	status = read_barcode(label, length, direction);
	return status;
}

void register_symbologies(void)
{
	int i;
	
	for (i = 0; i < sizeof(symbologies) / sizeof(symbologies[0]); i++)
	{
		if (symbologies[i].enabled)
			register_barcode(symbologies[i].symbology, 0, 0);
		else
			unregister_barcode(symbologies[i].symbology);
	}
}

void enable_all_symbologies(void)
{
	int i;
	
	for (i = 0; i < sizeof(symbologies) / sizeof(symbologies[0]); i++)
		symbologies[i].enabled = 1;

	register_symbologies();
}

void scan_setup(void)
{
	int position = 0;
	int i;
	int c;

	for (i = 0; i < sizeof(symbologies) / sizeof(symbologies[0]); i++)
	{
		gotoxy(3, 2 + i);
		printf ("%s", symbologies[i].name);
	}

	for (;;)
	{
		for (i = 0; i < sizeof(symbologies) / sizeof(symbologies[0]); i++)
		{
			gotoxy(1, 2 + i);
			printf (i == position ? "\x7e" : " ");

			gotoxy(15, 2 + i);
			printf (symbologies[i].enabled ? "ON  " : "OFF ");
			printf (i == position ? "\x7f" : " ");
		}

		gotoxy(20, 2 + position);

		c = sgetch();
		if (c == ESC)
			break;
		if (c == 0)
		{
			c = sgetch();
			switch(c)
			{
			case 59:
				if (position > 0)
					position--;
				break;
			case 60:
				if (position < sizeof(symbologies) / sizeof(symbologies[0]) - 1)
					position++;
				break;
			}
		}
		if (c == '\r')
			symbologies[position].enabled = ! symbologies[position].enabled;
	}
	register_symbologies();
}
