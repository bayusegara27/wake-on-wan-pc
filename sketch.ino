#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "WIFI";                   // WRITE YOUR WIFI SSID
const char* password = "123";     // WRITE YOUR WIFI PASSWORD
const char* web_password = "1f0g9usd1";         // WRITE SOMETHING SECURE AND RANDOM
const int relayPin = D1; // Menggunakan pin D1 untuk mengatur relay 
const int statusPin = D2; // Menggunakan pin D2 untuk membaca status pc

byte mac[6];                     // the MAC address of your Wifi shield

ESP8266WebServer server(8085);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "halo gaes");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(statusPin, INPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

  if (MDNS.begin("PowerComp")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/statuspc", []() {
    if (server.arg(0) == web_password) {
      int ledStatus = digitalRead(statusPin);
      if (ledStatus == HIGH) {
        server.send(200, "text/plain", "Device Online");
      } else {
        server.send(200, "text/plain", "Device Offline");
      }
    } else {
      server.send(403, "text/plain", "Forbidden");
    }
  });

  server.on("/short", []() {
    if (server.arg(0) == web_password) {
      digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
      delay(200);              // pause
      digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
      delay(500);              // pause
      server.send(200, "text/plain", "Button pressed");
    } else {
      server.send(200, "text/plain", "Button not pressed");
    }
  });
  server.on("/long", []() {
    if (server.arg(0) == web_password) {
      digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
      delay(5000);              // pause
      digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
      delay(500);              // pause
      server.send(200, "text/plain", "Button pressed");
    } else {
      server.send(200, "text/plain", "Button not pressed");
    }
  });
  server.on("/ultralong", []() {
    if (server.arg(0) == web_password) {
      digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
      delay(9000);              // pause
      digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
      delay(500);              // pause
      server.send(200, "text/plain", "Button pressed");
    } else {
      server.send(200, "text/plain", "Button not pressed");
    }
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
