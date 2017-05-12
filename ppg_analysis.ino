#include <SPI.h>
#include "SdFat.h"

//----------------------------- signalProcessing
#define LENGTH_2 1000

#define LOWSLOW 0.8
#define HIGHSLOW 1.2
#define LOWFAST 0.6
#define HIGHFAST 1.4
#define TH 120

int data_b3[LENGTH_2];
char linesFile[] = "lines_3.txt";
char pulsesFile[] = "pulses_3.txt";
char d1_file[] = "d1_signal_3.txt";
char parametersFile[] = "parameters_3.txt";
//----------------------------- 

#define FS 50.0   //Frequency (Hz)
#define LENGTH 500 //Fs*recording_time (hz*sec)
#define CAP_CYCLES 2 //(Fs*recording_time)/lenght
const float VOLTAGE_STEP = 0.0048828125; //AD 10bits converter (0-5 volts)

//----------------------------- INDEXES
float PPT;
float RI;
float DELTAT;
float CT;
float RRT;
float DELTAP; 
//----------------------------- end INDEXES

//----------------------------- PIN's
const byte LED_PIN = 13;
const byte SENSOR_PIN = 8;
//-----------------------------

volatile int signal_in;
volatile int data_b1[LENGTH];      //rough signal input buffer 1
volatile int data_b2[LENGTH];      //rough signal input buffer 2
//volatile float v_data[LENGTH];    //voltage signal input 
volatile int data_counter = 0;
volatile byte cap_flag = LOW; //flag to capture
volatile boolean buffer_flag = true;  //flag to choose the buffer
volatile float temp;
volatile int cTime;// store current time
volatile byte cycleCounter; //capturing cycles counter
float IBI;

//-----------------------------SD Card
const int chipSelect = 4;
SdFat sd;
SdFile myFile;
SdFile auxFile;
//-----------------------------

volatile byte ledFlag = LOW;
volatile boolean write_flag;
boolean read_flag;

char filename[] = "signal_3.txt";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);

  data_counter = 0;
  buffer_flag = true;
  cap_flag = LOW;
  cycleCounter = 1;

  cTime = 0;
  write_flag = false;    //flag to write 
  ledFlag = LOW;

  read_flag = true;

  IBI = 0;

  //removeFile(filename);
  
  //interruptSetup();
  SDCardSetup();
  readFileToVector(filename, data_b3, LENGTH_2);
  process_signal();
  //three_point_derivative_method();
  //find_b_peaks();
}

void loop() {
  
//  Serial.print("Counter: ");
//  Serial.print(data_counter);
//  Serial.print("\tData: ");
//  Serial.println(temp);
  /*if (write_flag == true){
    if (buffer_flag){
      writeDataToFile(filename, data_b2, LENGTH);
    }else{
      writeDataToFile(filename, data_b1, LENGTH);
    }
    write_flag = false;
  }*/

  /*if (cap_flag && read_flag && !write_flag){
    readFileToSerial(filename);
    read_flag = false;
  }*/
  
  digitalWrite(LED_PIN, ledFlag);
  //delay(10);
}
