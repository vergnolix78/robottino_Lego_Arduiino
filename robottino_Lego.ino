#include <Ultrasonic.h>
#include <Thread.h>

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

// Riferimento della distanza rilevata
int distance = 0;
// Led
int led = 7;
//ultrasuoni
const int pinTrigger = 3;
const int pinEcho    = 4;

unsigned long time;
unsigned long lampeggio_time = 0;

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
  //trheadEvitaOstacoli.setInterval(10);
  //trheadEvitaOstacoli.ThreadName = "myThread";
  trheadEvitaOstacoli.onRun(ultraSuoni);

  trheadComandoLed.enabled = true;
  //trheadComandoLed.setInterval(10);
  //trheadComandoLed.ThreadName = "myThread";
  
  trheadComandoLed.onRun(blinkLed);
  
  // Inizializzo la seriale
  Serial.begin(9600);
  
}

void loop() 
{
  
  // Attivo i thread
  if(trheadEvitaOstacoli.shouldRun()){
    trheadEvitaOstacoli.run();
  }

  if(trheadComandoLed.shouldRun()){
     trheadComandoLed.run();
  } 
 
  //Vedo avanti
  distance = ultraSuoni();
  if(distance > 20){
    // I motori girano in avanti                         
    motorAvanti();
  }else{
    motorStop();
    delay(500);
    motorGira();
    delay(500);
    motorStop();
  }
  
  delay(100);
 
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
  
  Serial.print("distanza: ");
  // dopo 38ms è fuori dalla portata del sensore
  if( durata > 38000 ){
    Serial.println("Fuori portata");
  }
  else{ 
    Serial.print(distanza); 
    Serial.println(" cm");
  }
   
  // Aspetta 1000 microsecondi prima di generare l'impulso successivo
  delay(500);
  return distanza;
}
  // calcolo del tempo attraverso il pin di echo-
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
