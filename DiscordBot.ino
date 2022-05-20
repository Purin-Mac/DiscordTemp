#include <Wire.h>
#include <WiFi.h>
#include <math.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>


#define TEMP_SDA 4
#define TEMP_SCL 5   
#define TEMP_ADDR 0x4D 

#define SECRET_SSID "******"
#define SECRET_PASS "*******" 
#define SECRET_WEBHOOK "********"
#define SECRET_TTS "true"

const char ssid[] = SECRET_SSID;    // Network SSID (name)
const char pass[] = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)
const String discord_webhook = SECRET_WEBHOOK;
const String discord_tts = SECRET_TTS;
int Limit_temp = 34;
String Unit = "C";
//openssl s_client -showcerts -connect discordapp.com:443 (get last certificate)
const char* discordappCertificate = \
                                    "-----BEGIN CERTIFICATE-----\n"
                                    "MIIDzTCCArWgAwIBAgIQCjeHZF5ftIwiTv0b7RQMPDANBgkqhkiG9w0BAQsFADBa\n"
                                    "MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl\n"
                                    "clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIw\n"
                                    "MDEyNzEyNDgwOFoXDTI0MTIzMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxGTAXBgNV\n"
                                    "BAoTEENsb3VkZmxhcmUsIEluYy4xIDAeBgNVBAMTF0Nsb3VkZmxhcmUgSW5jIEVD\n"
                                    "QyBDQS0zMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEua1NZpkUC0bsH4HRKlAe\n"
                                    "nQMVLzQSfS2WuIg4m4Vfj7+7Te9hRsTJc9QkT+DuHM5ss1FxL2ruTAUJd9NyYqSb\n"
                                    "16OCAWgwggFkMB0GA1UdDgQWBBSlzjfq67B1DpRniLRF+tkkEIeWHzAfBgNVHSME\n"
                                    "GDAWgBTlnVkwgkdYzKz6CFQ2hns6tQRN8DAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0l\n"
                                    "BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYI\n"
                                    "KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j\n"
                                    "b20wOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t\n"
                                    "bmlyb290MjAyNS5jcmwwbQYDVR0gBGYwZDA3BglghkgBhv1sAQEwKjAoBggrBgEF\n"
                                    "BQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sAQIw\n"
                                    "CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB\n"
                                    "AAUkHd0bsCrrmNaF4zlNXmtXnYJX/OvoMaJXkGUFvhZEOFp3ArnPEELG4ZKk40Un\n"
                                    "+ABHLGioVplTVI+tnkDB0A+21w0LOEhsUCxJkAZbZB2LzEgwLt4I4ptJIsCSDBFe\n"
                                    "lpKU1fwg3FZs5ZKTv3ocwDfjhUkV+ivhdDkYD7fa86JXWGBPzI6UAPxGezQxPk1H\n"
                                    "goE6y/SJXQ7vTQ1unBuCJN0yJV0ReFEQPaA1IwQvZW+cwdFD19Ae8zFnWSfda9J1\n"
                                    "CZMRJCQUzym+5iPDuI9yP+kHyCREU3qzuWFloUwOxkgAyXVjBYdwRVKD05WdRerw\n"
                                    "6DEdfgkfCv4+3ao8XnTSrLE=\n"
                                    "-----END CERTIFICATE-----\n";



WiFiMulti WiFiMulti;

void setup() {
  Wire.begin(TEMP_SDA, TEMP_SCL);
  Serial.begin(9600);
  connectWIFI();
  //sendDiscord("Hi! @everyone");
}

void loop() {
  while(Serial.available() == 0){
    float temp = readTemp();
//    Serial.println(temp);
    if (temp > Limit_temp) {
      sendDiscord("ขณะนี้มีอุณหภูมิที่สูงเกินกว่าที่กำหนด: " + String(temp) + " °C");
    } 
    delay(1000);
  }
  String value;
  value = Serial.readStringUntil('\n');
  if (value == "g"){
    float temp = readTemp();
    Serial.println(convetTemp(temp));
    Serial.println(Unit);
  }
  else if (value == "l"){
    Serial.println(Limit_temp);
  }
  else if (value == "u"){
    String serial_value;
    serial_value = Serial.readStringUntil('\n');
    serial_value.toUpperCase();
    Unit = serial_value;
  }
  else if (value == "s"){
    String serial_value;
    serial_value = Serial.readStringUntil('\n');
    Limit_temp = serial_value.toInt();
  }
}

void connectWIFI() {
  WiFiMulti.addAP(ssid, pass);
  WiFi.mode(WIFI_STA);
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(ssid);
  // wait for WiFi connection
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println("[WiFi] Connected");
}

void sendDiscord(String content) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(discordappCertificate);
    {
      HTTPClient https;
      Serial.println("[HTTP] Connecting to Discord...");
      Serial.println("[HTTP] Message: " + content);
      Serial.println("[HTTP] TTS: " + discord_tts);
      if (https.begin(*client, discord_webhook)) {  // HTTPS
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        int httpCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + discord_tts +"}");

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.print("[HTTP] Status code: ");
          Serial.println(httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.print("[HTTP] Response: ");
            Serial.println(payload);
          }
        } else {
          Serial.print("[HTTP] Post... failed, error: ");
          Serial.println(https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        Serial.printf("[HTTP] Unable to connect\n");
      }

      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("[HTTP] Unable to create client");
  }
}

float readTemp(){
  byte Temp_data[2];
  float Temp_value;
  Wire.beginTransmission(TEMP_ADDR);                                   //Start transmit with slave
  Wire.write(0);                                                      //Set register to temp read only
  Wire.endTransmission();                                             //Stop transmit with slave
  Wire.requestFrom(TEMP_ADDR, 2);                                      //Request 2 bytes from slave
  Temp_data[1] = Wire.read();                                         //Read MSB temp
  Temp_data[0] = Wire.read();                                         //Read LSB temp
  Temp_value = toCelsius(Temp_data[1], Temp_data[0]);
  return Temp_value;
}

float toCelsius(byte High_data, byte Low_data) {
  float Temp_value = (High_data << 1);                                //Shift left 1 bit to get Temp value
  if (High_data & (1 << 7)) {                                         //Check sign bit
    Temp_value *= -1;                                                 //Make Temp value be negative
  }
  for(int i = 0; i < 8; i++){
    if(Low_data & 1 << 7){
      Temp_value += pow(2, 0-i);
    }
    Low_data = Low_data << 1;
  }
  return Temp_value;
}

float convetTemp(float temp){
  if (Unit == "C") {
    return temp;
  }
  else if (Unit == "R") {
    return (temp + 273.15) * 1.8;
  }
  else if (Unit == "F") {
    return ((temp / 5) * 9) + 32;
  }
  else if (Unit == "K") {
    return temp + 273;
  }
}
