
#include <WiFi.h>
#include <WebServer.h>

// ==================================================
// 1. DECLARATION DES BROCHES
// ==================================================

#define LED_R1 18
#define LED_R2 19
#define LED_R3 21

#define BUZZER 22

#define HC_SR501 16
#define FC51 17

// ==================================================
// 2. CONFIGURATION WIFI
// ==================================================

const char* ssid = "ESP32-IoT-Alert";
const char* password = "12345678";

WebServer server(80);

// ==================================================
// 3. VARIABLES DU SYSTEME
// ==================================================

int pirState = LOW;
int obstacleState = HIGH;

int alertLevel = 0;

bool buzzerState = false;

unsigned long previousBuzzerTime = 0;
unsigned long previousBeepTime = 0;

// Duree du bip : 120 ms
const unsigned long beepDuration = 120;

// ==================================================
// 4. PAGE WEB
// ==================================================

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 IoT Alert</title>

<style>
body {
  background: #101820;
  color: white;
  font-family: Arial;
  text-align: center;
}

h1 { color: #00d9ff; }

.card {
  background: #1d2b36;
  padding: 20px;
  margin: 15px;
  border-radius: 15px;
}

#level {
  font-size: 40px;
  color: #ff4444;
}

#radar {
  width: 250px;
  height: 250px;
  border-radius: 50%;
  border: 3px solid #00ff88;
  margin: auto;
  position: relative;
  background: radial-gradient(circle, #173b2b, #071b12);
}

#dot {
  width: 15px;
  height: 15px;
  border-radius: 50%;
  background: red;
  position: absolute;
  left: 50%;
  transform: translateX(-50%);
  transition: top 0.3s;
}
</style>
</head>

<body>

<h1>ESP32 IoT Alert System</h1>

<div class="card">
  <h2>Niveau d'alerte</h2>
  <div id="level">0</div>
  <p id="status">Initialisation...</p>
</div>

<div class="card">
  <h2>Radar de proximité</h2>
  <div id="radar">
    <div id="dot" style="top:90%"></div>
  </div>
  <p>Visualisation indicative, pas une mesure réelle de distance.</p>
</div>

<div class="card">
  <p id="pir">HC-SR501 : --</p>
  <p id="fc">FC-51 : --</p>
  <p id="leds">LEDs : --</p>
  <p id="buzzer">Buzzer : --</p>
</div>

<script>

async function updateData() {

  try {

    const response = await fetch('/data');
    const data = await response.json();

    document.getElementById('level').innerText = data.level;

    document.getElementById('status').innerText =
      data.level == 0 ? "Aucune alerte" :
      data.level == 1 ? "Mouvement detecte" :
      data.level == 2 ? "Obstacle proche" :
      "ALERTE MAXIMALE";

    document.getElementById('pir').innerText =
      "HC-SR501 : " + (data.pir ? "Mouvement" : "Repos");

    document.getElementById('fc').innerText =
      "FC-51 : " + (data.obstacle ? "Obstacle" : "Libre");

    document.getElementById('leds').innerText =
      "LEDs : " + data.level;

    document.getElementById('buzzer').innerText =
      "Buzzer : " + (data.level > 0 ? "Actif" : "Arrete");

    let positions = [90, 70, 45, 20];

    document.getElementById('dot').style.top =
      positions[data.level] + "%";

  }

  catch(error) {

    document.getElementById('status').innerText =
      "Connexion en cours...";

  }
}

setInterval(updateData, 500);
updateData();

</script>

</body>
</html>
)rawliteral";

// ==================================================
// 5. AFFICHAGE DE LA PAGE WEB
// ==================================================

void handleRoot() {

  server.send(200, "text/html", webpage);

}

// ==================================================
// 6. ENVOI DES DONNEES VERS LE NAVIGATEUR
// ==================================================

void handleData() {

  String json = "{";

  json += "\"pir\":";
  json += (pirState == HIGH ? "true" : "false");

  json += ",\"obstacle\":";
  json += (obstacleState == LOW ? "true" : "false");

  json += ",\"level\":";
  json += alertLevel;

  json += "}";

  server.send(200, "application/json", json);

}

// ==================================================
// 7. INITIALISATION
// ==================================================

void setup() {

  Serial.begin(115200);

  pinMode(HC_SR501, INPUT);
  pinMode(FC51, INPUT);

  pinMode(LED_R1, OUTPUT);
  pinMode(LED_R2, OUTPUT);
  pinMode(LED_R3, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  digitalWrite(LED_R1, LOW);
  digitalWrite(LED_R2, LOW);
  digitalWrite(LED_R3, LOW);

  // Creation du reseau Wi-Fi

  Serial.println("Demarrage du Wi-Fi...");

  bool wifiOK = WiFi.softAP(ssid, password);

  Serial.print("Wi-Fi : ");
  Serial.println(wifiOK ? "OK" : "ECHEC");

  Serial.print("Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  // Configuration du serveur Web

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();

  Serial.println("Serveur Web : DEMARRE");
  Serial.println("En attente de connexion...");

}

// ==================================================
// 8. BOUCLE PRINCIPALE
// ==================================================

void loop() {

  // Traitement des requetes Web

  server.handleClient();

  // Lecture des capteurs

  pirState = digitalRead(HC_SR501);
  obstacleState = digitalRead(FC51);

  // ==================================================
  // 9. CALCUL DU NIVEAU D'ALERTE
  // ==================================================

  if (pirState == HIGH && obstacleState == LOW) {

    alertLevel = 3;

  }

  else if (obstacleState == LOW) {

    alertLevel = 2;

  }

  else if (pirState == HIGH) {

    alertLevel = 1;

  }

  else {

    alertLevel = 0;

  }

  // ==================================================
  // 10. COMMANDE DES LEDs
  // ==================================================

  digitalWrite(LED_R1, alertLevel >= 1 ? HIGH : LOW);
  digitalWrite(LED_R2, alertLevel >= 2 ? HIGH : LOW);
  digitalWrite(LED_R3, alertLevel >= 3 ? HIGH : LOW);

  // ==================================================
  // 11. COMMANDE DU BUZZER
  // ==================================================

  unsigned long currentTime = millis();

  unsigned long beepInterval;

  if (alertLevel == 3) {

    beepInterval = 200;

  }

  else if (alertLevel == 2) {

    beepInterval = 400;

  }

  else if (alertLevel == 1) {

    beepInterval = 800;

  }

  else {

    beepInterval = 0;

  }

  // Arret du buzzer en absence d'alerte

  if (alertLevel == 0) {

    digitalWrite(BUZZER, LOW);

    buzzerState = false;

  }

  else {

    // Activation du buzzer

    if (!buzzerState &&
        currentTime - previousBuzzerTime >= beepInterval) {

      digitalWrite(BUZZER, HIGH);

      buzzerState = true;

      previousBuzzerTime = currentTime;
      previousBeepTime = currentTime;

    }

    // Arret apres 120 ms

    if (buzzerState &&
        currentTime - previousBeepTime >= beepDuration) {

      digitalWrite(BUZZER, LOW);

      buzzerState = false;

    }

  }

  // ==================================================
  // 12. MONITEUR SERIE
  // ==================================================

  static unsigned long previousSerialTime = 0;

  if (currentTime - previousSerialTime >= 100) {

    previousSerialTime = currentTime;

    Serial.print("PIR : ");
    Serial.print(pirState);

    Serial.print(" | FC51 : ");
    Serial.print(obstacleState);

    Serial.print(" | Niveau : ");
    Serial.println(alertLevel);

  }

}
