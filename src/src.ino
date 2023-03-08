#define PIN_NES_DO 7
#define PIN_NES_CK 6
#define PIN_NES_PS 5

#define PLAYER_STATE 0 // Must be 0 for player 1 and 1 for player 2

// Bit-mappings from the nes-controller to the required format (Basically which bits must be shifted to which position)
byte* maps = new byte[8] {0,1,7,6,2,3,4,5};

// Last read data from the nes-shift register. Used to check if a change occurred
short preNesReadin = 0;

// Performs a read from the nes-shift-register
short readNesRegisterIn() {
  // Data to read in
  short x = 0;

  // Requests all data from the latch
  digitalWrite(PIN_NES_PS,HIGH);
  digitalWrite(PIN_NES_PS,LOW);

  // Iterates over all bits and reads them in
  for(int i=0;i<8;i++){
    // Also uses the predefined mappings here
    x |= (!digitalRead(PIN_NES_DO)) << maps[i];
    digitalWrite(PIN_NES_CK,HIGH);
    digitalWrite(PIN_NES_CK,LOW);
  }

  return x;
}

// Event: When new data is detected, the data also already includes the player-id
void onNewDataDetected(short data){
  // Outputs the received data
  Serial.write(data);
  Serial.write(data>>8);
  // Outputs an xor error check
  Serial.write(data ^ (data >> 8));
  Serial.flush();
}

void setup() {
  
  pinMode(PIN_NES_DO,INPUT);
  pinMode(PIN_NES_CK,OUTPUT);
  pinMode(PIN_NES_PS,OUTPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // Reads in the next data
  short data = readNesRegisterIn();

  // Checks if something changed
  if(data == preNesReadin){
    delay(5);
    return;
  }
  preNesReadin = data;

  // Sends the event
  onNewDataDetected(data<<1 | PLAYER_STATE);
  
  // Waits
  delay(5);
}
