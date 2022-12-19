README file for DOLPHIN Numeric ADVENTUS
Updated 07Feb2002 by Agris
The project has been made with MS VC++ 1.00
-------------------------------------------
1) The project can be built both for Dolphin and tests on a PC. In order to swap the projects, the following chages must be applied...
a) five identical sections in the main.cpp:
// call scanning routines
				int status = scan(label,&length,&direction);
/////////// for simulation on PC
//				c = dgetch(NOECHO);
//				if(c == ESC) break;
//					strcpy(label,"OP3BA53118AAYUT5521");
//					length = 24;
//					int status = BARCODE_OK;
/////////// for simulation on PC

b) two marked sections of the FileList constructor in the util.cpp file.

2) Project settings: 
memory model = medium (linked to dolphinm.lib), segment setup: SS==DS*
optimization level = default,
stack size 10240 bytes (at least such size worked).
3) the product code-name lookup database consists of rows of two fields. First, significant part of the product code, to distinguish among products. It does not have to be the full code. Second, product description, in text form. The separator between fields is ';', as can be exported from MSExcel in the 'MSDOS csv' format. Initially the db file (ItemList.csv) can be uploaded using the following commands on Dolphin:
cd c:\adventus\dircont
yx /b37600
followed by sending the file 'ItemList.csv' with a PC Dolphin File Transfer program (installed from Dolphin CD). Currently the db filename is fixed and cannot be changed from the LCD panel. The locations of program files are also restriced:
-the main executable 'a.exe' in 'c:\'
-scan files in 'c:\adventus\'
-service files ('dircont','ItemList.csv') in 'c:\adventus\dircont\'
'dircont' is used to store the contents of 'c:\adventus\'

DOLPHIN.BAS is the MS Excel Macro, which imports scan result file, including leading zeros in the serial number part.