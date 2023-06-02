#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "";
const char* password = "";
const char* mqtt_server = "172.20.10.7";

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
hw_timer_t *Timer0_Cfg = NULL;

const int Motor1A = 12; 
const int Motor1B = 14;
const int Motor2A = 27;
const int Motor2B = 26;
const int Motor1E = 18;
const int Motor2E = 19;
int last_button_time = 0;
int button_time;

void IRAM_ATTR bandaT() {
  button_time = millis();
  if (button_time - last_button_time > 250)
  {
        digitalWrite(Motor1A, HIGH);
        digitalWrite(Motor1B, LOW);
        timerAlarmWrite(Timer0_Cfg, timerReadMicros(Timer0_Cfg)+500000, false);
        timerAlarmEnable(Timer0_Cfg);
        last_button_time = button_time;
  }
}


void IRAM_ATTR Timer0_ISR()
{
    if (digitalRead(Motor1A) == 1)
      digitalWrite(Motor1A, LOW);
    if (digitalRead(Motor1B) == 1)
      digitalWrite(Motor1B, LOW);
    if (digitalRead(Motor2B) == 1)
      digitalWrite(Motor2B, LOW);
}

void IRAM_ATTR banda() {
  button_time = millis();
  if (button_time - last_button_time > 250)
  {
        digitalWrite(Motor1A, LOW);
        digitalWrite(Motor1B, HIGH);
        timerAlarmWrite(Timer0_Cfg, timerReadMicros(Timer0_Cfg)+500000, false);
        timerAlarmEnable(Timer0_Cfg);
        last_button_time = button_time;
  }
}




void IRAM_ATTR door() {
  button_time = millis();
  if (button_time - last_button_time > 250)
  {
        digitalWrite(Motor2A, LOW);
        digitalWrite(Motor2B, HIGH);
        timerAlarmWrite(Timer0_Cfg, timerReadMicros(Timer0_Cfg)+2000000, false);
        timerAlarmEnable(Timer0_Cfg);
        last_button_time = button_time;
  }
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (String(topic) == "esp32/motors") {
    Serial.print("Changing output to ");
    if(messageTemp == "banda"){
      Serial.println("banda on ");
      door();
    }
    else if(messageTemp == "door"){
      Serial.println("door open");
      banda();
    }
    else if(messageTemp == "cerrar"){
      Serial.println("door close");
      bandaT();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      client.subscribe("esp32/motors");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(Motor1A, OUTPUT);
  pinMode(Motor1B, OUTPUT);
  pinMode(Motor2A, OUTPUT);
  pinMode(Motor2B, OUTPUT);
  pinMode(Motor1E, OUTPUT);
  pinMode(Motor2E, OUTPUT);

  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    
  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      delay(1000);
  }

  WiFi.begin(ssid, password);
 
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.print("Local IP: "); Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(Motor1E, HIGH);
  digitalWrite(Motor2E, HIGH);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
