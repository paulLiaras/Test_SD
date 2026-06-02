#include <Arduino.h>

// Simple WAV file player example
//
// Three types of output may be used, by configuring the code below.
//
//   1: Digital I2S - Normally used with the audio shield:
//         http://www.pjrc.com/store/teensy3_audio.html
//
//   2: Digital S/PDIF - Connect pin 22 to a S/PDIF transmitter
//         https://www.oshpark.com/shared_projects/KcDBKHta
//
//   3: Analog DAC - Connect the DAC pin to an amplified speaker
//         http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
//
// To configure the output type, first uncomment one of the three
// output objects.  If not using the audio shield, comment out
// the sgtl5000_1 lines in setup(), so it does not wait forever
// trying to configure the SGTL5000 codec chip.
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// ==============================================================
#define MODE_HEXA // Commentez (//) cette ligne pour repasser en mode classique (1 fichier a la fois)
// ==============================================================

AudioPlaySdWav           playWav1;
AudioPlaySdWav           playWav2;
AudioPlaySdWav           playWav3;
AudioPlaySdWav           playWav4;
AudioPlaySdWav           playWav5;
AudioPlaySdWav           playWav6;

// On garde uniquement un objet de sortie pour générer l'horloge audio interne (le "tempo").
// Sans lui, la progression de lecture (positionMillis) n'avance pas !
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
//On Teensy LC, use this for the Teensy Audio Shield:
//AudioOutputI2Sslave    audioOutput;

// AUCUN AudioConnection (patchCord) n'est créé.
// Le son est lu par la carte SD, mais n'est envoyé vers aucune sortie.
// Il n'y aura donc aucune accumulation ni saturation.
// Use these with the Teensy Audio Shield
//#define SDCARD_CS_PIN    10
//#define SDCARD_MOSI_PIN  7   // Teensy 4 ignores this, uses pin 11
//#define SDCARD_SCK_PIN   14  // Teensy 4 ignores this, uses pin 13

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

void setup() {
  Serial.begin(9600);

  // Attendre que le moniteur série soit ouvert avant de continuer le programme
  while (!Serial);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
#ifdef MODE_HEXA
  AudioMemory(30); // Augmente pour supporter 6 lecteurs en meme temps
#else
  AudioMemory(8);
#endif

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
 //sgtl5000_1.enable();
 //sgtl5000_1.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(25);

  // Vérifie si le fichier a bien été trouvé et a commencé à être lu
  if (!playWav1.isPlaying()) {
    Serial.println("  -> Erreur: Fichier introuvable ou format invalide !");
    return;
  }

  Serial.print("  -> Duree totale : ");
  Serial.print(playWav1.lengthMillis());
  Serial.println(" ms");

  uint32_t lastPrint = 0;
  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // Affiche la progression toutes les secondes (1000 ms)
    if (millis() - lastPrint >= 1000) {
      Serial.print("  Avancement : ");
      Serial.print(playWav1.positionMillis());
      Serial.println(" ms");
      lastPrint = millis();
    }
  }
  Serial.println("  -> Lecture terminee.");
}

void playHexaFiles(const char *f1, const char *f2, const char *f3)
{
  Serial.println("\n========================================");
  Serial.println("Lancement de la lecture Hexa (6 fichiers Mono en silence)...");

  playWav1.play(f1);
  playWav2.play(f2);
  playWav3.play(f3);
  // playWav4.play(f4);
  // playWav5.play(f5);
  // playWav6.play(f6);

  delay(25);

  if (!playWav1.isPlaying() || !playWav2.isPlaying() || !playWav3.isPlaying() || !playWav4.isPlaying() || !playWav5.isPlaying() || !playWav6.isPlaying()) {
    Serial.println("  -> Erreur: Un ou plusieurs fichiers sont introuvables !");
    return;
  }

  uint32_t lengthMillis = playWav1.lengthMillis();
  Serial.print("  -> Duree totale : ");
  Serial.print(lengthMillis);
  Serial.println(" ms");

  uint32_t lastPrint = 0;
  while (playWav1.isPlaying() || playWav2.isPlaying() || playWav3.isPlaying() || playWav4.isPlaying() || playWav5.isPlaying() || playWav6.isPlaying()) {
    if (millis() - lastPrint >= 500) { // Mise à jour toutes les demi-secondes
      uint32_t pos = playWav1.positionMillis();
      int percent = (lengthMillis > 0) ? (pos * 100) / lengthMillis : 0;

      // Création d'une barre de progression visuelle [========>   ]
      Serial.print("  [");
      for (int i = 0; i < 20; i++) {
        if (i < (percent / 5)) Serial.print("=");
        else if (i == (percent / 5)) Serial.print(">");
        else Serial.print(" ");
      }
      Serial.print("] ");
      Serial.print(percent);
      Serial.print("%  (");
      Serial.print(pos);
      Serial.println(" ms)");

      lastPrint = millis();
    }
  }
  Serial.println("  -> Lecture hexa terminee.");
  Serial.println("========================================");
}

void loop() {
#ifdef MODE_HEXA
  playHexaFiles("1.wav", "2.wav", "3.wav");  // filenames are always uppercase 8.3 format
  delay(2000);
#else
  playFile("1.wav");  // filenames are always uppercase 8.3 format
  delay(500);
  playFile("2.wav");
  delay(500);
  playFile("3.wav");
  delay(1500);
#endif
}