#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"
#define DHTPIN 27
#define DHTTYPE DHT11
#define MI_ABS(x) ((x) < 0 ? -(x) : (x))
TFT_eSPI tft = TFT_eSPI();
DHT dht(DHTPIN, DHTTYPE);
/******
 * Define Constants
 ******/
const char *UBIDOTS_TOKEN = "BBUS-dLEMfK2bLu4HTl71h9LvpvL7NiVfu2"; // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";                                 // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";                                        // Put here your Wi-Fi password
const char *DEVICE_LABEL = "Practica2";                            // Put here your Device label to which data will be published
const char *VARIABLE_LABEL1 = "sw1";
const char *VARIABLE_LABEL2 = "sw2";
const char *TEMPERATURA_VARIABLE_LABEL = "Temp"; // Temperatura
const char *HUMEDAD_VARIABLE_LABEL = "Hum";      // humedad
const int PUBLISH_FREQUENCY = 5000;              // Update rate in milliseconds
unsigned long timer;
Ubidots ubidots(UBIDOTS_TOKEN);
int tamano;
int posicion;
char boton = '0';
char val = '0';
bool sw1State = false; // Estado inicial del sw1 (apagado)
bool sw2State = false; // Estado inicial del sw2 (apagado)
/******
 * Auxiliar Functions
 ******/
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    tamano = strlen(topic);
    posicion = tamano - 4;
    printf("switch: %c\n", topic[posicion]);
    boton = topic[posicion];
    val = payload[0];
    if (boton == '1')
        if ((char)payload[0] == '1')
        {
            sw1State = true;                       // Cambiar estado de sw1 a encendido
            tft.fillCircle(170, 25, 10, TFT_BLUE); // Dibujar círculo Azul
        }
        else
        {
            sw1State = false;                          // Cambiar estado de sw1 a apagado
            tft.fillCircle(170, 25, 10, TFT_DARKGREY); // Dibujar círculo gris
        }
    if (boton == '2')
        if ((char)payload[0] == '1')
        {
            sw2State = true;                        // Cambiar estado de sw2 a encendido
            tft.fillCircle(210, 25, 10, TFT_GREEN); // Dibujar círculo VERDE
        }
        else
        {
            sw2State = false;                          // Cambiar estado de sw2 a apagado
            tft.fillCircle(210, 25, 10, TFT_DARKGREY); // Dibujar círculo gris
        }
}
/******
 * Main Functions
 ******/
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
    ubidots.setCallback(callback);
    ubidots.setup();
    ubidots.reconnect();
    float Hum = dht.readHumidity();
    float Temp = dht.readTemperature();
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(3);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Practica 2", 10, 5, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString("Jhon Gomez", 10, 23, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Humedad", 10, 70, 2);
    tft.drawString("Temperatura", 140, 70, 2);
    tft.drawFastHLine(10, 50, 170, TFT_BLUE);
    tft.fillRect(110, 50, 3, 80, TFT_BLUE);
    tft.fillCircle(170, 25, 10, TFT_DARKGREY); // Círculo Azul
    tft.fillCircle(210, 25, 10, TFT_DARKGREY); // Círculo Verde
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2);
    Serial.println(F("DHTxx test!"));
    dht.begin();
    timer = millis();
}
void loop()
{
    if (!ubidots.connected())
    {
        ubidots.reconnect();
        ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
        ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2);
    }
    float Hum = dht.readHumidity();
    float Temp = dht.readTemperature();
    if ((MI_ABS(millis() - timer)) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
    {
        Serial.print("Temperatura: ");
        Serial.print(Temp);
        Serial.print(" | Humedad: ");
        Serial.println(Hum);
        tft.drawString(String(Temp), 150, 100, 4);
        tft.drawString(String(Hum), 20, 100, 4);
        ubidots.add(TEMPERATURA_VARIABLE_LABEL, Temp);
        ubidots.add(HUMEDAD_VARIABLE_LABEL, Hum);
        ubidots.publish(DEVICE_LABEL);
        timer = millis();
    }
    ubidots.loop();
    // Cambiar color de los círculos en base a los estados de sw1 y sw2
    if (sw1State)
    {
        tft.fillCircle(170, 25, 10, TFT_BLUE);
    }
    else
    {
        tft.fillCircle(170, 25, 10, TFT_DARKGREY);
    }
    if (sw2State)
    {
        tft.fillCircle(210, 25, 10, TFT_GREEN);
    }
    else
    {
        tft.fillCircle(210, 25, 10, TFT_DARKGREY);
    }
}