/*
  Vibration Classifier
  Ce code utilise l’IMU intégré pour lire les données d’accélération et de gyroscope
  en temps réel. Une fois qu’un certain nombre d’échantillons sont collectés, il applique
  un modèle TensorFlow Lite pour Microcontrollers afin de classifier la vibration.

  Carte compatible : Arduino Nano 33 BLE Sense
  Basé sur l'exemple original "IMU Classifier" de Don Coleman et Sandeep Mistry
  Modifié pour le projet de classification des vibrations
*/

#include <Arduino_LSM9DS1.h>
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

// Inclusion du modèle converti en tableau C++
#include "model_vibration.h"

const float accelerationThreshold = 0.5; // Seuil de détection en G
const int numSamples = 119;
int samplesRead = numSamples;

// Variables globales pour TensorFlow Lite
tflite::MicroErrorReporter tflErrorReporter;
tflite::AllOpsResolver tflOpsResolver;
const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Mémoire pour TensorFlow Lite Micro
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// Tableau des types de vibrations détectées
const char* VIBRATIONS[] = {
  "infinity",
  "HeartBeat"
};

#define NUM_VIBRATIONS (sizeof(VIBRATIONS) / sizeof(VIBRATIONS[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialisation de l’IMU
  if (!IMU.begin()) {
    Serial.println("Erreur : Impossible d'initialiser l'IMU !");
    while (1);
  }

  // Affichage des taux d'échantillonnage de l’IMU
  Serial.print("Taux d'échantillonnage de l'accéléromètre = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  Serial.print("Taux d'échantillonnage du gyroscope = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");

  Serial.println();

  // Chargement du modèle TensorFlow Lite
  tflModel = tflite::GetModel(model_vibration);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Erreur : Version du modèle incompatible !");
    while (1);
  }

  // Initialisation de l'interprète TensorFlow Lite
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);
  tflInterpreter->AllocateTensors();

  // Récupération des tenseurs d'entrée et de sortie du modèle
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // Attente d'un mouvement significatif
  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      if (aSum >= accelerationThreshold) {
        samplesRead = 0;
        break;
      }
    }
  }

  // Lecture des échantillons nécessaires
  while (samplesRead < numSamples) {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Normalisation des données IMU pour les adapter au modèle
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Exécution de l'inférence
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Erreur : L'exécution de l'inférence a échoué !");
          while (1);
        }

        // Affichage des résultats de classification
        for (int i = 0; i < NUM_VIBRATIONS; i++) {
          Serial.print(VIBRATIONS[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
        }
        Serial.println();
      }
    }
  }
}
