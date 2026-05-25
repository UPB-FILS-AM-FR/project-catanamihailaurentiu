#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =================================================================
// 🎛️ MANETA DE CONTROL:
// Pune 0 ACUM (cât timp testezi doar în Serial Monitor, fără ecran)
// Pune 1 LA FACULTATE (după ce ai lipit pinii și ai ecranul pe placă)
// =================================================================
#define FOLOSESTE_ECRAN 0 

// --- CONFIGURARE ECRAN OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32 // Pune 64 dacă ecranul tău e modelul mai mare
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- CONFIGURARE SENZOR 1: PITCH (NOTE) ---
#define TRIG_PITCH 5
#define ECHO_PITCH 18

// --- CONFIGURARE SENZOR 2: VOLUM ---
#define TRIG_VOL 19
#define ECHO_VOL 23

// --- CONFIGURARE AUDIO (PWM) ---
#define AUDIO_PIN 26
#define LEDC_CHANNEL 0
#define LEDC_RESOLUTION 8 

// Funcție pentru citirea distanței
float citesteDistanta(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long durata = pulseIn(echoPin, HIGH, 20000); 
  if (durata == 0) return -1; 
  return (durata * 0.034) / 2;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n==========================================");
  Serial.println("         THEREMIN DIGITAL UNIFICAT        ");
  Serial.println("==========================================");

  pinMode(TRIG_PITCH, OUTPUT);
  pinMode(ECHO_PITCH, INPUT);
  pinMode(TRIG_VOL, OUTPUT);
  pinMode(ECHO_VOL, INPUT);
  
  ledcSetup(LEDC_CHANNEL, 2000, LEDC_RESOLUTION);
  ledcAttachPin(AUDIO_PIN, LEDC_CHANNEL);
  
  // Inițializăm ecranul DOAR dacă maneta este pe 1
  #if FOLOSESTE_ECRAN == 1
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("[EROARE] OLED-ul nu a fost detectat hardware!"));
    } else {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("Theremin Digital");
      display.display();
      Serial.println("[INFO] Ecranul OLED a pornit.");
    }
  #else
    Serial.println("[INFO] Mod Test: Ecranul OLED este dezactivat din software.");
  #endif
}

void loop() {
  float distantaPitch = citesteDistanta(TRIG_PITCH, ECHO_PITCH);
  float distantaVol = citesteDistanta(TRIG_VOL, ECHO_VOL);
  
  int frecventa = 0;
  int volumPWM = 0;
  
  // Logica pentru NOTĂ
  if (distantaPitch > 5 && distantaPitch < 40) {
    frecventa = map(distantaPitch, 5, 40, 200, 1200);
    ledcWriteTone(LEDC_CHANNEL, frecventa);
  } else {
    ledcWriteTone(LEDC_CHANNEL, 0); 
  }
  
  // Logica pentru VOLUM
  if (distantaVol > 5 && distantaVol < 40) {
    volumPWM = map(distantaVol, 5, 40, 0, 150);
    ledcWrite(LEDC_CHANNEL, volumPWM);
  } else if (distantaPitch <= 5 || distantaPitch >= 40) {
    ledcWrite(LEDC_CHANNEL, 0);
  }
  
  // Afișare în Serial Monitor
  Serial.print("Note: ");
  if (frecventa > 0) {
    Serial.print(distantaPitch); Serial.print("cm -> "); Serial.print(frecventa); Serial.print("Hz | ");
  } else {
    Serial.print("MUTED | ");
  }
  
  Serial.print("Volum: ");
  if (volumPWM > 0) {
    int procentVolum = map(volumPWM, 0, 150, 0, 100);
    Serial.print(distantaVol); Serial.print("cm -> "); Serial.print(procentVolum); Serial.println("%");
  } else {
    Serial.println("0%");
  }
  
  // Actualizare Ecran OLED (DOAR dacă maneta este pe 1)
  #if FOLOSESTE_ECRAN == 1
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Nota: "); 
    if(frecventa > 0) { display.print(frecventa); display.println(" Hz"); } else { display.println("MUTED"); }
    display.setCursor(0, 16);
    display.print("Vol: "); 
    if(volumPWM > 0) { display.print(map(volumPWM, 0, 150, 0, 100)); display.println(" %"); } else { display.println("0 %"); }
    display.display();
  #endif
  
  delay(100);
}