#include <Ultrasonic.h>
#include <Thread.h>
#include <SoftwareSerial.h>

// Bluethoot
SoftwareSerial BTSerial (8,11);

//L293D
//Motor A
const int motorPin1  = 5;  
const int motorPin2  = 6;  
//Motor B
const int motorPin3  = 10; 
const int motorPin4  = 9;  

// Velocita motori
int velocita = 200;

// Creo i thread
Thread trheadEvitaOstacoli = Thread();
Thread trheadComandoLed = Thread();
Thread trheadSound = Thread();

// Riferimento della distanza rilevata
int distance = 0;
// Led
int led = 7;
//ultrasuoni
const int pinTrigger = 3;
const int pinEcho    = 4;

unsigned long time;
unsigned long lampeggio_time = 0;
//bottone
int btn = 12;
int currentState;  
int lastState = LOW;   
int active = 0;
//buzzer
byte buzzerPin = 2;
int buzzerRitardo = 100;      

int x;


void setup() 
{
  pinMode(led, OUTPUT);

  //Set pin motori
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  
  // Configuro i thread
  trheadEvitaOstacoli.enabled = true;
  trheadEvitaOstacoli.onRun(ultraSuoni);

  trheadComandoLed.enabled = true;
  trheadComandoLed.onRun(blinkLed);

  trheadSound.enabled = true;
  trheadSound.onRun(sound);

  // Inizializzo la seriale
  Serial.begin(9600);
  //bottone
  pinMode(btn, INPUT_PULLUP);
  //buzzer
  pinMode(buzzerPin, OUTPUT);
  //bt
  BTSerial.begin(9600);
  
}

void loop() 
{
  if(BTSerial.available()){
    x = BTSerial.read();
    Serial.write(char(x));
    if(char(x)=='a'){
      BTSerial.write("Robot Acceso");
      active=1;
      
    }
    if(char(x)=='b'){
      BTSerial.write("Robot Spento");
      active=0;
      motorStop();
    }
  }

  //Attivo un bottone per lo spegnimento e l'azionamento
  currentState = digitalRead(btn);
  if(lastState == HIGH && currentState == LOW){
    if(active==0){
      active=1;
    }else{
      active=0;
      motorStop();
    }
  }
  
  if (active==1){
    //Attivo i thread
    //Attivo ultrasuono
    if(trheadEvitaOstacoli.shouldRun()){
      trheadEvitaOstacoli.run();
    }
    //Attivo un led
    if(trheadComandoLed.shouldRun()){
      trheadComandoLed.run();
    } 
    //Attivo un suono
    if(trheadSound.shouldRun()){
      trheadSound.run();
  
    }
    
    //Vedo avanti
    distance = ultraSuoni();
    if(distance > 20){
      
      // I motori girano in avanti                         
      motorAvanti();
    }
    
    if(active==0){
      
      motorStop();
    }

    if(distance <= 20){
      
      motorStop();
      delay(500);
      motorGira();
      delay(500);
      motorStop();
    }
  
  }
 
  lastState = currentState;
  delay(500);  
}

int ultraSuoni(){
  // imposta l'uscita del trigger LOW
  digitalWrite(pinTrigger, LOW);
  // imposta l'uscita del trigger HIGH per 10 microsecondi
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
  
  // calcolo del tempo attraverso il pin di echo
  long durata = pulseIn(pinEcho, HIGH);
  long distanza = durata/58.31;
  delay(100);
  return distanza;
}
  
// Accendo e spengo un led
void blinkLed(){
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);                
}

void motorAvanti(){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void motorStop(){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

void motorGira(){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void sound(){
    digitalWrite(buzzerPin,HIGH);
    delay(buzzerRitardo);
    digitalWrite(buzzerPin,LOW);
    delay(buzzerRitardo);    
}

