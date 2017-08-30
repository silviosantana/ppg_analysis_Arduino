void SDCardSetup(){     
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
    sd.initErrorHalt();
  }
  //Serial.println("SD Settup done!");
} 

void removeFile(char* fname){
  sd.remove(fname);
}

void writeDataToFile(char* fname, volatile int* data, int len){
  // open the file for write at end like the Native SD library
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("file error: ");
    Serial.println("1");
  }

  // if the file opened okay, write to it:
  //Serial.print("Writing to file ");
  for (int i = 0; i < len; i++){
    myFile.println(data[i]);
  }
  
  // close the file:
  myFile.close();
  //Serial.println("Writing done.");
}

void readFileToSerial(char* fname){
  // re-open the file for reading:
  if (!myFile.open(fname, O_READ)) {
    sd.errorHalt("file error: ");
    Serial.println("2");
  }
  //Serial.println(fname);

  // read from the file until there's nothing else in it:
  int readData;
  while ((readData = myFile.read()) >= 0) {
    Serial.write(readData);
  }
  // close the file:
  myFile.close();
}

void readFileToVector(char* fname, int* data, int len){
  // re-open the file for reading:
  if (!myFile.open(fname, O_READ)) {
    sd.errorHalt("file error: ");
    Serial.println("3");
  }
  //Serial.println(fname);
  // read from the file until there's nothing else in it:
  int readData;
  int count = 0;

  char line[8];
  int n;
  while (((n = myFile.fgets(line, sizeof(line))) > 0) && count < len) {
    data[count] = atoi(line);
    count++;
  }
  // close the file:
  myFile.close();
}

void writeLineToFile(char* fname, int amp, float slope, int tPeak, int tBegin){
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("file error: ");
    Serial.println("4");
  }

  myFile.print(amp);
  myFile.print(";");
  myFile.print(slope);
  myFile.print(";");
  myFile.print(tPeak);
  myFile.print(";");
  myFile.println(tBegin);


  myFile.close();
}

void writeWaveParametersToFile(char* fname, int bg, int a, int dn, int b){
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("file error: ");
    Serial.println("5");
  }

  myFile.print(bg);
  myFile.print(",");
  myFile.print(a);
  myFile.print(",");
  myFile.print(dn);
  myFile.print(",");
  myFile.println(b);

  myFile.close();
}

void writeIndexesToFile(char* fname, float PPT, float RI, float CT, float DELTAT, float RRT, float DELTAP, float AS, float AR){
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("file error: ");
    Serial.println("6");
  }

  myFile.print("PPT: ");
  myFile.println(PPT);
  myFile.print("RI: ");
  myFile.println(RI);
  myFile.print("CT: ");
  myFile.println(CT);
  myFile.print("DELTAT: ");
  myFile.println(DELTAT);
  myFile.print("RRT: ");
  myFile.println(RRT);
  myFile.print("DELTAP: ");
  myFile.println(DELTAP);
  myFile.print("AS: ");
  myFile.println(AS);
  myFile.print("AR: ");
  myFile.println(AR);


  myFile.close();
}
