#define SPEED 170

// Pinii de control pentru L298N
int enA = 9; // pinul 9
int in1 = 8; // pinul 8
int in2 = 7; // pinul 7

void setup() {

  // Setam toti pinii ca mod de operare = OUTPUT
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop() {
  // Prin setarea starii pinului 8 la HIGH si pinului 7 la LOW, motorul se va deplasa la dreapta
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, SPEED); // Viteza motorului

  delay(2000); // Asteapta 2 sec

  // Prin setarea starii ambilor pini la LOW, motorul se va opri
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0); // Setez viteza motorului la 0

  delay(2000); // Asteapta 2 sec

  // Prin setarea starii pinului 8 la LOW si pinului 7 la HIGH, motorul se va deplasa la stanga
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, SPEED); // Viteza motorului

  delay(2000); // Asteapta 2 sec
}
