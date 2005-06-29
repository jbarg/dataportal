/**************************************************************************
Copyright ^Ó General Atomics, 1995-1999
All Rights Reserved
 
This file contains Original Code and/or Modifications of Original Code as
defined in and that are subject to the General Atomics Storage Resource
Broker Academic and Research Use  License (the 'License'). This license is
for academic and research use only. You may not use, copy, modify or
distribute SRB for commercial purposes, nor incorporate the SRB into
commercial products without the express written permission of General
Atomics nor may you distribute SRB to any third party.
 
You may not use this software except in compliance with the License. Please
obtain a copy of the License at http://www.npaci.edu/DICE/SRB/License and
read it before using this file.
 
The Original Code and all software distributed under the License are
distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESS OR IMPLIED, AND GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER
CENTER HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
NON-INFRINGEMENT OF ANY COPYRIGHT, PATENT , TRADEMARK OR OTHER RIGHTS.
GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER CENTER FURTHER  HAVE NO
OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCMENTS OR
MODIFICATIONS.. Please see the License for the specific language governing
rights and limitations under the License."
 
Please read the terms of this license carefully. By using the SRB software,
you are accepting and agreeing to the terms of this license with General
Atomics. If you are agreeing to this license on behalf of a company, or
other institution, you represent that you are authorized to bind the
company/institution to such a license. If you do not meet this criterion or
you do not agree to any of the terms of this license, you are not
authorized to use the software.
**************************************************************************/
 
/*
 * orbUtil.c - Utility Routines for ORBs
 */

#include "orbMDriverExtern.h" 
/*** UTILITY ROUTINES ****/

int
freeOrbStateInfo(orbStateInfo *orbSI) {
  if (orbSI != NULL) {
    if (orbSI->presentation != NULL) free (orbSI->presentation);
    if (orbSI->select != NULL) free (orbSI->select );
    if (orbSI->reject != NULL) free (orbSI->reject);
    if (orbSI->after != NULL) free (orbSI->after);
    if (orbSI->which != NULL) free (orbSI->which);
    if (orbSI->position != NULL) free (orbSI->position);
    if (orbSI->rsrcInfo != NULL) free (orbSI->rsrcInfo);
    if (orbSI->userName != NULL) free (orbSI->userName);
    free(orbSI);
  }
  return MDAS_SUCCESS;
}

int
getOrbStateInfo(orbStateInfo *orbSI, char *rsrcInfo,
         char *orbDataDesc, int orbFlags, 
	       int orbMode, char *userName)
{
  int i;
  char *orbDselect;
  char *orbDreject;
  char *orbDafter;
  char *orbDwhich;
  char *orbDposition;
  char *orbDtimeout;
  char *orbDnumofpkts;
  char *orbDpresentation;
  char *orbDnumbulkreads;
  char *tmpPtr;

  orbDselect = strstr(orbDataDesc,"<ORBSELECT>");
  orbDreject = strstr(orbDataDesc,"<ORBREJECT>");
  orbDafter  = strstr(orbDataDesc,"<ORBAFTER>");
  orbDwhich  = strstr(orbDataDesc,"<ORBWHICH>");
  orbDposition  = strstr(orbDataDesc,"<ORBPOSITION>");
  orbDtimeout  = strstr(orbDataDesc,"<ORBTIMEOUT>");
  orbDnumofpkts  = strstr(orbDataDesc,"<ORBNUMOFPKTS>");
  orbDpresentation  = strstr(orbDataDesc,"<ORBPRESENTATION>");
  orbDnumbulkreads  = strstr(orbDataDesc,"<ORBNUMBULKREADS>");


  if (orbDselect != NULL) {
    *orbDselect = '\0';
    orbDselect += 11;
    if ((tmpPtr  =  strstr(orbDselect,"</ORBSELECT>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDselect: %s\n",orbDselect);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->select  =strdup(orbDselect)) == NULL)
      return MEMORY_ALLOCATION_ERROR;
  }
  else 
    orbSI->select = NULL;

  if (orbDreject != NULL) {
    *orbDreject = '\0';
    orbDreject += 11;
    if ((tmpPtr  =  strstr(orbDreject,"</ORBREJECT>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDreject: %s\n",orbDreject);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->reject  =strdup(orbDreject)) == NULL)
	return MEMORY_ALLOCATION_ERROR;
  }
  else 
    orbSI->reject = NULL;

  if (orbDafter != NULL) {
    *orbDafter = '\0';
    orbDafter += 10;
    if ((tmpPtr  =  strstr(orbDafter,"</ORBAFTER>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDafter: %s\n",orbDafter);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->after  =strdup(orbDafter)) == NULL)
      return MEMORY_ALLOCATION_ERROR;
  }
  else 
    orbSI->after = NULL;

  if (orbDposition != NULL) {
    *orbDposition = '\0';
    orbDposition += 13;
    if ((tmpPtr  =  strstr(orbDposition,"</ORBPOSITION>")) == NULL) {
      fprintf(stdout,"getStateInfo: Error in orbDposition: %s\n",orbDposition);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->position  = strdup(orbDposition)) == NULL)
      return MEMORY_ALLOCATION_ERROR; 
  }
  else 
    orbSI->position  =  NULL;

  if (orbDpresentation != NULL) {
    *orbDpresentation = '\0';
      orbDpresentation += 17;
    if ((tmpPtr  =  strstr(orbDpresentation,"</ORBPRESENTATION>")) == NULL) {
      fprintf(stdout,"getStateInfo: Error in orbDpresentation: %s\n",
	      orbDpresentation);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->presentation  = strdup(orbDpresentation)) == NULL)
      return MEMORY_ALLOCATION_ERROR; 
  }
  else 
    orbSI->presentation  =  NULL;
  
  if (orbDwhich != NULL) {
    *orbDwhich = '\0';
    orbDwhich += 10;
    if ((tmpPtr  =  strstr(orbDwhich,"</ORBWHICH>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDwhich: %s\n",orbDwhich);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    if ((orbSI->which  = strdup(orbDwhich)) == NULL)
      return MEMORY_ALLOCATION_ERROR; 
  }
  else
    orbSI->which =  NULL;
  
  if (orbDtimeout != NULL) {
    *orbDtimeout = '\0';
    orbDtimeout += 12;
    if ((tmpPtr  =  strstr(orbDtimeout,"</ORBTIMEOUT>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDtimeout: %s\n",orbDtimeout);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    orbSI->timeout = atoi(orbDtimeout);
  }
  else 
    orbSI->timeout = 20;

  if (orbDnumofpkts != NULL) {
    *orbDnumofpkts = '\0';
    orbDnumofpkts += 14;
    if ((tmpPtr  =  strstr(orbDnumofpkts,"</ORBNUMOFPKTS>")) == NULL) {
      fprintf(stdout,"getStateInfo: Error in orbDnumofpkts: %s\n",
	      orbDnumofpkts);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
    orbSI->numofpkts = atoi(orbDnumofpkts);
  }
  else 
    orbSI->numofpkts = -1;

  if (orbDnumbulkreads != NULL) {
    *orbDnumbulkreads = '\0';
    orbDnumbulkreads += 17;
    if ((tmpPtr  =  strstr(orbDnumbulkreads,"</ORBNUMBULKREADS>")) == NULL) {
      fprintf(stdout, "getStateInfo:  Error in orbDnumbulkreads: %s\n",
	      orbDnumbulkreads);
      return(INP_ERR_RES_FORMAT);
    }
    *tmpPtr = '\0';
     orbSI->numbulkreads = atoi(orbDnumbulkreads);
  }
  else
    orbSI->numbulkreads = 1;

  orbSI->firstRead = 1;
  orbSI->orbFlags = orbFlags;
  orbSI->orbMode = orbFlags;
  if ((orbSI->userName  = strdup(userName)) == NULL)
    return MEMORY_ALLOCATION_ERROR; 
  if ((orbSI->rsrcInfo = strdup(rsrcInfo)) == NULL)
    return MEMORY_ALLOCATION_ERROR; 
  return MDAS_SUCCESS;
}

int
orbSpres(int first, orbStateInfo *orbSI, char *srcname,
	 double vorbtime, int pktid,
	 int nbytes, char *vorbpacket, char *buffer)
{
  int status,i;
    if (orbSI->presentation != NULL) {
      if (!strcmp(orbSI->presentation,"GPSHTML")) {
	status = orbSpresGPSHTML(first,orbSI->numofpkts,srcname,vorbtime,
				 pktid,nbytes,vorbpacket,buffer);
	return(status);
      }
      if (!strcmp(orbSI->presentation,"GPSXML")) {
	status = orbSpresGPSXML(first,orbSI->numofpkts,srcname,vorbtime,
				pktid,nbytes,vorbpacket,buffer);
	return(status);
      }
      if (strstr(orbSI->presentation,"WAVJAV1") == orbSI->presentation) {
	status = orbSpresWAVJAV1(first,orbSI->numofpkts,srcname,vorbtime,
			pktid,nbytes,vorbpacket,buffer, orbSI->presentation);
	return(status);
      }
      if (strstr(orbSI->presentation,"WAVJAV2") == orbSI->presentation) {
	status = orbSpresWAVJAV2(first,orbSI->numofpkts,srcname,vorbtime,
			pktid,nbytes,vorbpacket,buffer, orbSI->presentation);
	return(status);
      }
      else {
	status = orbSpresGeneric(first,orbSI->numofpkts,srcname,vorbtime,
				pktid,nbytes,vorbpacket,buffer);
	return(status);
      }
    }
    else {
      status = orbSpresGeneric(first,orbSI->numofpkts,srcname,vorbtime,
				pktid,nbytes,vorbpacket,buffer);
      return(status);
    }
}

int
orbSpresGeneric(int first, int last,char *srcname,double vorbtime, int pktid,
		int nbytes, char *vorbpacket, char *buffer)
{
  int i;

  if (last == 0) return (last);
  sprintf(buffer,"%s||%f||%i||%i||\n",srcname,vorbtime,pktid,nbytes);
  i = strlen(buffer);
  memmove(buffer+i,vorbpacket+2,nbytes-2);
  *(buffer+ i+nbytes-2 ) = '\n';
  *(buffer +i+nbytes+1) = '\0';
  return (nbytes +i );
}

int
orbSpresGPSHTML(int first, int last,char *srcname,double vorbtime, int pktid,
		int nbytes, char *vorbpacket, char *inbuffer)
{
  char *tmpPtr, *tmpPtr1;
  char buffer[MAX_HTML_ROW_SIZE];
  char tmpStr[TMP_STRING_SIZE];
  int i,j;

  if (last == 0) {
    strcat(inbuffer,"</FONT></BODY></HTML>\n");
    return(strlen(inbuffer));
  }


  
  if (first) {
    *inbuffer = '\0';
    sprintf(buffer,"<HTML><body bgcolor=#FFFFFFFF><FONT=#0000FF><TABLE><TR><TH>Header</TH><TH>Data</TH></TR>\n<TR><TD><B><FONT COLOR=#0000FF>Source:</FONT></B> %s  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>TimeStamp:</FONT></B> %f  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>Packet Id:</FONT></B> %i  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>Packet Size:</FONT></B> %i  &nbsp;&nbsp;&nbsp;</TD><TD>\n", 
	    srcname, vorbtime, pktid, nbytes);
  }
  else 
    sprintf(buffer,"<TABLE>\n<TR><TD><B><FONT COLOR=#0000FF>Source:</FONT></B> %s  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>TimeStamp:</FONT></B> %f  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>Packet Id:</FONT></B> %i  &nbsp;&nbsp;&nbsp;<BR><B><FONT COLOR=#0000FF>Packet Size:</FONT></B> %i  &nbsp;&nbsp;&nbsp;</TD><TD>\n", 
	    srcname, vorbtime, pktid, nbytes);

  tmpPtr = vorbpacket;
 while (tmpPtr < (vorbpacket + nbytes)) {
   while (*tmpPtr != '$' && tmpPtr < (vorbpacket + nbytes)) tmpPtr++;
  if (*tmpPtr != '$')
    break;
  if (strstr(tmpPtr,"$GPGGA") == tmpPtr) {
    strcat(buffer,"<B><FONT COLOR=#0000FF>PacketType:</FONT></B> $GPGGA<BR>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (strlen(tmpPtr) >= 6)
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Position UTC:</FONT></B> %c%c:%c%c:%s Z<BR>\n",*tmpPtr,
	      *(tmpPtr+1),*(tmpPtr+2),*(tmpPtr+3),tmpPtr+4);
    else 
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Position UTC:</FONT></B> %s Z<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Position Latitude:</FONT></B> %c%cd%s' %c<BR>\n",*tmpPtr, *(tmpPtr+1),  tmpPtr+2, *(tmpPtr1+1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Position Longitude:</FONT></B> %c%c%cd%s' %c<BR>\n",*tmpPtr, *(tmpPtr+1), *(tmpPtr+2), tmpPtr+3, *(tmpPtr1+1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    if (*tmpPtr == '0')
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Fix Quality:</FONT></B> Invalid<BR>\n");
    else if (*tmpPtr == '1')
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Fix Quality:</FONT></B> GPS Fix<BR>\n");
    else if (*tmpPtr == '1')
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Fix Quality:</FONT></B> Dif. GPS Fix<BR>\n");
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr + 2;
    tmpPtr1 = tmpPtr + 2;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Num of Satellites in View:</FONT></B> %i<BR>\n",atoi(tmpPtr));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Horizontal Dilution of Precision (HDOP):</FONT></B> %s<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Antenna Altitude:</FONT></B> %s Mtrs<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Height of geoid above WGS84 ellipsoid:</FONT></B> %s Mtrs<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Age of Differential GPS data:</FONT></B> %s secs<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != '*') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Differential reference station ID:</FONT></B> %s<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
  }
  else if (strstr(tmpPtr,"$GPVTG") == tmpPtr) {
    strcat(buffer,"<B><FONT COLOR=#0000FF>PacketType:</FONT></B> $GPVTG<BR>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    tmpPtr1++;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
/*    if (!strcmp(tmpPtr,",,")){*/
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Track Made Good:</FONT></B> %s<BR>\n",tmpPtr);
      strcat(buffer,tmpStr);
   /* }*/
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    tmpPtr1++;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    /* if (!strcmp(tmpPtr,",,")){ */
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Magnetic Track Made Good:</FONT></B> %s<BR>\n",tmpPtr);
      strcat(buffer,tmpStr);
   /* }*/
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (*(tmpPtr1 +1) == 'N')
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Ground Speed:</FONT></B> %s knots<BR>\n",tmpPtr);
    else
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Ground Speed:</FONT></B> %s %c<BR>\n",tmpPtr,*(tmpPtr1 +1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (*(tmpPtr1 +1) == 'K')
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Ground Speed:</FONT></B> %s kmph<BR>\n",tmpPtr);
    else
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Ground Speed:</FONT></B> %s %c<BR>\n",tmpPtr,*(tmpPtr1 +1));
    strcat(buffer,tmpStr);
  }
  else if ( strstr(tmpPtr,"$GPZDA") == tmpPtr) {
    strcat(buffer,"<B><FONT COLOR=#0000FF>PacketType:</FONT></B> $GPZDA<BR>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (strlen(tmpPtr) >= 6)
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Time UTC:</FONT></B> %c%c:%c%c:%s Z<BR>\n",*tmpPtr,
	      *(tmpPtr+1),*(tmpPtr+2),*(tmpPtr+3),tmpPtr+4);
    else 
      sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Time UTC:</FONT></B> %s Z<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;  
    tmpPtr1 = tmpPtr + 10;
    *(tmpPtr + 2) = '/';
    *(tmpPtr + 5) = '/';
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Date:</FONT></B> %s<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 =':';
    *(tmpPtr1 + 3) = '\0';
    sprintf(tmpStr,"<B><FONT COLOR=#FF0000>Local Time Correction:</FONT></B> %s<BR>\n",tmpPtr);
    strcat(buffer,tmpStr);
  }
  else {
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ' ') tmpPtr1++;
    tmpPtr1 ='\0';
    strcat(buffer,"<B><FONT COLOR=#FF0000>PacketType:</FONT></B> %s<BR>\n");
    strcat(buffer,  tmpPtr);
    strcpy(inbuffer,buffer);
    i = strlen(inbuffer);
    j = nbytes - (int) (tmpPtr1 - vorbpacket);
    memcpy((char *) (inbuffer + i),tmpPtr1, j);
    sprintf((char *) (inbuffer + i+j),"</TD></TR></TABLE></FONT><HR>\n");
    return (i+j+ strlen("</TD></TR></TABLE></FONT><HR>\n"));
  }
}
  strcat(buffer,"</TD></TR></TABLE></FONT><HR>\n");
  if (last == 0)
    strcat(buffer,"</FONT></BODY></HTML>\n");
  strcpy(inbuffer,buffer);
  return(strlen(inbuffer));
}

int
orbSpresGPSXML(int first, int last, char *srcname,double vorbtime, int pktid,
		int nbytes, char *vorbpacket, char *inbuffer)
{
  char *tmpPtr, *tmpPtr1;
  char buffer[MAX_HTML_ROW_SIZE];
  char tmpStr[TMP_STRING_SIZE];
  int i,j;

  if (last == 0) {
    strcat(inbuffer,"</PACKETSET>\n"); 
    return (strlen(inbuffer));
  }

  if (first) {
    *inbuffer = '\0';
    sprintf(buffer,"<?xml version=\"1.0\"?><PACKETSET><PACKET><HEADER><Source>%s</Source>\n<TimeStamp>%f</TimeStamp>\n<PacketId>%i</PacketId>\n<PacketSize>%i</PacketSize>\n</HEADER><BODY>\n", 
	    srcname, vorbtime, pktid, nbytes);
  }
  else 
    sprintf(buffer,"<PACKET><HEADER><Source>%s</Source>\n<TimeStamp>%f</TimeStamp>\n<PacketId>%i</PacketId>\n<PacketSize>%i</PacketSize>\n</HEADER><BODY>\n", 
	    srcname, vorbtime, pktid, nbytes);

  tmpPtr = vorbpacket;
 while (tmpPtr < (vorbpacket + nbytes)) {
   while (*tmpPtr != '$' && tmpPtr < (vorbpacket + nbytes)) tmpPtr++;
   if (*tmpPtr != '$')
     break;
  if (strstr(tmpPtr,"$GPGGA") == tmpPtr) {
    strcat(buffer,"<GPGGAPacket>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (strlen(tmpPtr) >= 6)
      sprintf(tmpStr,"<PositionUTC>%c%c:%c%c:%s Z</PositionUTC>\n",*tmpPtr,
	      *(tmpPtr+1),*(tmpPtr+2),*(tmpPtr+3),tmpPtr+4);
    else 
      sprintf(tmpStr,"<PositionUTC>%s Z</PositionUTC>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<PositionLatitude>%c%cd%s' %c</PositionLatitude>\n",*tmpPtr, *(tmpPtr+1),  tmpPtr+2, *(tmpPtr1+1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<PositionLongitude>%c%c%cd%s' %c</PositionLongitude>\n",*tmpPtr, *(tmpPtr+1), *(tmpPtr+2), tmpPtr+3, *(tmpPtr1+1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    if (*tmpPtr == '0')
      sprintf(tmpStr,"<FixQuality>Invalid</FixQuality>\n");
    else if (*tmpPtr == '1')
      sprintf(tmpStr,"<FixQuality>GPS Fix</FixQuality>\n");
    else if (*tmpPtr == '1')
      sprintf(tmpStr,"<FixQuality>Dif. GPS Fix</FixQuality>\n");
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr + 2;
    tmpPtr1 = tmpPtr + 2;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<NumOfSatellitesInView>%i</NumOfSatellitesInView>\n",atoi(tmpPtr));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<HorizontalDilutionOfPrecision>%s</HorizontalDilutionOfPrecision>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<AntennaAltitude>%s Mtrs</AntennaAltitude>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<HeightOfGeoidAboveWGS84Ellipsoid>%s Mtrs</HeightOfGeoidAboveWGS84Ellipsoid>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<AgeOfDifferentialGPSdata>%s secs</AgeOfDifferentialGPSdata>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != '*') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<DifferentialReferenceStationID>%s</DifferentialReferenceStationID>\n",tmpPtr);
    strcat(buffer,tmpStr);
    strcat(buffer,"</GPGGAPacket>\n");
  }
  else if (strstr(tmpPtr,"$GPVTG") == tmpPtr) {
    strcat(buffer,"<GPVTGPacket>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    tmpPtr1++;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
      sprintf(tmpStr,"<TrackMadeGood>%s</TrackMadeGood>\n",tmpPtr);
      strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    tmpPtr1++;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<MagneticTrackMadeGood>%s</MagneticTrackMadeGood>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (*(tmpPtr1 +1) == 'N')
      sprintf(tmpStr,"<GroundSpeed>%s knots</GroundSpeed>\n",tmpPtr);
    else
      sprintf(tmpStr,"<GroundSpeed>%s %c</GroundSpeed>\n",tmpPtr,*(tmpPtr1 +1));
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 3;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (*(tmpPtr1 +1) == 'K')
      sprintf(tmpStr,"<GroundSpeed>%s kmph</GroundSpeed>\n",tmpPtr);
    else
      sprintf(tmpStr,"<GroundSpeed>%s %c</GroundSpeed>\n",tmpPtr,*(tmpPtr1 +1));
    strcat(buffer,tmpStr);
    strcat(buffer,"</GPVTGPacket>\n");
  }
  else if ( strstr(tmpPtr,"$GPZDA") == tmpPtr) {
    strcat(buffer,"<GPZDAPacket>\n");
    tmpPtr += 7;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 ='\0';
    if (strlen(tmpPtr) >= 6)
      sprintf(tmpStr,"<TimeUTC>%c%c:%c%c:%s Z</TimeUTC>\n",*tmpPtr,
	      *(tmpPtr+1),*(tmpPtr+2),*(tmpPtr+3),tmpPtr+4);
    else 
      sprintf(tmpStr,"<TimeUTC>%s Z</TimeUTC>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr + 10;
    *(tmpPtr + 2) = '/';
    *(tmpPtr + 5) = '/';
    *tmpPtr1 ='\0';
    sprintf(tmpStr,"<Date>%s</Date>\n",tmpPtr);
    strcat(buffer,tmpStr);
    tmpPtr = tmpPtr1 + 1;
    tmpPtr1 = tmpPtr;
    while (*tmpPtr1 != ',') tmpPtr1++;
    *tmpPtr1 =':';
    *(tmpPtr1 + 3) = '\0';
    sprintf(tmpStr,"<LocalTimeCorrection>%s</LocalTimeCorrection>\n",tmpPtr);
    strcat(buffer,tmpStr);
    strcat(buffer,"</GPZDAPacket>\n");
  }
  else {
    strcpy(inbuffer,buffer);
    i = strlen(inbuffer);
    j = nbytes - (int) (tmpPtr - vorbpacket);
    memcpy((char *) (inbuffer + i),tmpPtr, j);
    sprintf((char *) (inbuffer + i+j),"</BODY>\n</PACKET>\n</PACKETSET>\n");
    return (i+j+ strlen("</BODY>\n</PACKET>\n</PACKETSET>\n"));
  }
 }
  strcat(buffer,"</BODY>\n</PACKET>\n");
  if (last == 0)
    strcat(buffer,"</PACKETSET>\n");
  strcpy(inbuffer,buffer);
  
  return(strlen(inbuffer));
}
int
orbSpresWAVJAV1(int first, int last, char *srcname,double vorbtime, int pktid,
		int nbytes, char *vorbpacket, char *inbuffer, char *pres)
{
  char *tmpPtr, *tmpPtr1;
  char buffer[MAX_HTML_ROW_SIZE];
  char tmpStr[TMP_STRING_SIZE];
  int i,j;
static  char *begPtr, *endPtr;

  if (last == 0) {
    strcat(inbuffer,"\"></APPLET></FONT></BODY></HTML>\n");
    return(strlen(inbuffer));
  }

  if (first) {
    *inbuffer ='\0';
    begPtr = strstr(pres,"||"); 
    if (begPtr == NULL) return(INP_ERR_RES_FORMAT);
    begPtr++; begPtr++;
    endPtr = strstr(begPtr,"||");
    if (endPtr == NULL) return(INP_ERR_RES_FORMAT);
    *endPtr = '\0';
    endPtr++; endPtr++; 
    tmpPtr = strstr(endPtr,"||");
    if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
    *tmpPtr = '\0';
    sprintf(buffer,"<HTML><body bgcolor=#FFFFFFFF><FONT=#0000FF><h3>Source:%s</h3><APPLET CODEBASE=\"http://www5.lawson.com/lawson/java\" CODE=\"lawson.jchart.ChartApplet.class\" WIDTH=400 HEIGHT=200><PARAM NAME=\"chartType\" VALUE=\"line,vbar,vstackbar,hbar,hstackbar\"><PARAM NAME=\"dataset0.labels\" VALUE=\"\"><PARAM NAME=\"dataset0.name\" VALUE=\"Channel %s\"><PARAM NAME=\"dataset0.values\" VALUE=\"",srcname,begPtr);
   }
  else {
    sprintf(buffer,",");
  }
  tmpPtr = strstr(vorbpacket+2,begPtr);
  if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
  tmpPtr += strlen(begPtr);
  tmpPtr1 = strstr(tmpPtr, endPtr);
  if (tmpPtr1 == NULL) return(INP_ERR_RES_FORMAT);
  *tmpPtr1 = '\0';

  strcat(buffer,tmpPtr);
  if (last == 0)
    strcat(buffer,"\"></APPLET></FONT></BODY></HTML>\n");
  strcpy(inbuffer,buffer);
  return(strlen(inbuffer));
}

int
orbSpresWAVJAV2(int first, int last, char *srcname,double vorbtime, int pktid,
		int nbytes, char *vorbpacket, char *buffer, char *pres)
{
  char *tmpPtr, *tmpPtr1;
  int i,j,ii;
static  char *begPtr, *endPtr;   /* Y coordinate */
static  char *begPtr2, *endPtr2; /* X coordinate */
static  char tmpStr1[MAX_CURVE_SIZE]; /* Y values */
static  char tmpStr2[MAX_CURVE_SIZE]; /* X values */

  if (last == 0) {
    strcat(buffer,"<PARAM NAME=\"dataset0.labels\" VALUE=\"");
    strcat(buffer,tmpStr2);
    strcat(buffer,"\"><PARAM NAME=\"dataset0.values\" VALUE=\"");
    strcat(buffer,tmpStr1);
    strcat(buffer,"\"></APPLET></FONT></BODY></HTML>\n");
    return(strlen(buffer));
  }

  ii =  first;

  if (first) {
    *buffer ='\0';
    tmpStr2[0] ='\0';
    tmpStr1[0] ='\0';
    begPtr = strstr(pres,"||"); 
    if (begPtr == NULL) return(INP_ERR_RES_FORMAT);
    begPtr++; begPtr++;
    endPtr = strstr(begPtr,"||");
    if (endPtr == NULL) return(INP_ERR_RES_FORMAT);
    *endPtr = '\0';
    endPtr++; endPtr++; 
    tmpPtr = strstr(endPtr,"||");
    if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
    *tmpPtr = '\0';
    tmpPtr++;tmpPtr++;
    begPtr2 = tmpPtr;
    endPtr2 = strstr(begPtr2,"||");
    if (endPtr2 == NULL) return(INP_ERR_RES_FORMAT);
    *endPtr2 = '\0';
    endPtr2++; endPtr2++; 
    tmpPtr = strstr(endPtr2,"||");
    if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
    *tmpPtr = '\0';
    sprintf(buffer,"<HTML><body bgcolor=#FFFFFFFF><FONT=#0000FF><h3>Source:%s</h3><APPLET CODEBASE=\"http://www5.lawson.com/lawson/java\" CODE=\"lawson.jchart.ChartApplet.class\" WIDTH=700 HEIGHT=200><PARAM NAME=\"chartType\" VALUE=\"line,vbar,vstackbar,hbar,hstackbar\"><PARAM NAME=\"dataset0.name\" VALUE=\"Y-axis: Channel %s & X-axis: Channel %s \">",srcname,begPtr, begPtr2);
  }
  else {
    strcat(tmpStr1,",");
    strcat(tmpStr2,",");
  }
  /*<PARAM NAME=\"dataset0.values\" VALUE=\"",srcname,begPtr);
    }
    else {
    sprintf(buffer,",");
    }
  */
  tmpPtr = strstr(vorbpacket+2,begPtr);
  if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
  tmpPtr += strlen(begPtr);
  tmpPtr1 = strstr(tmpPtr, endPtr);
  if (tmpPtr1 == NULL) return(INP_ERR_RES_FORMAT);
  *tmpPtr1 = '\0';
  strcat(tmpStr1,tmpPtr);
  
  *tmpPtr1 = endPtr[0];
  tmpPtr = strstr(vorbpacket+2,begPtr2);
  if (tmpPtr == NULL) return(INP_ERR_RES_FORMAT);
  tmpPtr += strlen(begPtr2);
  tmpPtr1 = strstr(tmpPtr, endPtr2);
  if (tmpPtr1 == NULL) return(INP_ERR_RES_FORMAT);
  *tmpPtr1 = '\0';
  strcat(tmpStr2,tmpPtr);
  *tmpPtr1 = endPtr2[0];
  if (last == 0) {
    strcat(buffer,"<PARAM NAME=\"dataset0.labels\" VALUE=\"");
    strcat(buffer,tmpStr2);
    strcat(buffer,"\"><PARAM NAME=\"dataset0.values\" VALUE=\"");
    strcat(buffer,tmpStr1);
    strcat(buffer,"\"></APPLET></FONT></BODY></HTML>\n");
  }
  return(strlen(buffer));
}

