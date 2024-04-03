//https://wokwi.com/projects/383400103927735297
//#include <math.h>
//#include <LiquidCrystal.h>
//#include <Wire.h>
//#include <RTClib.h>

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
int Temp1;
int Temp2;
int Temp3;
int Temp4;
int Temp;



#define AfficheLogSerial true
#define DelaiSeconde 1000 //normalement 1000

#define MoteurArriveeEau 11 //Vert 
#define MoteurLiberationSel 12 //Blancs
#define MoteurPurgeEau 10 //Gris
#define SortieChauffeEau 9 //Viollet
#define MoteurLavageGeneral 8  //Jaune
#define MoteurLiberationDosette 7 //Rouge/marron   (non utiliser)
//Orange non utilisé

#define LED3 5
#define LED2 4
#define LED1 3

#define CapteurBoutonDemarrage A1
#define CapteurTemperature A2

//#define goLight 0
//#define stopLight 1
//#define stopButton A3
//#define tiltSensor A0

boolean prog_long = true;

double TemperatureEauMaintien() {
  //d'apres mes calculs pour temprature pour 60° il faut être a 25 (27 = 62° et 50 = 35°)

  int Temp = analogRead(CapteurTemperature);
  if (Temp >= 23) {
    digitalWrite(SortieChauffeEau, LOW);
  }
  else {
    if (Temp <= 27) {
      digitalWrite(SortieChauffeEau, HIGH);
    }
  }
}

double TemperatureEau() {  // ça doit donner une estimation de la temperature
  Temp1 = analogRead(CapteurTemperature);
  delay(100);
  Temp2 = analogRead(CapteurTemperature);
  delay(100);
  Temp3 = analogRead(CapteurTemperature);
  delay(100);
  Temp4 = analogRead(CapteurTemperature);
  Temp = (Temp1 + Temp2 + Temp3 + Temp4) / 4;


//  if (AfficheLogSerial == true) Serial.print("capteau eau:");
//  if (AfficheLogSerial == true) Serial.print(Temp);
  if (Temp <= 5) return 1000;  // remperature maximale
  if (Temp <= 10) return 90;
  if (Temp <= 15) return 80;
  if (Temp <= 20) return 70;
  if (Temp <= 25) return 60;
  if (Temp <= 40) return 50;
  if (Temp <= 55) return 40;
  if (Temp <= 70) return 30;
  if (Temp <= 85) return 20;
  if (Temp <= 250) return 10;
}

void setup () {  
  if (AfficheLogSerial == true) Serial.begin(9600); 
  if (AfficheLogSerial == true) Serial.println("start Setup");
  pinMode(CapteurTemperature,   INPUT); 
  
  pinMode(MoteurArriveeEau,   OUTPUT); digitalWrite (MoteurArriveeEau, HIGH);  
  pinMode(SortieChauffeEau,    OUTPUT); digitalWrite (SortieChauffeEau, HIGH);    
  pinMode(MoteurLiberationSel,   OUTPUT); digitalWrite (MoteurLiberationSel, HIGH);  
  pinMode(MoteurLavageGeneral,    OUTPUT); digitalWrite (MoteurLavageGeneral, HIGH);    
  pinMode(MoteurPurgeEau,     OUTPUT); digitalWrite (MoteurPurgeEau, HIGH);     
  pinMode(MoteurLiberationDosette, OUTPUT); digitalWrite (MoteurLiberationDosette, HIGH); 

  pinMode(CapteurBoutonDemarrage,     INPUT_PULLUP);  
  pinMode(LED1,      OUTPUT); digitalWrite (LED1, LOW); 
  pinMode(LED2,      OUTPUT); digitalWrite (LED2, LOW); 
  pinMode(LED3,      OUTPUT); digitalWrite (LED3, LOW); 
  
  }

void afficheEtatLed(int vLED1, int vLED2, int vLED3) {

 if (vLED1 == 1) digitalWrite (LED1, HIGH);
 else  digitalWrite (LED1, LOW);
 if (vLED2 == 1) digitalWrite (LED2, HIGH);
 else  digitalWrite (LED2, LOW);
 if (vLED3 == 1) digitalWrite (LED3, HIGH);
 else  digitalWrite (LED3, LOW);
}

//void afficheEtatLed(char* etat) {
//
// if (etat[0] == '1') digitalWrite (LED1, HIGH);
// else  digitalWrite (LED1, LOW);
// if (etat[1] == '1') digitalWrite (LED2, HIGH);
// else  digitalWrite (LED2, LOW);
// if (etat[2] == '1') digitalWrite (LED3, HIGH);
// else  digitalWrite (LED3, LOW);
//}

void MetAjoutEtatAffichage(byte currentMode) {  //en attendant on se contente des 3 leds
// 001 Remplissage
// 010 Vidange
// 011 Lavage
// 100 Pre rincage
// 101 Rincage final
// 110 sechage vaisselle
// 111 Fin
  ///Serial.println(currentMode);
  switch (currentMode) {
    case 1:
      //lcd.print(F("Remplissage "));
    afficheEtatLed(0,0,1);
      break;
    case 2:
    afficheEtatLed(0,1,0);
      //lcd.print(F("Vidange "));
      break;
    case 3:
    afficheEtatLed(0,1,1);
      //lcd.print(F("Lavage "));
      break;
    case 4:
    afficheEtatLed(1,0,0);
      //lcd.print(F("Pre rincage "));
      break;
    case 5:
    afficheEtatLed(1,0,1);
      //lcd.print(F("Rincage final "));
      break;
    case 6:
    afficheEtatLed(1,1,0);
      //lcd.print(F("sechage vaisselle "));
      break;
  }
}


void RemplissageEau() {   //remplissage de la machine
  if (AfficheLogSerial == true) Serial.print("RemplissageEau");

  digitalWrite(MoteurArriveeEau, LOW);
  MetAjoutEtatAffichage(1);
  for(int i=0; i<=50; i++) {
      if (AfficheLogSerial == true) Serial.print(".");
      delay(DelaiSeconde);
      delay(DelaiSeconde);
  }
  digitalWrite(MoteurArriveeEau, HIGH);
  delay(100);
  if (AfficheLogSerial == true) Serial.println(".");
}

void VidangeEau() {     //on vidange l'eau
  if (AfficheLogSerial == true) Serial.print("VidangeEau");
  digitalWrite(MoteurPurgeEau, LOW);

  for(int i=0; i<=55; i++) {
      if (AfficheLogSerial == true) Serial.print(".");
      delay(DelaiSeconde);
      delay(DelaiSeconde);
  }

  MetAjoutEtatAffichage(2);
  digitalWrite(MoteurPurgeEau, HIGH);
  delay(100);
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
 if (AfficheLogSerial == true) Serial.println("ModeSechage");
 // digitalWrite(ventPin, LOW);
 //while (TemperatureEau() > 115) {
 //   delay(DelaiSeconde);
 MetAjoutEtatAffichage(6);
 // }
 // digitalWrite(ventPin, HIGH);
}


void PreRincage() {   // on fait simplement tourner le moteur de vidange
  if (AfficheLogSerial == true) Serial.print("PreRincage"); 
  unsigned long rinseTime = millis();
  digitalWrite(MoteurLavageGeneral, LOW);
  //delay(600000);
  if (AfficheLogSerial == true) Serial.print("ça tourne pendant 60 s");
  for(int i=0; i<=600; i++) {
      delay(DelaiSeconde);
  }
  //lcd.begin(20, 4);
  //lcd.clear();
//  while ((millis() - rinseTime) < 600000) {
  MetAjoutEtatAffichage(4);
//    delay(DelaiSeconde);
//  }
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
  if (AfficheLogSerial == true) Serial.println(""); 
}

void RincageFinal() {  //on fait le rincage final
if (AfficheLogSerial == true) Serial.print("RincageFinal"); 
  unsigned long rinseTime = millis();
  digitalWrite(SortieChauffeEau, LOW);
  digitalWrite(MoteurLavageGeneral, LOW);
  digitalWrite(MoteurLiberationDosette, LOW);
  delay(500);
  //lcd.begin(20, 4);
  //lcd.clear();
//  while ((millis() - rinseTime) < 30000) {
    MetAjoutEtatAffichage(5);
   // delay(30000);
   if (AfficheLogSerial == true) Serial.println("rincage 30s"); 
   for(int i=0; i<=30; i++) {
      delay(DelaiSeconde);
  }
//  }
  digitalWrite(MoteurLiberationDosette, HIGH);
  if (AfficheLogSerial == true) Serial.println("rincage 40°"); 
  while (TemperatureEau() < 60) {
    delay(DelaiSeconde);
    //MetAjoutEtatAffichage(5);
  }
  digitalWrite(SortieChauffeEau, HIGH);
  delay(5000);
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(100);
  if (AfficheLogSerial == true) Serial.println(""); 
  //lcd.begin(20, 4);
  //lcd.clear();
}

void LavageDeLaVaisselle() {
   if (AfficheLogSerial == true) Serial.println("LavageDeLaVaisselle"); 
  digitalWrite(SortieChauffeEau, LOW);
   if (AfficheLogSerial == true) Serial.println("==> SortieChauffeEau ON"); 
  digitalWrite(MoteurLavageGeneral, LOW);
   if (AfficheLogSerial == true) Serial.println("==> MoteurLavageGeneral ON"); 
  delay(1500);
  //lcd.begin(20, 4);
  if (AfficheLogSerial == true) Serial.println("ATTENTE EAU A 48°"); 
  while (TemperatureEau() < 48) {
  //if (AfficheLogSerial == true) Serial.println(TemperatureEau()); 
    MetAjoutEtatAffichage(3);
    delay(DelaiSeconde);
  }
  if (AfficheLogSerial == true) Serial.println("EAU A 48°"); 
  unsigned long soapTime = millis();
  digitalWrite(MoteurLiberationDosette, LOW);
  if (AfficheLogSerial == true) Serial.println("MoteurDOSETTE"); 
  if (AfficheLogSerial == true) Serial.println("Attend 30 secondes"); 
  while ((millis() - soapTime) < 30000) {
    delay(500);
    MetAjoutEtatAffichage(3);
  }
  digitalWrite(MoteurLiberationDosette, HIGH);
  if (AfficheLogSerial == true) Serial.println("Attend eau a 68°"); 
  while (TemperatureEau() < 68) 
  {
    delay(DelaiSeconde);
  //if (AfficheLogSerial == true) Serial.println(TemperatureEau()); 
    MetAjoutEtatAffichage(3);
  }
  if (AfficheLogSerial == true) Serial.println("Fin eau a 68°"); 
  if (prog_long == true) {
    if (AfficheLogSerial == true) Serial.println("lancement programme long"); 
    LavageDeLaVaisselleMaintien(360000, 68);
  }
  digitalWrite(SortieChauffeEau, HIGH);
  digitalWrite(MoteurLavageGeneral, HIGH); 
}

void LavageDeLaVaisselleMaintien(long temps, int temperatur) {
  digitalWrite(SortieChauffeEau, LOW);
  digitalWrite(MoteurLavageGeneral, LOW);
  if (AfficheLogSerial == true) Serial.println("Delai 1500"); 
  delay(1500);
  //lcd.begin(20, 4);
  //lcd.clear();
  unsigned long maintien = millis();
  if (AfficheLogSerial == true) Serial.println("conserve 60° pendant longtemps"); 
  while ((millis() - maintien) <= temps) {
    MetAjoutEtatAffichage(3);
    TemperatureEauMaintien();                   //maintien la temperature a 60°
  delay(1000);
//    if (AfficheLogSerial == true) Serial.println(TemperatureEau()); 
  }
  if (AfficheLogSerial == true) Serial.println("fin conserve 60°"); 
  
  digitalWrite(SortieChauffeEau, HIGH);
  
  delay(5000);
  digitalWrite(MoteurLavageGeneral, HIGH);
  delay(100);
  //lcd.begin(20, 4);
  //lcd.clear();
}

//2/ l’eau de remplissage stockée dans une partie du remplisseur/répartiteur en vue de la régénération n’est admise dans le pot à sel qu’en fin de programme 
//  (à l’ouverture de l’électrovanne de régénération). L’eau chargée de sel passe sur la résine pour lui ôter le calcaire retenu et se retrouve ensuite directement dans le puits central de la cuve…
//3/ L’eau salée chargée du calcaire prélevé sur la résine est alors directement vidangée (et ne passe donc pas sur la vaisselle lavée lors des phases précédentes).
void RegenerationSel() {
  if (AfficheLogSerial == true) Serial.println("Debut Sel"); 
  digitalWrite(MoteurLiberationSel, LOW);
  delay(10000);
  digitalWrite(MoteurLiberationSel, HIGH);
  if (AfficheLogSerial == true) Serial.println("Fin Sel"); 
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
  delay(500);
    
  if (!digitalRead(CapteurBoutonDemarrage))  //si le bouton est appuyer on lance la machine
  {
    cycleStart = millis();
    MetAjoutEtatAffichage(1);
   VidangeEau();    //on vidange l'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
   delay(DelaiSeconde);
   RemplissageEau();      //remplissage d'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
   delay(DelaiSeconde);
   LavageDeLaVaisselle(); //on fait la vaisselle
   if (AfficheLogSerial == true) Serial.println("OK"); 
   delay(DelaiSeconde);
   VidangeEau();   //on vidange l'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
   delay(DelaiSeconde);
   RemplissageEau();     //remplissage d'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    PreRincage();     //on fait le pré rincage
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    VidangeEau();   //on vidange l'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    RemplissageEau();     //remplissage d'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    RincageFinal();   //on fait le rincage final
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    RegenerationSel();  // ici on fait passé de l'eau salé dans le filtre pour enlever calcaire
   if (AfficheLogSerial == true) Serial.println("OK"); 
    delay(DelaiSeconde);
    VidangeEau();   //on vidange l'eau
   if (AfficheLogSerial == true) Serial.println("OK"); 
    //delay(DelaiSeconde);
  //en chauffant l'eau a 70° ça fait secher la vaisselle
    delay(DelaiSeconde);
      //ModeSechage();      //on seche à l'air chaud
    if (AfficheLogSerial == true) Serial.println("Fin Lavage"); 
}
  //  lcd.clear();
  //  lcd.setCursor(0, 3);
  //  lcd.print(F("Vaisselle propre!"));
  //  digitalWrite(goLight, HIGH);             //extinction de la led "lavage en cours"
  //  digitalWrite(stopLight, LOW);            //allumage de la led " vaisselle propre"
  
  }
