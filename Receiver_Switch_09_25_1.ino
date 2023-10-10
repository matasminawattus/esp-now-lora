#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define INTS 62

int last_recv = 0;
unsigned long lastPrintTime;
int received_packets = 0;
int dropped_packets = 0;
wifi_phy_rate_t wifiPhyRateMode = WIFI_PHY_RATE_2M_S;

//uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0x52, 0xBC, 0xF4}; // Senders MAC Pair 1
uint8_t broadcastAddress[] = {0x8C, 0x4B, 0x14, 0x1A, 0xBF, 0xD0}; //su antena pair 1 receiver
//uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0x52, 0xBC, 0xF4}; // Receiver MAC Pair 1
//uint8_t broadcastAddress[] = {0x7c, 0xdf, 0xa1, 0x5b, 0x55, 0xe0}; // c3 su sma antenna

typedef struct struct_message {
    int b[INTS];
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  if (myData.b[0] == last_recv+1){
      received_packets += 1;
  }
  else{
      dropped_packets += (myData.b[0]-last_recv);
  }
  last_recv = myData.b[0];
}

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  esp_wifi_config_espnow_rate(WIFI_IF_STA, wifiPhyRateMode);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {  
  // Check for commands received via the serial COM port
  if (Serial.available() > 0) {
    char command = Serial.read();
    switch (command) {
      case '1':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_1M_L;
        Serial.println("WIFI_PHY_RATE_1M_L");
        break;
      case '2':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_2M_L;
        Serial.println("WIFI_PHY_RATE_2M_L");
        break;
      case '3':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_5M_L;
        Serial.println("WIFI_PHY_RATE_5M_L");
        break;
      case '4':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_11M_L;
        Serial.println("WIFI_PHY_RATE_11M_L");
        break;
      case '5':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_2M_S;
        Serial.println("WIFI_PHY_RATE_2M_S");
        break;
      case '6':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_5M_S;
        Serial.println("WIFI_PHY_RATE_5M_S");
        break;
      case '7':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_11M_S;
        Serial.println("WIFI_PHY_RATE_11M_S");
        break;
      case '8':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_6M;
        Serial.println("WIFI_PHY_RATE_6M");
        break;
      case '9':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_LORA_250K;
        Serial.println("WIFI_PHY_RATE_LORA_250K");
        break;
      case 'A':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_MCS0_LGI;
        Serial.println("WIFI_PHY_RATE_MCS0_LGI");
        break;
      case 'B':
        Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
        wifiPhyRateMode = WIFI_PHY_RATE_MCS0_SGI;
        Serial.println("WIFI_PHY_RATE_MCS0_SGI");
        break;
      // Add more cases for other PHY rate modes as needed
      default:
        Serial.println("Invalid command");
        break;
      }

    // Apply the desired PHY rate mode
    Serial.println(esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ));
    esp_wifi_config_espnow_rate(WIFI_IF_STA, wifiPhyRateMode);
    Serial.print("Changed PHY rate mode to: ");
    Serial.println(wifiPhyRateMode);
    }

    if (millis() - lastPrintTime >= 1000) {
      lastPrintTime = millis();
      Serial.print("RECV");
      Serial.println(received_packets);
      Serial.print("DROP");
      Serial.println(dropped_packets);
      received_packets = 0;
      dropped_packets = 0;
  }
}
