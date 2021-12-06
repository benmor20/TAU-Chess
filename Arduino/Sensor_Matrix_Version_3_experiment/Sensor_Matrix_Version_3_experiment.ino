#define A 11 //select mux 
#define B 10 //select mux 
#define C 9 //select mux
#define A1 13 //select mux 
#define B1 12 //select mux 
#define C1 2 //select mux

#define sensor_read A0 //select
int threshold=550;

int C_list[8]={0,0,0,0,1,1,1,1};
int B_list[8]={0,0,1,1,0,0,1,1};
int A_list[8]={0,1,0,1,0,1,0,1};

void setup() {
  //pinMode(ground_row, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(sensor_read, INPUT);
  
  Serial.begin(9600);
  Serial.println("Setup done.");
}

void loop(){
  for (int i=0; i<8; i++){
    digitalWrite(A1,A_list[i]);
    digitalWrite(B1,B_list[i]);
    digitalWrite(C1,C_list[i]);

    delay(10);
    scansensor();
    Serial.println();
  }
  Serial.println();
  delay(1000);
}

void scansensor(){
  for (int i = 0; i < 8; i++) {
    digitalWrite(A, A_list[i]);
    digitalWrite(B, B_list[i]);
    digitalWrite(C, C_list[i]);
    Serial.print(threshold>analogRead(sensor_read));
  }
}
