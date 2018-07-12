# ESP8266 Deauth All
<p align="center">
  <img alt="Librarian" src="https://github.com/the-red-team/ESP8266_Deauth_All/blob/master/images/IMG_2917.JPG" width="600">
  <br>
  <br>
  <b>Arduino code for your ESP8266. Deauthenticate all clients from all access points</b>
  <br>
  <br>
  <a href="https://www.instagram.com/the.red.team4">Instagram</a>
 - <a href="https://www.youtube.com/channel/UCQdi3MDHHMm7u3cMkEY329Q">YouTube</a>
 - <a href="https://208.68.37.54">Website</a>
  <br>
  <br>
  <b>Like my content? Show your support by subscribing to my channel and following my page.</b>
</p>

## Table of Condiments
  - [How it works](#how-it-works)
  - [Setup](#setup)
    - [Hardware](#hardware)
  - [Issues](#issues)
  - [Trash Code](#trash-code)

## How it works
  1. Scan all channels twice
    <p><img alt="Scan" src="https://github.com/the-red-team/ESP8266_Deauth_All/blob/master/images/de3.jpg" width="500"></p>  
  2. Clean out any APs from the last scan that did not show up in the current scan
  3. Deauthenticate all devices for one minute by sending broadcast deauth frames from all APs
    <p><img alt="Deauth" src="https://github.com/the-red-team/ESP8266_Deauth_All/blob/master/images/de.jpg" width="400"></p>  
  4. Repeat

## Setup
1. Install and open the [Arduino IDE](https://www.arduino.cc/en/Main/Software)
2. Add URLs to board manager URLs (File, Preferences, Additional Boards Manager URLs):  
   * `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   * `http://phpsecu.re/esp8266/package_deauther_index.json`
3. Install ESP8266 boards (Tools, Boards Manager):  
   * `arduino-esp8266-deauther`
   * `esp8266`
4. Copy and paste the [code](https://github.com/the-red-team/ESP8266_Deauth_All/blob/master/deauth_all.ino) into your Arduino IDE
5. Select the WeMos D1 Mini Board (Tools, Board, <b>ESP8266 Deauther Modules</b>)
6. Upload

## Hardware
  * [WEMOS D1 Mini](https://www.amazon.com/Makerfocus-NodeMcu-Development-ESP8266-Compatible/dp/B01N3P763C/ref=sr_1_3?ie=UTF8&qid=1531324588&sr=8-3&keywords=wemos+d1+mini)

## Issues
Some devices are able to ignore deauth frames sent to broadcast so this may not work against all devices in your area

## Trash Code
So this code is complete garbage, but it works.  
There are a lot of repeats which could have just been used in a function (Searching for known MACs)  
but I said, "Fuck it" and just made this.
