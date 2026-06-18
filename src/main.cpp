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

// --- 4. LE ROUTAGE HEXAPHONIQUE (SD -> TDM2 -> DAISY -> TDM2 -> USB) ---

// A) SD -> TDM2 OUT (Vers Daisy, ports pairs 0, 2, 4, 6, 8, 10 = slots 0 à 5)
AudioConnection patchSdToTdm1(playWav1, 0, tdmOut, 0);
AudioConnection patchSdToTdm2(playWav1, 1, tdmOut, 2);

AudioConnection patchSdToTdm3(playWav2, 0, tdmOut, 4);
AudioConnection patchSdToTdm4(playWav2, 1, tdmOut, 6);

AudioConnection patchSdToTdm5(playWav3, 0, tdmOut, 8);
AudioConnection patchSdToTdm6(playWav3, 1, tdmOut, 10);

// B) TDM2 IN -> USB (Depuis Daisy, ports pairs 0, 2, 4, 6, 8, 10 vers PC)
AudioConnection patchTdmToUsb1(tdmIn, 0, usbOutHex, 0);
AudioConnection patchTdmToUsb2(tdmIn, 2, usbOutHex, 1);
AudioConnection patchTdmToUsb3(tdmIn, 4, usbOutHex, 2);
AudioConnection patchTdmToUsb4(tdmIn, 6, usbOutHex, 3);
AudioConnection patchTdmToUsb5(tdmIn, 8, usbOutHex, 4);
AudioConnection patchTdmToUsb6(tdmIn, 10, usbOutHex, 5);


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
  playWav2.play("2.wav"); // Réactivé
  playWav3.play("3.wav"); // Réactivé

  delay(50);

  while (playWav1.isPlaying() || playWav2.isPlaying() || playWav3.isPlaying()) {
    delay(100);
  }

  Serial.println("Fin. Redémarrage...");
  delay(2000);
}