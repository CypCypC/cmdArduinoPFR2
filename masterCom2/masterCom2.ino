#include <AFMotor.h>
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

char data;
float speed = 255;

const int echoPinFront = A0;
const int echoPinLeft = A2;
const int echoPinRight = A3;
const int trigPinFront = A1;
float durationFront, durationLeft, durationRight;

struct DistanceMeasurements {
  float front;
  float gauche;
  float droite;
};


void serialEvent1() {
  while (Serial1.available()) {
    char btData = Serial1.read();
    data = btData;
    Serial.println("btdata : ");
    Serial.println(data);
    cmd();
  }
}

void serialEvent2() {
  while (Serial2.available()) {
    char piData = Serial2.read();
    data = piData;
    Serial.println("piData : ");
    Serial.println(data);
    cmd();
  }
}

DistanceMeasurements measureDistances() {
  digitalWrite(trigPinFront, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinFront, LOW);
//faire trois mesures prend trop de temps mais c'est la c'est prêt
  durationFront = pulseIn(echoPinFront, HIGH);
  //durationLeft = pulseIn(echoPinLeft, HIGH);
  //durationRight = pulseIn(echoPinRight, HIGH);

  float distanceF = (durationFront * 0.0343) / 2;
  //float distanceL = (durationLeft * 0.0343) / 2;
  //float distanceR = (durationRight * 0.0343) / 2;
  float distanceL = 0;
  float distanceR = 0;

  Serial.println("avant : ");
  Serial.println(distanceF);
  Serial.println("gauche : ");
  Serial.println(distanceL);
  Serial.println("droite : ");
  Serial.println(distanceR);
  DistanceMeasurements measurements = { distanceF, distanceL, distanceR };
  return measurements;
}

int obstacle = 0;
int avancer = 0;
int verrouGauche = 0;
int verrouDroite = 0;
DistanceMeasurements measurements = measureDistances();

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);

  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(echoPinRight, INPUT);

  attachInterrupt(digitalPinToInterrupt(19), serialEvent1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(17), serialEvent2, CHANGE);
}

void loop() {
  capteurUTS();
  Serial.println("test");
}

void cmd() {
  switch (data) {
    case 'F':
      if (obstacle != 1) {
        avancer = 1;
        speed = 200;
        forward();
      } else {
        Serial.println("obstacle devant");
        avancer = 0;
      }
      break;
    case 'B':
      speed = 200;
      avancer = 0;
      backward();
      break;
    case 'L':
      avancer = 0;
      speed = 255;
      left();
      break;
    case 'R':
      avancer = 0;
      speed = 255;
      right();
      break;
    case 'S':
      stop();
      break;
    case '+':
      if (speed <= 200) {
        speed += 20;
        setMotorSpeed(speed);
      }
      break;
    case '-':
      if (speed >= 80) {
        speed -= 20;
        setMotorSpeed(speed);
      }
      break;
    case 'V':
      speed = 255;
      setMotorSpeed(speed);
      break;
    case 'M':
      speed = 177.5;
      setMotorSpeed(speed);
      break;
    default:
      stop();
      break;
  }
}

void capteurUTS() {
  measurements = measureDistances();
  if (measurements.front < 30 && measurements.front > 1) {
    obstacle = 1;
    // Serial.println("avant");
    if (avancer == 1) {
      stop();
    }
  } else {
    obstacle = 0;
  }
  // if (measurements.gauche < 30 && measurements.gauche > 1) {
  //   verrouGauche = 1;
  //   // Serial.println("gauche");
  // } else {
  //   verrouGauche = 0;
  // }
  // if (measurements.droite < 30 && measurements.droite > 1) {
  //   verrouDroite = 1;
  //   // Serial.println("droite");
  // } else {
  //   verrouDroite = 0;
  // }
}

// Fonction pour arrêter tous les moteurs
void stop() {
  setMotorSpeed(0);
  motor1.run(BRAKE);
  motor2.run(BRAKE);
  motor3.run(BRAKE);
  motor4.run(BRAKE);
}

// Fonction pour définir la vitesse de tous les moteurs
void setMotorSpeed(float spd) {
  motor1.setSpeed(spd);
  motor2.setSpeed(spd);
  motor3.setSpeed(spd);
  motor4.setSpeed(spd);
}

// Fonctions pour contrôler les mouvements du robot
void backward() {
  setMotorSpeed(speed);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void forward() {
  setMotorSpeed(speed);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void right() {
  setMotorSpeed(speed);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void left() {
  setMotorSpeed(speed);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}
