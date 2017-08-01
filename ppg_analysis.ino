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


char filenameA[] = "signal_16.txt";
char linesFile[] = "lines_16.txt";
char pulsesFile[] = "pulses_16.txt";
char d1_file[] = "d1_signal_16.txt";
char parametersFile[] = "parameters_16.csv";
char dataFile[] = "data_16.txt";
char filteredSignal[] = "filtered_16.txt";


char filenameB[] = "signal_16b.txt";
//----------------------------- 

#define FS 50.0   //Frequency (Hz)
#define LENGTH 1000 //Fs*recording_time (hz*sec)
#define CAP_CYCLES 1 //(Fs*recording_time)/lenght
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
const byte LED_PIN = 13;
const byte SENSOR_A_PIN = 8;
//-----------------------------

volatile int signalA_in;
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
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_A_PIN, INPUT);
  Serial.begin(115200);

  data_counter = 0;
  cap_flag = LOW;

  cTime = 0;
  write_flag = false;    //flag to write 
  ledFlag = LOW;
  process_flag = false;

  read_flag = true;

  IBI = 0;

  //removeFile(filenameA);
  //removeFile(filenameB);
  
  //interruptSetup();
  
  SDCardSetup();

  process_flag = true;
  

  
}

void loop() {
  
  /*if (write_flag == true){
    writeDataToFile(filenameA, data_b1, LENGTH);
    writeDataToFile(filenameB, data_b2, LENGTH);
    write_flag = false;
    Serial.println("File A");
    readFileToSerial(filenameA);
    Serial.println("File B");
    readFileToSerial(filenameB);
    process_flag = true;
  }*/

  if(process_flag == true){
    readFileToVector(filenameA, data_b1, LENGTH_2);
    moving_average(filteredSignal);
    process_signal();
    three_point_derivative_method();
    find_b_peaks();
    readFileToVector(filteredSignal, data_b2, LENGTH_2);
    compute_indexes();

    strcpy(filenameA, "signal_16b.txt");
    strcpy(linesFile, "lines_16b.txt");
    strcpy(pulsesFile, "pulses_16b.txt");
    strcpy(d1_file, "d1_signal_16b.txt");
    strcpy(parametersFile, "parameters_16b.csv");
    strcpy(dataFile, "data_16b.txt");
    strcpy(filteredSignal, "filtered_16b.txt");

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
  
  digitalWrite(LED_PIN, ledFlag);
  delay(10);
}
