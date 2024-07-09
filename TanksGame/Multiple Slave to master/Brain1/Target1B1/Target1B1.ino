/*+************* Includes *****************/
#include "target.h"

/*+*************** Globals ******************/
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM,PIN, NEO_GRB + NEO_KHZ800);
volatile float Voltage = 0U;
// Set your new MAC Address
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB2, 0x01};

uint8_t broadcastAddress[] = {0x42, 0xAE, 0xA4, 0x07, 0x0D, 0x02};

/******* Structure to send data to the brain ********/
typedef struct struct_message 
{
  int id;       // must be unique for each sender board
  int Score;    // Score to be sent
} struct_message;

struct_message myData;

int healthFrmBrain = 0;

esp_now_peer_info_t peerInfo;

/*+*************** Function definations ******************/

/**
 * @brief voltMeasure: Measures the voltage of the battery
 * 
 * @param None
 * 
 * @return double: measured voltages
*/
double voltMeasure()
{
  volatile int volt = analogRead(VOLT_PIN);// read the input
  volatile double voltage = map(volt,0, 2600, 0, 7.4);// map 0-1023 to 0-2500 and add correction offset
  return voltage + 1U;
}

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
  memcpy(&healthFrmBrain, incomingData, sizeof(healthFrmBrain));
  Serial.print("health Received from the brain: ");
  Serial.println(healthFrmBrain);

  /* Setting Neopixel depending on healthFrmBrain */
  if( (healthFrmBrain >= 70) and (healthFrmBrain <= 300) )
  {
    /* Setting neopixel to green, if score is between 70 and 100 */
    setneopixel(0, 255, 0);
  }
  else if ( (healthFrmBrain >= 40) and (healthFrmBrain <= 69) )
  {
    /* Setting neopixel to yellow, if score is between 40 and 69 */
    setneopixel(255, 255, 0);
  }
  else if ( (healthFrmBrain >= 0) and (healthFrmBrain <= 39) )
  {
    /* Setting neopixel to yellow, if score is between 40 and 69 */
    setneopixel(255, 0, 0);
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
  digitalWrite(GREEN_LED, 1);
  delay(GREEN_ON_TIME_MS);
  digitalWrite(GREEN_LED, 0);

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

  Serial.println("******* Setting the LED to GREEN ********");
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  
  pixels.begin();

  /* Turning ON GREEN */
  setneopixel(0, 255, 0);
}


void targetHitCallback()
{
  uint8_t sent_flag = 0;

  Serial.println("***** TARGET HIT ****** ");

  /* Send message via ESP-NOW */
  while(!sent_flag)
  {
    Serial.println("*** Sending the Score now ****");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) 
    {
      Serial.println("Score Sent to the Brain");
      Serial.println(" ");
      Serial.println("Awaiting Score results ");
      
      sent_flag = 1;
    }
    else 
    {
      Serial.println("Error sending the data");
      Serial.println("Trying to send the data again in 0.5 seconds");
      /*TODO: Add the support for getting the HIT count if someone hits, while the target is trying again to send */
      delay(500);
    }
  }
      
  /* Flashing the Red LED */
  for (int i = 0; i <= NUMBER_OF_BLINKS ; i++)
  {
    setneopixel(255, 0, 0);
    delay(300);
    setneopixel(0, 0, 0);
    delay(300);
  }
}

void setneopixel(int r, int g, int b)
{
  for(int i = 0; i <= NUM; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r,g,b));
    pixels.show();
  }
}

void loop() 
{
  // TARGET_ID 1 for target 1
  myData.id = TARGET_ID;
  myData.Score = MAX_SCORE;

  // Voltage = voltMeasure();

  // Serial.println("Voltage is: " + String(Voltage));

  /****** When target is hit ******/
  if(digitalRead(BUTTON_PIN) == 1) 
  {
    /* Callback function for HIT */
    targetHitCallback();
  }

  delay(10);
}