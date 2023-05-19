#include <LiquidCrystal.h>

#define MAX_VAL 20   // Valoarea maxima a contorului ce corespunde pozitiei de +90 grade a panoului solar
#define PRAG 1.20f   // Valoarea in modul a pragului impus pentru miscarea panoului
#define TIMESTAMP 20 // Cuanta de timp in milisecunde
#define SPEED 170    // Viteza motorului

int sensorPin0 = A0; // Pinul ce va primi tensiunea de la fotorezistorul din stanga -> pinul A0
int sensorPin1 = A1; // Pinul ce va primi tensiunea de la fotorezistorul din dreapta -> pinul A1

int sensorValue0; // valoarea citita de pinul A0 <=> valoarea tensiunii prin fotorezistorul din stanga
int sensorValue1; // valoarea citita de pinul A1 <=> valoarea tensiunii prin fotorezistorul din dreapta

float voltage0; // valoarea tensiunii prin fotorezistorul din stanga, convertita in volti
float voltage1; // valoarea tensiunii prin fotorezistorul din dreapta, convertita in volti

float diff; // diferenta dintre tensiunea data de fotorezistorul din stanga si cea data de fotorezistorul din dreapta

int contor = MAX_VAL / 2; // contorul ce va exprima pozitia de la -90 la +90 grade
                          // valoarea 0 va corespunde pozitiei de -90 grade
                          // valoarea MAX_VAL/2 va corespunde pozitiei de 0 grade (pozitia neutra)
                          // valoarea MAX_VAL va corespunde pozitiei de +90 grade

float prag_intuneric = 4.80f; // Valoarea tensiunii de pe fotorezistor ce semnifica faptul ca se afla intr-un loc intunecat

// Pinii de control pentru driver-ul motorului, L298N
int enA = 9; // pinul 9
int in1 = 8; // pinul 8
int in2 = 7; // pinul 7

// ! Atentie: Cu motorul luat cu partea neagra spre tine
// OUT2 din driver vine in dreapta motorului
// OUT1 din driver in stanga

int unit_grade = 180 / MAX_VAL; // Unitatea cu care se va adauga/scadea valoarea in grade in timpul miscarii
int grade = 0;                  // Initializam valoarea in grade la 0 (pozitia neutra)

// Initializarea LCD-ului impreuna cu pinii lui aferenti
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Legaturile sunt efectuate in modul urmator:
// VSS la masa si VDD la 5V

// RS la pinul 12 digital
// RW la masa
// EN la pinul 11 digital

// D4 la pinul 5 digital
// D5 la pinul 4 digital
// D6 la pinul 3 digital
// D7 la pinul 2 digital

void setup()
{

  Serial.begin(9600); // Initializam seriala cu bitrate-ul specific
  lcd.begin(16, 2);   // Initializam lcd-ul cu numarul de coloane si numarul de linii

  // Setam toti pinii de control al L298N ca mod de operare OUTPUT
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void miscare_panou_stanga()
{
  // Miscam panoul catre stanga:
  // Prin setarea starii pinului 8 la LOW si pinului 7 la HIGH, motorul se va deplasa la stanga
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
  // Prin setarea starii ambilor pini la LOW, motorul se va opri
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0); // Setez viteza motorului la 0
}

void Serial_print_unghi_pozitie()
{
  // Vom afisa pe seriala pozitia panoului in grade
  Serial.print("Unghi: ");
  Serial.print(grade);
  Serial.println(" grade");
}

void lcd_print_pozitie(int grade)
{
  // Afisam pe lcd pozitia panoului in grade

  lcd.clear(); // Resetam afisajul
  lcd.setCursor(0, 0); // Pe coloana 0, linia 0
  lcd.print("Pozitie:"); // Printam string-ul "Pozitie: "
  lcd.setCursor(0, 1); // Pe coloana 0, linia 1
  lcd.print(grade); // Printam variabila grade
  lcd.print(" grade"); // Dupa printam stringul " grade"
}

void loop()
{

  sensorValue0 = analogRead(sensorPin0); // citim tensiunea de pe fotorezistorul din stanga
  sensorValue1 = analogRead(sensorPin1); // citim tensiunea de pe fotorezistorul din dreapta

  voltage0 = sensorValue0 * (5.0 / 1023.0); // convertim valoarea citita (tens pe fotorezistorul din stanga), in volti
  voltage1 = sensorValue1 * (5.0 / 1023.0); // convertim valoarea citita (tens pe fotorezistorul din dreapta), in volti

  // Afisam tensiunile celor doua fotorezistoare
  Serial.print("A0: ");
  Serial.print(voltage0);
  Serial.println("V");

  Serial.print("A1: ");
  Serial.print(voltage1);
  Serial.println("V");

  // Calculam diferenta intre tensiunea care cade pe fotorezistorul din stanga si cea care cade pe fotorezistorul din dreapta
  diff = voltage0 - voltage1;

  // Daca diferenta trece de un anumit prag pozitiv
  if (diff >= PRAG)
  {
    if (contor < MAX_VAL) // Daca pozitia panoului nu a ajuns la +90 grade
    {
      Serial.println("DREAPTA!"); // Afisam string-ul "DREAPTA!" pe seriala
      miscare_panou_dreapta();

      contor++;                   // Crestem contorul
      grade = grade + unit_grade; // Crestem gradele catre +90
    }
    else // Daca pozitia panoului a ajuns la +90 grade
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
  }
  else if (diff <= -PRAG) // Daca diferenta trece de un anumit prag negativ
  {
    if (contor > 0) // Daca pozitia panoului nu a ajuns la -90 grade
    {
      Serial.println("STANGA!"); // Afisam string-ul "STANGA!" pe seriala
      miscare_panou_stanga();

      contor--;                   // Scadem contorul
      grade = grade - unit_grade; // Scadem gradele catre -90
    }
    else // Daca pozitia panoului a ajuns la -90 grade
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
  }
  else // Daca diferenta este sub un anumit prag atat pe partea pozitiva cat si pe partea neg (in modul este mai mica decat pragul stabilit)
  {
    // Daca cele doua tensiuni date de fotorezistoare au atins pragul de intuneric si nu se afla la pozitia neutra de 0 grade
    if (voltage0 >= prag_intuneric && voltage1 >= prag_intuneric && contor != MAX_VAL / 2)
    {
      // Aduc panoul la 0 grade
      if (contor > MAX_VAL / 2)
      {
        while (contor != MAX_VAL / 2)
        {
          Serial.println("STANGA!"); // Afisam string-ul "STANGA!" pe seriala

          miscare_panou_stanga();
          contor--;                   // Scadem contorul
          grade = grade - unit_grade; // Scadem gradele catre 0

          lcd_print_pozitie(grade);
          delay(TIMESTAMP); // Pentru a se pastra metrica timpului
        }
      }
      else if (contor < MAX_VAL / 2)
      {
        while (contor != MAX_VAL / 2)
        {
          Serial.println("DREAPTA!"); // Afisam string-ul "DREAPTA!" pe seriala

          miscare_panou_dreapta();
          contor++;                   // Crestem contorul
          grade = grade + unit_grade; // Crestem gradele catre 0

          lcd_print_pozitie(grade);
          delay(TIMESTAMP); // Pentru a se pastra metrica timpului
        }
      }
    }
    else
    {
      Serial.println("STOP!"); // Afisam string-ul "STOP!" pe seriala
      oprire_panou();
    }
  }

  Serial_print_unghi_pozitie();

  lcd_print_pozitie(grade);

  delay(TIMESTAMP);
}
