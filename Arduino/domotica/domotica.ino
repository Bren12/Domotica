#define D0 16  // Aqui va el led del fotorresistor
#define D1  5  // trigPin - sensor ultrasónico
#define D2  4  // echoPin - sensor ultrasónico
#define D4 2   // Aquí va el sensor de temperatura
#define D5 14  // LED blinking
#define D6 12  // LED para sensor temperatura
#define D7 13  // LED para sensor ultrasonico
#define D8 15  // LED para avisar mensaje publicado

// Identificador de este nodeMCU
int i = 12;

// Velocidad
int vel = 0;

// ----- Definiciones de fotorresistor
int photoPin = A0;
int ledphotoResistor = 16;  // Equivale a D0
int x = -1;  //0..1023

// ----- Definiciones del sensor de temperatura DHT11
#include "DHT.h"
#define DHTPIN D4  // Posicion equivalente a D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h, t;

// ----- Definiciones del sensor ultrasonico
int trigPin = D1;
int echoPin = D2;
float v = 331.5+0.6*20;   // m
float d;

// ----- Definiciones de ESP8266MQTT
/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>  // Si no existe la biblioteca
                           // Nota bajar .zip https://www.arduinolibraries.info/libraries/pub-sub-client
                           // Instalar Menu: Programa / Submenu: Incluir libreria / Anadir biblioteca PubSubClientxxx.zip
const char* ssid = ""; // Ingresar nombre de la red
const char* password = ""; // Ingresar contrasena de la red
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* topico_salida = "TopicoOutEquipo5";
const char* topico_entrada = "TopicoInEquipo5";
char sTopicoOutEquipo5[60];

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (60)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// =================================================================
// ----- Funcion que Conecta a Wifi del Router
// We start by connecting to a WiFi network
void setup_wifi() {
  delay(10);
  Serial.println();    Serial.print("Connecting to ");   Serial.println(ssid);
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println(""); Serial.println("WiFi connected"); Serial.print("IP address: "); Serial.println(WiFi.localIP());
}

// ----- Funcion que Conecta a Broker MQTT
void setup_mqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
}


// ----- Funcion que Conecta a Broker MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  digitalWrite(D5, HIGH); delay(100); digitalWrite(D5, LOW);  // Aviso Led azul en D5 Pin 8

                              // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D8, HIGH);   // Turn the LED on (Note that LOW is the voltage level
                              // but actually the LED is on; this is because
                              // it is active low on the ESP-01). No es cierto en D6.
  } else {
    digitalWrite(D8, LOW);    // Turn the LED off by making the voltage HIGH
  }
}

// ----- Funcion que Conecta o reconecta a Broker MQTT
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      client.subscribe(topico_entrada);
      client.publish(topico_salida, sTopicoOutEquipo5);      

      // ... and resubscribe
      //client.subscribe("inTopic");
     
    } else {
      Serial.print("failed, rc="); Serial.print(client.state()); Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void conectarMQTT() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
           
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
    
    Serial.print(now); Serial.print("Publish message: "); Serial.println(sTopicoOutEquipo5);
    client.publish(topico_salida, sTopicoOutEquipo5);
    
 //REDV   client.publish(topico_salida, msg);
//    client.publish(topico_entrada, msg);
  }
}


// =================================================================
// ----- Funcion detector de iluminacion mediante un fotorresitor
// Puerto A0, lee valores de 0 a 1023 dependiendo de la resistencia
void medirLuzledPhotoResistor() {
  x = analogRead(photoPin);
  Serial.print("Valor de voltaje ");
  Serial.println(x);
  if (x > 100)
    digitalWrite(ledphotoResistor, HIGH);
  else
    digitalWrite(ledphotoResistor, LOW);
}

// =================================================================
// ----- Funcion que lee temperatura y humedad con el sensor DHT11
void medirTemperatura() {
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  float f = dht.readTemperature(true);
     
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.print("Falló al leer el sensor DHT\n");
    return;
  }
  
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humedad: "); Serial.print(h);
  Serial.print(", Temperatura: "); Serial.print(t);
  Serial.print("(C), "); Serial.print(f);
  Serial.print("(F), Indice de calor (C)"); Serial.print(hic);
  Serial.print(", Indice de calor (F)"); Serial.print(hif);
  Serial.print("\n");

  if (t > 26)
     digitalWrite(D6,HIGH);
  else 
     digitalWrite(D6,LOW);
     
}


// =================================================================
// ----- Funcion que dispara un rayo, lo apaga y espera el echo
//       para medir distancia
void medirDistanciaCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin,LOW);

  //listen for echo
  float tUs = pulseIn(echoPin, HIGH);  //microseconds
  float t = tUs / 1000.0 / 1000.0 / 2.0;    //s
  float dM = t*v;  //m
  d = dM*100;  // cm  
  Serial.print("Distancia(cm): ");
  Serial.println(d);

  if (d < 100)
     digitalWrite(D7,HIGH);
  else 
     digitalWrite(D7,LOW);
  
  delay(200);  //ms
}

// =================================================================
void setup() {
   pinMode(D0,OUTPUT); // Led del fotorresistor
   pinMode(D5,OUTPUT); // Led blinking
   pinMode(D6,OUTPUT); // Led del sensor de temperatura
   pinMode(D7,OUTPUT); // Led del sensor de temperatura
   pinMode(D8,OUTPUT); // Led del sensor de temperatura
   
   // ----- Iniciar consola: Herramientas, Monitor serie
   Serial.begin(9600);  

   // ----- Abrir Wifi y MQTT
   setup_wifi();
   setup_mqtt();
   
   // ----- Inicializacion sensor temperatura - humedad
   Serial.println(F("DHTxx test!"));
   Serial.println("DHTxx test!");
   dht.begin();
   
   // ----- Inicializacion fotorresistor
   pinMode(ledphotoResistor, OUTPUT);
   pinMode(photoPin, INPUT);
   Serial.println("Iniciando lectura fotorresistor");

   // ----- Inicializacion sensor de distancia
   pinMode(trigPin,OUTPUT);
   pinMode(echoPin,INPUT);
   Serial.println("Iniciando Sensor Distancia");
}

void loop() {
   // blinking
   digitalWrite(D5,HIGH);
   delay(500);
   digitalWrite(D5,LOW);
   delay(500);
   
   medirTemperatura();
   medirLuzledPhotoResistor();
   medirDistanciaCm();
   
   // Print en MQTT
   snprintf (sTopicoOutEquipo5, MSG_BUFFER_SIZE, "{\"t\":%5.2f,\"h\":%5.2f,\"v\":%d,\"d\":%5.2f,\"vel\":%d,\"i\":%d}",t,h,x,d,vel,i);
   conectarMQTT();
   
   Serial.println("******************************************");
}
