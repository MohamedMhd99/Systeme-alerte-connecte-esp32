# Systeme-alerte-connecte-esp32
# Système d'alerte connecté – ESP32

## Présentation du projet

Ce projet consiste à concevoir et réaliser un système d'alerte connecté basé sur une carte ESP32. Il utilise plusieurs capteurs pour détecter les mouvements et la présence d'obstacles, puis déclenche une alerte progressive à travers des indicateurs lumineux et sonores.

Le système intègre également une interface Web permettant de visualiser en temps réel l'état des capteurs et le niveau d'alerte.

## Objectifs

- Détecter les mouvements et les obstacles à l'aide de capteurs.
- Mettre en place un système d'alerte à plusieurs niveaux.
- Commander des indicateurs lumineux et une alarme sonore.
- Développer une interface Web accessible depuis un smartphone ou un ordinateur.
- Exploiter les fonctionnalités Wi-Fi de l'ESP32.

## Matériel utilisé

- ESP32 – unité centrale de traitement.
- HC-SR501 – capteur infrarouge passif de détection de mouvement.
- FC-51 – capteur infrarouge de détection d'obstacles.
- 3 LED rouges – indicateurs visuels.
- Buzzer actif – alarme sonore.
- Résistances de protection.
- Breadboard et fils de connexion.

## Fonctionnement

Le système repose sur une logique d'alerte progressive :

- **Niveau 1 :** détection de mouvement par le HC-SR501, activation de la première LED.
- **Niveau 2 :** détection d'un obstacle par le FC-51, activation des deux premières LED.
- **Niveau 3 :** détection simultanée par les deux capteurs, activation des trois LED et du buzzer à fréquence d'alerte plus élevée.

Les niveaux dépendent de l'état logique des capteurs et non d'une mesure précise de distance.

## Interface Web

L'ESP32 crée son propre réseau Wi-Fi afin de permettre l'accès à une interface Web locale.

Cette interface permet notamment :

- De visualiser l'état des capteurs.
- De consulter le niveau d'alerte.
- D'afficher une représentation graphique de l'activité du système.

L'interface est hébergée directement sur l'ESP32, sans nécessiter de serveur externe.

## Technologies utilisées

- C++ / Arduino
- ESP32
- Wi-Fi
- HTML
- CSS
- JavaScript
- Protocole HTTP
- JSON


## Perspectives d'amélioration

- Ajouter des capteurs supplémentaires pour améliorer la détection.
- Améliorer l'interface Web et les visualisations.
- Ajouter un système de notification à distance.
- Concevoir une carte électronique dédiée au projet.
- Optimiser la consommation énergétique.
