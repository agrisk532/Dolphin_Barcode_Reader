#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include "dolphin.h"
#include "screen.h"
#include "util.h"
#include <process.h>

extern "C" int enable_all_symbologies(void);
extern "C" int scan(char*,int*,int*);
extern "C" void display_decode(int,char*);
extern "C" void scan_setup(void);
extern void main_setup(void);

// globals //////////////////////
extern Screen* g_sc[MENUS];
	FileList* g_fl;
	Product* g_dpr; // default product
	Product* g_pr;  // current product

// constructs and shows (or activates already existing) menu number 'n'
void menuShow(int n)
{
	if(g_sc[n] == NULL) // create new menu object
	{
		switch(n)
		{
		case MENU_MAIN:
			g_sc[MENU_MAIN] = new ScreenMain();
			break;
		case MENU_SCAN:
			g_sc[MENU_SCAN] = new ScreenScan();
			break;
		case MENU_SCANPRODUCTCODE:
			g_sc[MENU_SCANPRODUCTCODE] = new ScreenScanProductCode();
			break;
		case MENU_SCANSETUP:
			g_sc[MENU_SCANSETUP] = new ScreenScanSetup();
			break;
		case MENU_SCANSERIAL:
			g_sc[MENU_SCANSERIAL] = new ScreenScanSerial();
			break;
		case MENU_SCANSAVE:
			g_sc[MENU_SCANSAVE] = new ScreenScanSave();
			break;
		case MENU_FILES:
			g_sc[MENU_FILES] = new ScreenFiles();
			break;
		case MENU_SETUP:
			g_sc[MENU_SETUP] = new ScreenSetup();
			break;
		}
	}
	else // activate existing menu object
	{
		g_sc[n]->paintAll();
	}
}                 
/////////////////////////////////
int main(void)
{
// create objects
	g_fl = new FileList();
	g_dpr = new Product();
	g_pr = new Product();
// the main loop exit criteria
	int noExit = 1;
// initialize pointers to menus objects
	for(int i=0; i<MENUS; i++)
		g_sc[i] = NULL;
// keeps the number of the displayed menu on the screen
	int activeMenu = MENU_MAIN;
// display the main menu
	menuShow(MENU_MAIN);
// at the beginning enable all symbologies
// in the main loop they can be changed in the scan setup
	enable_all_symbologies();
// turn the LCD cursor off
	cursor_off();
// start the kbd message loop
	for(;noExit;)
	{
		int c = dgetch(NOECHO);
		char* p;
		switch(c)
		{
		case F1 | 0x100:
			switch(activeMenu)
			{
			case MENU_MAIN:
				menuShow(MENU_SCAN);
				activeMenu = MENU_SCAN;
				break;
			case MENU_SCAN:
				for(;;)
				{
				// for decoded info
					char label[1024];
					int length = 0;
					int direction = 0;
					char symb[40];

					menuShow(MENU_SCANPRODUCTCODE);
					activeMenu = MENU_SCANPRODUCTCODE;
					g_dpr->displaySomething(g_sc[MENU_SCANPRODUCTCODE],1,2,"Code:",g_dpr->getCode());
					g_dpr->displaySomething(g_sc[MENU_SCANPRODUCTCODE],3,2,"Name:",g_dpr->getName());
					g_dpr->displaySomething(g_sc[MENU_SCANPRODUCTCODE],5,1,"ID:",g_dpr->getIDname());
// call scanning routines
					int status = scan(label,&length,&direction);
///////////
//					c = dgetch(NOECHO);
//					if(c == ESC) break;
//					strcat(label,"111222333444555666777888");
//					length = 24;
//					int status = BARCODE_OK;
///////////
					int scanagain = 1;
					switch(status)
					{
					case BARCODE_OK:
						display_decode(direction, symb);
						while (onbhit());        /* Wait until the onkey is released before */
						set_led(get_led() &~ LED_GREEN);    /* Continuing. */
						break;
					case ANY_KEY_PRESSED:
						scanagain = 0;
						break;
					case BARCODE_TIMEOUT:
						dsleep();
						break;
					}
					if(scanagain == 0)
						break;
// set names for the default product
					g_dpr->setCode(label,length);
					g_dpr->setIDname(symb,strlen(symb));
// set names for the product
					g_pr->setCode(label,length);
					g_pr->setIDname(symb,strlen(symb));
//	look up the product name in the database file
//					g_dpr->setName(label,length);
				}
				break;
			case MENU_SCANSERIAL:
				menuShow(MENU_SCANSAVE);
				activeMenu = MENU_SCANSAVE;
				g_pr->displaySomething(g_sc[MENU_SCANSAVE],2,2,"To File:",g_pr->getFilename());
				break;
			case MENU_FILES:	// ??????????????????????
				// scroll the filelist up
				break;
			default:
				break;
			}
			break;
		case F2 | 0x100:
			switch(activeMenu)
			{
			case MENU_MAIN:
				menuShow(MENU_FILES);
				activeMenu = MENU_FILES;
				g_fl->setScrollableWindow(2,5);
				int ret;
				ret = g_fl->selectFile();
				if(ret == 1)
				{
					menuShow(MENU_MAIN);
					activeMenu = MENU_MAIN;
    			}
				break;
			case MENU_SCAN:
				for(;;)
				{
				// for decoded info
					char label[1024];
					int length = 0;
					int direction = 0;
					char symb[40];

					menuShow(MENU_SCANSERIAL);
					activeMenu = MENU_SCANSERIAL;
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],1,2,"Code:",g_pr->getCode());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],3,2,"Serial:",g_pr->getSerial());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],5,1,"ID:",g_pr->getIDname());
// call scanning routines
					int status = scan(label,&length,&direction);
///////////
//					c = dgetch(NOECHO);
//					if(c == ESC) break;
//					strcat(label,"888777666555444333222111");
//					length = 24;
//					int status = BARCODE_OK;
///////////
					int scanagain = 1;
					switch(status)
					{
					case BARCODE_OK:
						display_decode(direction, symb);
						while (onbhit());        /* Wait until the onkey is released before */
						set_led(get_led() &~ LED_GREEN);    /* Continuing. */
						break;
					case ANY_KEY_PRESSED:
						scanagain = 0;
						break;
					case BARCODE_TIMEOUT:
						dsleep();
						break;
					}
					if(scanagain == 0)
						break;
					g_pr->setSerial(label,length);
					g_pr->setIDname(symb,strlen(symb));
				}
				break;
			case MENU_SCANSAVE:
// keyboard input handling, new filename input
//				if(is_numeric()) // Alpha Dolphin (will not work on a PC terminal)
				{
					char buf[20];
					int c;
					int pos=1; // cursor position

					buf[0]='\0';
    				p=buf;
					g_sc[MENU_SCANSAVE]->setAndPaintRow(5,"                    ");
					gotoxy(1,6);
					cursor_on();

					for(;;)
					{
						if(kbhit())
						{                    
							c = ngetch(1,3); // without character echo
							if(c==0 && kbhit())  // extended character
							{
								c = ngetch(1,3); // just empty the buffer
								continue;
							}
							if(c == BS)
							{
								if(pos==1) continue;
								cursor_off();
								gotoxy(pos-1,6);
								putchar(' ');
								gotoxy(--pos,6);
								cursor_on();
								*(--p) = ' ';
								continue;
							}
							if(c == ESC)
							{
								g_pr->displaySomething(g_sc[MENU_SCANSAVE],5,2,"","                    ");
								cursor_off();
								buf[0]='\0'; // empty the buffer
                				break;
							}
							else if(c == CR)
							{   
								*p='\0';
// check the filename for the '8.3' pattern ??????????????????????????
								g_dpr->setFilename(buf,strlen(buf));
	            				g_pr->setFilename(buf,strlen(buf));
								g_pr->displaySomething(g_sc[MENU_SCANSAVE],2,2,"To File:",g_pr->getFilename());
								g_sc[MENU_SCANSAVE]->setAndPaintRow(5,"                    ");
								cursor_off();
								menuShow(MENU_SCANSAVE);
								activeMenu = MENU_SCANSAVE;
								break;
							}
							else if(isalnum(c) || c == '.')
							{
								if(pos>12) continue;  //check 8.3 ????????
								*p++ = c;
								putchar(c);
								pos++;
								continue;
							}
							else
								continue;
						}
					}	
				    if(strlen(buf) > 0)
				    {
						g_dpr->setFilename(buf,strlen(buf));
						g_pr->setFilename(buf,strlen(buf));
						g_pr->displaySomething(g_sc[MENU_SCANSAVE],2,2,"To File:",g_pr->getFilename());
						g_sc[MENU_SCANSAVE]->setAndPaintRow(5,"                    ");
						cursor_off();
						menuShow(MENU_SCANSAVE);
						activeMenu = MENU_SCANSAVE;
					}
					break;
				}
			case MENU_SETUP:   /////////?????????????????????
			// scroll the filelist down
				break;
			default:
				break;
			}
			break;
		case F3 | 0x100:
			switch(activeMenu)
			{
			case MENU_MAIN:
				menuShow(MENU_SETUP);
				activeMenu = MENU_SETUP;
				main_setup();
				break;
			case MENU_SCAN:
				menuShow(MENU_SCANSETUP);
				activeMenu = MENU_SCANSETUP;
			    scan_setup();
				break;
			default:
				break;
			}
			break;
		case ESC:
		case BS:
			switch(activeMenu)
			{
			case MENU_MAIN:
// clean up memory
				for(i=0;i<=MENUS;i++)
				{
					if(g_sc[i] == NULL) continue;
					else
						delete g_sc[i];
				}
				if(g_fl != NULL) delete g_fl;
				if(g_dpr != NULL) delete g_dpr;
				if(g_pr != NULL) delete g_pr;
				_chdir("C:\\");
				cursor_on();
				return 1;
			case MENU_SCAN:
				menuShow(MENU_MAIN);
				activeMenu = MENU_MAIN;
				break;
			case MENU_SCANPRODUCTCODE:
				menuShow(MENU_SCAN);
				activeMenu = MENU_SCAN;
				break;
			case MENU_SCANSETUP:
				menuShow(MENU_SCAN);
				activeMenu = MENU_SCAN;
				break;
			case MENU_SCANSERIAL:
				menuShow(MENU_SCAN);
				activeMenu = MENU_SCAN;
				break;
			case MENU_SCANSAVE:
				menuShow(MENU_SCANSERIAL);
				activeMenu = MENU_SCANSERIAL;
				for(;;)
				{
				// for decoded info
					char label[1024];
					int length = 0;
					int direction = 0;
					char symb[80];

					menuShow(MENU_SCANSERIAL);
					activeMenu = MENU_SCANSERIAL;
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],1,2,"Code:",g_pr->getCode());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],3,2,"Serial:",g_pr->getSerial());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],5,1,"ID:",g_pr->getIDname());
// call scanning routines
					int status = scan(label,&length,&direction);
////////
//					c = dgetch(NOECHO);
//					if(c == ESC) break;
//					strcat(label,"111222333444555666777888");
//					length = 24;
//					int status = BARCODE_OK;
///////
					int scanagain = 1;
					switch(status)
					{
					case BARCODE_OK:
						display_decode(direction, symb);
						while (onbhit());        /* Wait until the onkey is released before */
						set_led(get_led() &~ LED_GREEN);    /* Continuing. */
						break;
					case ANY_KEY_PRESSED:
						scanagain = 0;
						break;
					case BARCODE_TIMEOUT:
						dsleep();
						break;
					}
					if(scanagain == 0)
						break;
					g_pr->setSerial(label,length);
					g_pr->setIDname(symb,strlen(symb));
				}
				break;
			case MENU_SETUP:
				menuShow(MENU_MAIN);
				activeMenu = MENU_MAIN;
				break;
			case MENU_FILES:
				menuShow(MENU_MAIN);
				activeMenu = MENU_MAIN;
				break;
			default:
				break;
			}
			break;
		case CR:
			switch(activeMenu)
			{
			case MENU_SCANSAVE:
// check filename and save scan results in a file
				g_pr->save();

				for(;;)
				{
				// for decoded info
					char label[1024];
					int length = 0;
					int direction = 0;
					char symb[40];

					menuShow(MENU_SCANSERIAL);
					activeMenu = MENU_SCANSERIAL;
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],1,2,"Code:",g_pr->getCode());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],3,2,"Serial:",g_pr->getSerial());
					g_pr->displaySomething(g_sc[MENU_SCANSERIAL],5,1,"ID:",g_pr->getIDname());
// call scanning routines
					int status = scan(label,&length,&direction);
///////////
//					c = dgetch(NOECHO);
//					if(c == ESC) break;
//					strcat(label,"888777666555444333222111");
//					length = 24;
//					int status = BARCODE_OK;
///////////
					int scanagain = 1;
					switch(status)
					{
					case BARCODE_OK:
						display_decode(direction, symb);
						while (onbhit());        /* Wait until the onkey is released before */
						set_led(get_led() &~ LED_GREEN);    /* Continuing. */
						break;
					case ANY_KEY_PRESSED:
						scanagain = 0;
						break;
					case BARCODE_TIMEOUT:
						dsleep();
						break;
					}
					if(scanagain == 0)
						break;
					g_pr->setSerial(label,length);
					g_pr->setIDname(symb,strlen(symb));
				}
				break;
			default:
				break;
			}
		    break;
		default: // alphanumeric for NEW file name
			break;
        }
	}
	return 0;
}
