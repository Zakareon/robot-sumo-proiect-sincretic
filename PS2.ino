// Semnal PWM
volatile int f_A=0;
volatile int f_B=0;
volatile int dc_A;
volatile int dc_B;

// Senzor HC-SR04
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

int etapa = 0;
int raza;
int curent = 0;
bool value;
int flag;

// Senzor HW-511
#define Sensor_HW 2

void init_timer()
{
  TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
  TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  //ICR1 = 0x3FFF;
}

void control_motoare()
{
  if(Serial.available() >= 7)
  {
    char input[7];
    Serial.readBytesUntil('\n',input,7);
    char* dc_A_str=strtok(input, "A ");
    char* dc_B_str=strtok(NULL, "B ");
    f_A = atoi(dc_A_str);
    f_B = atoi(dc_B_str);
    dc_A=(int)(255*f_A)/100;
    dc_B=(int)(255*f_B)/100;
    analogWrite(3,dc_A);
    analogWrite(5,dc_B);
  }
}

void senzor_HC_SR04()
{
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration = pulseIn(echoPin,HIGH);
  distance = duration*0.034/2;
}

void senzor_HW()
{
  value = digitalRead(Sensor_HW);
}

void setup() {
  Serial.begin(9600);
  init_timer();
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(Sensor_HW,INPUT);
}

void loop() {
  // Sensor HC-SR04
  if(etapa == 0) // etapa de initializare
  {
    while(millis() < 5000)
    {
      Serial.print("test");
      analogWrite(3,50);
      analogWrite(5,50);
    }
    //delay(5000);
    etapa = 2;
  }
  while(etapa == 1) // etapa de rotatie 
  {
    senzor_HC_SR04();
    analogWrite(3,50);
    analogWrite(5,0);
    /*if(distance < raza)
    {
      etapa = 2;
    }*/
  }
  while(etapa == 2) // etapa de impingere obiect
  {
    senzor_HW();
    analogWrite(3,100); 
    analogWrite(5,100);

    if(value == 0)
    {
      etapa = 3;
    }
  }
  while(etapa == 3) // etapa de intoarcere in centru
  {
    curent = millis();
    while(millis() - curent < 700)
    {
      analogWrite(3,50);
      analogWrite(5,0);
    }
    if(millis() - curent > 700)
    {
      flag = 1;
    }
    if(flag)
    {
      curent = millis();
      while(millis() - curent < 1000)
      {
        analogWrite(3,50); 
        analogWrite(5,50);
      }
      if(millis() - curent > 1000)
        etapa = 2;
    }
  }
}
