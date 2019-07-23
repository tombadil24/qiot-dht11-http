#include <LWiFi.h>

#include "DHT.h"
#define dht_dpin 3
#define WARNPIN 13
#define DHTTYPE DHT11   // DHT 11
DHT dht(dht_dpin, DHTTYPE);

char ssid[] = "TP-LINK_1A9F";         // CHANGE WIFI SSID HERE
char pass[] = "04210252";             // CHANGE WIFI PASSWORD HERE
int keyIndex = 0;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }

  server.begin();
  
  Serial.println("Connected to wifi");
  printWifiStatus();   

  // Activate DHT11
  dht.begin();
  pinMode(WARNPIN, OUTPUT);
  digitalWrite(WARNPIN, LOW);
}

void loop()
{
  //listen for incoming clients
  WiFiClient client = server.available();

  if(client){
    Serial.println("new client");
    String msg;
    String param;
    // an HTTP request ends with a blank line
    if(client.connected()){
      while(client.available()){
        char c = client.read();
        msg+=String(c);
      }
    }
    Serial.print(msg);
    int posi = msg.indexOf("Referer: ");
    if(posi>0){
      String sb = msg.substring(posi);
      int p = sb.indexOf("/");
      param = sb.substring(p+2);
      p = param.indexOf("/");
      param = param.substring(p,p+5);
    }
    else{
      posi = msg.indexOf("GET");
      param = msg.substring(posi+4,posi+9);
    }
    Serial.println(param);
    // send a standard HTTP response header
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type:text/html");
    client.println("Connection: close"); // the connection will be closed after completion of the response

    client.println();

    if(param=="/warn"){
      digitalWrite(WARNPIN, HIGH);
      client.println("WARNING !!");
    }
    if(param=="/cler"){
      digitalWrite(WARNPIN, LOW);
      client.println("CLEAR");
    }
    if(param=="/temp"){
      float t = dht.readTemperature();
      client.println(t);
    }
    if(param == "/humi"){
      float h = dht.readHumidity();
      client.println(h);
    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  // give the web browser time to receive the data
  delay(100);
}

void printWifiStatus() { 
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
