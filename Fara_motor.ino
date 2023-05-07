#define MAX_VAL 4 // Valoarea maxima a contorului ce corespunde pozitiei de +180grade a panoului solar
#define PRAG 1.20f // Valoarea in modul a pragului impus pentru miscarea panoului
#define TIMESTAMP 1000 // Cuanta de timp in milisecunde

int sensorPin0 = A0; // Pinul ce va primi tensiunea de la fotodioda din stanga -> pinul A0
int sensorPin1 = A1; // Pinul ce va primi tensiunea de la fotodioda din dreapta -> pinul A1

int sensorValue0; // valoarea citita de pinul A0 <=> valoarea tensiunii prin fotorezistorul din stanga
int sensorValue1; // valoarea citita de pinul A1 <=> valoarea tensiunii prin fotorezistorul din dreapta

float voltage0; // valoarea tensiunii prin fotorezistorul din stanga, convertita in volti
float voltage1; // valoarea tensiunii prin fotorezistorul din dreapta, convertita in volti

float diff; // diferenta dintre tensiunea data de fotorez din stanga si cea data de fotorez din dreapta

int contor = MAX_VAL/2; // contorul ce va exprima pozitia de la 0 la 180 grade
                        // valoarea 0 va corespunde pozitiei de 0 grade
                        // valoarea MAX_VAL va corespunde pozitiei de 180 grade

void setup() {

  Serial.begin(9600); // initializam seriala cu baudrate-ul specific

}

void loop() {
  
  sensorValue0 = analogRead(sensorPin0); // citim tensiunea de pe fotorez din stanga
  sensorValue1 = analogRead(sensorPin1); // citim tensiunea de pe fotorez din dreapta

  voltage0 = sensorValue0 * (5.0/1023.0); // convertim valoarea citita (tens pe fotorez din stanga), in volti
  voltage1 = sensorValue1 * (5.0/1023.0); // convertim valoarea citita (tens pe fotorez din dreapta), in volti
  
  // Afisam tensiunile celor doua fotorez
  Serial.print("A0: ");
  Serial.print(voltage0);
  Serial.println("V");

  Serial.print("A1: ");
  Serial.print(voltage1);
  Serial.println("V");

  // Calculam diferenta intre tensiunea care cade pe fotorez din stanga si cea care cade pe fotorez din dreapta
  diff = voltage0 - voltage1;

  // Daca diferenta trece de un anumit prag pozitiv si pozitia panoului nu a ajuns la 180 grade
  if(diff >= PRAG && contor < MAX_VAL)
  {
    Serial.println("DREAPTA!"); // Miscam panoul catre dreapta
    contor++; // Crestem contorul (crestem gradele catre 180)
  }
  else if(diff <= -PRAG && contor > 0) // Daca diferenta trece de un anumit prag negativ si panoul nu este la 0 grade
  {
    Serial.println("STANGA!"); // Miscam panoul catre stanga
    contor--; // Scadem contorul (scadem gradele catre 0)
  }
  else // Daca diferenta este sub un anumit prag atat pe partea pozitiva cat si pe partea neg (in modul este mai mica decat pragul stabilit)
  {
    Serial.println("STOP!"); // Panoul nu se misca
  }

  delay(TIMESTAMP);
}
