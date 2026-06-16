#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN   BUILTIN_SDCARD

// 1. Les 3 lecteurs de fichiers SD
AudioPlaySdWav          playWav1;
AudioPlaySdWav          playWav2;
AudioPlaySdWav          playWav3;

// 2. Les 2 mixeurs
AudioMixer4             mixerLeft;
AudioMixer4             mixerRight;

// 3. LA CORRECTION : Sortie USB standard 2 canaux
AudioOutputUSB          usbOut; 

// --- ROUTAGE GAUCHE (Canal 0 des fichiers -> Mixeur Gauche) ---
AudioConnection patchG1(playWav1, 0, mixerLeft,  0);
AudioConnection patchG2(playWav2, 0, mixerLeft,  1);
AudioConnection patchG3(playWav3, 0, mixerLeft,  2);

// --- ROUTAGE DROIT (Canal 1 des fichiers -> Mixeur Droit) ---
AudioConnection patchD1(playWav1, 1, mixerRight, 0);
AudioConnection patchD2(playWav2, 1, mixerRight, 1);
AudioConnection patchD3(playWav3, 1, mixerRight, 2);

// --- SORTIE (Mixeurs -> USB) ---
AudioConnection outGauche(mixerLeft,  0, usbOut, 0);
AudioConnection outDroite(mixerRight, 0, usbOut, 1);

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

  Serial.println("--- Début de l'initialisation ---");
  
  AudioMemory(40);

  // Application du gain (-10dB environ) pour éviter la saturation du mélange
  for (int i=0; i<3; i++) {
    mixerLeft.gain(i, 0.33);
    mixerRight.gain(i, 0.33);
  }

  Serial.println("Initialisation de la carte SD...");
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("ERREUR : Impossible d'accéder à la carte SD !");
    while (1); // Bloque ici si pas de carte
  }
  Serial.println("Carte SD OK !");
}

void loop() {
  Serial.println("Lecture des fichiers...");
  
  // Lancement simultané
  playWav1.play("1.wav");
  playWav2.play("2.wav");
  playWav3.play("3.wav");

  delay(50); // Laisse le temps au processeur d'ouvrir les fichiers

  // Vérification de sécurité
  if (!playWav1.isPlaying() && !playWav2.isPlaying() && !playWav3.isPlaying()) {
    Serial.println("Erreur: Les fichiers 1.wav, 2.wav et 3.wav sont introuvables à la racine !");
    delay(2000);
    return;
  }

  // Boucle d'attente : on ne fait rien tant que la musique tourne
  while (playWav1.isPlaying() || playWav2.isPlaying() || playWav3.isPlaying()) {
    delay(100);
  }

  Serial.println("Fin de lecture. Redémarrage dans 2 secondes...");
  delay(2000);
}