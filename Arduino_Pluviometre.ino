/*
  Station Meteo Pro - module pluviomètre déporté
  avec : 
     - Arduino Uno
     - pluviometre lextronic LEX
     - routeur TPLink nano
  
  Source :     https://www.sla99.fr
  Site météo : https://www.meteo-four38.fr
  Date : 2020-05-18
  
  Changelog : 
  18/05/2020  v1    version initiale
*/

#include <Ethernet.h>
#include <ArduinoHttpClient.h>

#define PLUVIOMETRE 2   //pin D2, interruption n°0

volatile unsigned int countPluviometre = 0; //variable pour l'interruption du pluviomètre pour compter les impulsions

//Gestion web server
byte mac[6] = { 0xBE, 0xEF, 0x00, 0xFD, 0xC7, 0x95 };
char macstr[18];

char server[] = "192.168.1.2";  
String KEY_WS="4566676544322"; 
EthernetClient client;          //client pour appeler le webservice sur le synology
HttpClient client2 = HttpClient(client, server, 81);

unsigned long previousMillis=   0;
unsigned long delaiProgramme =  60000L;   //60 sec

void setup()
{
  
  Serial.begin(9600);
  Serial.println("attente module wifi ...");
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(30000); //delai d'attente du module wifi
  Serial.println("démarrage ...");
  
  if(!Ethernet.begin(mac)){
    Serial.println("Network failed");
  }
  else{ 
    //Initialisation        
//    server.begin();    
    Serial.println(Ethernet.localIP());
    Serial.println(Ethernet.gatewayIP());
  }
  
  
  pinMode(PLUVIOMETRE, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(PLUVIOMETRE),interruptPluviometre,RISING) ;
}
/* 
 *  Fonction d'interruption du pluviomètre qui incrémente un compteur à chaque impulsion
 */
void interruptPluviometre(){
  countPluviometre++;
  Serial.print("1 impulsion ");
}

void loop(){  
  unsigned long currentMillis = millis(); // read time passed
  if (currentMillis - previousMillis > delaiProgramme){
    previousMillis = millis();
  
    String url = "/stationmeteo/pluvio.php?key="+KEY_WS+"&pluvio="+countPluviometre;
    client2.get(url);
    
    // read the status code and body of the response
    int statusCode = client2.responseStatusCode();
    String response = client2.responseBody();
    
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    client2.stop();
    //Si transmission OK, RAZ compteur pluvio
    if(statusCode == 200) countPluviometre = 0;
  }
}
    
  


