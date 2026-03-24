#include <LiquidCrystal.h>
LiquidCrystal lcd(6,7,5,4,3,2);

#include <Servo.h>
Servo myservo;  // create servo object to control a servo

#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);



char route_string[50];

char res[130];


int gas    = 10;
int buzzer = 13;

char rfid_string[20];
char gas_string[10];

void serialFlush()
{
  while(Serial.available() > 0) 
  {
    char t = Serial.read();
  }
} 

void myserialFlush()
{
  while(mySerial.available() > 0) 
  {
    char t = mySerial.read();
  }
} 


char check(char* ex,int timeout)
{
  int i=0;
  int j = 0,k=0;
  while (1)
  {
    sl:
    if(mySerial.available() > 0)
    {
      res[i] = mySerial.read();
      if(res[i] == 0x0a || res[i]=='>' || i == 100)
      {
        i++;
        res[i] = 0;break;
      }
      i++;
    }
    j++;
    if(j == 30000)
    {
      k++;
     // Serial.println("kk");
      j = 0;  
    }
    if(k > timeout)
    {
      //Serial.println("timeout");
      return 1;
     }
  }//while 1
  if(!strncmp(ex,res,strlen(ex)))
  {
   // Serial.println("ok..");
    return 0;
   }
  else
  {
   // Serial.print("Wrong  ");
   // Serial.println(res);
    i=0;
    goto sl;
   }
} 

char buff[200],k=0;
void upload(unsigned int s1,unsigned int s2,unsigned int s3);
char readserver(void);
void clearserver(void);

const char* ssid = "iotserver";
const char* password = "iotserver123";



int sti=0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void beep()
{
  digitalWrite(buzzer, LOW);delay(2000);digitalWrite(buzzer, HIGH);
}
void setup() 
{  
  char ret;  
   
  Serial.begin(9600);serialEvent();
  mySerial.begin(9600);

  pinMode(buzzer, OUTPUT);
  pinMode(gas, INPUT);  myservo.attach(11);
  
  digitalWrite(buzzer, HIGH);
  myservo.write(0);delay(1500);
    
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0, 0);lcd.print("App of RFID Tech");
  lcd.setCursor(0, 1);lcd.print("Maximum Spanning");
     delay(2500);
     
  wifiinit();
  delay(2500);
  
  lcd.clear();
  lcd.print("G:");
  
 serialEvent(); 
}

char bf3[50];
int g=0,f=0,count=0,lc=0;

void loop() 
{
  memset(gas_string,'\0',strlen(gas_string)); 
  if(digitalRead(gas) == LOW)
    {
      lcd.setCursor(2,0);lcd.print("ON ");
      strcpy(gas_string,"ON");
      for(int ti=0;ti<=90;ti++)
         {
          myservo.write(ti);        
          delay(10);
         }
       upload(rfid_string,gas_string);  
      for(int ti=90;ti>=0;ti--)
         {
          myservo.write(ti);        
          delay(10);
         }
    }
  if(digitalRead(gas) == HIGH)
    {
      lcd.setCursor(2,0);lcd.print("OFF");
      strcpy(gas_string,"OFF");
    }
    
   if(stringComplete)
     {
      memset(rfid_string,'\0',strlen(rfid_string));
      
       if(inputString == "55007C4B1F7D")
         {
          lcd.setCursor(0,1);lcd.print("Valid Card");
          strcpy(rfid_string,"ValidCard");
          for(int ti=0;ti<=90;ti++)
             {
               myservo.write(ti);        
                delay(10);
             }
          upload(rfid_string,gas_string);  
          for(int ti=90;ti>=0;ti--)
             {
              myservo.write(ti);        
                delay(10);
             }
          lcd.setCursor(0,1);lcd.print("            ");     
         }
       if(inputString == "55007B4A7E1A")
         {
          lcd.setCursor(0,1);lcd.print("Invalid Card");
          strcpy(rfid_string,"ValidCard");
          upload(rfid_string,gas_string);  
          lcd.setCursor(0,1);lcd.print("            ");               
         }
      
       sti=0;
       inputString = "";
       stringComplete = false;               
     }
 
  delay(100);
}

char bf2[50];
void upload(const char *s1,const char *s2)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("U");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");
    
  //http://projectsfactoryserver.in/storedata.php?name=pf5&s1=25&s2=35
  //sprintf(buff,"GET http://embeddedspot.top/iot/storedata.php?name=iot139&s1=%u&s2=%u&s3=%u\r\n\r\n",s1,s2);
  
      delay(8000);
     /*
      if(s1 == 1){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s1=ON\r\n\r\n");}
      if(s1 == 2){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s1=OFF\r\n\r\n");}
      if(s1 == 3){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s2=Wet\r\n\r\n");}
      if(s1 == 4){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s2=Dry\r\n\r\n");}
     */
      memset(buff,0,strlen(buff));   
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1671&s1=%s&s2=%s\r\n\r\n",s1,s2);
//      buff = buff + moss + "\r\n\r\n";
     // strcat(buff,s3);
         
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
         
              delay(2000);
              
              mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" ");  
}

char readserver(void)
{
  char t;
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("R");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //http://projectsfactoryserver.in/last.php?name=amvi001L

      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/last.php?name=iot9L\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
       //read status
        while(1)
        {
           while(!mySerial.available());
            t = mySerial.read();
           // Serial.print(t);
            if(t == '*' || t == '#')
            {
              if(t == '#')return 0;
              while(!mySerial.available());
               t = mySerial.read();
             //  Serial.print(t);
               delay(1000);
                myserialFlush();
               return t;
            }
        }
              delay(2000);
              
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
return t;
}

void clearserver(void)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("C");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s10=0\r\n\r\n");
      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot4&s10=0\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
          delay(2000);
          myserialFlush();
      
              
              
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
}


void wifiinit()
{
 char ret;  
  st:
  mySerial.println("ATE0");
  ret  = check((char*)"OK",50);
  mySerial.println("AT");
  ret  = check((char*)"OK",50);
  if(ret != 0)
  {
    delay(1000);
   goto st; 
  }
  
     lcd.clear();lcd.setCursor(0, 0);lcd.print("CONNECTING");  
  mySerial.println("AT+CWMODE=1");
   ret  = check((char*)"OK",50);
 cagain:
    
  myserialFlush();
  mySerial.print("AT+CWJAP=\"");
  mySerial.print(ssid);
  mySerial.print("\",\"");
  mySerial.print(password);
  mySerial.println("\"");
  if(check((char*)"OK",300))goto cagain;    
  mySerial.println("AT+CIPMUX=1");
  delay(1000);
 

  lcd.clear();lcd.setCursor(0, 0);lcd.print("WIFI READY"); 
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}
void convertk(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
  // lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}



void serialEvent() 
{
  while (Serial.available()) 
        {
         
         char inChar = (char)Serial.read();
              inputString += inChar;
              sti++;
          if(sti == 12)
            {sti=0;
              stringComplete = true;      
            }
        }
}
