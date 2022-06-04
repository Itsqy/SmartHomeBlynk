#define BLYNK_TEMPLATE_ID "TMPLjlfKd9k6"
#define BLYNK_DEVICE_NAME "Smarthome"
#define BLYNK_AUTH_TOKEN "XpFeVLtz05NP4tsSmVMeLf3s85NU0cww"
#define BLYNK_PRINT Serial // sttatus konek ke server nya kebaca ,nilai outputnya juga 
#include <ESP8266WiFi.h> //library fun wifi nodemcu
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>
#include <Servo.h>
#include <NewPing.h> //Library untuk HC-SR04 // masing2 sensor
#define Lampu D0 //Relay 1 //kalo 7 5 aja bisa liat pin alias di pinout web
#define Kipas D1 //Relay 2

char ssid[] = "Paymont23"; //Sesuaikan
char pass[] = "085693254311"; //Sesuaikan
 
#define DHTPIN D4
#define DHTTYPE DHT11
#define Board                   ("ESP8266")
#define Pin                     (A0)
#define Type                    ("MQ-9") //MQ9
#define Voltage_Resolution      (5)
#define ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define RatioMQ9CleanAir        (9.6) //RS / R0 = 60 ppm 
#define TRIGGER_PIN D5 //Pin Trigger HC-SR04 pada NodeMCU // mengeluarkan gelmbang ultra
#define ECHO_PIN D6 //Pin Echo HC-SR04 pada NodeMCU //menerima patulan gelombang dari trigger pin D5
#define MAX_DISTANCE 250 //Maksimum Pembacaan Jarak (cm) 

  
DHT dht(DHTPIN, DHTTYPE);
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //Aktifkan Fungsi HCSR-04 //Aktifkan fungsi HC-SR04 
BlynkTimer timer;
 
void sendSensor()
{
  MQ9.update();
  MQ9.setA(1000.5); MQ9.setB(-2.186); 
  float LPG = MQ9.readSensor(); 
  MQ9.setA(599.65); MQ9.setB(-2.244); 
  float CO = MQ9.readSensor();

  int jarak = sonar.ping_cm(); //Hasil Pembacaan Jarak 

  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V7, LPG);
  Blynk.virtualWrite(V8, CO);
  Blynk.virtualWrite(V9, jarak);
}

void setup()
{
  Serial.begin(115200); // untuk channel
  pinMode(Lampu, OUTPUT); //Set Relay 1 jadi Output
  pinMode(Kipas, OUTPUT); //Set Relay 2 jadi Output
  digitalWrite(Lampu, HIGH); //Set nilai Awal Relay 1
  digitalWrite(Kipas, HIGH); //Set nilai Awal relay 2
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  MQ9.setRegressionMethod(1);
  MQ9.init(); 
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ9.update();
    calcR0 += MQ9.calibrate(RatioMQ9CleanAir);
    Serial.print(".");
  }
  MQ9.setR0(calcR0/10);
  timer.setInterval(1000L, sendSensor);
}
 
void loop()
{
  Blynk.run();
  timer.run();
}
