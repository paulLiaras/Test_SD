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

// --- 2. LES MIXEURS (Notre table de mixage virtuelle) ---
AudioMixer4 mixerLeft;
AudioMixer4 mixerRight;

// --- 3. LA SORTIE (USB Stéréo vers le PC) ---
AudioOutputUSB usbOut;

// --- 4. LE ROUTAGE HEXAPHONIQUE (Le Passthrough) ---

// CORDE 1 : Fichier 1 (Gauche) -> Mixeur Gauche (Canal 0)
AudioConnection patchCorde1(playWav1, 0, mixerLeft, 0);

// CORDE 2 : Fichier 1 (Droite) -> Mixeur Droit (Canal 0)
AudioConnection patchCorde2(playWav1, 1, mixerRight, 0);

// CORDE 3 : Fichier 2 (Gauche) -> Mixeur Gauche (Canal 1)
AudioConnection patchCorde3(playWav2, 0, mixerLeft, 1);

// CORDE 4 : Fichier 2 (Droite) -> Mixeur Droit (Canal 1)
AudioConnection patchCorde4(playWav2, 1, mixerRight, 1);

// CORDE 5 : Fichier 3 (Gauche) -> Mixeur Gauche (Canal 2)
AudioConnection patchCorde5(playWav3, 0, mixerLeft, 2);

// CORDE 6 : Fichier 3 (Droite) -> Mixeur Droit (Canal 2)
AudioConnection patchCorde6(playWav3, 1, mixerRight, 2);

// ENVOI AU PC : Mixeurs -> Câble USB
AudioConnection outGauche(mixerLeft, 0, usbOut, 0);
AudioConnection outDroite(mixerRight, 0, usbOut, 1);


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  while (!Serial) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  Serial.println("--- Démarrage du Test Hexaphonique ---");
  AudioMemory(40);

  // ---------------------------------------------------------
  // LA TABLE DE MIXAGE (Test des cordes)
  // Règle le volume entre 0.0 (Muet) et 1.0 (Maximum)
  // ---------------------------------------------------------

  // PAIRE 1 (Cordes 1 & 2)
  mixerLeft.gain(0, 0.8);  // Corde 1 (Oreille Gauche)
  mixerRight.gain(0, 0.8); // Corde 2 (Oreille Droite)

  // PAIRE 2 (Cordes 3 & 4)
  mixerLeft.gain(1, 0.0);  // Corde 3 (Muet pour le test)
  mixerRight.gain(1, 0.0); // Corde 4 (Muet pour le test)

  // PAIRE 3 (Cordes 5 & 6)
  mixerLeft.gain(2, 0.0);  // Corde 5 (Muet pour le test)
  mixerRight.gain(2, 0.0); // Corde 6 (Muet pour le test)
  // ---------------------------------------------------------

  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Erreur SD !");
    while (1);
  }
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