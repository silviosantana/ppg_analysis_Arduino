#include <SPI.h>
#include "SdFat.h"
#include <string.h>

//----------------------------- signalProcessing
#define LENGTH_2 1000

#define LOWSLOW 0.8
#define HIGHSLOW 1.2
#define LOWFAST 0.6
#define HIGHFAST 1.4
#define TH 30


char filenameA[] = "signal_19.txt";
char linesFile[] = "lines_19.txt";
char pulsesFile[] = "pulses_19.txt";
char d1_file[] = "d1_signal_19.txt";
char parametersFile[] = "parameters_19.csv";
char dataFile[] = "data_19.txt";
char filteredSignal[] = "filtered_19.txt";


char filenameB[] = "signal_20.txt";
//----------------------------- 

#define FS 50.0   //Frequency (Hz)
#define LENGTH 1000 //Fs*recording_time (hz*sec)
#define CAP_CYCLES 1 //(Fs*recording_time)/lenght
#define CLIP_TIME 0.25
//const float VOLTAGE_STEP = 0.0048828125; //AD 10bits converter (0-5 volts)

//----------------------------- INDEXES
float PPT;
float RI;
float DELTAT;
float CT;
float RRT;
float DELTAP; 
float AS;
float AR;
//----------------------------- end INDEXES

//----------------------------- PIN's
//const byte LED_PIN = 13;
const byte SENSOR_A_PIN = 0;
const byte SENSOR_B_PIN = 1;
//int SENSOR_A_ENABLE_IR = 1;
int SENSOR_A_ENABLE_V = 7;
int SENSOR_B_ENABLE_IR = 3;
int SENSOR_B_ENABLE_V = 2;
const byte START_PIN = 5;
//-----------------------------

volatile int signalA_in;
volatile int signalB_in;
int data_b1[LENGTH];      //rough signal input buffer 1
int data_b2[LENGTH];      //rough signal input buffer 2
volatile int data_counter = 0;
volatile byte cap_flag = LOW;     //flag to capture
volatile boolean buffer_flag = true;  //flag to choose the buffer
//volatile float temp;
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
boolean process_flag;
boolean read_flag;



void setup() {
  //pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_A_PIN, INPUT);
  pinMode(SENSOR_B_PIN, INPUT);

  //pinMode(SENSOR_A_ENABLE_IR, OUTPUT);
  pinMode(SENSOR_A_ENABLE_V, OUTPUT);
  pinMode(SENSOR_B_ENABLE_IR, OUTPUT);
  pinMode(SENSOR_B_ENABLE_V, OUTPUT);

  pinMode(START_PIN, INPUT);

  Serial.begin(115200);

  data_counter = 0;
  cap_flag = LOW;

  cTime = 0;
  write_flag = false;    //flag to write 
  ledFlag = LOW;
  process_flag = false;

  read_flag = true;

  IBI = 0;

  SDCardSetup();

  removeFile(filenameA);
  removeFile(filenameB);
  
  interruptSetup();

  //process_flag = true;
}

void loop() {

  //digitalWrite(SENSOR_A_ENABLE_IR, HIGH);
  digitalWrite(SENSOR_B_ENABLE_IR, HIGH);
  digitalWrite(SENSOR_A_ENABLE_V, LOW);
  digitalWrite(SENSOR_B_ENABLE_V, LOW);

  if(digitalRead(START_PIN)){
    cTime = 600;
    Serial.println("START_PIN");
  }
  
  if (write_flag == true){
    writeDataToFile(filenameA, data_b1, LENGTH);
    writeDataToFile(filenameB, data_b2, LENGTH);
    write_flag = false;
    
    /*Serial.println("File A");
    readFileToSerial(filenameA);
    Serial.println("File B");
    readFileToSerial(filenameB);*/
    process_flag = true;
  }

  if(process_flag == true){
    readFileToVector(filenameA, data_b1, LENGTH_2);
    moving_average(filteredSignal);
    process_signal();
    three_point_derivative_method();
    find_b_peaks();
    readFileToVector(filteredSignal, data_b2, LENGTH_2);
    compute_indexes();

    strcpy(filenameA, "signal_20.txt");
    strcpy(linesFile, "lines_20.txt");
    strcpy(pulsesFile, "pulses_20.txt");
    strcpy(d1_file, "d1_signal_20.txt");
    strcpy(parametersFile, "parameters_20.csv");
    strcpy(dataFile, "data_20.txt");
    strcpy(filteredSignal, "filtered_20.txt");

    readFileToVector(filenameA, data_b1, LENGTH_2);
    moving_average(filteredSignal);
    process_signal();
    three_point_derivative_method();
    find_b_peaks();
    readFileToVector(filteredSignal, data_b2, LENGTH_2);
    compute_indexes();

    process_flag = false;
  }

  /*if (cap_flag && read_flag && !write_flag){
    readFileToSerial(filename);
    read_flag = false;
  }*/
  
  //digitalWrite(LED_PIN, ledFlag);
  delay(10);
}
