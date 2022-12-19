#include <stdio.h>

#ifndef __AFX_UTIL_H
#define __AFX_UTIL_H

/* Define character key set */ 
#define BS        0x08	 /* 08  */
#define LF        0x0A	 /* 10  */
#define CR        0x0D	 /* 13  */
#define ESC       0x1B	 /* 27  */
#define SP        0x20	 /* 32  */

/* Define extended character key set */
#define F1        0x3B	 /* 59  */
#define F2        0x3C	 /* 60  */
#define F3        0x3D	 /* 61  */
#define F4        0x3E	 /* 62  */
#define SF1       0x48	 /* 72  */
#define SF2       0x50	 /* 80  */
#define SF3       0x4D	 /* 77  */
#define SF4       0x4B	 /* 75  */
#define CLR       0x53	 /* 83  */ 
#define SHIFT     0x86   /* 134 */    
#define LIGHT	  0x28   /* 40  */
#define CONTRAST  0x29   /* 41  */
#define CALC	  0x2A   /* 42  */ 
#define ONSCAN	  0x2B   /* 43  */
#define SENTR	  CR	 /* shift, enter */

#define NKEYDELAY 0x0C	 /* 12  */
#define FILLCHAR  0x20	 /* 32  */             

/* Bitfield defines for dgetch() & get_kybrd_input()*/
#define NOECHO		0x01         							/* echo characters */
#define REVERSE		0x02        							/* reverse char/text */
#define SFUNCT		0x04         							/* Shifted functions return SF1, SF2, SF3 and SF4 */
#define RTNONSCAN	0x08         							/* ON/SCAN returned as char EONSCAN (0x12b) */ 
#define NUMLOCK		0x10         							/* Enable NUMLOCK */

/* defines for sleep time outs */
#define SLEEPTIMEOUT 1080									/* Timer ticks(18/sec), before unit sleeps (1080 = ~ 1 minutes)*/
#define RFSLEEPTIMEOUT 0   									/* Addional seconds before RF terminal Sleeps*/   

/* define for application      */
#define FOUND     1
#define NOT_FOUND 0
#define INVFILE "INV.DAT" 

#define FILES_MAX 100	// max allowed number of files for scan results

// functions
int dgetch(int);
//
class Screen;
////////////////////////////////////////////////////////////////////////////////////////////
// keeps the 1:1 list of files on the disk
class FileList
{
private:
	char* m_FileList[FILES_MAX];	// keeps the pointers to disk filenames with scan results, written with this program
	int m_number;		// keeps the number of files in the m_filelist
	int m_sRow1;		// scrollable window, row1
	int m_sRow2;		// scrollable window, row2
	int m_sUpperRow;    // upper row of the m_FileList in the scroll window
	int m_cursorPos;	// position of cursor
	char m_filePath[100]; // default path to scan result files
	char m_dirFile[100]; // full path to the directory contents file
	char m_contDir[100]; // path to directory with files to send
public:
	FileList();
	virtual ~FileList();
	void addFile(char* label,int length); 		// adds a filename to the m_filelist, if file does not exist, create it
	void deleteFile(int number); 				// deletes a file from disk and from m_FileList
	int getNumberOfFiles(){return m_number;} 	// returns the number of files in the list
	char* getFilename(int number);				// retrieves the filename, corresponding to 'number'
	void deleteAll();							// removes all files from the m_filelist and deletes files from the disk
	void readFiles();							// reads the filelist from the default directory and stores in m_fileList
	void CreateFileList();						// retrieves filenames from 'dir_cont' and stores them in the object
	void analyzeString(char* buffer);			// analyzes string for filename
	void setScrollableWindow(int r1, int r2);	// sets the text scrollable window
	int selectFile(void);						// file selector for MENU_FILES
	int selectFileSave(void);					// file selector for MENU_SCANSAVE
	void paintScrollRegion(int row,int menu);	// paints scroll region of menu
	void setChar(int row,char c);				// shows the character 'c' in an LCD row. Row numbering 1-based
	int MsgBox(char* title,char* subtitle);		// displays a message box on the screen
	int MsgBoxOK(char* title,char* subtitle);	// displays an OK message box on the screen
	int MsgBoxEdit(char* title, char* subtitle);// edit filename message box
	int sendFile(int file);						// sends away a file
	int receiveFile(char* buffer);				// receives a file, its name in buffer
	char* getFilePath() {return m_filePath;}	// returns the location of scan result files
	int createFile(char* name);					// creates a file at location given in 'name'
	int fileExists(char* name);					// checks whether file already exists
};
////////////////////////////////////////////////////////////////////////////////////////////
// keeps an instance of a product
class Product
{
private:
	char m_code[1040];  	// keeps the product code
	char m_serial[1040];	// keeps the product serial number
	char m_name[1040];	// keeps the product symbolic name
	char m_filename[21];// keeps the current filename
	char m_IDname[21];  // keeps the current scancode ID name
	int m_codeLength;
	int m_serialLength;
	int m_nameLength;
	int m_filenameLength;
	int m_IDnameLength;
public:
	Product();
	virtual ~Product();

	void setCode(char* label,int length); // length without the trailing '\0'
	void setSerial(char* label,int length);
	void setName(char* label,int length);
	void setFilename(char* label, int length);
	void setIDname(char* label, int length);

	char* getCode() {return m_code;}
	char* getSerial() {return m_serial;}
	char* getName() {return m_name;}
	char* getFilename() {return m_filename;}
	char* getIDname() {return m_IDname;}
	int getCodeLength() {return m_codeLength;}
	int getNameLength() {return m_nameLength;}
	int getSerialLength() {return m_serialLength;}
	int getFilenameLength() {return m_filenameLength;}
	int getIDnameLength() {return m_IDnameLength;}

    void displaySomething(Screen* sc,int row,int rows,char* begin,char* contents);
	void save();	// saves the object in the m_filename
};
/////////////////////////////////////////////////////////////////////////
//	product code-name lookup database

class LookupDatabase
{
private:
	int m_lines;		// number of entries in database
	FILE* m_f;			// file handle
	char m_filename[100];	// keeps pathname of the file with database
public:
	LookupDatabase(char*);
	virtual ~LookupDatabase();

	int getLinesInFile();	// returns the number of lines in the file m_f
	int getNumber() {return m_lines;}
// searches for matching code, returns the description and the entry number in the database file
	int searchCode(char* code, char* description);
	void analyzeString(char* buffer,char* code, char* name);
	char* getFilename() {return m_filename;}
	FILE* getFileHandle() {return m_f;}
	void setFileHandle(FILE* f) {m_f = f;}
};

#endif
