// Screen.cpp: implementation of the Screen class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <process.h>
#include "Screen.h"
#include "dolphin.h"
#include "util.h"

#define min(a,b) ((a>=b) ? b : a)

int ngetch(int,int);
//-----------------------------------------------------------
// pointers to all possible screen menus, created when needed
Screen* g_sc[MENUS];
// 0 - main
// 1 - scan
// 2 - scan product code
// 3 - scan setup
// 4 - scan serial number
// 5 - scan save
// 6 - files
// 7 - setup
//-----------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Screen::Screen() {}

Screen::~Screen() {}

//////////////////////////////////////////////////////////////////////
// sets and displays the contents of a row, numbering of rows from zero
// long rows are truncated at the edge of LCD display
void Screen::setAndPaintRow(int row, char* txt)
{
	int length = min(COLUMNS,strlen(txt)); // not including trailing '\0'
	for(int i=0;i<length;i++)
		m_contents[row][i] = txt[i];
	m_contents[row][length]='\0';
	if(length<COLUMNS)
	{
		for(int j=length;j<COLUMNS;j++)
			m_contents[row][j] = ' ';
		m_contents[row][COLUMNS]='\0';
	}	

	gotoxy(1,row+1);
	printf("%s",m_contents[row]);
}

void Screen::clear()
{
	clrscr();
}

void Screen::paintAll()
{
	
	for(int i=0;i<ROWS;i++)
	{
		gotoxy(1,i+1);
		printf("%s",m_contents[i]);
	}
}
////////////////////////////////////////////////////////////////////
ScreenMain::ScreenMain()
{
	m_contents[0] = "        MAIN        ";
	m_contents[1] = "                    ";
	m_contents[2] = "F1  - SCAN          ";
	m_contents[3] = "F2  - FILES         ";
	m_contents[4] = "F3  - SETUP         ";
	m_contents[5] = "ESC - EXIT          ";
	m_contents[6] = "                    ";
	m_contents[7] = "                    ";

	this->paintAll();
}

ScreenMain::~ScreenMain() {}

//-----------------------------------------
ScreenScan::ScreenScan()
{
	m_contents[0] = "        SCAN        ";
	m_contents[1] = "                    ";
	m_contents[2] = "F1  - PRODUCT CODE  ";
	m_contents[3] = "F2  - SERIAL NUMBER ";
	m_contents[4] = "F3  - SETUP         ";
	m_contents[5] = "ESC - BACK          ";
	m_contents[6] = "                    ";
	m_contents[7] = "                    ";

	this->paintAll();
}

ScreenScan::~ScreenScan() {}

//------------------------------------------
ScreenScanProductCode::ScreenScanProductCode()
{
	m_contents[0] = " SCAN PRODUCT CODE  ";
	m_contents[1] = "CODE:               ";
	m_contents[2] = "                    ";
	m_contents[3] = "NAME:               ";
	m_contents[4] = "                    ";
	m_contents[5] = "                    ";
	m_contents[6] = "ID:                 ";
	m_contents[7] = "ON-SCAN     ESC-BACK";

	this->paintAll();
}

ScreenScanProductCode::~ScreenScanProductCode()
{

}
//-------------------------------------------
ScreenScanSetup::ScreenScanSetup()
{
	m_contents[0] = "     SCAN SETUP     ";
	m_contents[1] = "                    ";
	m_contents[2] = "                    ";
	m_contents[3] = "                    ";
	m_contents[4] = "                    ";
	m_contents[5] = "                    ";
	m_contents[6] = "                    ";
	m_contents[7] = "ENTR-ON/OFF ESC-BACK";

	this->paintAll();
}

ScreenScanSetup::~ScreenScanSetup()
{

}
//--------------------------------------------
ScreenScanSerial::ScreenScanSerial()
{
	m_contents[0] = "    SCAN SERIAL #   ";
	m_contents[1] = "CODE:               ";
	m_contents[2] = "                    ";
	m_contents[3] = "NAME:               ";
	m_contents[4] = "                    ";
	m_contents[5] = "ID:                 ";
	m_contents[6] = "ON-SCAN     ESC-BACK";
	m_contents[7] = "F1-SAVE             ";

	this->paintAll();
}

ScreenScanSerial::~ScreenScanSerial()
{

}
/////////////////////////////////////////////////////////////////////
ScreenScanSave::ScreenScanSave()
{
	m_contents[0] = "     SCAN SAVE      ";
	m_contents[1] = "TO FILE:            ";
	m_contents[2] = "SELECT FROM LIST:   ";
	m_contents[3] = "                    ";
	m_contents[4] = "                    ";
	m_contents[5] = "                    ";
	m_contents[6] = "F1F2,F4-SELCT F3-NEW";
	m_contents[7] = "ENTR-ACCEPT ESC-BACK";

	this->paintAll();
}

ScreenScanSave::~ScreenScanSave()
{

}
//----------------------------------------
ScreenFiles::ScreenFiles()
{
	m_contents[0] = "       FILES        ";
	m_contents[1] = "                    ";
	m_contents[2] = "                    ";
	m_contents[3] = "                    ";
	m_contents[4] = "                    ";
	m_contents[5] = "F1F2-SELECT ESC-BACK";
	m_contents[6] = "ENTER-SEND    F3-DEL";
	m_contents[7] = "F4-DELALL SPC-RCV DB";

	this->paintAll();
}

ScreenFiles::~ScreenFiles()
{

}
//----------------------------------------
ScreenSetup::ScreenSetup()
{
	m_contents[0] = "        SETUP       ";
	m_contents[1] = "                    ";
	m_contents[2] = "                    ";
	m_contents[3] = "F1  - DATE/TIME     ";
	m_contents[4] = "                    ";
	m_contents[5] = "                    ";
	m_contents[6] = "                    ";
	m_contents[7] = "ESC - BACK          ";

	this->paintAll();
}

ScreenSetup::~ScreenSetup()
{

}
//////////////////////////////////////////////////////////////////////////////////////////////
ScreenSetupDate::ScreenSetupDate()
{
	m_contents[0] = "    SET DATE/TIME   ";
	m_contents[1] = "                    ";
	m_contents[2] = "                    ";
	m_contents[3] = "F1  - DATE          ";
	m_contents[4] = "F2  - TIME          ";
	m_contents[5] = "                    ";
	m_contents[6] = "                    ";
	m_contents[7] = "ESC - BACK          ";

	this->paintAll();
}

ScreenSetupDate::~ScreenSetupDate()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////
//	this routine changes the system date
void ScreenSetupDate::setDate()
{
	for(;;)
	{
		if(kbhit())
		{                    
			int c = ngetch(1,5); // without character echo
			if(c==ESC || c==BS)
			{
				cursor_off();
				return;
			}
			if(c==0 && kbhit())  // extended character
			{
				c = ngetch(1,5); // just empty the buffer
				switch(c)
				{
				case F1:	// set date
					clear();
					cursor_on();
					system("DATE");
					cursor_off();
					this->paintAll();
					break;
				case F2:
					clear();
					cursor_on();
					system("TIME");
					cursor_off();
					this->paintAll();
					break;
				}
			}
		}
	}
}
