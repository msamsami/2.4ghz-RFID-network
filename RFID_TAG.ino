// RFID TAG (Generic ESP8266 Module)

#include "ESP8266WiFi.h"

IPAddress server(192, 168, 4, 1);  // The fixed IP address of the server, i.e., the RFID Reader
WiFiClient client;

// TAG ID -------------------
const uint16_t tagID = 20;  // This can be a 16bit value, from 0 to 65535
// --------------------------

#define maxConnectingDelay  4  // Maximum acceptable delay for connecting to the reader (in seconds)

char password[13];
char ssid[13];
String arraySSID[50];
int cnt;  // This variable stores the number of nearby access points that are a member of our RFID network
float cnt2;
bool cFlag;
bool notFoundFlag = false;
bool sendFlag;
String lastReader = "";
String tempSSID;
String tempSSIDf = "";
String answer = "";
int tempRSSI[255];
int maxRSSI;
int maxRSSIind;
int numTries = 0;

void setup() {
  Serial.begin(115200);
  delay(10); 
  WiFi.mode(WIFI_STA);  // Setting WiFi to station mode
  WiFi.disconnect();  // Disconnecting from all access points, if any
  delay(10);
  Serial.println("Setup is completed.");
}

void loop() {
  Serial.println("\rStarting scan");
  
  while (tempSSIDf == "") {  // Search until a proper reader is found
    
    int n = WiFi.scanNetworks();  // Finding the number of nearby networks
    Serial.println("Scan completed.");
    cnt = 0;  // Resetting the counter
    memset(arraySSID, 0, sizeof(arraySSID));  // Clearing the temporary array for storing founded SSIDs
    memset(tempRSSI, 0, sizeof(tempRSSI));  // Clearing the temporary array for storing the corresponding signals' strength

    // Collecting all of our RFID readers from the nearby networks ------------------
      for (int i = 0; i < n; ++i) {
        tempSSID = WiFi.SSID(i);
        if (tempSSID.substring(0,9) == "RFIDRoom-") {
          arraySSID[cnt] = tempSSID;
          tempRSSI[cnt] = WiFi.RSSI(i);
          cnt++;
        }
      }
    // ------------------------------------------------------------------------------
    
    if (cnt == 0) {
      notFoundFlag = true;
      Serial.println("No RFID readers found! Searching again...\r\n-----");
    } 
    else {

      // Selecting the reader with the most powerful signal ------------------------
      maxRSSI = tempRSSI[0];
      maxRSSIind = 0;
      for (int j = 1; j<=cnt-1; j++) {
        if (tempRSSI[j] > maxRSSI) {
          maxRSSI = tempRSSI[j];
          maxRSSIind = j;
        }
      }
      tempSSIDf = arraySSID[maxRSSIind];
      // ----------------------------------------------------------------------------
      
    }
  }

    Serial.print("Selected reader: ");
    Serial.print(tempSSIDf); // Printing the SSID of our selected RFID reader
    
    if (notFoundFlag == false) { 
      if (lastReader == tempSSIDf) {  // Tag is still in the last reader's area
        sendFlag = false;
      }
      else {  // Tag is in a new reader's area
        sendFlag = true;
      }
    }
    else {  // Tag is in a new reader's area
      sendFlag = true;
    }
    
    if (sendFlag == true) {  
      
      tempSSIDf.toCharArray(ssid, 13);  // Converting the String value of selected reader's SSID to char array
      
      cFlag = true;  // Resetting connection flag
      cnt2 = 0.0;  // Resetting connecting delay
  
      genPass(tempSSIDf).toCharArray(password, 13);  // Generating password of the reader
      Serial.print("\r\nGenerated password: ");
      Serial.print(password);
      
      WiFi.begin(ssid, password);  // Connecting to the selected reader
      Serial.print("\r\nConnecting");
      while (WiFi.status() != WL_CONNECTED) {  // To check if it is connected
        Serial.print(".");
        delay(400);
        cnt2 = cnt2 + 0.4;
        if (cnt2 > maxConnectingDelay) {
          Serial.println(" Couldn't connect successfully!\r\n-----");
          cFlag = false;  // Means that we couldn't establish a successful connection
          break;
        }
      }
  
      if (cFlag == true) {
        Serial.println(" Connected successfully.");
        // Printing some network parameters --------------------------------------
        //Serial.print("Status: "); Serial.println(WiFi.status());    
        Serial.print("IP: ");     Serial.println(WiFi.localIP());
        //Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
        Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
        //Serial.print("SSID: "); Serial.println(WiFi.SSID());
        Serial.print("RSSI: "); Serial.println(WiFi.RSSI());
        // -----------------------------------------------------------------------
        answer = "";
        numTries = 0;
        
        while (answer != "OK" && numTries <= 5) {  // Trying up to 5 times to send the tag ID to the selected reader
          client.connect(server, 80);  // Establishing the connection to the server, i.e., the selected reader
          client.println(String(String(tagID) + "\r"));  // Sending the tag ID to the reader
          answer = client.readStringUntil('\r');   // Receiving the acknowledgement from the reader
          numTries++;
        }
        
        Serial.print("From reader: ");
        if (answer == "OK") {  // Means that reader could successfully receive the tag ID
          Serial.print("OK\r\n-----\r\n");
          notFoundFlag = false;
          lastReader = tempSSIDf;
        }
        else {
          Serial.println("none\r\n-----");
        }
        
        client.flush();
        WiFi.disconnect();  // Disconnecting from the reader

      }
              
  }
  else {
    Serial.print("\r\nStill in the last area. No need to send!\r\n-----\r\n");
  }

  tempSSIDf = "";
  if (cnt2 <= 4.8) {
    delay(5000-(cnt2*1000));
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

