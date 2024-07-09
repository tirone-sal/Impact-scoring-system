#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <esp_wifi.h>

/* LED's on each target would be connected to PIN 15 */
#define PIN               15
#define NUM               9
#define VOLT_PIN          (34U)
#define RED_LED           (33U)
#define BUTTON_PIN        23
#define NUMBER_OF_BLINKS  2

/**
 * @brief HIT Brief
 * If 
 * id = 1, front hit
 * id = 2, side hit
 * id = 3, back hit
 */
#define TARGET_ID           (1U)
#define MIN_SCORE     5
#define MAX_SCORE     10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM,PIN, NEO_GRB + NEO_KHZ800);
volatile float Voltage = 0U;
// Set your new MAC Address
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB2, 0x01};

uint8_t broadcastAddress[] = {0x42, 0xAE, 0xA4, 0x07, 0x0D, 0x02};

double voltMeasure()
{
  volatile int volt = analogRead(VOLT_PIN);// read the input
  volatile double voltage = map(volt,0, 2600, 0, 7.4);// map 0-1023 to 0-2500 and add correction offset
  return voltage + 1U;
}



/******* Structure to send data to the brain ********/
typedef struct struct_message {
    int id;       // must be unique for each sender board
    int Score;    // Score to be sent
} struct_message;

struct_message myData;

/************* Data to receive data from brain *************/
int health = 0U;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  memcpy(&health, incomingData, sizeof(health));
  Serial.print("Health Received from the brain: ");
  Serial.println(health);

  /****************** Setting Neopixel depending on HEALTH *******************/
  if( (health >= 70) and (health <= 300) )
  {
    /* Setting neopixel to green, if score is between 70 and 100 */
    setneopixel(0, 255, 0);
  }
  else if ( (health >= 40) and (health <= 69) )
  {
    /* Setting neopixel to yellow, if score is between 40 and 69 */
    setneopixel(255,255,0);
  }
  else if ( (health >= 0) and (health <= 39) )
  {
    /* Setting neopixel to yellow, if score is between 40 and 69 */
    setneopixel(255, 0, 0);
  }

  Serial.println();
  delay(50);
}

 
void setup() 
{
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  Serial.println(" ***** New mac address set ********");
  delay(500);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);

  pinMode(VOLT_PIN, INPUT);

  /**
   * Register Datasend and Datarcv callback functions 
   */
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
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
  pinMode(BUTTON_PIN,INPUT);
  
  pixels.begin();

  /* Turning ON GREEN */
  setneopixel(0, 255, 0);
}


void targetHitCallback()
{
  Serial.println("***** TARGET HIT ****** ");
  delay(100);

  for (int i = 0; i <= NUMBER_OF_BLINKS ; i++)
  {
    setneopixel(255, 0, 0);
    delay(600);
    setneopixel(0, 0, 0);
    delay(600);
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

  Voltage = voltMeasure();

  Serial.println("Voltage is: " + String(Voltage));

  /****** When target is hit ******/
  if(digitalRead(BUTTON_PIN) == 1) 
  {
    // Blinking RED LED
    targetHitCallback();

    // Send message via ESP-NOW
    Serial.println("*** Sending the Score now ****");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) 
    {
      Serial.println("Sent Score");
      Serial.println(" ");
      Serial.println("Awaiting Score results ");
    }
    else 
    {
      Serial.println("Error sending the data");
    }
  }

  delay(10);
}