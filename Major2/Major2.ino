#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
const int leftForward = 5;    //D1
const int leftBackward = 4;   //D2
const int rightForward = 0;   //D3
const int rightBackward = 2;  //D4
int flag=0; // to send sms and turn the knob on only once when the gas leak occurs
String apiKey = "BK8XEGSDOMCEYS3W"; //API key from ThingSpeak
const char *ssid = "Honor 6X"; //wifi username
const char *pass = "rvce1234"; // wifi password
const char* host = "maker.ifttt.com"; //to trigger sms
const int httpsPort = 443; 
const char* server = "api.thingspeak.com";
const int led=14;
WiFiClient client;
void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  pinMode(led,OUTPUT);
}
void loop()
{
  
  float h = analogRead(A0); // reads the value from the MQ5 sensor
  if (isnan(h))
  {
  Serial.println("Failed to read from MQ-5 sensor!");
  return;
  }

  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(h); 
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.print("Gas meter");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
    delay(3000);
  }
  client.stop();
  Serial.println("Waiting...");
  if(h<80)
  { Serial.println("low");
    pause();
  }
  else 
  { if(flag==0) 
    { sms();
      knob();
      flag=1;
    }
    fan();
    // thingspeak needs minimum 15 sec delay between updates
    delay(3000);
  }
}
void knob()
{  
    pinMode(rightForward,OUTPUT);
    pinMode(rightBackward,OUTPUT);
    digitalWrite(rightForward,HIGH);
    digitalWrite(rightBackward,LOW);
    delay(100);
                                // to stop the knob
    digitalWrite(rightForward,LOW);
    digitalWrite(rightBackward,HIGH);
    delay(100);
    pinMode(rightForward,INPUT);
    pinMode(rightBackward,INPUT);
    delay(100);
}
void fan()
{  pinMode(leftForward,OUTPUT);
  pinMode(leftBackward,OUTPUT);
  digitalWrite(leftForward,HIGH);
  digitalWrite(leftBackward,LOW);
  delay(100);
}
void pause()                    //to stop the fan
{ digitalWrite(leftForward,LOW);
  digitalWrite(leftBackward,HIGH);
  delay(100);
  digitalWrite(leftForward,INPUT);
  digitalWrite(leftBackward,INPUT);
  delay(2000);
 }
void sms()
{ WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
   String url = "/trigger/ESP/with/key/ielZ9jfWNQwSZ0jUxVCmqsPe4sn_HxCNWUIYfeCxbGU";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line); // when this line gets printed an sms is sent to the user's phone
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.println("high");
  
}

