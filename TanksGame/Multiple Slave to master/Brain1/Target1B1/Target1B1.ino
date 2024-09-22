/*+************* Includes *****************/
#include "target.h"

/*+*************** Globals ******************/
volatile float Voltage = 0U;
// Set your new MAC Address
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB1, 0x01};

uint8_t broadcastAddress[] = {0x42, 0xAE, 0xA4, 0x07, 0x0D, 0x01};

uint8_t targetLedAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xA1, 0x01};

/******* Structure to send data to the brain ********/
typedef struct struct_message 
{
  int id;       // must be unique for each sender board
} struct_message;


/* Variable to be sent to the Target LED 
 * value = 1 -> HIT (flash RED)
 * value = 2 -> TURN GREEN
 * value = 3 -> Turn Yellow
 * value = 4 -> Turn Red
 */
int functionVar = 0;

struct_message myData;

int healthFrmBrain = 0;

esp_now_peer_info_t peerInfo;

/*+*************** Function definations ******************/

/**
 * @brief OnDataSent: callback funcntion when data is sent via ESPNow
 * 
 * @param mac_addr: mac address of the receiver
 * @param status: Status of the sent message
 * 
 * @return void
 * 
*/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/**
 * @brief OnDataRecv: callback funcntion when data is received via ESPNow
 * 
 * @param mac: mac address of the sender
 * @param incomingData: pointer to the variable of incoming data from the sender
 * @param len: length of the received message
 * 
 * @return void
 * 
*/
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  esp_err_t transmission_status = ESP_FAIL; 

  memcpy(&healthFrmBrain, incomingData, sizeof(healthFrmBrain));
  Serial.print("health Received from the brain: ");
  Serial.println(healthFrmBrain);

  Serial.println("Sending the command to the Target.");

  /* Setting Neopixel depending on healthFrmBrain */
  if( (healthFrmBrain >= 70) and (healthFrmBrain <= 300) )
  {
    /* Setting neopixel to green */
    Serial.println("\n Sending Green color switching \n");
    functionVar = 2;
    transmission_status = esp_now_send(targetLedAddress, (uint8_t *) &functionVar, sizeof(functionVar));
  }
  else if ( (healthFrmBrain >= 40) and (healthFrmBrain <= 69) )
  {
    /* Setting neopixel to yellow */
    Serial.println("\n Sending yellow color switching \n");
    functionVar = 3;
    transmission_status = esp_now_send(targetLedAddress, (uint8_t *) &functionVar, sizeof(functionVar));
  }
  else if ( (healthFrmBrain >= 0) and (healthFrmBrain <= 39) )
  {
    /* Setting neopixel to Red */
    Serial.println("\n Sending yellow Red switching \n");
    functionVar = 4;
    transmission_status = esp_now_send(targetLedAddress, (uint8_t *) &functionVar, sizeof(functionVar));
  }

  if (transmission_status == ESP_OK) 
  {
    Serial.println("Score Sent to the Target LED");
    delay(500);
  }

  Serial.println();
  delay(10);
}


/*+*************** Initializations ******************/
void setup() 
{
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  /* Assigning a custom MAC address */
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  Serial.println(" ***** New mac address set ********");
  delay(100);

  /* Init ESP-NOW */
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);

  // pinMode(VOLT_PIN, INPUT);

  /* Turning Green LED ON for 2 seconds */
  //digitalWrite(GREEN_LED, 1);
 // delay(GREEN_ON_TIME_MS);
 // digitalWrite(GREEN_LED, 0);

  /* Register Datasend and Datarcv callback functions */
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  /* Register peer */
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

    /* Register peer */
  memcpy(peerInfo.peer_addr, targetLedAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("******* Initiating the Hit detection ********");
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}


void targetHitCallback()
{
  esp_err_t transmission_status = ESP_FAIL;

  Serial.println("***** TARGET HIT ****** ");

  /* Flashing the Red LED */
  functionVar = 1;
  transmission_status = esp_now_send(targetLedAddress, (uint8_t *) &functionVar, sizeof(functionVar));
  
  if (transmission_status == ESP_OK) 
  {
    Serial.println("***** HIT color indication sent ****** ");
  }

  delay(2500);

  /* Send message via ESP-NOW */
  Serial.println("*** Sending the Score now ****");
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) 
  {
    Serial.println("Score Sent to the Brain");
    Serial.println(" ");
    Serial.println("Awaiting Score results ");
  }
}

void loop() 
{
  // TARGET_ID 1 for target 1
  myData.id = TARGET_ID;

  /****** When target is hit ******/
  if(digitalRead(BUTTON_PIN) == 1) 
  {
    /* Callback function for HIT */
    targetHitCallback();
  }

  delay(10);
}