#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <DHT.h>
#include "secrets.h"

#define HTTP_REST_PORT 8080
ESP8266WebServer httpRestServer(HTTP_REST_PORT);

#define LAMP_SWITCH D8

// Digital pins
#define WATER_LVL_VCC D6 // output pin for Water level sensor
#define WATER_LVL_OUT A0
#define DHT_OUT D2 // output pin for DHT sensor

// Variables
#define DHT_TYPE DHT11 // Type of connected DHT sensor, may also be DHT12 or DHT22

DHT dht_sensor(DHT_OUT, DHT_TYPE); // creating DHT sensor obj
int water_level = 0;               // store the water level value from water sensor
float humidity;                    // store humidity from DHT sensor
float temperature;                 // store temperature from DHT sensor, default in Celsius degree
float heat_index = 0;              // store temperature perceived human
// int loop_variable = 50;             // helps controlling water sensor

#pragma region ServerStuff
ESP8266WebServer server(80); // sets default port for webserver

IPAddress local_IP(192, 168, 43, 21);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

void blinkOnRestCall()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(10);
    digitalWrite(LED_BUILTIN, HIGH);
}

void getHelloWord()
{
    // returns Hello world message to client, mainly for testing purposes
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void getInfo()
{
    // returns main sensors state
    server.send(200, "text/json",
                "{\"temperature\": " + String(temperature, 1) + ", \"humidity\": " + String(humidity, 1) + ", \"water\": " + String(water_level) + "}");
}

void lampSwitch()
{
    int state = digitalRead(LAMP_SWITCH);
    digitalWrite(LAMP_SWITCH, !state);
    if (state == 1)
    {
        server.send(200, "text/json", "{\"state\": \"OFF\"}");
    }
    else
    {
        server.send(200, "text/json", "{\"state\": \"ON\"}");
    }
}

void restServerRouting()
{
    // Defines routing
    server.on("/", HTTP_GET, getInfo);
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
    server.on(F("/lampSwitch"), HTTP_GET, lampSwitch);
}

// Manage not found URL
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

#pragma endregion ServerStuff

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(9600);
    
    
    pinMode(LAMP_SWITCH, OUTPUT);
    dht_sensor.begin(); // initialize DHT sensor

    pinMode(WATER_LVL_VCC, OUTPUT);   // configure WATER_LVL_VCC pin as an OUTPUT (on/off switch)
    digitalWrite(WATER_LVL_OUT, LOW); // turn the water sensor OFF

    // Configures static IP address

    WiFi.mode(WIFI_STA);

    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }

    WiFi.begin(local_ssid, local_password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(local_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    restServerRouting();               // Set server routing
    server.onNotFound(handleNotFound); // Set not found response
    server.begin();                    // Start server
    Serial.println("HTTP server started");
    
}

void loop()
{
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.handleClient(); // Handle client requests every 3000ms/3s

    humidity = dht_sensor.readHumidity();
    temperature = dht_sensor.readTemperature();

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    else
    {
        heat_index = dht_sensor.computeHeatIndex(temperature, humidity);
    }

    Serial.println("Water level: " + String(water_level));
    Serial.println("Lamp state: " + String(digitalRead(LAMP_SWITCH) ? "ON" : "OFF"));
    Serial.println(F("---DHT outputs---"));
    Serial.println("Temperature: " + String(temperature));
    Serial.println("Humidity: " + String(temperature));
    Serial.println("Heat index: " + String(heat_index));
    Serial.println();
    Serial.println();

    delay(500);
}
