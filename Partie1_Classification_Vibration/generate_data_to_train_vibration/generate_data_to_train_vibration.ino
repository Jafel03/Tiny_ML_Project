#include <Arduino_LSM9DS1.h>

const float vibrationThreshold = 0.5;  // Seuil pour détecter une vibration

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Échec de l'initialisation de l'IMU !");
    while (1);
  }

  Serial.println("aX,aY,aZ,gX,gY,gZ"); // En-tête CSV
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // Détectons une vibration et commencons l'enregistrement
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);

    float vibration = fabs(aX) + fabs(aY) + fabs(aZ);

    if (vibration >= vibrationThreshold) {
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(gX, gY, gZ);

        // Envoyons les données en continu à Python
        Serial.print(aX, 3);
        Serial.print(',');
        Serial.print(aY, 3);
        Serial.print(',');
        Serial.print(aZ, 3);
        Serial.print(',');
        Serial.print(gX, 3);
        Serial.print(',');
        Serial.print(gY, 3);
        Serial.print(',');
        Serial.println(gZ, 3);
      }
    }
  }
}
