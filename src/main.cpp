#include <Arduino.h>
#include <DHT.h>

// Analog pins
#define WATER_LVL_OUT   A5


// Digital pins
#define WATER_LVL_VCC   7  // output pin for Water level sensor
#define DHT_OUT         4  // output pin for DHT sensor

// Variables
#define DHT_TYPE        DHT11  // Type of connected DHT sensor, may also be DHT12 or DHT22

DHT dht_sensor(DHT_OUT, DHT_TYPE);  // creating DHT sensor obj
int water_level = 0;  // store the water level value from water sensor
float humidity;  // store humidity from DHT sensor
float temperature;  // store temperature from DHT sensor, default in Celsius degree
float heat_index = 0;  // store temperature perceived human

void setup() {
    Serial.begin(9600);

    dht_sensor.begin();  // initialize DHT sensor

    pinMode(WATER_LVL_VCC, OUTPUT);   // configure WATER_LVL_VCC pin as an OUTPUT (on/off switch)
    digitalWrite(WATER_LVL_OUT, LOW); // turn the water sensor OFF
}

void loop() {
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
