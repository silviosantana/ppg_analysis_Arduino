#include <SPI.h>
#include "SdFat.h"

//----------------------------- signalProcessing
#define LENGTH_2 1000

#define LOWSLOW 0.8
#define HIGHSLOW 1.2
#define LOWFAST 0.6
#define HIGHFAST 1.4
#define TH 120

char filenameA[] = "signal_15.txt";
char linesFile[] = "lines_15.txt";
char pulsesFile[] = "pulses_15.txt";
char d1_file[] = "d1_signal_15.txt";
char parametersFile[] = "parameters_15.txt";
//----------------------------- 

#define FS 50.0   //Frequency (Hz)
#define LENGTH 1000 //Fs*recording_time (hz*sec)
#define CAP_CYCLES 1 //(Fs*recording_time)/lenght
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
const byte SENSOR_A_PIN = 8;
//-----------------------------

volatile int signalA_in;
volatile int data_b1[LENGTH];      //rough signal input buffer 1
int data_b2[LENGTH];      //rough signal input buffer 2
volatile int data_counter = 0;
volatile byte cap_flag = LOW;     //flag to capture
volatile boolean buffer_flag = true;  //flag to choose the buffer
volatile float temp;
volatile int cTime;// store current time
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


char filenameB[] = "signal_t2.txt";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_A_PIN, INPUT);
  Serial.begin(115200);

  data_counter = 0;
  cap_flag = LOW;

  cTime = 0;
  write_flag = false;    //flag to write 
  ledFlag = LOW;

  read_flag = true;

  IBI = 0;

  removeFile(filenameA);
  
  //interruptSetup();
  SDCardSetup();
  readFileToVector(filenameA, data_b2, LENGTH_2);
  process_signal();
  three_point_derivative_method();
  find_b_peaks();
  compute_indexes();
}

void loop() {
  
  /*if (write_flag == true){
    writeDataToFile(filenameA, data_b1, LENGTH);
    writeDataToFile(filenameB, data_b2, LENGTH);
    write_flag = false;
    readFileToSerial(filenameB);
  }*/

  /*if (cap_flag && read_flag && !write_flag){
    readFileToSerial(filename);
    read_flag = false;
  }*/
  
  digitalWrite(LED_PIN, ledFlag);
  //delay(10);
}
