#include "dolphin.h"
#include "util.h"
#include <stdio.h>
#include <conio.h>
#include <direct.h>
#include <ctype.h>
#include <process.h>
#include <string.h>
#include <malloc.h>
#include "screen.h"
#include <fstream.h>
#include <time.h>
#include <io.h>
#include "errno.h"

#define min(a,b) ((a>=b) ? b : a)
#define CHAR_SEL '*'  // used for marking line in the file selection

typedef char* charpoint;

extern FileList* g_fl;
extern	Product* g_dpr; // default product object
extern Product* g_pr;
extern Screen* g_sc[MENUS];
extern LookupDatabase* g_db;

int ngetch(int,int);
/******************************************************************************  
set_light()                                                                      
Purpose: 	Default method for users to turn on light                            
Arguments:	int timeout		                                                     
Returns: 	void                                                                 
Author: 	                                                                                        
Created: 	                                                                     
Modified:                                                                        
******************************************************************************/
static void set_light(void)
{
	light_on_tout(3);
}

/******************************************************************************  
calc()                                                                           
Purpose: 	Default method for users to turn on light
Arguments:	int timeout		                                                     
Returns: 	void                                                                 
Author: 	                                                                                        
Created: 	                                                                     
Modified:                                                                        
******************************************************************************/
static void calc(void)
{
	buzz();
}

void (*ptr_shift_function)(int shiftstate)=NULL;    		/* prototype for  pointer to num lock display status function */
void (*ptr_calc_function)(void)=calc;						/* prototype for pointer to calc function */
void (*ptr_contr_function)(void)=calc;				/* prototype for pointer to contr function */
void (*ptr_light_function)(void)=set_light; 				/* prototype for pointer to light function */
void (*ptr_autolight_function)(void)=NULL;   				/* prototype for pointer to autolight function */
/////////////////////////////////////////////////////////////////////////
int swaitkey(int sleep, int rfsleep,int mode)
{
	int rc;
	
	rc=waitkey(sleep,mode);
	if (rc==WK_TIMEOUT)
	{
		#ifndef NOSLEEPONSWAITKEY              
		if (!homebase())
		{
			if( RF_AreDriversLoaded()&& rfsleep!=0)
				RF_Sleep(rfsleep); 
			else 
				dsleep();                         
	 	}
		#endif
	}
	return rc;
}
/******************************************************************************
dgetch()                                                                       
Operation:  Gets character from keyboard, does not echo to screen.  Detects
			numeric, alpha or pc                                               			
			ch=dgetch(NOECHO |  SFUNC);                                        
Arguments:  int mode                                                           
			Bit 1, 0 = echo characters,  1 = do not echo characters (good for menus)  
			Bit 2, 0 = normal echo text,  1 = reverse echo text 
			Bit 3, 0 = Shifted functions return +,-,X and /  
			       1 = Shifted functions return SF1, SF2, SF3 and SF4
			Bit 4, 0 = ON/SCAN not returned/passes 
			       1 = ON/SCAN returned as char EONSCAN (0x12b)                			
Returns:    Returns character                                                  
******************************************************************************/
int dgetch(int mode)
{ 	
	int ks, swk_rc;
	int shift_flag = 0;	 
	int ctrl_flag = 0;	
	
	do              
	{		
  		ctrl_flag = 0;
		
		if((mode & RTNONSCAN) == RTNONSCAN)  		/* if dgetch is passed RTONSCAN, pass it to swaitkey */		
		{
			do
			{   				
				swk_rc = swaitkey(SLEEPTIMEOUT,RFSLEEPTIMEOUT,(WK_KEYSTROKE|WK_ONKEY)); /* wait till key pressed */ 
				if (swk_rc == WK_ONKEY) return(0x12b);							/*return if ON key pressed */
				
			}while(swk_rc == WK_TIMEOUT);
		}	
		else			
		    swaitkey(SLEEPTIMEOUT, RFSLEEPTIMEOUT, WK_KEYSTROKE); 				/* wait till key pressed */ 		
		  
	
		if(is_numeric())
		{
			ks = ngetch(mode, NKEYDELAY); 			/* Use ngetch() for numeric dolphin */  
			if (ks == 0) ks = ngetch(mode, NKEYDELAY) | 0x100;
		}
		else
		{
			ks = getch();                  		    /* Use getch() for alpha dolphin */
			if (ks == 0) ks = getch() | 0x100;  
			return(ks);
		}		  		
		
		if (*ptr_autolight_function!=NULL) (*ptr_autolight_function)();  		
		
		switch(ks)
		{
			case LIGHT | 0x100: 
			 	if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
			 	if (*ptr_light_function!=NULL) (*ptr_light_function)();
				shift_flag = 0;
			 	ctrl_flag=1;                      	/* set control flag, need to loop for next char */
				break;        
			case CONTRAST | 0x100:
			  	if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
			 	if (*ptr_contr_function!=NULL) (*ptr_contr_function)();
			 	shift_flag = 0;
			 	ctrl_flag=1;                        /* set control flag, need to loop for next char */
			 	break;
			case CALC | 0x100:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
			 	if (*ptr_calc_function!=NULL) (*ptr_calc_function)();
			 	shift_flag = 0; 
			 	ctrl_flag=1;                        /* set control flag, need to loop for next char */
			 	break;                               
			case SHIFT | 0x100: 
				if(shift_flag==0)
				{
					shift_flag = 1;
					if (*ptr_shift_function!=NULL) (*ptr_shift_function)(0);
				}
				else
				{
					shift_flag = 0;     	
					if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
				}	
				break;
			case F1 | 0x100:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
				if ((shift_flag))
				{ 
					ks = '+';
					if (mode & SFUNCT) ks = SF1 | 0x100;          	    		
				}
				shift_flag = 0;
				break;       	
			case F2 | 0x100:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);	     	
				if ((shift_flag))
				{
				 	ks = '-';
				 	if (mode & SFUNCT) ks = SF2 | 0x100;       	      	        
				}
				shift_flag = 0; 
				break;
			case F3 | 0x100:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);	     
				if ((shift_flag))
				{
					ks = '*';
					if (mode & SFUNCT) ks = SF3 | 0x100;       	    	          
				}
				shift_flag = 0;
				break;  	    
			case F4 | 0x100:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);	     
				if ((shift_flag))
				{
					ks = '/';               
					if (mode & SFUNCT) ks = SF4 | 0x100;                 
				}
				shift_flag = 0;
				break;       	
			case BS:
				if(shift_flag) ks = ESC;
				shift_flag = 0;
				break;         
			case ESC:
				if(shift_flag == 0) ks = BS;
				shift_flag = 0;
				break;	
			default:
				if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
				shift_flag = 0;
				break;
		}  	/* end of switch(ks) */	  
	} while(shift_flag || ctrl_flag || (ks == (SHIFT | 0x100))); 
 
  if (*ptr_shift_function!=NULL) (*ptr_shift_function)(1);
  return(ks);
}
////////////////////////////////////////////////////////////////////////////////
// gets the directory contents of 'c:\adventus'
FileList::FileList()
{
	char tmp[40];
	m_number = 0;
// path to folder where all files are kept ///////////////////////
	strcpy(m_filePath,"C:\\ADVENTUS\\");						//
	strcpy(m_contDir,"C:\\ADVENTUS\\DIRCONT\\");				//
// path to the directory contents files on DOLPHIN
	strcpy(m_dirFile,"C:\\ADVENTUS\\DIRCONT\\DIRCONT");         //
// path to the directory contents files on PC
//	strcpy(m_dirFile,"C:\\ADVENTUS\\DIRCONT\\A.TXT");         //
//////////////////////////////////////////////////////////////////
	int ret;
	ret = _chdrive(3);		// change drive to 'c:\'
	if(ret == -1) exit(0);  // drive not changed

	ret = _chdir(m_filePath);
	if(ret == -1)
	{
		strcpy(tmp,"MKDIR ");
		strcat(tmp,m_filePath);
		system(tmp);
		ret = _chdir(m_filePath);
		if(ret == -1) exit(0);	// folder could not be created
		ret = _chdir(m_contDir);
		if(ret == -1)
		{
			strcpy(tmp,"MKDIR ");
			strcat(tmp,m_contDir);
			system(tmp);
			ret = _chdir(tmp);
			if(ret == -1) exit(0);	// folder could not be created
		}
	}
	else
	{
		ret = _chdir(m_contDir);
		if(ret == -1)
		{
			strcpy(tmp,"MKDIR ");
			strcat(tmp,m_contDir);
			system(tmp);
			ret = _chdir(tmp);
			if(ret == -1) exit(0);	// folder could not be created
		}
	}

// for PC comment all marked below lines ///////////////////////////////////
	char buffer[100];
	strcpy(buffer,"DIR ");
	strcat(buffer,m_filePath);
	strcat(buffer," > ");
	strcat(buffer,m_dirFile);
	ret = system(buffer); // get the directory contents
	if(ret == -1) exit(0);  // command exec error
////for PC comment all marked above lines///////////////////////////////////
	
	for(int i=0;i<FILES_MAX;i++) m_FileList[i] = NULL;
// retrieves filenames from 'dir_cont' and stores them in the object
	CreateFileList();
}
////////////////////////////////////////////////////////////////////////////////
FileList::~FileList()
{
	for(int i=0;i<m_number;i++)
		if(m_FileList[i] != 0)
			delete [] m_FileList[i];

// remove the directory contents file
	char buffer[100];
	strcpy(buffer,m_dirFile);
	_unlink(buffer);
}
////////////////////////////////////////////////////////////////////////////////
// retrieves filenames from 'dir_cont' and stores them in the object
void FileList::CreateFileList()
{
	char buffer[100];

	FILE* f = fopen(m_dirFile,"r");
	if(f == NULL) exit(0); // error in file open
	while(fgets(buffer,100,f))
		analyzeString(buffer);  // analyzes string for filename
	fclose(f);
}
////////////////////////////////////////////////////////////////////////////////
// analyzes string for filename, consisting of 1 or 2 words
void FileList::analyzeString(char* buffer)
{
	char tmp[100];
	char* ptmp = tmp;
	char* p = buffer;
	char b[2][20];
	int words = 0;
	int i = 0;

	for(;;) // loop over all characters in the *buffer
	{
		if(*p == '[' && i == 0 && words == 0)	// directory
			return;
		if(ispunct(*p))
		{
			b[words++][i] = '\0';
			i=0;
			if(words == 2) break;
			else
			{
				p++;
				continue;
			}
		}
		if(isalnum(*p))
		{
			b[words][i++] = *p++;
			continue;
		}
		else if(isspace(*p))
		{
			if(p == buffer)
			{
				p++;
				continue; // first symbol is space
			}
			if(isalnum(*(p-1)))
			{
				b[words++][i] = '\0';
				i=0;
				if(words == 2) break;
				else
				{
					p++;
					continue;
				}
			}
			else
			{
				p++;
				continue;
			}
		}
		else if(*p == '\n' || *p == '\0') 
		{
			if(i == 0 && words == 0) return;  // empty line
			if(isalnum(*(p-1)))
			{
				b[words++][i] = '\0';
				break;
			}
			else
				break;
		}
	}
	strcpy(tmp,b[0]);
	if(words == 2)
	{
		strcat(tmp,".");
		strcat(tmp,b[1]);
	}
	char* ret1 = 0;
	char* ret2 = 0;
	ret1 = strstr(tmp,".File");
	ret2 = strstr(tmp,".free");
	if(ret1 != 0 || ret2 != 0) return;
	else
		this->addFile(tmp,strlen(tmp));
}
////////////////////////////////////////////////////////////////////////////////
// adds a filename to the m_FileList and create a new file if it does not exist
// only 100 files permitted
void FileList::addFile(char* label,int length)
{
	m_number++;
	if(m_number > FILES_MAX-1) exit(0);
	m_FileList[m_number-1] = new char[length+1];
	strcpy(m_FileList[m_number-1],label);
}
////////////////////////////////////////////////////////////////////////////////
// deletes file from the disk and m_FileList
void FileList::deleteFile(int number)  // zero indexed file list
{
	if(number >= m_number) return; // out of range
// erase file
    char path[80];
   	int ret;
	strcpy(path,m_filePath);
	strcat(path,m_FileList[number]);
	ret = _unlink(path);
	if(ret != 0) exit(0);

	delete [] m_FileList[number];
	for(int i=number;i<m_number-1;i++)
		m_FileList[i] = m_FileList[i+1];
	m_number--;
}
////////////////////////////////////////////////////////////////////////////////
// returns pointer to a filename number 'number', zero based indexing
char* FileList::getFilename(int number)
{
   return m_FileList[number];
};
////////////////////////////////////////////////////////////////////////////////
// remove all files and entries in m_FileList
void FileList::deleteAll()
{
    char path[40];
   	int ret;
	for(int i=0;i<m_number;i++)
	{
		strcpy(path,m_filePath);    
		strcat(path,m_FileList[i]);
		ret = _unlink(path);
		if(ret != 0) exit(0);
		delete [] m_FileList[i];
	}
	m_number = 0;
}
////////////////////////////////////////////////////////////////////////////////
// sets the text scroll window lines in display (LCD)
void FileList::setScrollableWindow(int row1, int row2)
{
	m_sRow1 = row1;
	m_sRow2 = row2;
}
////////////////////////////////////////////////////////////////////////////////
// displays filelist in a scrollable window and selects a file, which can be
// erased or sent to PC
int FileList::selectFile()
{
	char buffer[100];

	int rows = m_sRow2 - m_sRow1 + 1;  // rows in the scrollable window
	m_cursorPos = m_sRow1; // initially cursor is in the LCD row m_sRow1
	int topRow = 0; // row of m_FileList in m_sRow1, zero-based indexing
// initial display
// row '0' of m_FileList is displayed in the m_sRow1
	paintScrollRegion(topRow,MENU_FILES);

	if(m_number) setChar(m_sRow1,CHAR_SEL);
// kbd handler
	int c;
	int ret;
	int selFile;

	for(;;)
	{
		if(kbhit())
		{
			c = dgetch(NOECHO);
			switch(c)
			{
			case F1 | 0x100:
// scroll up
				if(m_cursorPos > m_sRow1)
				{
					if(m_number)
					{
						setChar(m_cursorPos--,' ');
						setChar(m_cursorPos,CHAR_SEL);
					}
					break;
				}
				if(m_cursorPos == m_sRow1 && topRow > 0)
				{
					m_cursorPos = m_sRow1;
					paintScrollRegion(--topRow,MENU_FILES);
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
				}
				break;
			case F2 | 0x100:
// scroll down
				if(m_cursorPos < m_sRow2 && m_cursorPos < m_sRow1 + (m_number - topRow) - 1)
				{
					if(m_number)
					{
						setChar(m_cursorPos++,' ');
						setChar(m_cursorPos,CHAR_SEL);
					}
					break;
				}
				if(m_cursorPos == m_sRow2 && topRow+rows-1 < m_number-1)
				{
					paintScrollRegion(++topRow,MENU_FILES);
					m_cursorPos = m_sRow2;
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
				}
				break;
// enter pressed
			case CR:
// send the selected file, zero-based indexing
				if(m_number)
				{				
					selFile = topRow + (m_cursorPos - m_sRow1);
					ret = MsgBox("|  send file?  |",m_FileList[selFile]);
					if(ret == 1)
					{
						int r;
						r = sendFile(selFile); // zero-based numbered file in m_FileList
						if (r == 0) MsgBoxOK("|  file sent   |",m_FileList[selFile]);
						else 
							MsgBoxOK("|file send err |",m_FileList[selFile]);
					}
				}
					break;
			case F3 | 0x100:
				if(m_number)
				{				
					selFile = topRow + (m_cursorPos - m_sRow1);
					ret = MsgBox("| delete file? |",m_FileList[selFile]);
					if(ret == 1)
					{                                                                 
						strcpy(buffer,m_FileList[selFile]);	
						deleteFile(selFile); // zero-based numbered file in m_FileList
						MsgBoxOK("| file deleted |",buffer);
						m_cursorPos = m_sRow1; // initially cursor is in the LCD row m_sRow1
						topRow = 0; // row of m_FileList in m_sRow1, zero-based indexing
						paintScrollRegion(topRow,MENU_FILES);
						if(m_number)setChar(m_sRow1,CHAR_SEL);
					}
				}
				break;
			case F4 | 0x100:
				if(m_number)
				{				
					ret = MsgBox("|  delete all  |","    files?");
					if(ret == 1)
					{
    					deleteAll();
						MsgBoxOK("|  all files   |","   deleted");
						m_cursorPos = m_sRow1; // initially cursor is in the LCD row m_sRow1
						topRow = 0; // row of m_FileList in m_sRow1, zero-based indexing
						paintScrollRegion(topRow,MENU_FILES);
					}
				}
				break;
			case SP:
// receive a file
				ret = MsgBox("|receive file? |","");
				if(ret == 1)
				{
					int r;
					char buffer[100];
					r = receiveFile(buffer); // zero-based numbered file in m_FileList
					if(r ==0) MsgBoxOK("| file received|","database");
					else
						MsgBoxOK("|receive error |","database");
				}
				break;
			case ESC:
			case BS:
				return 1;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// displays a message box on the screen
int FileList::MsgBox(char* title,char* subtitle)
{
/*	patterns

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  | delete file? |  ";		<- title
	m_contents[3] = "  |              |  ";		<- subtitle	
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  | F1-Yes F2-No |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |  send file?  |  ";
	m_contents[3] = "  |              |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  | F1-Yes F2-No |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |receive file? |  ";
	m_contents[3] = "  |              |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  | F1-Yes F2-No |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |  delete all  |  ";
	m_contents[3] = "  |    files?    |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  | F1-Yes F2-No |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";
*/
// backup the screen
	char backup[8][21];
	char* p;
	int i,j;
	for(i=0;i<8;i++)
	{
		p = g_sc[MENU_FILES]->getRow(i);
		for(j=0;j<20;j++)
			backup[i][j] = *p++;
		backup[i][20] = '\0';
	}
// display the MsgBox
	char buffer[21];
	strcpy(buffer,"----------------");
	gotoxy(3,2); printf("%s",buffer);
	strcpy(buffer,title);
	gotoxy(3,3); printf("%s",buffer);
	strcpy(buffer,"| ");
	strcat(buffer,subtitle);
	int len = strlen(subtitle) + 2;
	if(len < 14)
	{
		for(int j=len;j<14;j++)
			buffer[j] = ' ';
		buffer[14] = '\0';
	}
	strcat(buffer," |  ");
	gotoxy(3,4); printf("%s",buffer);
	strcpy(buffer,"|--------------|");
	gotoxy(3,5); printf("%s",buffer);
	strcpy(buffer,"| F1-Yes F2-No |");
	gotoxy(3,6); printf("%s",buffer);
	strcpy(buffer,"----------------");
	gotoxy(3,7); printf("%s",buffer);
// start keybooard handler
	for(;;)
	{
		if(kbhit())
		{
			int c = dgetch(NOECHO);
			switch(c)
			{
			case F1 | 0x100:
// restore screen
				for(i=0;i<8;i++)
				{
					gotoxy(1,i+1);
					printf("%20s",backup[i]);
				}
				if(m_number) setChar(m_cursorPos,CHAR_SEL);
				return 1;
			case F2 | 0x100:
// restore screen
				for(i=0;i<8;i++)
				{
					gotoxy(1,i+1);
					printf("%20s",backup[i]);
				}
				if(m_number) setChar(m_cursorPos,CHAR_SEL);
				return 0;
			default:
				continue;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// displays a message box on the screen
int FileList::MsgBoxOK(char* title,char* subtitle)
{
/*	patterns

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  | file deleted |  ";
	m_contents[3] = "  |              |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  |    F1-OK     |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |  file sent   |  ";
	m_contents[3] = "  |              |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  |    F1-OK     |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  | file received|  ";
	m_contents[3] = "  |              |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  |    F1-OK     |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |  all files   |  ";
	m_contents[3] = "  |   deleted    |  ";
	m_contents[4] = "  |              |  ";
	m_contents[5] = "  |    F1-OK     |  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";
*/

// backup the screen
	char backup[8][21];
	char* p;
	int i,j;
	for(i=0;i<8;i++)
	{
		p = g_sc[MENU_FILES]->getRow(i);
		for(j=0;j<20;j++)
			backup[i][j] = *p++;
		backup[i][20] = '\0';
	}
// display the MsgBox
	char buffer[21];
	strcpy(buffer,"----------------");
	gotoxy(3,2); printf("%s",buffer);
	strcpy(buffer,title);
	gotoxy(3,3); printf("%s",buffer);
	strcpy(buffer,"| ");
	strcat(buffer,subtitle);
	int len = strlen(subtitle) + 2;
	if(len < 14)
	{
		for(int j=len;j<14;j++)
			buffer[j] = ' ';
		buffer[14] = '\0';
	}
	strcat(buffer," |  ");
	gotoxy(3,4); printf("%s",buffer);
	strcpy(buffer,"|--------------|");
	gotoxy(3,5); printf("%s",buffer);
	strcpy(buffer,"|     F1-OK    |");
	gotoxy(3,6); printf("%s",buffer);
	strcpy(buffer,"----------------");
	gotoxy(3,7); printf("%s",buffer);
// start keybooard handler
	for(;;)
	{
		if(kbhit())
		{
			int c = dgetch(NOECHO);
			switch(c)
			{
			case F1 | 0x100:
// restore screen
				for(i=0;i<8;i++)
				{
					gotoxy(1,i+1);
					printf("%20s",backup[i]);
				}
				if(m_number) setChar(m_cursorPos,CHAR_SEL);
				return 1;
			case F2 | 0x100:
// restore screen
				for(i=0;i<8;i++)
				{
					gotoxy(1,i+1);
					printf("%20s",backup[i]);
				}
				if(m_number) setChar(m_cursorPos,CHAR_SEL);
				return 0;
			default:
				continue;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//   row (zero-based) of m_FileList is displayed in the m_sRow1. m_FileList fills rows until m_sRow2
//   and displays scroll symbols, if any
//	menu is one from MENU_xxx items
void FileList::paintScrollRegion(int row, int menu)
{
	char buffer[21];

	if(row > m_number) return; // out of range
	int rows = m_sRow2 - m_sRow1 + 1;  // rows in the scrollable window
// no files
	if(m_number == 0)
	{
		for(int i=m_sRow1-1;i<m_sRow2;i++)
			g_sc[menu]->setAndPaintRow(i," ");
		g_sc[menu]->setAndPaintRow((m_sRow2+m_sRow1)/2-1,"     NO FILES");
		return;
	}

	if(row == m_number) // No rows to paint. Only possible at row == 0
	{
		for(int i=0;i<rows;i++)
			g_sc[menu]->setAndPaintRow(i+m_sRow1-1," ");
		return;
	}
// OK, there are files
	for(int i=0;i<rows;i++)
	{
		if(row+i < m_number)
		{
			strcpy(buffer,"    ");
			strcat(buffer,m_FileList[row+i]);
			int len = strlen(buffer);
			while(len < COLUMNS)
				buffer[len++] = ' ';
			buffer[COLUMNS] = '\0';
		}
		else  // draw empty row
		{
			int len = 0;
			while(len < COLUMNS)
				buffer[len++] = ' ';
			buffer[COLUMNS] = '\0';
		}
// draw the 'scroll up' symbol
		if(i==0 && row>0)
			buffer[COLUMNS-1] = 'U';
// draw the 'scroll down' symbol
		if(i==rows-1 && row+rows < m_number)
			buffer[COLUMNS-1] = 'D';

		int rw = m_sRow1-1+i;
		g_sc[menu]->setAndPaintRow(rw,buffer);
	}
}
////////////////////////////////////////////////////////////////////////////////
// shows the character 'c' in an LCD row. Row numbering 1-based
void FileList::setChar(int row,char c)
{
	gotoxy(2,row);
	printf("%c",c);
}
////////////////////////////////////////////////////////////////////////////////
int FileList::sendFile(int number)
{
	int ret;
	char buffer[100];
	strcpy(buffer,m_filePath);
	strcat(buffer,m_FileList[number]);
	ret = fty_send(buffer,57600L,m_contDir);
	if(ret == 0) return 0;
	else
		return -1;
}
////////////////////////////////////////////////////////////////////////////////
int FileList::receiveFile(char* buffer)
{
	int ret;
	FILE* f = g_db->getFileHandle();
	if(f != NULL)
	{
		fclose(f);
		g_db->setFileHandle(NULL);
	}
	char file[] = "ITEMLIST.CSV";
	char buf[100];
	strcpy(buf,m_contDir);
	strcat(buf,file);
	ret = _unlink(buf);
	if(ret == 0 || (ret == -1 && errno == ENOENT))
	{
		ret = fty_receive(buffer,57600L,m_contDir);
		if(ret == 0) return 0;
	}
	else
		return -1;
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
//  construction / destruction
//
Product::Product()
{
	setCode("unknown",7);
	setSerial("unknown",7);
	setName("unknown",7);
	setFilename("UNKNOWN.CSV",11);
	setIDname("unknown",7);
}
Product::~Product()
{

}
////////////////////////////////////////////////////////////////////////////////
//  sets product code, length is without the trailing '\0'
void Product::setCode(char* label,int length)
{
	if(length > 1024) return;
	else
	{
		m_codeLength = length;
		length += 1; // include '\0'
    	for(int i=0;i<length;i++)
			m_code[i] = label[i];
	}
}
////////////////////////////////////////////////////////////////////////////////
// sets product serial number, length is without the trailing '\0'
void Product::setSerial(char* label,int length)
{
	if(length > 1024) return;
	else
	{
		m_serialLength = length;
		length += 1; // include '\0'
		for(int i=0;i<length;i++)
			m_serial[i] = label[i];
	}
}
////////////////////////////////////////////////////////////////////////////////
// sets product name, length is without the trailing '\0'
void Product::setName(char* label,int length)
{
	if(length > 1024) return;
	else
	{
		m_nameLength = length;
		length += 1; // include '\0'
    	for(int i=0;i<length;i++)
			m_name[i] = label[i];
	}
}
////////////////////////////////////////////////////////////////////////////////
// sets filename for the product (to save it in), length is without the trailing '\0'
void Product::setFilename(char* label,int length)
{
	if(length > 21) return; // dos filenames
	else
	{
		m_filenameLength = length;
		length += 1;	// include '\0'
		for(int i=0;i<length;i++)
			m_filename[i] = label[i];
	}
}
////////////////////////////////////////////////////////////////////////////////
// sets product name, length is without the trailing '\0'
void Product::setIDname(char* label,int length)
{
	if(length > 21) return;
	else
	{
		m_IDnameLength = length;
		length += 1; // include '\0'
    	for(int i=0;i<length;i++)
			m_IDname[i] = label[i];
	}
}
///////////////////////////////////////////////////////////////////////////////////////
// displays a text in a menu window.
// sc - on which menu sreen to draw
// row - zero based index of the row to paint
// rows - number of rows to paint (1 or 2 or 3)
// begin - text for the beginning of line (like 'Code:', 'Name:',...), terminated with '\0'
// contents - text to display, terminated with '\0'
void Product::displaySomething(Screen* sc,int row,int rows,char* begin,char* contents)
{
	char buffer[1024];
	if(rows>3) return;

	strcpy(buffer,begin);
	strcat(buffer,contents);
	if(rows==1)
		sc->setAndPaintRow(row,buffer);  // long rows will be truncated at the COLUMNS length
	else if(rows ==2)
	{
		int length = strlen(buffer); // without the trailing '\0'
		if(length<=COLUMNS) // buffer fits in one row. Fill with spaces another row
		{
			sc->setAndPaintRow(row,buffer);
			for(int j=0;j<COLUMNS;j++)
				buffer[j] = ' ';
			buffer[COLUMNS] = '\0';
			sc->setAndPaintRow(row+1,buffer);
		}
		else	// buffer will be drawn in two rows
		{
			sc->setAndPaintRow(row,buffer);
			int diff = length - COLUMNS;
			for(int j=0;j<diff;j++)
				buffer[j]=buffer[COLUMNS+j];
				buffer[diff]='\0';
			if(diff<COLUMNS)
			{
				for(j=diff;j<COLUMNS;j++)
					buffer[j]=' ';
				buffer[COLUMNS] = '\0';
			}
			sc->setAndPaintRow(row+1,buffer);
		}
	}
	else // output in 3 rows
	{
		int length = strlen(buffer); // without the trailing '\0'
		if(length<=COLUMNS) // buffer fits in one row. Fill with spaces another row
		{
			sc->setAndPaintRow(row,buffer);
			for(int j=0;j<COLUMNS;j++)
				buffer[j] = ' ';
			buffer[COLUMNS] = '\0';
			sc->setAndPaintRow(row+1,buffer);
		}
		else if(length<2*COLUMNS)	// buffer will be drawn in two rows
		{
			sc->setAndPaintRow(row,buffer);
			int diff = length - COLUMNS;
			for(int j=0;j<diff;j++)
				buffer[j]=buffer[COLUMNS+j];
				buffer[diff]='\0';
			if(diff<COLUMNS)
			{
				for(j=diff;j<COLUMNS;j++)
					buffer[j]=' ';
				buffer[COLUMNS] = '\0';
			}
			sc->setAndPaintRow(row+1,buffer);
		}
		else	// buffer drawn in 3 rows
		{
			sc->setAndPaintRow(row,buffer);
			sc->setAndPaintRow(row+1,&buffer[COLUMNS]);
			int diff = length - 2*COLUMNS;
			for(int j=0;j<diff;j++)
				buffer[j]=buffer[2*COLUMNS+j];
				buffer[diff]='\0';
			if(diff<COLUMNS)
			{
				for(j=diff;j<COLUMNS;j++)
					buffer[j]=' ';
				buffer[COLUMNS] = '\0';
			}
			sc->setAndPaintRow(row+2,buffer);
		}
	}
}
// saves the product in the file 'm_filename'
void Product::save()
{   
	char filename[100];
	char buffer[20];
	char name[1000];

	strcpy(filename,g_fl->getFilePath());
	strcat(filename,g_pr->getFilename());
	FILE* f = fopen(filename,"a");
	if(f==0) exit(0); // error. Cannot open the file
	strcpy(name,g_pr->getName());
	_strdate(filename);
	_strtime(buffer);
	if(fprintf(f,"%s,%s,%s,%s,%s\n",g_pr->getCode(),g_pr->getSerial(),filename,buffer,name) < 0) exit(0);
	fclose(f);
}
//////////////////////////////////////////////
// main setup handler (F3 from the main menu
void main_setup()
{
}
////////////////////////////////////////////////////////////////////////////////
// displays filelist in a scrollable window and selects a file, which can be
// erased or sent to PC
int FileList::selectFileSave()
{
	int rows = m_sRow2 - m_sRow1 + 1;  // rows in the scrollable window
	m_cursorPos = m_sRow1; // initially cursor is in the LCD row m_sRow1
	int topRow = 0; // row of m_FileList in m_sRow1, zero-based indexing
// initial display
// row '0' of m_FileList is displayed in the m_sRow1
	paintScrollRegion(topRow,MENU_SCANSAVE);
	if(m_number) setChar(m_sRow1,CHAR_SEL);
// kbd handler
	int c, ret;
	int selFile;
	char buffer[100];

	for(;;)
	{
		if(kbhit())
		{
			c = dgetch(NOECHO);
			switch(c)
			{
			case F1 | 0x100:
// scroll up
				if(m_cursorPos > m_sRow1)
				{
					if(m_number)
					{
						setChar(m_cursorPos--,' ');
						setChar(m_cursorPos,CHAR_SEL);
					}
					break;
				}
				if(m_cursorPos == m_sRow1 && topRow > 0)
				{
					m_cursorPos = m_sRow1;
					paintScrollRegion(--topRow,MENU_SCANSAVE);
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
				}
				break;
			case F2 | 0x100:
// scroll down
				if(m_cursorPos < m_sRow2 && m_cursorPos < m_sRow1 + (m_number - topRow) - 1)
				{
					if(m_number)
					{
						setChar(m_cursorPos++,' ');
						setChar(m_cursorPos,CHAR_SEL);
					}
					break;
				}
				if(m_cursorPos == m_sRow2 && topRow+rows-1 < m_number-1)
				{
					paintScrollRegion(++topRow,MENU_SCANSAVE);
					m_cursorPos = m_sRow2;
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
				}
				break;
			case F4 | 0x100:
// file selected from the list
				if(m_number)
				{				
					selFile = topRow + (m_cursorPos - m_sRow1); // index of file in the m_FileList[]
					g_dpr->setFilename(m_FileList[selFile],strlen(m_FileList[selFile]));
					g_pr->setFilename(m_FileList[selFile],strlen(m_FileList[selFile]));
					g_pr->displaySomething(g_sc[MENU_SCANSAVE],1,1,"To file:",g_pr->getFilename());
				}
				break;				
// enter pressed
			case CR:
// create the file, if not existing
				strcpy(buffer,g_pr->getFilename());
				if(fileExists(buffer) == 0)
					g_fl->addFile(buffer,strlen(buffer));
				return 0;
			case F3 | 0x100:
// create new file with name in buffer
				ret = MsgBoxEdit("|     Enter    |","| new filename |");
				if(ret == 1) // new filename escaped
					break;
				else if(ret == 0) // new filename accepted
				{
					g_pr->displaySomething(g_sc[MENU_SCANSAVE],1,1,"To file:",g_pr->getFilename());				
					paintScrollRegion(topRow,MENU_SCANSAVE);
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
					break;
				}
			case ESC:
			case BS:
				return 1;  // save cancelled
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//	checks if file exists
int FileList::fileExists(char* name)
{
	char b1[20];
	char b2[20];
	strcpy(b1,name);
	for(int i=0;i<m_number;i++)
	{
		strcpy(b2,m_FileList[i]);
		if(strcmp(strupr(b1),strupr(b2)) == 0)
			return 1;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
// displays a message box on the screen
int FileList::MsgBoxEdit(char* title, char* subtitle)
{
/*	pattern

	m_contents[0] = "                    ";
	m_contents[1] = "  ----------------  ";
	m_contents[2] = "  |     Enter    |  "
	m_contents[3] = "  | new filename |  ";
	m_contents[4] = "  | ____________ |  ";
	m_contents[5] = "  |F1-OK  F2-BACK|  ";
	m_contents[6] = "  ----------------  ";
	m_contents[7] = "                    ";
*/

// backup the screen
	char backup[8][21];
	char* p;
	int i,j;
	for(i=0;i<8;i++)
	{
		p = g_sc[MENU_SCANSAVE]->getRow(i);
		for(j=0;j<20;j++)
			backup[i][j] = *p++;
		backup[i][20] = '\0';
	}
// display the MsgBox
	char buffer[21];
	strcpy(buffer,"----------------");
	gotoxy(3,2); printf("%s",buffer);
	strcpy(buffer,title);
	gotoxy(3,3); printf("%s",buffer);
	strcpy(buffer,subtitle);
	gotoxy(3,4); printf("%s",buffer);
	strcpy(buffer,"|              |");
	gotoxy(3,5); printf("%s",buffer);
	strcpy(buffer,"|F1-OK  F2-BACK|");
	gotoxy(3,6); printf("%s",buffer);
	strcpy(buffer,"----------------");
	gotoxy(3,7); printf("%s",buffer);
// start keybooard handler
//	if(is_numeric()) // Alpha Dolphin (will not work on a PC terminal)
//	{
		const int ROW = 5;
		const int LEFTMARGIN = 5;
		const int RIGHTMARGIN = 16;
		char buf[20];
		int c;
		int pos=LEFTMARGIN; // cursor position
		p = buf;

		gotoxy(LEFTMARGIN,ROW);
		cursor_on();

		for(;;)
		{
			if(kbhit())
			{                    
				c = ngetch(1,5); // without character echo
				if(c==0 && kbhit())  // extended character
				{
					c = ngetch(1,5); // just empty the buffer
					switch(c)
					{
					case F1:	//accepted
						cursor_off();
						*p='\0';
					    if(strlen(buf) > 0)
					    {
							g_dpr->setFilename(buf,strlen(buf));
							g_pr->setFilename(buf,strlen(buf));
							g_pr->displaySomething(g_sc[MENU_SCANSAVE],1,1,"To File:",g_pr->getFilename());
							cursor_off();
// create a file, if it does not exist
							if(fileExists(buf) == 0)
							{
								char buffer[100];
								g_fl->addFile(buf,strlen(buf));
								strcpy(buffer,m_filePath);
								strcat(buffer,buf);
								createFile(buffer);
							}
						}
// restore screen
						for(i=0;i<8;i++)
						{
							gotoxy(1,i+1);
							printf("%20s",backup[i]);
						}
						return 0;
					case F2:	//cancelled
// restore screen
						cursor_off();
						for(i=0;i<8;i++)
						{
							gotoxy(1,i+1);
							printf("%20s",backup[i]);
						}
					if(m_number) setChar(m_cursorPos,CHAR_SEL);
						return 1;
					}
				}
				if(c == BS)
				{
					if(pos==LEFTMARGIN) continue;
					cursor_off();
					gotoxy(pos-1,ROW);
					putchar(' ');
					gotoxy(--pos,ROW);
					cursor_on();
					*(--p) = ' ';
					continue;
				}
				else if(isalnum(c) || c == '.')
				{
					if(pos>RIGHTMARGIN) continue;  //check 8.3 ????????
					*p++ = c;
					putchar(c);
					pos++;
					continue;
				}
				else
					continue;
			}
		}	
//	}
}
//////////////////////////////////////////////////////////////////////////
// creates a file
int FileList::createFile(char* name)
{
	FILE* f = fopen(name,"a");
	fclose(f);
	return 0;
}
////////////////////////////////////////////////////////////
//	construction/destruction
LookupDatabase::LookupDatabase(char* file)
{
	m_lines = 0;
	strcpy(m_filename,file);
	m_f = fopen(m_filename,"r");
	if(m_f == NULL) exit(0);	// cannot open the file
}
////////////////////////////////////////////////////////////
//
LookupDatabase::~LookupDatabase()
{
	if(m_f != NULL) fclose(m_f);
}
////////////////////////////////////////////////////////////
// counts rows in file m_f
int LookupDatabase::getLinesInFile()
{
	int rows = 0;
	char buffer[401];
	rewind(m_f);
	while(feof(m_f)== 0)
	{
		fgets(buffer,400,m_f);
		rows++;
	}
	m_lines = rows-1;
	return m_lines;
}
///////////////////////////////////////////////////////////
// searches decription for the matching code
int LookupDatabase::searchCode(char* code,char* description)
{
	char buffer[101];
	char codeInFile[101];
	char nameInFile[101];

	if(m_f == NULL) m_f = fopen(m_filename,"r");
	if(m_f == NULL) exit(0);	// cannot open the file
	rewind(m_f);

	for(int i=0;i<m_lines;i++)
	{
		fgets(buffer,101,m_f);
		analyzeString(buffer,codeInFile,nameInFile);

		if(strstr(code,codeInFile) == NULL)
			continue;
		else
		{
			strcpy(description,nameInFile);
			return i;
		}
	}
	return -1;  // lookup unsuccessful
}
////////////////////////////////////////////////////////////
// analyzes buffer and fills code and name
void LookupDatabase::analyzeString(char* buffer,char* code, char* name)
{
	int len = strlen(buffer);
	char buf[2][101];
	char c;
	int word = 0;
	int cnt = 0;
	const char separator = ';';	// separator between code and name fields

	for(int i=0;i<len;i++)
	{
		c = buffer[i];
		if(isalnum(c) || (word == 1 && (ispunct(c) || c == ' ')))
		{
			buf[word][cnt++] = c;
			continue;
		}
		else if(c == ';')
		{
			buf[word++][cnt++] = '\0';
			cnt = 0;
			if(word >= 2)
			{
			    strcpy(code,buf[0]);
			    strcpy(name,buf[1]);
				return;
			}
		}
		else if(c == '\n' || c == '\0')
		{
			buf[word][cnt] = '\0';
		    strcpy(code,buf[0]);
		    strcpy(name,buf[1]);
			return;
		}
		else if(isspace(c))
			continue;
	}
}
