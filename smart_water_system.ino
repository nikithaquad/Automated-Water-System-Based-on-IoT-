
#include<SoftwareSerial.h>
#include <Servo.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>   //https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
LiquidCrystal_I2C lcd(0x27, 16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

SoftwareSerial esp(10,11);  //RX,TX  ESP8266
boolean thingSpeakWrite(float value1);

const int trigPin = 4;    // UPPER
const int echoPin = 5;
long duration;
int distance;

const int trigPin2 = 6;   // LOWER
const int echoPin2 = 7;
long duration2;
int distance2;

int motorpin1 = 8;     // PUMP
int motorpin2 = 9; 

int motorpin3 = 2;     // pump
int motorpin4 = 3;

int Red_Led=A0;
int yellow_Led=A1;
int Green_Led=A2;

int servoPin = A3; 
Servo Servo1;

/**************************************************************************************************************/

String ssid="nikitha";                                 // Wifi network SSID
String password ="nikitha123";                         // Wifi network password
String apikey="U7XXS0EOYG4TETXP";
boolean DEBUG=true;
int count=0;

int gdist;
int gdist2;


/******************************* showResponse ***************************************************************/

void showResponse(int waitTime)
{
    long t=millis();
    char c;
    while (t+waitTime>millis())
    {
      if (esp.available())
      {
        c = esp.read();
        if (DEBUG) 
        {
          Serial.print(c);
        }
      }
    }               
}

/******************************Setup Function ****************************************************************************/

void setup()
{
     Serial.begin(9600); // Starts the serial communication
     esp.begin(115200); // wifi
     lcd.init();       // Initialize the =LCD and Print a message to the LCD.
     lcd.backlight(); // To Power ON the back light
     delay(1000);
     
     Servo1.attach(servoPin);
     pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
     pinMode(echoPin, INPUT); // Sets the echoPin as an Input
     pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
     pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
     pinMode(Red_Led,OUTPUT);
     pinMode(yellow_Led,OUTPUT);
     pinMode(Green_Led,OUTPUT);
     pinMode(motorpin1,OUTPUT);       
     pinMode(motorpin2,OUTPUT); 
     pinMode(motorpin3,OUTPUT);       
     pinMode(motorpin4,OUTPUT);
     delay(1000);
     
     Servo1.write(0);
     digitalWrite(Red_Led,LOW);
     digitalWrite(yellow_Led,LOW);
     digitalWrite(Green_Led,LOW); 
     digitalWrite(motorpin1,LOW);    // MOTOR OFF
     digitalWrite(motorpin2,LOW);
     digitalWrite(motorpin3,LOW);    // MOTOR OFF
     digitalWrite(motorpin4,LOW);
     delay(1000);

     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("  Smart Water ");
     lcd.setCursor(0,1);
     lcd.print("    System");
     delay(2500);
      
     esp.println("AT+CWMODE=1");                                             // set esp8266 as client
     showResponse(1000);
     esp.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");                  // set your home router SSID and password
     showResponse(5000);
     Serial.println();
     if (DEBUG)
     {
      Serial.println("Setup completed");
     }
      
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Wi-Fi Connected"); 
     delay(2500);
     
}

/****************************** thingSpeakWrite ***********************************************/

boolean thingSpeakWrite( float gdist)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";                                                    
  cmd += "184.106.153.149";                                                                
  cmd += "\",80";

  esp.println(cmd);
  if (DEBUG)
  {
    //Serial.println(cmd);
  }
  if(esp.find("Error"))
  {
    if (DEBUG) 
    {
      //Serial.println("AT+CIPSTART error");
    }
    return false;
  }
  
  String getStr = "GET /update?api_key=";             // prepare GET string
  getStr += apikey;
  
  getStr +="&field1=";
  getStr += String(gdist);
  getStr += "\r\n\r\n";

  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  esp.println(cmd);
  if (DEBUG)  //Serial.println(cmd);
  
  delay(100);
  if(esp.find(">"))
  {
    esp.print(getStr);
    if (DEBUG) 
    {
      //Serial.print(getStr);
    }
  }
  else
  {
    esp.println("AT+CIPCLOSE");
    if (DEBUG)   
    {
      //Serial.println("AT+CIPCLOSE");
    }
    return false;
  }
  return true;
}

/***************************  Ultrasonic_Sensor  ****************************************/
 
void Ultrasonic_upper()
{ 
      digitalWrite(trigPin, LOW);             // Clears the trigPin
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);            // Sets the trigPin on HIGH state for 10 micro seconds
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      
      duration = pulseIn(echoPin, HIGH);      // Reads the echoPin, returns the sound wave travel time in microseconds
      distance= duration*0.034/2;             // Calculating the distance
      gdist=distance;
      
      if( distance >= 20)
      {
        digitalWrite(motorpin3,HIGH);    // MOTOR ON FOR FILL THE TANK
        digitalWrite(motorpin4,HIGH);
        
        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.println("Tank is Empty");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE = ");
        lcd.print(distance);
        lcd.setCursor(0,1);
        lcd.print("Tank is Empty");
        delay(1000);

        digitalWrite(motorpin3,LOW);    // MOTOR ON FOR FILL THE TANK
        digitalWrite(motorpin4,LOW);
      }  
      else if( distance >5 && distance <20)// Motor on for half tank
      {
        digitalWrite(motorpin3,HIGH);    // MOTOR FOR FILL THE TANK 
        digitalWrite(motorpin4,LOW);
        
        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.println("Tank is half");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE = ");
        lcd.print(distance);
        lcd.setCursor(0,1);
        lcd.print("Tank is half");
        delay(10000);

        Ultrasonic_lower();
      }
      else
      {  
        digitalWrite(motorpin3,LOW);    // MOTOR FOR FILL THE TANK 
        digitalWrite(motorpin4,LOW);
        
        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.println("Tank is full");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE = ");
        lcd.print(distance);
        lcd.setCursor(0,1);
        lcd.print("Tank is full");
        delay(10000);  

        Ultrasonic_lower();
      }
}

void Ultrasonic_lower()
{ 
      digitalWrite(trigPin2, LOW);             // Clears the trigPin
      delayMicroseconds(2);
      digitalWrite(trigPin2, HIGH);            // Sets the trigPin on HIGH state for 10 micro seconds
      delayMicroseconds(10);
      digitalWrite(trigPin2, LOW);
      
      duration2 = pulseIn(echoPin2, HIGH);      // Reads the echoPin, returns the sound wave travel time in microseconds
      distance2= duration2*0.034/2;             // Calculating the distance
      gdist2=distance2;
      
      if( distance2 >= 20)
      {
        Servo1.write(0);
        digitalWrite(motorpin1,HIGH);    // pump ON FOR FILL THE TANK
        digitalWrite(motorpin2,LOW);
        
        Serial.print("Distance2: ");
        Serial.println(distance2);
        Serial.println("bucket filling");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE2 = ");
        lcd.print(distance2);
        lcd.setCursor(0,1);
        lcd.print("bucket filling");
        delay(10000);
        
        digitalWrite(Red_Led,LOW);
        digitalWrite(yellow_Led,LOW);
        digitalWrite(Green_Led,HIGH);
        digitalWrite(motorpin1,LOW);    // pump FOR FILL THE TANK 
        digitalWrite(motorpin2,LOW);
        Serial.println("bucket filed 10L");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("bucket filed 10L");
        delay(10000);

        
      }  
      else if( distance2 >10 && distance2 <20)
      {
        Servo1.write(60);
        digitalWrite(motorpin1,HIGH);    // pump FOR FILL THE TANK 
        digitalWrite(motorpin2,LOW);
        
        Serial.print("Distance2: ");
        Serial.println(distance2);
        Serial.println("bucket filling");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE2 = ");
        lcd.print(distance2);
        lcd.setCursor(0,1);
        lcd.print("bucket filling");
        delay(10000);

        digitalWrite(Green_Led,LOW);
        digitalWrite(yellow_Led,HIGH);
        digitalWrite(Red_Led,LOW);
        digitalWrite(motorpin1,LOW);    // pump FOR FILL THE TANK 
        digitalWrite(motorpin2,LOW);
        Serial.println("bucket filed 15L");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("bucket filed 15L");
        delay(10000);
      }
      else
      {  
        Servo1.write(90);
        digitalWrite(Green_Led,LOW);
        digitalWrite(yellow_Led,LOW);
        digitalWrite(Red_Led,HIGH);
        digitalWrite(motorpin1,LOW);    // pump FOR FILL THE TANK 
        digitalWrite(motorpin2,LOW);
        
        Serial.print("Distance2: ");
        Serial.println(distance2);
        Serial.println("bucket is full");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DISTANCE2 = ");
        lcd.print(distance2);
        lcd.setCursor(0,1);
        lcd.print("bucket is full");
        delay(10000);  
      }
}

/*************************** Loop Function ****************************************/

void loop ()    
{ 
  Ultrasonic_upper();
  delay(1000); 
  //Ultrasonic_lower();
  
  thingSpeakWrite(gdist);
  Serial.println("\nDATA UPLOADED TO CLOUD\n");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("DATA UPLOADED ");
  lcd.setCursor(0,1);
  lcd.print("TO CLOUD ");
  delay(1000); 
}
