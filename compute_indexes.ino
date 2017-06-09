void split_parameters(char* r_line, int* t_begin, int* a_peak, int* d_notch, int* b_peak, int pos){
  char* pch;
  pch = strtok (r_line,";");
  t_begin [pos] = atoi(pch);
  pch = strtok (NULL, ";");
  a_peak [pos] = atof(pch);
  pch = strtok (NULL, ";");
  d_notch [pos] = atoi(pch);
  pch = strtok (NULL, ";");
  b_peak [pos] = atoi(pch);
}


void compute_indexes(){
  int t_begin;
  int t_a_peak; 
  int d_notch;
  int t_b_peak;

  float prev_t_begin;
  float prev_a_peak;
  float prev_d_notch;
  float prev_b_peak;

  PPT = 0;
  int PPTcounter = 0;
  RI = 0;
  int RIcounter = 0;
  DELTAT = 0;
  int DELTATcounter = 0;
  CT = 0;
  int CTcounter = 0;
  RRT = 0;
  int RRTcounter = 0;
  DELTAP = 0;
  int DELTAPcounter = 0;

  if (!auxFile.open(parametersFile, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }

  byte n;
  char r_line[20];
  while ((n = (int) auxFile.fgets(r_line, sizeof(r_line))) > 0){
    split_parameters(r_line, &t_begin, &t_a_peak, &d_notch, &t_b_peak, 0);
    
    if(CT > 0){
      if ((t_a_peak - prev_a_peak) <= (IBI + 10)){
        PPT += (t_a_peak - prev_a_peak)/FS;
        PPTcounter++;

        RRT += (prev_a_peak - prev_t_begin)/(FS*(t_begin - prev_t_begin));
        RRTcounter++;

        DELTAP += (t_begin - prev_t_begin)/FS;
        DELTAPcounter++;
      }
    }

    if (t_b_peak != -1){
      DELTAT += (t_b_peak - t_a_peak)/FS;
      DELTATcounter++;

      RI += (data_b2[t_a_peak] - data_b2[t_b_peak]);
      RIcounter++;
    }

    CT += (t_a_peak - t_begin)/FS;
    CTcounter++;

    prev_t_begin = t_begin;
    prev_a_peak = t_a_peak;
    prev_d_notch = d_notch;
    prev_b_peak = t_b_peak;      
  }
  auxFile.close();

  PPT = PPT/PPTcounter;
  RI = RI/RIcounter;
  CT = CT/CTcounter;
  DELTAT = DELTAT/DELTATcounter;
  RRT = RRT/RRTcounter;
  DELTAP = DELTAP/DELTAPcounter;

  Serial.print(PPT);
  Serial.print("\t");
  Serial.print(RI);
  Serial.print("\t");
  Serial.print(CT);
  Serial.print("\t");
  Serial.print(DELTAT);
  Serial.print("\t");
  Serial.print(RRT);
  Serial.print("\t");
  Serial.println(DELTAP);
}