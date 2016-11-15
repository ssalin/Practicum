/*
*
*  BT Serial Test
*
*
*/


// Auth Data - For validating the host
typedef struct{
  bool auth_set;    // Authentication Paired
  int touch;        // Last-updated (date/time)
  double key;       // Auth-key
  double poll_freq; // Polling Frequency (power management)
} auth_data;

auth_data *auth_dat;              // Auth Data


void setup() {
  
  // Init Auth Struct
  *auth_dat = {false,0,0.0,0.0};
  Serial.begin(9600);       // Enable Serial
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Test");
  delay(10000);

}
