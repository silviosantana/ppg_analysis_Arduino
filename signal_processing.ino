//signal at data_b2

int slope_signal(float slope){
  if (slope == 0){
    return 0;
  }else if (slope > 0){
    return 1;
  }else{
    return (-1);
  }
}

void split_line(char* r_line, int* amp, float* slope, int* peak, int* begin, int pos){
  char* pch;
  pch = strtok (r_line,";");
  amp [pos] = atoi(pch);
  pch = strtok (NULL, ";");
  slope [pos] = atof(pch);
  pch = strtok (NULL, ";");
  peak [pos] = atoi(pch);
  pch = strtok (NULL, ";");
  begin [pos] = atoi(pch);
}


void process_signal(){
  int m = 2;

  char r_line[25];
  byte n;
  

  removeFile(linesFile);
  removeFile(pulsesFile);
  
  float thLow = 0; 
  float thHigh = 0;
  int lambda;
  int tCounter = 0;
  int zCounter = 0;
  int peakCounter = 0;
  
  int z_amp [3];
  float z_slope [3];
  int z_peak [3];
  int z_begin[3];
  
  int line_amp;
  float line_slope;
  int line_peak;
  int line_begin;
  
  int prev_amp;
  float prev_slope;
  int prev_peak;
  int prev_begin;

  prev_slope = (float)(data_b2[m]-data_b2[0])/m;
  prev_amp = (data_b2[m]-data_b2[0]);
  prev_peak = m;
  prev_begin = 0;
  tCounter += m;

  zCounter = 0;

  //-----------------------------------------------IMS Algorithm BEGIN
  while ((tCounter + m) < LENGTH_2){
    
    line_slope = (float)(data_b2[tCounter + m]-data_b2[tCounter])/m;
    line_amp = (data_b2[tCounter + m]-data_b2[tCounter]);
    line_peak = tCounter + m;
    line_begin = tCounter;

    //Serial.print(prev_slope);
    //Serial.print("\t");
    //Serial.print(line_slope);
    //Serial.print("\t");
    //Serial.println((slope_signal(line_slope) == slope_signal(prev_slope)));

    if (slope_signal(line_slope) == slope_signal(prev_slope)){
      //concatenate lines
      prev_slope = line_slope;
      prev_amp = prev_amp + line_amp;
      prev_peak = line_peak;
      //prev_begin still the same
    }else{
      //_____________save prev line as z to file HERE
      writeLineToFile(linesFile, prev_amp, prev_slope, prev_peak, prev_begin);
      //Serial.println(line_slope);

      prev_slope = line_slope;
      prev_amp = line_amp;
      prev_peak = line_peak;
      prev_begin = line_begin;

      zCounter ++;
    }
    tCounter += m;
  }
  //-----------------------------------------------IMS Algorithm END

  
  //-----------------------------------------------THRESHOLD Algorithm BEGIN
  if (!myFile.open(linesFile, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }

  myFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 0);
  if (z_amp[0] >= TH){
    thLow = z_amp[0]*LOWSLOW;
    thHigh = z_amp[0]*HIGHSLOW;
  }
  myFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 1);
  myFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 2);

  while ((n = (int) myFile.fgets(r_line, sizeof(r_line))) > 0){
    //Serial.println(r_line);
    split_line(r_line, &line_amp, &line_slope, &line_peak, &line_begin, 0);

    if (z_amp[1] >= TH){

      if (thLow == 0 && thHigh == 0){
        thLow = z_amp[1]*LOWSLOW;
        thHigh = z_amp[1]*HIGHSLOW;
      }else if (z_slope[1] > 0 && z_slope[0] != 0 && z_slope[2] != 0){
        if(z_amp[1] >= thLow && z_amp[1] <= thHigh){
          thLow = (thLow + ((float)z_amp[1])*LOWFAST)/2;
          thHigh = (thHigh + ((float)z_amp[1])*HIGHFAST)/2;
          lambda = 0;
        }else{
          if (lambda > 0){
            thLow = (thLow + ((float)z_amp[1])*LOWSLOW)/2;
            thHigh = (thHigh + ((float)z_amp[1])*HIGHSLOW)/2;
          }
          lambda++;
        }
      }
    }

    z_amp[0] = z_amp[1];
    z_amp[1] = z_amp[2];
    z_amp[2] = line_amp;
    z_slope[0] = z_slope[1];
    z_slope[1] = z_slope[2];
    z_slope[2] = line_slope;
    z_peak[0] = z_peak[1];
    z_peak[1] = z_peak[2];
    z_peak[2] = line_peak;

  }
  myFile.close();
  //-----------------------------------------------THRESHOLD Algorithm END


  //-----------------------------------------------ARTIFACT Algorithm BEGIN
  /*
    1. while for all the lines again
    2. compare to threshold high and low, and compare previous and next slopes
    3. if valid, save line to valid lines file
  */
  peakCounter = 0;
  if (!auxFile.open(linesFile, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }

  auxFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 0);
  auxFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 1);
  auxFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, z_begin, 2);

  int IBIcounter = 0;
  byte IBIflag = 0;
  IBI = 0;
  prev_peak = 0;

  if (z_amp[0] <= thHigh && z_amp[0] >= thLow && z_slope[1] != 0){
    writeLineToFile(pulsesFile, z_amp[0], z_slope[0], z_peak[0], z_begin[0]);
    peakCounter++;
    
    IBIflag = 1;
    prev_peak = z_peak[0];
  }


  while ((n = (int) auxFile.fgets(r_line, sizeof(r_line))) > 0){
    //Serial.println(r_line);
    split_line(r_line, &line_amp, &line_slope, &line_peak, &line_begin, 0);

    if (z_amp[1] <= thHigh && z_amp[1] >= thLow && z_slope[0] != 0 && z_slope[2] != 0){
      writeLineToFile(pulsesFile, z_amp[1], z_slope[1], z_peak[1], z_begin[1]);
      peakCounter++;

      if (IBIflag == 1){
        IBI += (z_peak[1] - prev_peak);
        IBIcounter++;
      }
      prev_peak = z_peak[1];
      IBIflag = 1;
      
    }else{
      if(z_amp[1] >= TH){
        IBIflag = 0;
      }
    }  

    z_amp[0] = z_amp[1];
    z_amp[1] = z_amp[2];
    z_amp[2] = line_amp;
    z_slope[0] = z_slope[1];
    z_slope[1] = z_slope[2];
    z_slope[2] = line_slope;
    z_peak[0] = z_peak[1];
    z_peak[1] = z_peak[2];
    z_peak[2] = line_peak;
    z_begin[0] = z_begin[1];
    z_begin[1] = z_begin[2];
    z_begin[2] = line_begin;

  }
  auxFile.close();

  if (z_amp[1] <= thHigh && z_amp[1] >= thLow && z_slope[0] != 0 && z_slope[2] != 0){
    writeLineToFile(pulsesFile, z_amp[1], z_slope[1], z_peak[1], z_begin[1]);
    peakCounter++;

    if (IBIflag == 1){
      IBI += (z_peak[1] - prev_peak);
      IBIcounter++;
    }
    prev_peak = z_peak[1];
    IBIflag = 1;

  }else{
    if(z_amp[1] >= TH){
      IBIflag = 0;
    }
  }

  if (z_amp[2] <= thHigh && z_amp[2] >= thLow && z_slope[1] != 0){
    writeLineToFile(pulsesFile, z_amp[2], z_slope[2], z_peak[2], z_begin[2]);
    peakCounter++;

    if (IBIflag == 1){
      IBI += (z_peak[2] - prev_peak);
      IBIcounter++;
    }
    IBIflag = 1;
  }

  IBI = IBI/IBIcounter;
  //Serial.println(IBIcounter);
  Serial.print("IBI: ");
  Serial.println(IBI);

  //-----------------------------------------------ARTIFACT Algorithm END

  //Serial.println(z_amp[0]);
  //Serial.println(z_slope[0]);
  //Serial.println(z_peak[0]);
  Serial.println(zCounter);
  Serial.println(thHigh);
  Serial.println(thLow);
  //Serial.println(iCounter);
  //Serial.println(tCounter);
  //Serial.println(peakCounter);

  removeFile(dataFile);

  if (!auxFile.open(dataFile, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }

  auxFile.print("IBI: ");
  auxFile.println(IBI);
  auxFile.print("thHigh: ");
  auxFile.println(thHigh);
  auxFile.print("thLow: ");
  auxFile.println(thLow);
  auxFile.print("#lines: ");
  auxFile.println(zCounter);

  auxFile.close();
}

void three_point_derivative_method(){
  float T = 1/FS;
  int d1_point;

  int LEN = 1000;

  int b = 0;
  int e = 2;

  removeFile(d1_file);

  if (!myFile.open(d1_file, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }

  while (e <= LEN){
    d1_point = (int) ((data_b2[e] - data_b2[b])/(2*T));
    //writeFloatToFile(d1_file, d1_point);

    myFile.println(d1_point);
    //Serial.println(d1_point);
    b++;
    e++;
  }
  myFile.close();

}

int findNextLocalMin(int b, int e){
  int result;
  while ((b + 1) <= e && data_b2[b + 1] < data_b2[b]){
    b++;
  }

  if (b >= e){
    result = -1;
  }else{
    result = b;
  }
  return result;
}

int findNextLocalMax(int b, int e){
  int result = 0;
  while ((b + 1) <= e && data_b2[b + 1] > data_b2[b]){
    b++;
  }

  if (b >= e){
    result = -1;
  }else{
    result = b;
  }
  return result;
}

int findNextZeroCrossing (int b, int e){ // e se b nao existe?
  int result = 0;
  while (b < e && data_b2[b] > 0){
    b++;
  }

  if (b >= e){
    result = -1;
  }else{
    result = b;
  }
  return result;
}

int findPrevZeroCrossing (int b, int e){
  int result = 0;
  while (b > 0 && data_b2[b] > 0){
    b--;
  }

  if (b <= 0){
    result = -1;
  }else{
    result = b;
  }
  return result;
}

int findIntervalMax (int b, int e, int limit){
  int max = data_b2[b];
  int result = b;
  int i = b + 1;

  while (i <= e && i <= limit){
    if (data_b2[i] >= max){
      max = data_b2[i];
      result = i;
    }

    i++;
  }

  if (result == e || result == limit){
    result = -1;
  }
  return result;
}


void find_b_peaks(){
  /*
  E SE B NAO EXISTIR?
  */

  int OFFSET = (int) (240/1000)*FS; //aparentemente muito grande

  int line_amp;
  float line_slope;
  int line_peak;
  int line_begin;

  int localMin;
  int localMax;
  int nextZero;
  int prevZero;

  removeFile(parametersFile);

  readFileToVector(d1_file, data_b2, LENGTH_2 - 2);

  if (!auxFile.open(pulsesFile, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }

  byte n;
  int interval_length = (int) (0.4*IBI); //40% of interbit interval
  Serial.println(interval_length);
  char r_line[25];
  while ((n = (int) auxFile.fgets(r_line, sizeof(r_line))) > 0){
    split_line(r_line, &line_amp, &line_slope, &line_peak, &line_begin, 0);

    //Serial.println(line_peak+1);
    localMin = findNextLocalMin(line_peak, LENGTH_2 -2);
    localMax = findIntervalMax(localMin, (localMin + interval_length),LENGTH_2 - 2);

    if (data_b2[localMax] > 0){
      nextZero = findNextZeroCrossing(localMax, LENGTH_2 - 2);
      prevZero = findPrevZeroCrossing(localMax, LENGTH_2 - 2);
    }else{
      nextZero = localMax;
      //prevZero = localMax - OFFSET;  //determinar offset correto
      //prevZero = localMax;
      prevZero = line_peak + ((int) (0.25*IBI));
    }

    //t_begin | t_a_peak | dicrotic notch | t_b_peak
    writeWaveParametersToFile(parametersFile, line_begin, line_peak, prevZero, nextZero);

    //Serial.print(line_begin + 1);
    //Serial.print("\t");
    //Serial.print(line_peak + 1);
    //Serial.print("\t");
    //Serial.print(prevZero + 1);
    //Serial.print("\t");
    //Serial.println(nextZero + 1);
    
  }
  auxFile.close();

}