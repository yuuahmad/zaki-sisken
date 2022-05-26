#include <Arduino.h>

int ThermistorPin = A0;                                                   // menggunakan pin A0 untuk sensor thermistor
int Vo;                                                                   // nilai thermistor dan nilai
double R1 = 10000;                                                        // nilai resistor tetap yang digunakan adalah 100k ohm
double logR2, R2, T, Tc, I, VoltaseR1;                                    // logR2 adalah nilai logaritma dari R2, R2 adalah nilai resistansi thermistor, T adalah nilai suhu dalam kelvin, Tc nilai suhu dalam celcius, I adalah nilai arus total yang mengalir pada 2 resitor, dan V1 adalah nilai tegangan di resistor retap
double c1 = 6.253244482e-03, c2 = -6.908019094e-04, c3 = 44.47723879e-07; // kalibrasi sendiri
double voltase;                                                           // nilai voltase dari sensor suhu
// nilai diatas didapatkan dari situs ini
// https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
// dengan menggunakan data
// suhu       ||      resistansi
// 9                  196,5k ohm
// 28                 85,6k ohm
// 70                 19,5k ohm

// inisialisasi pin relay dan nilai pid
#include <PID_v1.h>
#define pin_relay D5 // relay dihubungkan ke d5
// nilai relay adalah nilai yang akan digunakan untuk menyakalan relay(analogwrite)
double setPoint, nilai_relay;
double Kp = 50, Ki = 5, Kd = 1; // parameter untuk nilai pid
PID myPID(&Tc, &nilai_relay, &setPoint, Kp, Ki, Kd, DIRECT);

// didapat dari aplikasi blynk agar dapat berkomunikasi dengannya
#define BLYNK_TEMPLATE_ID "TMPL1KaIVCas"
#define BLYNK_DEVICE_NAME "Template Zaki sistem kendali"
#define BLYNK_AUTH_TOKEN "sUUFKW18vk9_hQ_DhO1xgIcxeG88xoRS"
#define BLYNK_PRINT Serial      // menampilkan informasi telah tersambung ke blynk
#include <ESP8266WiFi.h>        //mendapatkan liblarry esp8266
#include <BlynkSimpleEsp8266.h> //mendapatkan liblarry blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "KONTRAKAN UYE"; // nama wifi
char pass[] = "KUSANG123";     // pass wifi
BlynkTimer timer;

BLYNK_WRITE(V4) // apapun yang perubahabn yang terjadi pada v4(virtual pin 4) dapatkan data itu
{
  setPoint = param.asDouble(); // kode untuk mendapatkan nilai set point pada blynk
}

void myTimerEvent() // kirim semua data yang didapatkan di nodemcu ke blynk
{
  Blynk.virtualWrite(V2, millis() / 1000); // kirim data waktu aktif perangkat
  Blynk.virtualWrite(V3, Tc);              // kirim data nilai suhu
  Blynk.virtualWrite(V5, nilai_relay);     // kirim nilai relay ke blynk (nilai sum pid)
  Blynk.virtualWrite(V6, voltase);         // kirim nilai voltase thermistor ke blynk
  Blynk.virtualWrite(V7, R2);              // nilai resistansi sensor suhu (thermistor)
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  // buat pin A0 sebagai input
  pinMode(ThermistorPin, INPUT);
  // turn the PID on
  myPID.SetMode(AUTOMATIC);
  // mulai liblarry blynk
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  myPID.Compute();                     // dapatkan nilai pid dari beberapa variabel yang sudah diinginkan
  analogWrite(pin_relay, nilai_relay); // kontrol relay menggunakan nilai pid yang telah didapatkan
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);                       // nilai resistansi sensor
  logR2 = log(R2);                                            // menghitung nilai log dari r2 (resistansi sensor)
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // menggunakan persamaan Steinhart-Hart untuk mendapatkan nilai temperatur dalam kelvin
  Tc = T - 273.15;                                            // menghitung nilai temperatur dalam celcius dengan mengkonversi temperatur dalam kelvin

  // menghitung nilai voltase yang ngak ngotak
  // dengan didapatkannya nilai R2 (r thermistor maka kita bisa mendapatkan nilai arus dengan persamaan sederhana)
  // I = V / R dimana V adalah voltase total, dan R adalah R total (R thermistor + R tetap)
  I = 3.3 / (R1 + R2);
  // hitung nilai voltase pada resistor 100 k ohm (V2)
  VoltaseR1 = I * R1;
  // akhirnya, kita mendapatkan nilai voltase pada thermistor dengan rumus sederhana
  voltase = 3.3 - VoltaseR1;

  Serial.print("temp: ");
  Serial.print(Tc); // tampilkan nilai temperatur di serial monitor
  Serial.print(" C res_thermistor: ");
  Serial.print(R2); // tampilkan nilai sesistansi thermistor
  Serial.print(" k ohm voltase_thermistor: ");
  Serial.println(voltase);
}
