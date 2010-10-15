/*****************************************************************************
  File name:    typedefs.h

  Description:  This header file contains type definitions.

  Copyright (C) 2004, HUE-Mobile Enterprises, All Rights Reserved

  The information contained herein is confidential property of HUE-Mobile 
  Enterprises.  The use, copying, transfer or disclosure of such 
  information is prohibited except by express written agreement with 
  HUE-Mobile Enterprises.
******************************************************************************
            Change Log
     Date       By Whom                     Change
--------------+-------------------+-------------------------------------------
  Jun-17-2004  Alex Jiang          Created.
  
******************************************************************************/
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#define TRUE    1
#define FALSE   0

typedef unsigned char   boolean;

#define to_boolean(b)   (((b) != 0) ? TRUE : FALSE)

typedef unsigned char   uint8;
typedef unsigned short int  uint16;
typedef unsigned long int   uint32;

typedef signed char int8;
typedef signed short int    int16;
typedef signed long int int32;


#endif //TYPEDEFS_H
