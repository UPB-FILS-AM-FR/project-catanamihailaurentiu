#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define FOLOSESTE_ECRAN 1 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32 
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- PINI ---
#define TRIG_FREQ 5
#define ECHO_FREQ 18  

#define TRIG_VOL 19
#define ECHO_VOL 21   

#define AUDIO_PIN 25  
#define LEDC_CHANNEL 0
#define LEDC_RESOLUTION 8 

#define NEOPIXEL_PIN 27 
#define NUM_PIXELS 10 
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define POT_PIN 34    
#define BUTTON_1 4    
#define BUTTON_2 15   

unsigned long precedentSenzori = 0;
const long intervalSenzori = 50; 

unsigned long precedentAfisare = 0;
const long intervalAfisare = 200; 

int modCuloare = 0;
bool esteMute = false;
bool stareButon1Anterioara = HIGH;
bool stareButon2Anterioara = HIGH;

float distantaFreq = -1;
float distantaVol = -1;
int valoarePot = 0;
int frecventa = 0;
int volumPWM = 0;
int leduriDeAprins = 0;

// --- FUNCȚIE DE CITIRE CU FILTRU DE MEDIERE (ELIMINĂ INTERFERENȚA) ---
float citesteDistantaFiltrata(int trigPin, int echoPin) {
  long sumaDurate = 0;
  int citiriValide = 0;
  
  for (int i = 0; i < 3; i++) { // Citim de 3 ori pentru a elimina vârfurile de bruiaj
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long durata = pulseIn(echoPin, HIGH, 12000); // Timeout strâns (12ms) ca să nu agațe audio
    if (durata > 100 && durata < 20000) { // Ignorăm direct erorile și zgomotul masiv
      sumaDurate += durata;
      citiriValide++;
    }
    delayMicroseconds(500); // Pauză mică între citiri ca să se stingă ecoul parazit
  }
  
  if (citiriValide == 0) return -1;
  long durataMedie = sumaDurate / citiriValide;
  return (durataMedie * 0.034) / 2;
}

void setup() {
  Serial.begin(115200);
  delay(400);

  pinMode(TRIG_FREQ, OUTPUT);
  pinMode(ECHO_FREQ, INPUT);
  pinMode(TRIG_VOL, OUTPUT);
  pinMode(ECHO_VOL, INPUT);
  
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  
  ledcSetup(LEDC_CHANNEL, 2000, LEDC_RESOLUTION);
  ledcAttachPin(AUDIO_PIN, LEDC_CHANNEL);
  ledcWrite(LEDC_CHANNEL, 0);
  
  pixels.begin();
  pixels.setBrightness(40); 
  pixels.show(); 
  
  #if FOLOSESTE_ECRAN == 1
    Wire.begin(22, 23); 
    if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("THEREMIN FILTRAT");
      display.display();
    }
  #endif
}

void loop() {
  unsigned long curentMilis = millis();

  // 1. CITIRE HARDWARE CU FILTRARE DIGITALĂ
  if (curentMilis - precedentSenzori >= intervalSenzori) {
    precedentSenzori = curentMilis;
    
    distantaFreq = citesteDistantaFiltrata(TRIG_FREQ, ECHO_FREQ);
    distantaVol = citesteDistantaFiltrata(TRIG_VOL, ECHO_VOL);
    valoarePot = analogRead(POT_PIN);

    // 2. LOGICĂ SUNET ȘI CALCUL LEDURI
    if (distantaFreq > 4 && distantaFreq < 50 && !esteMute) {
      frecventa = map(distantaFreq, 4, 50, 220, 1200);
      frecventa = constrain(frecventa, 50, 2000);
      ledcWriteTone(LEDC_CHANNEL, frecventa);

      if (distantaVol > 4 && distantaVol < 40) {
        volumPWM = map(distantaVol, 4, 40, 15, 140);
      } else {
        volumPWM = map(valoarePot, 0, 4095, 0, 140);
      }
      volumPWM = constrain(volumPWM, 0, 140);
      ledcWrite(LEDC_CHANNEL, volumPWM);

      // Mapare fluidă pentru cele 10 LED-uri
      leduriDeAprins = map(distantaFreq, 4, 45, NUM_PIXELS, 1);
      leduriDeAprins = constrain(leduriDeAprins, 1, NUM_PIXELS);
    } else {
      ledcWriteTone(LEDC_CHANNEL, 0); 
      ledcWrite(LEDC_CHANNEL, 0);
      leduriDeAprins = 0;
    }

    // 3. REFRESH CONTROLAT BANDĂ LED
    pixels.clear();
    if (esteMute) {
      for(int i=0; i<NUM_PIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 50, 0)); 
    } else {
      for (int i = 0; i < leduriDeAprins; i++) {
        if (modCuloare == 0) pixels.setPixelColor(i, pixels.Color(0, 255, 0));      
        else if (modCuloare == 1) pixels.setPixelColor(i, pixels.Color(0, 0, 255)); 
        else pixels.setPixelColor(i, pixels.Color(255, 0, 0));                     
      }
    }
    pixels.show();
  }

  // 4. LOGICĂ BUTOANE
  bool stareButon1Curenta = digitalRead(BUTTON_1);
  if (stareButon1Anterioara == HIGH && stareButon1Curenta == LOW) {
    modCuloare++; if (modCuloare > 2) modCuloare = 0;
    delay(40); 
  }
  stareButon1Anterioara = stareButon1Curenta;
  
  bool stareButon2Curenta = digitalRead(BUTTON_2);
  if (stareButon2Anterioara == HIGH && stareButon2Curenta == LOW) {
    esteMute = !esteMute;
    delay(40); 
  }
  stareButon2Anterioara = stareButon2Curenta;

  // 5. REFRESH ECRAN ȘI SERIAL
  if (curentMilis - precedentAfisare >= intervalAfisare) {
    precedentAfisare = curentMilis;
    
    int procVol = map(volumPWM, 0, 140, 0, 100);
    Serial.print("Freq: "); Serial.print(distantaFreq); Serial.print("cm | ");
    Serial.print("LEDs: "); Serial.print(leduriDeAprins); Serial.print(" | ");
    Serial.print("Cul: "); Serial.println(modCuloare);

    #if FOLOSESTE_ECRAN == 1
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      if (frecventa > 0) {
        display.print("F: "); display.print(frecventa); display.print("Hz  V: "); display.print(procVol); display.println("%");
      } else {
        display.println(esteMute ? "MUTE ACTIVE" : "OUT OF RANGE");
      }
      display.setCursor(0, 16);
      display.print("Cul: "); display.print(modCuloare);
      display.print(" | LED: "); display.print(leduriDeAprins);
      display.display();
    #endif
  }
}
