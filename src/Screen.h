// Screen.h: interface for the Screen class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AFX_SCREEN_H
#define __AFX_SCREEN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Dolphin text screen size
#define ROWS 8
#define COLUMNS 20
// total number of menus(screens) in application
#define MENUS 9
// Menu numbering
#define MENU_MAIN 0
#define MENU_SCAN 1
#define MENU_SCANPRODUCTCODE 2
#define MENU_SCANSETUP 3
#define MENU_SCANSERIAL 4
#define MENU_SCANSAVE 5
#define MENU_FILES 6
#define MENU_SETUP 7
#define MENU_SETUPDATE 8

//////////////////////////////////////////////////////////////////////////////////////////
class Screen
{
public:
	char* m_contents[ROWS];	// holds the contents of a menu screen (8x20 chars)
public:
	Screen();
	virtual ~Screen();
	void clear();	// clears contents and redraws the screen
	void paintAll();	// repaints the screen with contents
	void setAndPaintRow(int row, char* txt); // sets and paints the row
	char* getRow(int row) {return m_contents[row];}  // zero-based
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenMain : public Screen
{
public:
	ScreenMain();
	virtual ~ScreenMain();
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenScan : public Screen
{
public:
	ScreenScan();
	virtual ~ScreenScan();
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenScanProductCode : public Screen
{
public:
	ScreenScanProductCode();
	virtual ~ScreenScanProductCode();
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenScanSetup : public Screen
{
public:
	ScreenScanSetup();
	virtual ~ScreenScanSetup();
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenScanSerial : public Screen
{
public:
	ScreenScanSerial();
	virtual ~ScreenScanSerial();
};
//////////////////////////////////////////////////////////////////////////////////////////
class ScreenScanSave : public Screen
{
public:
	ScreenScanSave();
	virtual ~ScreenScanSave();
};
//-----------------------------------
class ScreenFiles : public Screen
{
public:
	ScreenFiles();
	virtual ~ScreenFiles();
};
//-----------------------------------
class ScreenSetup : public Screen
{
public:
	ScreenSetup();
	virtual ~ScreenSetup();
};
//-----------------------------------
class ScreenSetupDate : public Screen
{
public:
	ScreenSetupDate();
	virtual ~ScreenSetupDate();
	void setDate();					// prompts and sets the system date
};


#endif // !defined(AFX_SCREEN_H__99B20611_1360_4D2A_B650_B92C41891819__INCLUDED_)
