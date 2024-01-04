#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "config.h"
#include "util.h"
#include "gps.h"

GPS_Runtime runtime;

int GPS_copyField(char *dest, char *src, int cnt)
{
    int i;

	for(i=0;i<cnt;i++) {
        dest[i] = src[i];
	}
	dest[cnt]=',';

	return (cnt+1);
}

int GPS_copyField2(char *dest, char *src)
{
	int ndx=0;

	if(*src == 0 || *src == ',') {
	     dest[0] = '0';
		 dest[1] = ',';
		 dest[2] = 0;
		 return 2;
	}

	while(ndx<20) {
	   if(src[ndx] == 0 || src[ndx]=='.' || src[ndx] == ',') {
			 break;
	   }
	   dest[ndx]=src[ndx];
	   ndx++;
	}
	dest[ndx]=',';
	dest[ndx+1]=0;
	return ndx+1;
}

int GPS_copyField3(char *dest, char *src, char term_char, int max_buf)
{
	int ndx=0;

	if(*src == 0 || *src == term_char) {
	     dest[0] = 0;
		 return 0;
	}

	while(ndx<max_buf) {
	   if(src[ndx] == 0 || src[ndx]==term_char) {
			 break;
	   }
	   dest[ndx]=src[ndx];
	   ndx++;
	}
	dest[ndx]=0;
	return ndx;
}

int GPS_ConvertKnotToKmh(int knot)
{
    return( (knot*185) /100 );
 	// u16 km_h = (u16) (atof(adl_gpsSpeed.speed_knots) * 1.8519984);
}

int GPS_processString(char *pos_str)
{
    int cs1_ok, cs2_ok;
	int i, res;

	// we need to convert the string....
 	//
 	if(!strncmp(pos_str, "$GPRMC", 6)) {

		GPS_ProcessString_RMC(runtime.GPS_Buffer);
	}
	else if(!strncmp(pos_str, "$GPGGA", 6)) {
		GPS_ProcessString_GGA(&runtime.GPS_Buffer[MAX_GPS_BUFFER/2]);
	}

	// NOTE: if the GPS coordinate is valid, then we determine the max
	// number of satellites used in the system. This will give us info about
	// how well GPS system works
	//
    if(runtime.gps_valid) {

		if(runtime.gpsMaxSatellites < runtime.gps_no_of_sats) {

			// for statistics
			//
		 	runtime.gpsMaxSatellites = runtime.gps_no_of_sats;
		 }
	 }
}

void GPS_getDataString(char *msg_str, int msg_format)
{
	if(!runtime.gps_valid) {
		sprintf(msg_str, "U%s,X", runtime.imei_str);
		return;
	}

	switch(msg_format)
	{
		case MESSAGE_FORMAT_LONG:
			sprintf(msg_str, "A%s,%d,%s%s,%s,%s,%s,%s,%s", runtime.imei_str, runtime.gps_no_of_sats, runtime.gps_date, runtime.gps_time, runtime.gps_lat, runtime.gps_long, runtime.gps_altitude, runtime.gps_speed_km, runtime.gps_dir);
			break;
		case MESSAGE_FORMAT_SHORT:
   	        sprintf(msg_str, "B%s,%s,%s,%s,%d,%d,%d", runtime.imei_str, runtime.gps_time, runtime.gps_lat, runtime.gps_long, runtime.gps_altitude_short, runtime.gps_speed_km_short, runtime.gps_dir_short);
			break;
	 }
}

// There is a provision for a checksum at the end of each
// sentence which may or may not be checked by the unit
// that reads the data. The checksum field consists of a '*'
// and two hex digits representing the exclusive OR of all
// characters between, but not including, the '$' and '*'.
// A checksum is required on some sentences.
//
int GPS_ValidateChecksum(char *str)
{
 	 int i=1;
	 char *ptr, *ptr1;
	 int cs_val;
	 int valu=0;
	 char cs[4];

	 if(str[0] != '$') return FALSE;

	 ptr1 = strchr(str, '*');
	 if(ptr1 == NULL) {
	     return FALSE;
	 }
	 cs[0] = *(ptr1+1);
	 cs[1] = *(ptr1+2);
	 cs[2] = 0;

	 cs_val = hexasc2num(cs,2);

	 for(ptr=&str[1]; ptr < ptr1; ptr++) {
	     valu ^= *ptr;
	 }

	 return (cs_val == valu);
}

// INPUT
//         $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
//                0      1 2        3 4         5 6     7     8      9     10
//
int GPS_ProcessString_RMC(char *str)
{
    char *plc[13];
	char *ptr, tmp[10];
	int i, hr, ndx, nosat;

	// determine the positions of comma signs
	//
	ptr = str;
	for(i=0;i<10;i++) {
		plc[i] = strchr(ptr, ',');
		plc[i]++;
		ptr = plc[i];
	}

	if(*plc[1] == 'V') {
	    // GPS coordinate is not valid
		//
	    runtime.gps_valid = FALSE;
		return FALSE;
	}

	runtime.gps_valid = TRUE;

	strncpy(runtime.gps_time, plc[0], 6); runtime.gps_time[6] = 0;
	strncpy(runtime.gps_date, plc[8], 6); runtime.gps_date[6] = 0;

	ptr = plc[2];
	GPS_copyField3(runtime.gps_lat_short, ptr, ',', MAX_GPS_LONGTITUDE);
	if(*ptr == '0') ptr++;	 			 		 // discard preceeding 0s
	if(*ptr == '0') ptr++;						 // discard preceeding 0s
	GPS_copyField3(runtime.gps_lat, ptr, ',', MAX_GPS_LATITUDE);
	GPS_TruncateNumber(runtime.gps_lat); 	     // discard 0s at the right
	tmp[0] = *plc[3]; tmp[1]=0;
	strcat(runtime.gps_lat, tmp);

	ptr = plc[4];
	GPS_copyField3(runtime.gps_long_short, ptr, ',', MAX_GPS_LONGTITUDE);
	if(*ptr == '0') ptr++;   					 // discard preceeding 0s
	if(*ptr == '0') ptr++;						 // discard preceeding 0s
	GPS_copyField3(runtime.gps_long, ptr, ',', MAX_GPS_LONGTITUDE);
	GPS_TruncateNumber(runtime.gps_long); 	   	 // discard 0s at the right
	tmp[0] = *plc[5]; tmp[1]=0;
	strcat(runtime.gps_long, tmp);

	GPS_copyField3(tmp, plc[6], ',', MAX_GPS_SPEED);
	ptr = strchr(tmp, '.');
	if(ptr) {
	    *ptr = 0;
	}
	i = atoi(tmp);
	i = (185*i) / 100;		// knots to km
	sprintf(runtime.gps_speed_km, "%d", i);
	runtime.gps_speed_km_short = i;

	GPS_copyField3(tmp, plc[7], ',', MAX_GPS_DIRECTION);
	ptr = strchr(tmp, '.');
	if(ptr) {
	    *ptr = 0;
	}
	i = atoi(tmp);
	sprintf(runtime.gps_dir, "%d", i);
	runtime.gps_dir_short = i;

	return TRUE;
}

// truncate zeros on the rightmost part of the number
//
void GPS_TruncateNumber(char *str)
{
 	 int i, len;

	 // if string is null, get out
	 //
	 if(str[0] == 0) return;

	 // if no dot sign in the floating point number no need to do anything
	 //
	 if(!strchr(str,'.')) return;

	 // get the length of the string
	 //
	 len = strlen(str);
	 for(i=len-1;i>=0;i--) {
	     if(str[i] != '0') break;
		 else str[i]=0;
	 }
}

// $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
//        0      1        2 3         4 5 6  7   8     9 10   11
//
int GPS_ProcessString_GGA(char *str)
{
    char *plc[10];
	char *ptr, tmp[10];
	int i, hr, ndx, nosat;

	// determine the positions of comma signs
	//
	ptr = str;
	for(i=0;i<12;i++) {
		plc[i] = strchr(ptr, ',');
		plc[i]++;
		ptr = plc[i];
	}

	if(*plc[5] == ',' || *plc[5] == '0') {
	   	runtime.gps_valid = FALSE;
		return FALSE;
	}

	runtime.gps_valid = TRUE;

	GPS_copyField2(tmp, plc[6]);
	runtime.gps_no_of_sats = atoi(tmp);

	GPS_copyField2(tmp, plc[8]);
	i = atoi(tmp);
	sprintf(runtime.gps_altitude, "%d", i);
	runtime.gps_altitude_short = i;

	GPS_copyField3(runtime.gps_hdop, plc[7], ',', MAX_GPS_HDOP);

	return TRUE;
}

void GPS_Test(void)
{
}
