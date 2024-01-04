void GPS_ProcessString(char *procStr, char *rawStr);
int GPS_copyField(char *dest, char *src, int cnt);
int GPS_copyField2(char *dest, char *src);
int GPS_copyField3(char *dest, char *src, char term_char, int max_buf);

void GPS_GetCurrentPosition_2(char *pos_str);
int GPS_ProcessString_GGA(char *str);
int GPS_ProcessString_RMC(char *str);
int GPS_ValidateChecksum(char *str);
void GPS_TruncateNumber(char *str);

void GPS_Test(void);

