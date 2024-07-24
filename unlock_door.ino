#include <WiFi.h>
#include "time.h"

#define gpio16LEDPin 16 /* One LED connected to GPIO16 - RX2 */
#define gpio17LEDPin 17 /* One LED connected to GPIO17 - TX2 */

const char* ssid = ""; /* Add your router's SSID */
const char* password = ""; /*Add the password */

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

int gpio16Value; 
int gpio17Value;

WiFiServer espServer(80);

String request;

void setup() 
{
  Serial.begin(115200); /* Begin Serial Communication with 115200 Baud Rate */
  
  pinMode(gpio16LEDPin, OUTPUT);
  pinMode(gpio17LEDPin, OUTPUT);

  digitalWrite(gpio16LEDPin, LOW);
  digitalWrite(gpio17LEDPin, LOW);
  
  Serial.print("\n");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("*");
    delay(100);
  }
  Serial.print("\n");
  Serial.print("Connected to Wi-Fi: ");
  Serial.println(WiFi.SSID());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  LocalTimeStart();
  
  delay(100);
  /* The next four lines of Code are used for assigning Static IP to ESP32 */
  /* Do this only if you know what you are doing */
  /* You have to check for free IP Addresses from your Router and */
  /* assign it to ESP32 */
  /* If you are comfortable with this step, */
  /* please un-comment the next four lines and make necessary changes */
  /* If not, leave it as it is and proceed */
  //IPAddress ip(192,168,1,6);   
  //IPAddress gateway(192,168,1,1);   
  //IPAddress subnet(255,255,255,0);   
  //WiFi.config(ip, gateway, subnet);
  delay(2000);
  Serial.print("\n");
  Serial.println("Starting ESP32 Web Server...");
  espServer.begin();
  Serial.println("ESP32 Web Server Started");
  Serial.print("\n");
  Serial.print("The URL of ESP32 Web Server is: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.print("\n");
  Serial.println("Use the above URL in your Browser to access ESP32 Web Server\n");
}

void loop()
{
  WiFiClient client = espServer.available();
  if(!client)
  {
    return;
  }

  Serial.println("New Client!!!");

  boolean currentLineIsBlank = true;
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      request += c;
      Serial.write(c);
      
      if (c == '\n' && currentLineIsBlank)
      {
        
        if (request.indexOf("/GPIO16ON") != -1) 
        {
          digitalWrite(gpio16LEDPin, LOW);
          gpio16Value = HIGH;
          Serial.println("GPIO16 LED is ON");
          digitalWrite(gpio16LEDPin, HIGH);
          delay(2000);
          digitalWrite(gpio16LEDPin, LOW);
          delay(200);
          
          client.println("<script>location.assign('http://192.168.1.100/')</script>");
          client.print("<script>location.reload(true)</script>"); 
          gpio16Value = LOW;
              
        } 
                
        /* HTTP Response in the form of HTML Web Page */
        //client.println("HTTP/1.1 200 OK");
        //client.println("Content-Type: text/html");
        //client.println("Connection: close");
        client.println(); //  IMPORTANT

        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        
        client.println("<head>");
        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<link rel=\"icon\" href=\"data:,\">");
        
        client.println("<style>");
        
        client.println("html { font-family: Courier New; display: inline-block; margin: 0px auto; text-align: center;}");
        client.println(".button {border: none; color: white; padding: 10px 20px; text-align: center;");
        client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
        client.println(".button1 {background-color: #FF0000;}");
        client.println(".button2 {background-color: #00FF00;}");
        
        client.println("</style>");
        
        client.println("</head>");
        
        client.println("<body>");
        
        client.println("<h2>ESP32 Web Server Door Unlocker</h2>");
        
        if(gpio16Value == LOW) 
        {
          client.println("<p>UNLOCK DOOR</p>");
          client.println("<p><a href=\"/GPIO16ON\"><button onclick=\"button button1\">Click to Unlock</button></a></p>");
          client.println("<img src=\"https://eadn-wc03-1142452.nxedge.io/cdn/wp-content/uploads/2016/08/Unlock-512-500x500.png\" width=\"250\" height=\"250\" alt=\"LOCK\">");  
          client.println("<p> </p>");
          //client.println(startTimeHour[0]);
        }
           
        
        client.println("</body>");
        
        client.println("</html>");
        
        break;
        
    }
    if(c == '\n')
    {
      currentLineIsBlank = true;
    }
    else if(c != '\r')
    {
      currentLineIsBlank = false;
    }
    //client.print("\n");
    
    }
  }
 
  delay(1);
  request = "";
  //client.flush();
  client.stop();
  Serial.println("Client disconnected");
  Serial.print("\n");
}

void LocalTimeStart(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char startTimeHour[3];
  strftime(startTimeHour,3, "%I", &timeinfo);
  Serial.println(&timeinfo,"Start Time - %I:%M:%S");
}
