/*+************* Includes *****************/
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <esp_wifi.h>
/*+*************** Macros ******************/
#define PIN                15
#define NUM_OF_LEDS        9
#define RED_LED            33
#define NUMBER_OF_BLINKS   3
#define GREEN_LED          35
#define GREEN_ON_TIME_MS   2000
#define RED_FLASH_TIME_MS  600
#define DELAY_HIT_FLASH_MS 2000 

/*+*************** Globals ******************/
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Set your new MAC Address
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xA1, 0x01};

uint8_t targetAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB1, 0x01};

int healthFrmBrain = 0;

esp_now_peer_info_t peerInfo;

/* Variable received from the Target */
int functionVar = 0;

/*+*************** Function definations ******************/

/**
 * @brief OnDataSent: callback funcntion when data is sent via ESPNow
 * There are some other pins like the pin number IO17 or soe
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
  memcpy(&functionVar, incomingData, sizeof(functionVar));
  Serial.println("Function variavle Received from the brain: " + functionVar); 
  /*
  * .......Switch case Logic.....
  * value = 1 -> HIT (flash RED)
  * value = 2 -> TURN GREEN
  * value = 3 -> Turn Yellow
  * value = 4 -> Turn Red
  */
  switch (functionVar) 
  {
    case 1:
      targetHitCallback();
      break;
    case 2:
      Serial.println("\nUpdated COLOR: Setting the LED to the Green\n");
      setneopixel(0, 255, 0);
      break;
    case 3:
      Serial.println("\nUpdated COLOR: Setting the LED to the Yellow\n");
      setneopixel(255, 255, 0);
      break;
    case 4:
      Serial.println("\nUpdated COLOR: Setting the LED to the RED\n");
      setneopixel(255, 0, 0);
      break;
    default:
      Serial.println("Something wrong received ");
      break;
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

  /* Register Datasend and Datarcv callback functions */
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  /* Register peer */
  memcpy(peerInfo.peer_addr, targetAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  
  pixels.begin();

  /* Turning ON GREEN */
  setneopixel(0, 255, 0);
  setneopixel(0, 255, 0);
}


void targetHitCallback()
{

  Serial.println("***** TARGET HIT ****** ");

  /* Flashing the Red LED */
  for (int i = 0; i < NUMBER_OF_BLINKS ; i++)
  {
    setneopixel(255, 0, 0);
    delay(RED_FLASH_TIME_MS);
    setneopixel(0, 0, 0);
    delay(RED_FLASH_TIME_MS);
  }

  /* Either wait for the new score to arrive or just turn the flash OFF 
   * and let the new score hit which changes the color automatically */

  setneopixel(0, 0, 0); /* Turning it OFF */
  setneopixel(0, 0, 0); 
}

void setneopixel(int r, int g, int b)
{
  for(int i = 0; i <= NUM_OF_LEDS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r,g,b));
    pixels.show();
  }
}

void loop() 
{
  delay(80);
}