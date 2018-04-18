// RFID READER (ESP8266 WeMos D1 R2)

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// For conneting to an internet network --------------------
const char serverWeb[] = "test.host.com";  // The url of the host to which data would post
const char* MY_SSID = "RFIDnet";  // SSID of the WiFi that is connected to the internet
const char* MY_PWD =  "012x034z056b";  // Password of the WiFi that is connected to the internet
// --------------------------------------------------------

const char *ssid = "RFIDRoom-002";  // Reader's WiFi name (SSID)
const int id = 2;  // Reader's ID number

char password[13];
int numTries = 0;
String request;

WiFiServer server(80);
/* http://192.168.4.1 */

void setup() {
  pinMode(D3, OUTPUT);
  WiFi.softAPdisconnect(false);  // Turning the AP off
  delay(500);
  Serial.begin(115200);
  delay(10);
  Serial.print("Reader: ");
  Serial.print(ssid);
  
  //WiFi.config(ip, gateway, subnet);  // This can be used for preconfiguration, to have a fixed IP
  WiFi.mode(WIFI_AP_STA);  // Access Point and Station Mode
  WiFi.disconnect();
  
  delay(100);
  
  Serial.println("\r\nConfiguring access point...");
  genPass(ssid).toCharArray(password, 13);  // Generating password of the reader
  WiFi.softAP(ssid, password);  // Starting the access point
  
  IPAddress myIP = WiFi.softAPIP();  // Getting the IP address
  
  Serial.print("AP IP address: ");
  Serial.print(myIP);

  WiFi.begin(MY_SSID, MY_PWD);  // Connecting to on the internet WiFi
  Serial.print("\r\nConnecting to the Internet");

  while (WiFi.status() != WL_CONNECTED) { // To check if it is connected
    delay(400);
    Serial.print(".");
  }
  Serial.println(" Connected successfully.");
  digitalWrite(D3, HIGH);
  server.begin();  // Running a http server
  
  Serial.print("HTTP server started\r\n");  
  //Serial.println(WiFi.localIP());  // Printing the local IP address
}

void loop() {

  WiFiClient client = server.available();
  
  if (client) {  // To check if any tag is connected
    request = "";
    numTries = 0;
    if (client.connected()) {
      //digitalWrite(ledPin, HIGH);  // To show that communication is ON
      Serial.print("\r\nTag connected");
      while (request == "" && numTries <=5) {
        request = client.readStringUntil('\r'); // Receiving the tag ID
        if (request != "") {
          client.println(String("OK\r"));  // Sending acknowledgement
        }
        numTries++;
      }
      client.flush();  // Clearing the buffer
      client.stop();  // Disconnect the tag from the server, i.e., reader
      
      Serial.print(", ID: "); 
      if (request == "") {
        Serial.println("none");
      }
      else {
        Serial.print(request);  // Printing the tag ID
        Serial.print("\r\n");   
        //digitalWrite(ledPin, LOW); // To show that communication is OFF
        
        // Sending data to the Internet -------------------------------------
        WiFiClient client;
  
        if (client.connect(serverWeb, 80)) {
            Serial.println("Connected to web server.");
            //WiFi.printDiag(Serial);
          
            /* Forming the POST request. It should be like:  "var1name=value1&var2name=value2&var3name=value3&varNname=valueN" */
            String data = "reader=" + String(id) + "&tag=" + request; 
  
            client.println("POST /RFID/data.php HTTP/1.1");   // The sub-address of php file should be written in the middle
            client.print("Host: test.host.com\n");  // The url of the host should be written at the end              
            client.println("User-Agent: ESP8266/1.0");
            client.println("Connection: close"); 
            client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.print(data.length());
            client.print("\n\n");
            client.print(data);
            delay(200);
            
//            if ( client.available() ) {  // To see the answer of the request by the host server
//              String c = client.readStringUntil('\r');
//              Serial.println(c);
//            }
          
            /* Blink to show one ID has been transmitted */
            digitalWrite(D3, LOW);
            delay(150);
            digitalWrite(D3, HIGH);

            client.stop();  // Disconnecting from the host
        }     
        // ------------------------------------------------------------------
      }
    }
  
  }

}

String genPass (String ssid) {
  
  char last;
  String pass;
  String IDnumStr = ssid.substring(9);
  int IDnum = IDnumStr.toInt();
  last = ssid.charAt(11);

  switch (last) {
    case '0':
      if (IDnum > 10) {
        IDnum = (IDnum*110) + floor(IDnum/10) + ceil(sqrt(IDnum)) + 25;
        pass = "5Hd.3aR2" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*210) + floor(IDnum/2) + ceil(sqrt(IDnum)*11) + 15;
        pass = "84d.3a86" + String(IDnum);
      }
      else if (IDnum == 0) {
        pass = "4g^23bT844";
      }
      break;
  case '1':
      if (IDnum > 10) {
        IDnum = (IDnum*120) + floor(IDnum/5) + ceil(sqrt(IDnum)) + 25;
        pass = "%Hy.3a2^" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*2654) + floor(IDnum/11) + ceil(sqrt(IDnum)*9) + 15;
        pass = "5Hd.3a2t" + String(IDnum);
      }
      break;
  case '2':
      if (IDnum > 10) {
        IDnum = (IDnum*130) + floor(IDnum/23) + ceil(sqrt(IDnum)) + 45;
        pass = "ertT45$3" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*3468) + floor(IDnum/11) + ceil(sqrt(IDnum)*24) + 120;
        pass = "dfw$g74S" + String(IDnum);
      }
      break;
  case '3':
      if (IDnum > 10) {
        IDnum = (IDnum*341) + floor(IDnum/31) + ceil(sqrt(IDnum)) + 145;
        pass = "#g345JS7" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*2411) + floor(IDnum/3) + ceil(sqrt(IDnum)*45) + 100;
        pass = "t$h&ftD7" + String(IDnum);
      }  
      break;
  case '4':
      if (IDnum > 10) {
        IDnum = (IDnum*190) + floor(IDnum/20) + ceil(sqrt(IDnum)) + 415;
        pass = "f345Hrt7" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*1924) + floor(IDnum/5) + ceil(sqrt(IDnum)*86) + 230;
        pass = "wer5*66^" + String(IDnum);
      }  
      break;
  case '5':
      if (IDnum > 10) {
        IDnum = (IDnum*164) + floor(IDnum/12) + ceil(sqrt(IDnum)) + 145;
        pass = "45J78@31" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*3001) + floor(IDnum/3) + ceil(sqrt(IDnum)*14) + 520;
        pass = "w34&B2@g" + String(IDnum);
      }  
      break;
  case '6':
      if (IDnum > 10) {
        IDnum = (IDnum*154) + floor(IDnum/16) + ceil(sqrt(IDnum)) + 245;
        pass = "erer^2HS" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*2700) + floor(IDnum/2) + ceil(sqrt(IDnum)*66) + 2320;
        pass = "d0O@pL64" + String(IDnum);
      }  
      break;
  case '7':
      if (IDnum > 10) {
        IDnum = (IDnum*454) + floor(IDnum/76) + ceil(sqrt(IDnum)) + 222;
        pass = "vnrTR33$" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*2439) + floor(IDnum/4) + ceil(sqrt(IDnum)*26) + 3523;
        pass = "5G62Gd#&" + String(IDnum);  
      }
      break;
  case '8':
      if (IDnum > 10) {
        IDnum = (IDnum*354) + floor(IDnum/26) + ceil(sqrt(IDnum)) + 260;
        pass = "45yrhR63#43F" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*775) + floor(IDnum/2) + ceil(sqrt(IDnum)*16) + 5200;
        pass = "dergt$t664s@" + String(IDnum);  
      }
      break;
  case '9':
      if (IDnum > 10) {
        IDnum = (IDnum*514) + floor(IDnum/15) + ceil(sqrt(IDnum)) + 145;
        pass = "$hYi34hW98" + String(IDnum);
      }
      else if (IDnum <= 10) {
        IDnum = (IDnum*375) + floor(IDnum/3) + ceil(sqrt(IDnum)*46) + 3312;
        pass = "srt$56j32G" + String(IDnum);  
      }
      break;
  }
  pass = pass.substring(0,12);
  return pass;
}


