#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <esp_wifi.h>
#include <RF24.h>
 
/* ID of this Brain */
#define ID          1
#define TANK_ID     1

#define VOLT_PIN    (34U)
#define RED_LED     (33U)
#define GREEN_LED    35

void SendNextionCommand(String object, String msg);

double volt_measure()
{
  volatile int volt = analogRead(VOLT_PIN);// read the input
  volatile double voltage = map(volt,0, 2600, 0, 7.4);// map 0-1023 to 0-2500 and add correction offset
  return voltage + 1U;
}

int GameEndFlag = 0U;
volatile float Voltage = 0U;

/*
* Last two members would be different
 
 For Slave Only change the last two members of the slaveMACAddress as follows:
	0xB1  (Brain Number) 
	0x01  (Slave Number i.e 0x01,0x02, 0x03....)
	
 For Brain Address only iterate the last member of the Brain as:
	0x01 for brain 1
	0x02 for brain 2
	and so on.............
*/

// MAC addresses of slave to which score is needed to be sent.
uint8_t slaveMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB1, 0x01};

// New mac address of Brain to communicate with salve via ESP NOW
uint8_t newMACAddress[] =  {0x42, 0xAE, 0xA4, 0x07, 0x0D, 0x01};

/**
 * @brief HIT Brief
 * If 
 * id = 1, front hit
 * id = 2, side hit
 * id = 3, back hit
 */

unsigned long StartTime = 0;
unsigned long TotalTime = 0;
unsigned long TimeLeft = 0;

RF24 radio(4, 5); 
const uint64_t address = 0xF0F0F0F0E1LL;
int counter = 0;

String TankName = "";
String TeamName = "";
 
/************* Structure to receive data from Admin *************/
struct StructureOfTeam
{
  String team_name;
  int health;
  unsigned char go = 0;
  unsigned char time = 0;
  unsigned char id = 0;
};

StructureOfTeam TeamData;
 
/************* Structure to send data to Admin *************/
struct StructureOfBrain
{
  unsigned char counter;
  unsigned char brain_id;
  unsigned char health;
  unsigned char tankID;
};

StructureOfBrain BrainData;

/************* Structure to receive data from Slave *************/
typedef struct StructureOfTargets 
{
  int id;
  int Score;
} StructureOfTargets;

/* Initial Score value */
int Final_Score = 100;

// Create a StructureOfTargets called rcvSlaveData
StructureOfTargets rcvSlaveData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent to the slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received from the slave
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) 
{
  // int rcvd_id;
  int rcvd_score;
  Serial.println("Score received from the admin");
  
  if(GameEndFlag == 0)
  {
    char macStr[18];
    Serial.print("Packet received from Target: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    Serial.println(macStr);

    memcpy(&rcvSlaveData, incomingData, sizeof(rcvSlaveData));
    Serial.printf("Target board ID %u: %u bytes\n", rcvSlaveData.id, len);

    // Update the structures with the new incoming data
    rcvd_score = rcvSlaveData.Score;
    Serial.printf("Score value Received: %d \n", rcvd_score);

    /* Check if new score value is less than zero or not */
    if( (Final_Score - rcvd_score ) >= 0 ) 
    {
      Final_Score = Final_Score - rcvd_score;
    }
    else
    {
      Final_Score = 0;
    }

    Serial.printf("Updated Score Value: %d \n", Final_Score);
    SendNextionCommand("health", String(Final_Score));
    SendNextionCommand("health", String(Final_Score));

    /********* Send Hit Status to the Nextion Display **********/
    switch (rcvSlaveData.id)
    {
    case 1:
      SendNextionCommand("t5", String("FRONT HIT"));
      SendNextionCommand("t5", String("FRONT HIT"));
      break;
    case 2:
      SendNextionCommand("t5", String("SIDE HIT"));
      SendNextionCommand("t5", String("SIDE HIT"));
      break;
    case 3:
      SendNextionCommand("t5", String("BACK HIT"));
      SendNextionCommand("t5", String("BACK HIT"));
      break;
    
    default:
      break;
    }

    
    /********** Sending data to the admin **************/

    BrainData.counter = counter;
    BrainData.health = Final_Score;
    BrainData.brain_id = 1;
    BrainData.tankID = TANK_ID;
    
    Serial.println(" ");
    Serial.print("Packet No. = ");
    Serial.println(BrainData.counter);
    
    Serial.print("Health = ");
    Serial.print(BrainData.health);
  
    Serial.print("Brain ID = ");
    Serial.print(BrainData.brain_id);
  
    Serial.println(" ");
    delay(100);
    
    radio.write(&BrainData, sizeof(StructureOfBrain));
    
    Serial.println("Data Packet Sent");

    Serial.println("");
    
    counter++;
      
    Serial.println();

    /************ Sending updated health to target **************/

    // Register peer
    memcpy(peerInfo.peer_addr, slaveMACAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
    }
    
    delay(50);
    
    // Send message via ESP-NOW
    Serial.println("*** Sending the Score now ****");
    esp_err_t result = esp_now_send(slaveMACAddress, (uint8_t *) &Final_Score, sizeof(Final_Score));
    
    if (result == ESP_OK) 
    {
      Serial.println("Sent Score to the slave");
    }
    else 
    {
      Serial.println("Error sending the data");
    }
  }

}
 
int recvData()
{
  if ( radio.available() ) 
  {
    radio.read(&TeamData, sizeof(StructureOfTeam));
    return 1;
  }

  return 0;
}

void setup() {
  
  Serial.begin(9600);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  Serial.println(" ***** New mac address set ********");
  delay(500);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  radio.begin();                  //Starting the Wireless communication
  Serial.println("Receiver Started....");

  radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //This sets the module as receiver

  // pinMode(GREEN_LED, OUTPUT);
  // digitalWrite(GREEN_LED, LOW);

  // pinMode(RED_LED, OUTPUT);
  // digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);

  /* Turning Green LED ON for a time */
  digitalWrite(GREEN_LED, 1);
  delay(2000);
  digitalWrite(GREEN_LED, 0);

  // pinMode(VOLT_PIN, INPUT);

  SendNextionCommand("start", String(" "));
  SendNextionCommand("start", String(" "));

}
 
void SendNextionCommand(String object, String msg)
{
  String command = "";
  command = object+".txt=\""+String(msg)+"\"";
  Serial.print(command);
  /**
   * Put a delauy here for the Nextion display and confirm the 
   * availability of the Nextion buffer.
  */

  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  delay(50); 
}

void loop() 
{
  // Voltage = volt_measure();

  // Serial.println("Voltage is: " + String(Voltage));

  // SendNextionCommand("t5",String(Voltage));
  // SendNextionCommand("t5", String(Voltage));

  // if(Voltage <= 3)
  // {
  //   while(1)
  //   {
  //     // Serial.println("*** ALERT: Voltage is down, voltage down ******");

  //     digitalWrite(RED_LED, HIGH);
  //     delay(500);
  //     digitalWrite(RED_LED, LOW);
  //     delay(500);
  //   }
  // }

  /* When admin is just sending the data 
  * but the game is yet to start */
  while(TeamData.go == 0)
  {
    if(recvData())
    {
      if( (TeamData.go == 0) and (TeamData.id == TANK_ID))
      {

        Serial.println("********** rcvd team tank: " + TeamData.team_name);
        Serial.print("Team Name = ");
        TeamName = TeamData.team_name.substring(0, TeamData.team_name.indexOf(" "));
        Serial.println(TeamName);

        Serial.print("Tank Name = ");
        TankName = TeamData.team_name.substring(TeamData.team_name.indexOf(" "), TeamData.team_name.length());
        Serial.println(TankName);
         
        Serial.print("Health Given = ");
        Serial.println(TeamData.health);

        /* Setting Final score equal to health */
        Final_Score = TeamData.health;

        Serial.print("GO = ");
        Serial.println(TeamData.go);

        Serial.print("Time in Minutes = ");
        TotalTime = TeamData.time * 60;
        Serial.println(TeamData.time);

        Serial.println();

        /* Updating data on Nextion HMI */
        SendNextionCommand("time", String(TeamData.time) + " : " + "00");
        SendNextionCommand("time", String(TeamData.time) + " : " + "00");
        SendNextionCommand("health", String(TeamData.health));
        SendNextionCommand("team", String(TeamName));
        SendNextionCommand("tank", String(TankName));
        SendNextionCommand("t5", String("Neutral"));
        SendNextionCommand("start", String(" "));
        SendNextionCommand("start", String(" "));

        // Register peer
        memcpy(peerInfo.peer_addr, slaveMACAddress, 6);
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
        
        // Add peer        
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
          Serial.println("Failed to add peer");
        }
        
        // Send message via ESP-NOW
        Serial.println("*** Sending the Score now ****");
        esp_err_t result = esp_now_send(slaveMACAddress, (uint8_t *) &Final_Score, sizeof(Final_Score));
        
        if (result == ESP_OK) 
        {
          Serial.println("Sent Score");
        }
        else 
        {
          Serial.println("Error sending the data");
        }
      }

      if((TeamData.go))
      {
        SendNextionCommand("start", String("3"));
        delay(1000);
        SendNextionCommand("start", String("2"));
        delay(1000);
        SendNextionCommand("start", String("1"));
        delay(1000);       

        SendNextionCommand("start", String("START"));
        delay(2000);
        
        SendNextionCommand("start", String(" "));
        delay(1000);          

        digitalWrite(GREEN_LED, HIGH);

        radio.openWritingPipe(address); //Setting the address where we will send the data
        radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
        radio.stopListening();          //This sets the module as transmitter

        StartTime = millis();
        Serial.println("Start Time : "+ String(StartTime));

        break;
      }
    }
  }


  if( ((millis() - StartTime) / 1000) <= TotalTime )
  {
    int minute = 0;
    int seconds = 0;
    String countdown = "";

    TimeLeft =  TotalTime - ((millis() - StartTime) / 1000);
    minute = (int) TimeLeft / 60;
    seconds = TimeLeft - (minute * 60);

    if(minute < 10)
    {
      countdown = "0" + String(minute) + " : "+ String(seconds);
    }
    if(seconds < 10)
    {
      countdown = String(minute) + " : 0"+ String(seconds);
    }
    
    
    Serial.println("");
    SendNextionCommand("time", String(countdown));
    SendNextionCommand("time", String(countdown));
    delay(100);
  }
  else
  {
    Serial.printf("\nTime left : %d", 0);

    SendNextionCommand("time", String(0)); 
    SendNextionCommand("time", String(0)); 
    SendNextionCommand("start", String("Game Ended")); 

    /* TODO: Add the ending of game logic here */
    GameEndFlag = 1;
    while(1);
  }

  delay(500);  
}
