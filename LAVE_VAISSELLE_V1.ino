//https://wokwi.com/projects/383400103927735297
#include <math.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>

//RTC_DS1307 RTC;

//LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

//const char* dayOfWeek[] = {
//  "Lundi ", "Mardi", "Mercredi", "Jeudi", "Vendredi ", "Samedi ", "Dimanche"
//};

//const char* currentMonth[] = {
//  "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"
//};
//boolean pm = false;

unsigned long cycleStart;
double tempArray[25];
byte arrayIndex = 0;
int K2000 = 0;

#define AfficheLogSerial True

#define SortieChauffeEau 13 //Viollet
#define MoteurLiberationDosette 12 //Rouge
//Blancs MoteurLiberationSel 11
#define MoteurLavageGeneral 10  //Jaune
#define MoteurPurgeEau 9 //Gris
#define MoteurArriveeEau 8 //Vert 
//Orange non utilisé

#define CapteurBoutonDemarrage 6
#define LED3 5
#define LED2 4
#define LED1 3

#define CapteurTemperature A2

//#define goLight 0
//#define stopLight 1
//#define stopButton A3
//#define tiltSensor A0

boolean prog_long = false;

double TemperatureEauMaintien() {
  //d'apres mes calculs pour temprature pour 60° il faut être a 25 (27 = 62° et 50 = 35°)

  int Temp = analogRead(CapteurTemperature);
  if (Temp >= 23) {
    digitalWrite(SortieChauffeEau, HIGH);
  }
  else {
    if (Temp <= 27)) {
      digitalWrite(SortieChauffeEau, LOW);
    }
  }
}

double TemperatureEau() {  // ça doit donner une estimation de la temperature
  int Temp = analogRead(CapteurTemperature);
  if (Temp <= 5) return 1000;  // remperature maximale
  if (Temp <= 10) return 90;
  if (Temp <= 15) return 80;
  if (Temp <= 20) return 70;
  if (Temp <= 25) return 60;
  if (Temp <= 40) return 50;
  if (Temp <= 55) return 40;
  if (Temp <= 70) return 30;
  if (Temp <= 85) return 20;
}

void setup () {  
  if (AfficheLogSerial == True) Serial.begin(9600); 
  if (AfficheLogSerial == True) Serial.println("Setup");
  pinMode(CapteurTemperature,   INPUT); 
  pinMode(SortieChauffeEau,    OUTPUT); digitalWrite (SortieChauffeEau, HIGH);    
  pinMode(MoteurLiberationDosette, OUTPUT); digitalWrite (MoteurLiberationDosette, HIGH); 
  pinMode(MoteurLavageGeneral,    OUTPUT); digitalWrite (MoteurLavageGeneral, HIGH);    
  pinMode(MoteurPurgeEau,     OUTPUT); digitalWrite (MoteurPurgeEau, HIGH);     
  pinMode(MoteurArriveeEau,   OUTPUT); digitalWrite (MoteurArriveeEau, HIGH);  

  pinMode(CapteurBoutonDemarrage,     INPUT_PULLUP);  
  pinMode(LED1,      OUTPUT); digitalWrite (LED1, LOW); 
  pinMode(LED2,      OUTPUT); digitalWrite (LED2, LOW); 
  pinMode(LED3,      OUTPUT); digitalWrite (LED3, LOW); 
  
  }

void afficheEtatLed(char* etat) {

 if (etat[0] == '1') digitalWrite (LED1, HIGH);
 else  digitalWrite (LED1, LOW);
 if (etat[1] == '1') digitalWrite (LED2, HIGH);
 else  digitalWrite (LED2, LOW);
 if (etat[2] == '1') digitalWrite (LED3, HIGH);
 else  digitalWrite (LED3, LOW);
}

void MetAjoutEtatAffichage(byte currentMode) {  //en attendant on se contente des 3 leds
// 001 Remplissage
// 010 Vidange
// 011 Lavage
// 100 Pre rincage
// 101 Rincage final
// 110 sechage vaisselle
// 111 Fin

  switch (currentMode) {
    case 1:
      //lcd.print(F("Remplissage "));
	  afficheEtatLed("001");
      break;
    case 2:
	  afficheEtatLed("010");
      //lcd.print(F("Vidange "));
      break;
    case 3:
	  afficheEtatLed("011");
      //lcd.print(F("Lavage "));
      break;
    case 4:
	  afficheEtatLed("100");
      //lcd.print(F("Pre rincage "));
      break;
    case 5:
	  afficheEtatLed("101");
      //lcd.print(F("Rincage final "));
      break;
    case 6:
	  afficheEtatLed("110");
      //lcd.print(F("sechage vaisselle "));
      break;
  }
}


void RemplissageEau() {   //remplissage de la machine
  if (AfficheLogSerial == True) Serial.println("RemplissageEau");

  unsigned long fillTime = millis();
  digitalWrite(MoteurArriveeEau, HIGH);
  //while ((millis() - fillTime) < 105000) {
  //  delay(100);
  MetAjoutEtatAffichage(1);
  //}
  delay(105000);
  digitalWrite(MoteurArriveeEau, LOW);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

void VidangeEau() {     //on vidange l'eau
  if (AfficheLogSerial == True) Serial.println("VidangeEau");
  unsigned long drainTime = millis();
  digitalWrite(MoteurPurgeEau, HIGH);
  delay(110000);
  //lcd.begin(20, 4);
  // lcd.clear();
  //while ((millis() - drainTime) < 105000) {
  //  delay(100);
  MetAjoutEtatAffichage(2);
  //}
  digitalWrite(MoteurPurgeEau, LOW);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

void DetectionPorteOuverte() {   // detection porte ouverte, inactif dans version de base
//  long doorOpened = millis();
//  while (digitalRead(tiltSensor)) {
//    if ((millis() - doorOpened) > 120000) {
//      lcd.setCursor(0, 3);
//      lcd.print(" Porte ouverte ");
//    }
//  }
}

void ModeSechage() {   // au lieu de ça il doit y avoir un autre systeme de sechage a voir
 if (AfficheLogSerial == True) Serial.println("ModeSechage");
 // digitalWrite(ventPin, LOW);
 //while (TemperatureEau() > 115) {
 //   delay(1000);
 MetAjoutEtatAffichage(6);
 // }
 // digitalWrite(ventPin, HIGH);
}


void PreRincage() {   // on fait simplement tourner le moteur de vidange
  if (AfficheLogSerial == True) Serial.println("PreRincage"); 
  unsigned long rinseTime = millis();
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(600000);
  //lcd.begin(20, 4);
  //lcd.clear();
//  while ((millis() - rinseTime) < 600000) {
  MetAjoutEtatAffichage(4);
//    delay(1000);
//  }
  digitalWrite(MoteurLavageGeneral, LOW);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

void RincageFinal() {  //on fait le rincage final
if (AfficheLogSerial == True) Serial.println("RincageFinal"); 
  unsigned long rinseTime = millis();
  digitalWrite(SortieChauffeEau, HIGH);
  digitalWrite(MoteurLavageGeneral, HIGH);
  digitalWrite(MoteurLiberationDosette, HIGH);
  delay(500);
  //lcd.begin(20, 4);
  //lcd.clear();
//  while ((millis() - rinseTime) < 30000) {
    MetAjoutEtatAffichage(5);
    delay(30000);
//  }
  digitalWrite(MoteurLiberationDosette, LOW);
  while (TemperatureEau() < 60) {
    delay(1000);
    //MetAjoutEtatAffichage(5);
  }
  digitalWrite(SortieChauffeEau, LOW);
  delay(5000);
  digitalWrite(MoteurLavageGeneral, LOW);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

void LavageDeLaVaisselle() {
   if (AfficheLogSerial == True) Serial.println("LavageDeLaVaisselle"); 
  digitalWrite(SortieChauffeEau, HIGH);
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(1500);
  //lcd.begin(20, 4);
  lcd.clear();
  while (TemperatureEau() < 48) {
    MetAjoutEtatAffichage(3);
	if (AfficheLogSerial == True) Serial.println("temp EAU:" & TemperatureEau()); 
    delay(1000);
  }
  unsigned long soapTime = millis();
  digitalWrite(MoteurLiberationDosette, HIGH);
  while ((millis() - soapTime) < 30000) {
    delay(500);
    MetAjoutEtatAffichage(3);
  }
  digitalWrite(MoteurLiberationDosette, LOW);
  while (TemperatureEau() < 68) {
	if (AfficheLogSerial == True) Serial.println("temp EAU:" & TemperatureEau()); 
    delay(1000);
    MetAjoutEtatAffichage(3);
  }
  if (prog_long == true) {
    LavageDeLaVaisselleMaintien(360000, 68);
  }
}

void LavageDeLaVaisselleMaintien(long temps, int temperatur) {
  digitalWrite(SortieChauffeEau, HIGH);
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(1500);
  //lcd.begin(20, 4);
  //lcd.clear();
  unsigned long maintien = millis();
  while ((millis() - maintien) <= temps) {
    MetAjoutEtatAffichage(3);
    TemperatureEauMaintien();                   //maintien la temperature a 60°
    if (AfficheLogSerial == True) Serial.println("temp EAU:" & TemperatureEau()); 
  }
  digitalWrite(SortieChauffeEau, LOW);
  delay(5000);
  digitalWrite(MoteurLavageGeneral, LOW);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

void loop() {
  // VidangeEau();
  //lcd.setCursor(0, 0);
  //lcd.print(F("Date: "));
  //digitalWrite(0, LOW);
  //digitalWrite(1, LOW);
  K2000 ++;
  if (K2000 > 2) K2000 = 0;
  if (K2000 == 0) MetAjoutEtatAffichage(4);
  if (K2000 == 1) MetAjoutEtatAffichage(2);
  if (K2000 == 2) MetAjoutEtatAffichage(1);
  delay(200);
    
  if (!digitalRead(CapteurBoutonDemarrage))  //si le bouton est appuyer on lance la machine
  {
    cycleStart = millis();
    VidangeEau();    //on vidange l'eau
    delay(1000);
    RemplissageEau();      //remplissage d'eau
    delay(1000);
    LavageDeLaVaisselle(); //on fait la vaisselle
    delay(1000);
    VidangeEau();   //on vidange l'eau
    delay(1000);
    RemplissageEau();     //remplissage d'eau
    delay(1000);
    PreRincage();     //on fait le pré rincage
    delay(1000);
    VidangeEau();   //on vidange l'eau
    delay(1000);
    RemplissageEau();     //remplissage d'eau
    delay(1000);
    RincageFinal();   //on fait le rincage final
    delay(1000);
    VidangeEau();   //on vidange l'eau
    delay(1000);
    ModeSechage();      //on seche à l'air chaud
  }
  //  lcd.clear();
  //  lcd.setCursor(0, 3);
  //  lcd.print(F("Vaisselle propre!"));
  //  digitalWrite(goLight, HIGH);             //extinction de la led "lavage en cours"
  //  digitalWrite(stopLight, LOW);            //allumage de la led " vaisselle propre"
  if (AfficheLogSerial == True) Serial.println("Fin Lavage"); 

  }
