#include <Arduino.h>
// inisialisasi pin relay dan nilai pid
#include <PID_v1.h>
#define pin_relay 14          //D5 pada node mcu
double setPoint, nilai_relay; //nilai relay adalah nilai yang akan digunakan untuk menyakalan relay(analogwrite)
//Specify the links and initial tuning parameters
double Kp = 50, Ki = 5, Kd = 1;
double nilai_suhu = 0;
PID myPID(&nilai_suhu, &nilai_relay, &setPoint, Kp, Ki, Kd, DIRECT);

// ini adalah perintah untuk sensor suhunya
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into io 4 (d0 di nodemcu)
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensorSuhu(&oneWire);
// inisialisasi data suhu

/*************************************************************
  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/
// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL1KaIVCas"
#define BLYNK_DEVICE_NAME "Template Zaki sistem kendali"
#define BLYNK_AUTH_TOKEN "sUUFKW18vk9_hQ_DhO1xgIcxeG88xoRS"
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
// inisialisasi wifi pada nodemcu dan blynknya
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "KONTRAKAN UYE";
char pass[] = "KUSANG123";
BlynkTimer timer;
// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

BLYNK_WRITE(V4)
{
  setPoint = param.asDouble();
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
  Blynk.virtualWrite(V3, nilai_suhu);
  Blynk.virtualWrite(V5, nilai_relay);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  sensorSuhu.begin();

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  sensorSuhu.requestTemperatures();
  nilai_suhu = sensorSuhu.getTempCByIndex(0);
  myPID.Compute();
  analogWrite(pin_relay, nilai_relay);
  Serial.print("nilai kontrol = ");
  Serial.println(nilai_relay);

  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
