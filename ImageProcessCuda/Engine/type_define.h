#ifndef __TYPE_DEFINE_H__
#define __TYPE_DEFINE_H__

 typedef unsigned long       DWORD;
 typedef int                 BOOL;
 typedef unsigned char       BYTE;
 typedef unsigned short      WORD;
 typedef float               FLOAT;
 typedef char CHAR;
 typedef short SHORT;
 typedef long LONG;

 struct myRect
 {
	 int x;
	 int y;
	 int width;
	 int height;
	 int bInvert;
	 int pnInd[4];
	 int ocrdis[4];
 };

#endif