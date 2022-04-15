#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Arduino.h>
#include <DHT.h>


#define HTTP_REST_PORT 8080
ESP8266WebServer httpRestServer(HTTP_REST_PORT);

//#define NUM_LEDS      100
//#define LED_TYPE   WS2811
//#define COLOR_ORDER   GRB
//#define DATA_PIN        3
////#define CLK_PIN       4
//#define VOLTS          12
//#define MAX_MA       4000

// Analog pins

// Digital pins
#define WATER_LVL_VCC   D6  // output pin for Water level sensor
#define WATER_LVL_OUT   D1
#define DHT_OUT         D2  // output pin for DHT sensor

// Variables
#define DHT_TYPE        DHT11  // Type of connected DHT sensor, may also be DHT12 or DHT22

DHT dht_sensor(DHT_OUT, DHT_TYPE);  // creating DHT sensor obj
int water_level = 0;  // store the water level value from water sensor
float humidity;  // store humidity from DHT sensor
float temperature;  // store temperature from DHT sensor, default in Celsius degree
float heat_index = 0;  // store temperature perceived human

// WiFi credentials
const char* ssid = "Heh";
const char* password = "Kwbsike7";

ESP8266WebServer server(80);

// Serving Hello world
void getHelloWord() {
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the REST Web Server"));
    });
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
}
 
// Manage not found URL
void handleNotFound() {
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
}

void setup() {
    Serial.begin(9600);

    dht_sensor.begin();  // initialize DHT sensor

    pinMode(WATER_LVL_VCC, OUTPUT);   // configure WATER_LVL_VCC pin as an OUTPUT (on/off switch)
    digitalWrite(WATER_LVL_OUT, LOW); // turn the water sensor OFF

    WiFi.mode(WIFI_STA);
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
 
    // Activate mDNS this is used to be able to connect to the server
    // with local DNS hostmane esp8266.local
    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }
 
    // Set server routing
    restServerRouting();
    // Set not found response
    server.onNotFound(handleNotFound);
    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();

    digitalWrite(WATER_LVL_VCC, HIGH);  // turn the water sensor ON
    delay(10);  // wait 10 milliseconds
    water_level = analogRead(WATER_LVL_OUT);  // read the analog value from water sensor
    digitalWrite(WATER_LVL_VCC, LOW);  // turn the water sensor OFF (reduces corrosion rate)

    delay(2000);

    heat_index = 0;
    humidity = dht_sensor.readHumidity();
    temperature = dht_sensor.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    else{
        heat_index = dht_sensor.computeHeatIndex(temperature, humidity);
    }

    Serial.println("Water level: " + String(water_level));
    Serial.println(F("---DHT outputs---"));
    Serial.println("Temperature: " + String(temperature));
    Serial.println("Humidity: " + String(temperature));
    Serial.println("Heat index: " + String(heat_index));
    Serial.println();

    delay(1000);

// TODO plug in the rest of sensors, pass outputs and establish connection via ESP8266
}