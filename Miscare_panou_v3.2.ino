#include <LiquidCrystal.h>

#define MAX_VAL 20 // Valoarea maxima a contorului ce corespunde pozitiei de +180grade a panoului solar
#define PRAG 1.20f // Valoarea in modul a pragului impus pentru miscarea panoului
#define TIMESTAMP 20 // Cuanta de timp in milisecunde
#define SPEED 170 // Viteza motorului

int sensorPin0 = A0; // Pinul ce va primi tensiunea de la fotodioda din stanga -> pinul A0
int sensorPin1 = A1; // Pinul ce va primi tensiunea de la fotodioda din dreapta -> pinul A1

int sensorValue0; // valoarea citita de pinul A0 <=> valoarea tensiunii prin fotorezistorul din stanga
int sensorValue1; // valoarea citita de pinul A1 <=> valoarea tensiunii prin fotorezistorul din dreapta

float voltage0; // valoarea tensiunii prin fotorezistorul din stanga, convertita in volti
float voltage1; // valoarea tensiunii prin fotorezistorul din dreapta, convertita in volti

float diff; // diferenta dintre tensiunea data de fotorez din dreapta si cea data de fotorez din stanga

int contor = 0; // contorul ce va exprima pozitia de la 0 la 180 grade
                        // valoarea 0 va corespunde pozitiei de 0 grade
                        // valoarea MAX_VAL va corespunde pozitiei de 180 grade

int prag_stanga, prag_dreapta;

// Pinii de control pentru L298N
int enA = 9; // pinul 9
int in1 = 8; // pinul 8
int in2 = 7; // pinul 7

int unit_grade = 180/MAX_VAL;
int grade = 0;

LiquidCrystal lcd(12,11,5,4,3,2);
// Legaturile sunt efectuate in modul urmator:
// VSS la masa si VDD la 5V

// RS la pinul 12 digital
// RW la masa
// EN la pinul 11 digital

// D4 la pinul 5 digital
// D5 la pinul 4 digital
// D6 la pinul 3 digital
// D7 la pinul 2 digital

void setup() {

  Serial.begin(9600); // initializam seriala cu baudrate-ul specific
  lcd.begin(16,2);

  // Setam toti pinii de control al L298N ca mod de operare = OUTPUT
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void miscare_panou_stanga()
{
  // Miscam panoul catre stanga:
  // Prin setarea starii ambilor pini la LOW, motorul se va opri
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, SPEED); // Viteza motorului
}

void miscare_panou_dreapta()
{
  // Miscam panoul catre dreapta:
  // Prin setarea starii pinului 8 la HIGH si pinului 7 la LOW, motorul se va deplasa la dreapta
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, SPEED); // Viteza motorului
}

void oprire_panou()
{
  // Panoul se va opri:
  // Prin setarea starii pinului 8 la LOW si pinului 7 la HIGH, motorul se va deplasa la stanga
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0); // Setez viteza motorului la 0
}

void print_unghi_pozitie()
{
  Serial.print("Unghi: ");
  Serial.print(grade);
  Serial.println(" grade");
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
    if(voltage1 <= 1.80f) prag_dreapta = MAX_VAL;
    if(voltage1 <= 2.70f && voltage1 > 1.80f) prag_dreapta = MAX_VAL - (MAX_VAL/4);
    if(voltage1 <= 3.20f && voltage1 > 2.70f) prag_dreapta = MAX_VAL - (MAX_VAL/2);

    if(contor < prag_dreapta)
    {
      Serial.println("DREAPTA!"); // Afisam string-ul "DREAPTA!" pe seriala
      miscare_panou_dreapta();
      
      contor++; // Crestem contorul (crestem gradele catre 180)
      grade = grade + unit_grade;
    }
    else
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
    print_unghi_pozitie();
  }
  else if(diff <= -PRAG && contor > 0) // Daca diferenta trece de un anumit prag negativ si panoul nu este la 0 grade
  {
    if(voltage0 <= 1.80f) prag_stanga = 0;
    if(voltage0 <= 2.70f && voltage0 > 1.80f) prag_stanga = 0 + (MAX_VAL/4);
    if(voltage0 <= 3.20f && voltage0 > 2.70f) prag_stanga = 0 + (MAX_VAL/2);

    if(contor > prag_stanga)
    {
      Serial.println("STANGA!"); // Afisam string-ul "STANGA!" pe seriala
      miscare_panou_stanga();

      contor--; // Scadem contorul (scadem gradele catre 0)
      grade = grade - unit_grade;
    }
    else 
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
    print_unghi_pozitie();
  }
  else // Daca diferenta este sub un anumit prag atat pe partea pozitiva cat si pe partea neg (in modul este mai mica decat pragul stabilit)
  {
    if(contor > MAX_VAL/2)
    {
      Serial.println("STANGA!"); // Afisam string-ul "STANGA!" pe seriala
      miscare_panou_stanga();

      contor--; // Scadem contorul (scadem gradele catre 0)
      grade = grade - unit_grade;
    }
    else if(contor < MAX_VAL/2)
    {
      Serial.println("DREAPTA!"); // Afisam string-ul "DREAPTA!" pe seriala
      miscare_panou_dreapta();
      
      contor++; // Crestem contorul (crestem gradele catre 180)
      grade = grade + unit_grade;
    }
    else 
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
    print_unghi_pozitie();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pozitie:");
  lcd.setCursor(0, 1);
  lcd.print(grade);
  lcd.print(" grade");

  delay(TIMESTAMP);
}
