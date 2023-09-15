#include <DFRobot_SHT3x.h>
DFRobot_SHT3x sht3x(&Wire,/*address=*/0x45,/*RST=*/4);

#define SENSOR_DATA_PIN   (7)   // Sensor PWM interface
#define INTERRUPT_NUMBER   (4)   // interrupt number

//// Used in interrupt, calculate pulse width variable
volatile unsigned long pwmHighStartTicks=0, pwmHighEndTicks=0;
volatile unsigned long pwmHighVal=0, pwmLowVal=0;
//// interrupt flag
volatile uint8_t flag=0;

void interruptChange()
{
  if (digitalRead(SENSOR_DATA_PIN)) {
    pwmHighStartTicks = micros();    // store the current micros() value
    if(2 == flag){
      flag = 4;
      if(pwmHighStartTicks > pwmHighEndTicks) {
        pwmLowVal = pwmHighStartTicks - pwmHighEndTicks;
      }
    }else{
      flag = 1;
    }
  } else {
    pwmHighEndTicks = micros();    // store the current micros() value
    if(1 == flag){
      flag = 2;
      if(pwmHighEndTicks > pwmHighStartTicks){
        pwmHighVal = pwmHighEndTicks - pwmHighStartTicks;
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
    //Initialize the chip to detect if it can communicate properly.
  while (sht3x.begin() != 0) {
    Serial.println("Failed to initialize the chip, please confirm the chip connection");
    delay(1000);
  }
  Serial.print("chip serial number: ");
  Serial.println(sht3x.readSerialNumber());
  if(!sht3x.softReset()){
    Serial.println("Failed to reset the chip");
  }
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_1Hz)){
    Serial.println("Failed to enter the periodic mode");
  }
  Serial.println("------------------Read data in cycle measurement mode-----------------------");
  delay(1000);
  Serial.println("co2 sensor beginning...");
  pinMode(SENSOR_DATA_PIN, INPUT);
  attachInterrupt(INTERRUPT_NUMBER, interruptChange, CHANGE);
}


void loop() {
  if(flag == 4){
    flag = 1;
    float pwmHighVal_ms = (pwmHighVal * 1000.0) / (pwmLowVal + pwmHighVal);

    if (pwmHighVal_ms < 0.01){
      Serial.println("Fault");
    }
    else if (pwmHighVal_ms < 80.00){
      Serial.println("preheating");
    }
    else if (pwmHighVal_ms < 998.00){
      float concentration = (pwmHighVal_ms - 2) * 5;
      // Print pwmHighVal_ms
//      Serial.print("pwmHighVal_ms:");
//      Serial.print(pwmHighVal_ms);
//      Serial.println("ms");
      //Print CO2 concentration
      Serial.print("C");
      Serial.println(concentration);
    }else{
//      Serial.println("Beyond the maximum range : 398~4980ppm");
    }
  }
  Serial.print("T");
  Serial.println(sht3x.getTemperatureC());
  Serial.print("H");
  Serial.println(sht3x.getHumidityRH());
  
  delay(2000); 
}
