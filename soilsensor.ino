#include <SoftwareSerial.h> //include the software serial library
int sensor; //variable to store sensor values
int data; //variable to stored mapped sensor values
int pump_status; //variable to store pump status values




SoftwareSerial esp8266(3, 4); //set the software serial pins RX pin = 3, TX pin = 4 
//definition of variables
#define DEBUG true //show messages between ESP8266 and Arduino in serial port, when set to true
#define SSID "CHAITUMAVERICK 2130" //replace x with your wifi network name
#define pwd "796@8yW7" //replace x with your wifi network password


String sendAT(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
void updateTS(String T,String P){
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, DEBUG);    
  delay(2000);
  String cmdlen;
  String cmd="GET /update?key=EKR3P1JLD3AXZGNE&field1="+T+"&field2="+P+"\r\n"; // update the temprature and humidity values on thingspeak url,replace xxxxxxx with your write api key
   cmdlen = cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n", 2000, DEBUG);
   esp8266.print(cmd);
   Serial.println("");
   sendAT("AT+CIPCLOSE\r\n", 2000, DEBUG);
   Serial.println("");
   delay(2000);
   }
void setup()
{
  pinMode(A0,INPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);// begin the serial communication with baud of 9600
  esp8266.begin(9600);// begin the software serial communication with baud rate 9600
  
  sendAT("AT+RST\r\n", 2000, DEBUG); // call sendAT function to send reset AT command
  sendAT("AT\r\n", 1000, DEBUG);
  sendAT("AT+CWMODE=1\r\n", 1000, DEBUG); //call sendAT function to set ESP8266 to station mode
  sendAT("AT+CWLAP",1000,DEBUG);
  sendAT("AT+CWJAP=\"SSID\",\"pwd\"", 2000, DEBUG); //AT command to connect wit the wifi network

  /*while(!esp8266.find("OK")) {
    Serial.println("waiting");//wait for connection
  } */
  sendAT("AT+CIFSR\r\n", 1000, DEBUG); //AT command to print IP address on serial monitor
  sendAT("AT+CIPMUX=0\r\n", 1000, DEBUG); //AT command to set ESP8266 to multiple connections

  
}




void loop(){
  // put your main code here, to run repeatedly:
  sensor=analogRead(A0); //read raw sensor data and store it in variable sensor
  data=map(sensor,0,1023,100,0); //map the raw sensor data and store the result in data variable
  String sensor_value=String(data); //convert interger to string data type
  Serial.print("Soil Moisture: "); //print soil moisture on serial monitor
  Serial.println(data); //print soil moisture value on serial monitor

  if(data<40){ //check if sensor value is less then 76
    digitalWrite(13,HIGH); //switch on the water pump
    pump_status=100; //update pump status variable value to 100
  }
  else{
    digitalWrite(13,LOW); //switch off the water pump
    pump_status=0; //update pump status variable value to 0
  }
  

  String pump = String(pump_status); //convert integer to string data type
  updateTS(sensor_value,pump); //call the function to update ThingSpeak channel
  delay(3000);
}








