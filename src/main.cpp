#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD

// --- 1. LES LECTEURS (3 fichiers stéréo) ---
AudioPlaySdWav playWav1;
AudioPlaySdWav playWav2;
AudioPlaySdWav playWav3;

// --- 2. LA SORTIE (USB 6 Canaux vers le PC) ---
// On remplace la sortie Stéréo par la sortie Hexaphonique
AudioOutputUSBHex usbOutHex;

// --- 3. LES INTERFACES TDM2 ---
AudioOutputTDM2 tdmOut;
AudioInputTDM2  tdmIn;

// --- 4. LE ROUTAGE STEREO (SD -> TDM2 -> DAISY -> TDM2 -> USB) ---

// A) SD -> TDM2 OUT (Vers Daisy, sur les ports pairs 0 et 2 = slots 0 et 1)
AudioConnection patchSdToTdm1(playWav1, 0, tdmOut, 0);
AudioConnection patchSdToTdm2(playWav1, 1, tdmOut, 2);

// B) TDM2 IN -> USB (Depuis Daisy, ports pairs 0 et 2 vers PC)
AudioConnection patchTdmToUsb1(tdmIn, 0, usbOutHex, 0);
AudioConnection patchTdmToUsb2(tdmIn, 2, usbOutHex, 1);


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  // Sécurité Moniteur Série
  while (!Serial) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  Serial.println("--- Démarrage du Test Hexaphonique (USB 6 Canaux) ---");
  
  // On alloue un peu plus de mémoire car l'USB 6 canaux est plus gourmand
  AudioMemory(60);

  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Erreur SD !");
    while (1);
  }
  Serial.println("Carte SD OK !");
}

void loop() {
  Serial.println("Lecture en cours...");
  
  playWav1.play("1.wav");
  // playWav2.play("2.wav"); // Désactivé pour le test stéréo
  // playWav3.play("3.wav"); // Désactivé pour le test stéréo

  delay(50);

  while (playWav1.isPlaying()) {
    delay(100);
  }

  Serial.println("Fin. Redémarrage...");
  delay(2000);
}