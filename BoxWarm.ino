#include <EncButton.h> // Encoder
#include <GyverSegment.h> // Display
#include <microDS18B20.h> // Thermocouple
#include <GyverPWM.h> // PWM
#include <GyverPID.h> // PI regulator



EncButton eb(2, 7, 8);
Disp1637Colon disp(4, 5);
MicroDS18B20<11> sensor;
GyverPID regulator(10.0, 0.5, 0.0, 2000);


int Themperature;
int DoWarmOld;
int DoWarmNew;
int Warm;
int sp;

long displaySetTempTime;


//There will be a logic with a lid
void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  pinMode(9, INPUT);
}


void loop() {
  
  eb.tick();
  disp.tick();
  sensor.requestTemp();


  //Turn
  if (eb.right()) DoWarmNew += 1;
  if (eb.left()) DoWarmNew -= 1;


  //Limited cycle
  if(DoWarmNew >= 80) DoWarmNew -= 60; 
  if(DoWarmNew <= 20) DoWarmNew += 60;

  if (DoWarmOld != DoWarmNew) {
    DoWarmOld = DoWarmNew;
    displaySetTempTime = millis();
  }

  if (millis() - displaySetTempTime < 1000) {
    //Display Encoder
    disp.setCursor(0);
    disp.print((int)DoWarmNew);
    disp.update();
  } else {
    //Display Themperature
    disp.setCursor(0);
    disp.print((int)Themperature);
    disp.update();
  }


  //Timer
  static uint32_t tmr1 = millis();
  if (millis() - tmr1 >= 2000){
    tmr1 = millis();

    //Get Temperature
    static uint32_t tmr2 = millis();
    if (millis() - tmr2 >= 150){
      tmr2 = millis();
      Themperature = sensor.getTemp();
      regulator.input = Themperature; //--> Themperatue now
    }
  }

  //Serial.println(Themperature);

  //Start the heating program by clicking
  if (eb.click()){
    sp = 1;
    Warm = DoWarmOld; //Fix
    regulator.setpoint = Warm; //--> PID Themperature maintenc
  }

 if (sp == 1) analogWrite(3, (int)regulator.getResultTimer());  // PWM --> Relay

}