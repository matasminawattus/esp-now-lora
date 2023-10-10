//sender veikia su switchais
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define BYTES 249
#define INTS 62
#define SEND_INTERVAL 100

int n = 0;
uint8_t good_to_send = 1;
unsigned long lastSentTime;
unsigned long lastPrintTime;
int sent_packets = 0;
int8_t power = 72;
wifi_phy_rate_t wifiPhyRateMode = WIFI_PHY_RATE_5M_S;
//wifi_phy_rate_t wifiPhyRateMode = WIFI_PHY_RATE_MCS0_SGI;

//uint8_t broadcastAddress[] = {0x8C, 0x4B, 0x14, 0x1A, 0xBF, 0xD0}; //su antena pair 1 receiver
uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0x52, 0xBC, 0xF4}; // Receiver MAC Pair 1

//uint8_t broadcastAddress[] = {0x58, 0xcf, 0x79, 0xe3, 0x3d, 0x38}; // c3 su sma antena receiver


typedef struct struct_message {
  int b[INTS];
} struct_message;
struct_message myData_send;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    n += 1;
  }
  good_to_send = 1;
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
}

esp_now_peer_info_t peerInfo;

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Set the initial Wi-Fi PHY rate mode
  esp_wifi_config_espnow_rate(WIFI_IF_STA, wifiPhyRateMode);

  // Set tx power
  esp_wifi_set_max_tx_power(power);
  esp_wifi_get_max_tx_power(&power);
  Serial.print("esp_wifi_get_max_tx_power(&power)= ");
  Serial.println(power);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  } else {
    Serial.println("ESP-NOW initialized");
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  } else {
    Serial.println("Peer added");
  }
}

void loop() {
  // Check for commands received via the serial COM port
  if (Serial.available() > 0) {
    char command = Serial.read();
    switch (command) {
      case '1':
        wifiPhyRateMode = WIFI_PHY_RATE_1M_L;
        Serial.println("WIFI_PHY_RATE_1M_L");
        break;
      case '2':
        wifiPhyRateMode = WIFI_PHY_RATE_2M_L;
        Serial.println("WIFI_PHY_RATE_2M_L");
        break;
      case '3':
        wifiPhyRateMode = WIFI_PHY_RATE_5M_L;
        Serial.println("WIFI_PHY_RATE_5M_L");
        break;
      case '4':
        wifiPhyRateMode = WIFI_PHY_RATE_11M_L;
        Serial.println("WIFI_PHY_RATE_11M_L");
        break;
      case '5':
        wifiPhyRateMode = WIFI_PHY_RATE_2M_S;
        Serial.println("WIFI_PHY_RATE_2M_S");
        break;
      case '6':
        wifiPhyRateMode = WIFI_PHY_RATE_5M_S;
        Serial.println("WIFI_PHY_RATE_5M_S");
        break;
      case '7':
        wifiPhyRateMode = WIFI_PHY_RATE_11M_S;
        Serial.println("WIFI_PHY_RATE_11M_S");
        break;
      case '8':
        wifiPhyRateMode = WIFI_PHY_RATE_6M;
        Serial.println("WIFI_PHY_RATE_6M");
        break;
      case '9':
        wifiPhyRateMode = WIFI_PHY_RATE_LORA_250K;
        Serial.println("WIFI_PHY_RATE_LORA_250K");
        break;
      case 'A':
        wifiPhyRateMode = WIFI_PHY_RATE_MCS0_LGI;
        Serial.println("WIFI_PHY_RATE_MCS0_LGI");
        break;
      case 'B':
        wifiPhyRateMode = WIFI_PHY_RATE_MCS0_SGI;
        Serial.println("WIFI_PHY_RATE_MCS0_SGI");
        break;
      // Add more cases for other PHY rate modes as needed
      default:
        Serial.println("Invalid command");
        break;
    }

    // Apply the desired PHY rate mode
    esp_wifi_config_espnow_rate(WIFI_IF_STA, wifiPhyRateMode);
    Serial.print("Changed PHY rate mode to: ");
    Serial.println(wifiPhyRateMode);
  }

  if (good_to_send) {
    myData_send.b[0] = n;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData_send, sizeof(myData_send));
    good_to_send = 0;
    lastSentTime = millis();
  }

  if (millis() - lastSentTime >= SEND_INTERVAL) {
    good_to_send = 1;
  }

  if (millis() - lastPrintTime >= 1000) {
    lastPrintTime = millis();
    Serial.println("SENT");
    Serial.println(" WifiPower = ");
    Serial.println(WiFi.getTxPower());
    sent_packets = 0;
  }
}
