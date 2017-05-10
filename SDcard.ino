void SDCardSetup(){     
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
    sd.initErrorHalt();
  }
} 

void removeFile(char* fname){
  sd.remove(fname);
}

void writeDataToFile(char* fname, volatile int* data, int len){
  // open the file for write at end like the Native SD library
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
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
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println(fname);

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
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println(fname);
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

void writeLineToFile(char* fname, int amp, float slope, int tPeak){
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }

  myFile.print(amp);
  myFile.print(";");
  myFile.print(slope);
  myFile.print(";");
  myFile.println(tPeak);

  myFile.close();
}
