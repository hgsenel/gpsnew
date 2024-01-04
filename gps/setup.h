// format of the gps information packet to be sent to the server
//
#define MESSAGE_FORMAT_LONG				  			 		0
#define MESSAGE_FORMAT_SHORT								1

// the default message format
//
#define MESSAGE_FORMAT_DEFAULT   MESSAGE_FORMAT_COMPATIBLE

#define MAX_GPS_BUFFER      300
#define MAX_GPS_TIME     	7	
#define MAX_GPS_DATE        7
#define MAX_GPS_LATITUDE    15
#define MAX_GPS_LONGTITUDE  15
#define MAX_GPS_ALTITUDE	15
#define MAX_GPS_SPEED		15
#define MAX_GPS_DIRECTION	15
#define MAX_GPS_HDOP		10
#define MAX_OS_VERSION		20
#define MAX_TCP_VERSION		20
#define MAX_GPS_VERSION     20
#define MAX_ANTENNA_STATUS	20
#define MAX_Q2501B_BUFFER 	500
#define MAX_OPERATOR_NAME   10

typedef struct _gps_runtime_struct 
{
    int         version;			   				 // program version

    char   	imei_str[25];

    int  	gps_valid;							// is the current position valid? 
    char 	gps_time[MAX_GPS_TIME];			    // 110232       Time - 11:02:32
    char 	gps_date[MAX_GPS_DATE];			    // 230394       Date - 23rd of March 1994
    char 	gps_lat[MAX_GPS_LATITUDE];			// latitude  with direction at the end NWSE
    char 	gps_lat_short[MAX_GPS_LATITUDE];	// latitude  with direction at the end NWSE
    char 	gps_long[MAX_GPS_LONGTITUDE];       // longtitude with direction at the end NWSE
    char 	gps_long_short[MAX_GPS_LONGTITUDE]; // longtitude with direction at the end NWSE
    int  	gps_no_of_sats;		                // number of satellites
    char 	gps_altitude[MAX_GPS_ALTITUDE];	    // altitude in meter+cm
    int  	gps_altitude_short;	                // altitude in meters
    char 	gps_speed_km[MAX_GPS_SPEED];        // speed in km/h
    int  	gps_speed_km_short;                 // speed in km/h (short)
    char 	gps_hdop[MAX_GPS_HDOP];	   		    // horizontal position of dilution
    char 	gps_dir[MAX_GPS_DIRECTION];	        // direction w.r.t. north pole 
    int  	gps_dir_short;	                    // direction w.r.t. north pole (only degrees)

    char 	GPS_Buffer[MAX_GPS_BUFFER];
    int 	gpsMaxSatellites;
} GPS_Runtime;

