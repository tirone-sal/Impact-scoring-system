
/**
 * Brain.ino
 * 
 * This file contatins the important funcitons and declerations for the
 * brain module of the tanks game to work with.
 * 
 * We can add the other function implementations such as the webpage 
 * estimations as well as the other html tags in the file.
 * 
 * This will contain the next generation tags and scoreboards, which we are planning to implement
 * in the second version.
*/

/************** Includes *****************/
#include "Brain1.h"
#include "audio_module.h"

/************** Globals *****************/
Audio audio;
int GameEndFlag = 0U;
volatile float Voltage = 0U;
/*
* Last two members would be different
 
 For target Only change the last two members of the targetMACAddress as follows:
	0xB1  (Brain Number) 
	0x01  (target Number i.e 0x01,0x02, 0x03....)
	
 For Brain Address only iterate the last member of the Brain as:
	0x01 for brain 1
	0x02 for brain 2
	and so on.............
*/


uint8_t targetMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0xB1, 0x01}; // MAC addresses of target to which score is needed to be sent.
uint8_t newMACAddress[] =  {0x42, 0xAE, 0xA4, 0x07, 0x0D, 0x01}; // New mac address of Brain to communicate with salve via ESP NOW
unsigned long StartTime = 0;
unsigned long TotalTime = 0;
unsigned long TimeLeft = 0;
const uint64_t address = 0xF0F0F0F0E1LL;
int counter = 0;
uint8_t scoresToBeMinus[5] = {0};
uint8_t targetNum = 0;

String TankName = "";
String TeamName = "";
RF24 radio(4, 5); 
 
/************* Structure to receive data from Admin *************/
struct StructureOfTeam
{
  String team_name;
  int health;
  uint8_t go = 0;
  uint8_t time = 0;
  uint8_t id = 0;
  uint8_t target_num = 0;
  uint8_t targetScores[MAX_TARGETS] = {0};
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

/************* Structure to receive data from target *************/
typedef struct StructureOfTargets 
{
  int id;
} StructureOfTargets;

/* Initial Score value */
int Final_Score = 100;

// Create a StructureOfTargets called rcvTargetData
StructureOfTargets rcvTargetData;

// Create peer interface
esp_now_peer_info_t peerInfo;

double volt_measure()
{
  volatile int volt = analogRead(VOLT_PIN);// read the input
  volatile double voltage = map(volt,0, 2600, 0, 7.4);// map 0-1023 to 0-2500 and add correction offset
  return voltage + 1U;
}

// callback when data is sent to the target
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received from the target
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) 
{
  Serial.println("A target has been HIT");
  
  if(GameEndFlag == 0)
  {
    char macStr[18];
    Serial.print("Packet received from Target: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    Serial.println(macStr);

    memcpy(&rcvTargetData, incomingData, sizeof(rcvTargetData));
    Serial.printf("Target board ID %u: %u bytes\n", rcvTargetData.id, len);

    Serial.println("\n Sore to be minus from target 1 is : " + String(scoresToBeMinus[0]));

    /* Check if new score value is greater than zero or not */
    if( (Final_Score - scoresToBeMinus[0]) >= 0 ) 
    {
      Final_Score = Final_Score - scoresToBeMinus[0];
    }
    else
    {
      Final_Score = 0;
    }

    Serial.printf("Updated Score Value: %d \n", Final_Score);
    SendNextionCommand("health", String(Final_Score));
    SendNextionCommand("health", String(Final_Score));

    /* Send Hit Status to the Nextion Display */
    SendNextionCommand("t5", String("FRONT HIT"));
    SendNextionCommand("t5", String("FRONT HIT"));

    /* Sending data to the admin */

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
    
    while(1)
    {
      if(radio.write(&BrainData, sizeof(StructureOfBrain)))
      {
        Serial.println("Data Packet Sent");
        break;
      }
      else
      {
        Serial.println("Data packet is not sent trying again in half second ");
        delay(500);
      }
    }

    Serial.println("");
    
    counter++;
      
    Serial.println();

    /************ Sending updated health to target **************/

    // Register peer
    memcpy(peerInfo.peer_addr, targetMACAddress, 6);
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
    esp_err_t result = esp_now_send(targetMACAddress, (uint8_t *) &Final_Score, sizeof(Final_Score));
    
    if (result == ESP_OK) 
    {
      Serial.println("Sent Score to the target");
    }
    else 
    {
      Serial.println("Error sending the data");
    }
  }

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

  delay(20);
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

void setup() 
{
  
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

  pinMode(GREEN_LED, OUTPUT);

  /* Turning Green LED ON for a time */
  digitalWrite(GREEN_LED, 1);
  delay(2000);
  digitalWrite(GREEN_LED, 0);

  SendNextionCommand("start", String(" "));
  SendNextionCommand("start", String(" "));

  /************* Initializing the Audio module ******************/

  Serial.println("INIT:  Initializing the Audio");
  pinMode(SD_CS, OUTPUT);      
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  if(!SD.begin(SD_CS))
  {
    Serial.println("Error talking to SD card!");
    while(true);  // end program
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15); // 0...21
  // audio.connecttoFS(SD,"/Amaze.mp3");

}
 
void loop() 
{
  while(TeamData.go == 0)
  {
    if(recvData())
    {
      Serial.println("Data received");
      
      if( (TeamData.go == 0) and (TeamData.id == TANK_ID))
      {
        Serial.println("********** rcvd team tank: " + TeamData.team_name);
        Serial.print("Team Name = ");
        TeamName = TeamData.team_name.substring(0, TeamData.team_name.indexOf(" "));
        Serial.println(TeamName);

        Serial.println("Tank ID: " + String(TeamData.id));

        Serial.print("Tank Name = ");
        TankName = TeamData.team_name.substring(TeamData.team_name.indexOf(" "), TeamData.team_name.length());
        Serial.println(TankName);
         
        Serial.print("Health Given = ");
        Serial.println(TeamData.health);

        Serial.print("Number of Targets Received: ");
        targetNum = TeamData.target_num;
        Serial.println(targetNum);
        
        for (int target = 0; target < targetNum; target++)
        {
          scoresToBeMinus[target] = TeamData.targetScores[target];
          Serial.println("Target 1 Score: " + String(scoresToBeMinus[target]));
        }

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

        memcpy(peerInfo.peer_addr, targetMACAddress, 6); // Register peer
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
                
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
          Serial.println("Failed to add peer");
        }
        
        /* Sending the score to the targets */
        Serial.println("*** Sending the Score now ****");
        esp_err_t result = esp_now_send(targetMACAddress, (uint8_t *) &Final_Score, sizeof(Final_Score));
        
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
        radio.setAutoAck(true);
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
    while(1)
    {
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
      delay(500);
    }
  }

  delay(500);  
}