//signal at data_b3

int slope_signal(float slope){
  if (slope == 0){
    return 0;
  }else if (slope > 0){
    return 1;
  }else{
    return (-1);
  }
}

void split_line(char* r_line, int* amp, float* slope, int* peak, int pos){
  char* pch;
  pch = strtok (r_line,";");
  amp [pos] = atoi(pch);
  pch = strtok (NULL, ";");
  slope [pos] = atof(pch);
  pch = strtok (NULL, ";");
  peak [pos] = atoi(pch);
}


void process_signal(){
  int m = 2;

  char r_line[20];
  byte n;
  int auxCounter = 0;

  removeFile(linesFile);
  
  float thLow = 0; 
  float thHigh = 0;
  int lambda;
  int tCounter = 0;
  int zCounter = 0;
  
  int z_amp [3];
  float z_slope [3];
  int z_peak [3];
  
  int line_amp;
  float line_slope;
  int line_peak;
  
  int prev_amp;
  float prev_slope;
  int prev_peak;

  prev_slope = (float)(data_b3[m]-data_b3[0])/m;
  prev_amp = (data_b3[m]-data_b3[0]);
  prev_peak = m;
  tCounter += m;

  zCounter = 0;

  //-----------------------------------------------IMS Algorithm BEGIN
  while ((tCounter + m) < LENGTH_2){
    
    line_slope = (float)(data_b3[tCounter + m]-data_b3[tCounter])/m;
    line_amp = (data_b3[tCounter + m]-data_b3[tCounter]);
    line_peak = tCounter + m;

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
    }else{
      //_____________save prev line as z to file HERE
      writeLineToFile(linesFile, prev_amp, prev_slope, prev_peak);
      //Serial.println(line_slope);

      prev_slope = line_slope;
      prev_amp = line_amp;
      prev_peak = line_peak;

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
  split_line(r_line, z_amp, z_slope, z_peak, 0);
  if (z_amp[0] >= TH){
    thLow = z_amp[0]*LOWSLOW;
    thHigh = z_amp[0]*HIGHSLOW;
  }
  myFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, 1);
  myFile.fgets(r_line, sizeof(r_line));
  split_line(r_line, z_amp, z_slope, z_peak, 2);


  while ((n = (int) myFile.fgets(r_line, sizeof(r_line))) > 0){
    //Serial.println(r_line);
    split_line(r_line, &line_amp, &line_slope, &line_peak, 0);

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
  //-----------------------------------------------ARTIFACT Algorithm END

  //Serial.println(z_amp[0]);
  //Serial.println(z_slope[0]);
  //Serial.println(z_peak[0]);

  Serial.println(thLow);
  Serial.println(thHigh);
  //Serial.println(iCounter);
  //Serial.println(tCounter);
  Serial.println(zCounter);
}

