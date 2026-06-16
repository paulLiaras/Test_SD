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

// --- 3. LE ROUTAGE DIRECT (Passthrough pur 6 canaux) ---

// Fichier 1 -> Canaux USB 0 & 1 (Cordes 1 & 2)
AudioConnection patchCorde1(playWav1, 0, usbOutHex, 0);
AudioConnection patchCorde2(playWav1, 1, usbOutHex, 1);

// Fichier 2 -> Canaux USB 2 & 3 (Cordes 3 & 4)
AudioConnection patchCorde3(playWav2, 0, usbOutHex, 2);
AudioConnection patchCorde4(playWav2, 1, usbOutHex, 3);

// Fichier 3 -> Canaux USB 4 & 5 (Cordes 5 & 6)
AudioConnection patchCorde5(playWav3, 0, usbOutHex, 4);
AudioConnection patchCorde6(playWav3, 1, usbOutHex, 5);


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
  playWav2.play("2.wav");
  playWav3.play("3.wav");

  delay(50);

  while (playWav1.isPlaying() || playWav2.isPlaying() || playWav3.isPlaying()) {
    delay(100);
  }

  Serial.println("Fin. Redémarrage...");
  delay(2000);
}