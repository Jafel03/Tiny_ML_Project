# Tiny_ML_Project
Ce projet permet la classification des différents types de vibrations.
Étapes de mise en œuvre :

1️⃣ Dans le dossier `DataGeneration_Arduino` executer le fichier `.ino` afin qu'elle puisse effectuer des mesures avec l'accéléromètre.  

2️⃣ Exécutez le script Python generate_data_to_train_vibration` pour récupérer les données.  
   - Saisissez soit le nombre de points d'échantillonnage, soit la durée d'acquisition en foncion de ce que vous desirez.  
   - Indiquez le nom du fichier CSV de sortie.  
   - Démarrez l'acquisition en cliquant sur executer le script et patientez jusqu'à l'affichage d'une notification indiquant la fin du processus.  

3️⃣ Une fois l'acquisition terminée, le fichier CSV généré sera stocké dans le même dossier. Déplacez-le ensuite dans `Training_Vbration/dataset`.  

4️⃣ Ouvrez le fichier Jupyter Notebook " arduino_tiny_ml1.ipynb " et suivez les étapes du code pour entraîner le modèle de machine learning.  
   - Faire quelques ajustements du script pour répondre aux besoins du projet.  

5️⃣ Après l'entraînement, un fichier `model_vibration.h (vous pouvez le renommer a votre convenance)` sera généré dans le dossier du Jupyter Notebook. Copiez-le  vers `Inference_Arduino/inference_vibrations` et transférez le fichier `.ino` correspondant sur la carte Arduino.  

6️⃣ Activez le moniteur série dans l'IDE Arduino pour observer les prédictions du modèle en temps réel.  


