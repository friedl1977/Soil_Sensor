/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/friedl/Desktop/Projects/Soil_Sensor/src/Soil_Sensor.ino"
void setup();
void setup();
int water_type(String command);
int sun_type(String command);
void soil();
void U_V();
void Error_states ();
void publish ();
void loop();
#line 1 "/Users/friedl/Desktop/Projects/Soil_Sensor/src/Soil_Sensor.ino"
#define BLYNK_TEMPLATE_ID "TMPL4doYO7xzH"
#define BLYNK_TEMPLATE_NAME "Smart Pot"
#define BLYNK_AUTH_TOKEN "Nmjfq45ZMWASwdD6IXEQMx1x0xj4hYvC"

const char *WEBHOOK_NAME = "Blynk_Send";          // Particle Webhook name

#include "DEV_Config.h"
#include "LTR390.h"

unsigned long lastTime = 0;

// Soil sensor declarations // 

//int water_type = 0;
int soil_pin = A5;                                //  Analog pin for sensor reading
int val = 0;
int ave_soil_val;                                 //  Determine ave reading from sample readings
int soil_percentage;                              //  Map values
int number_attempts = 10;                         //  Number of sample readings.  Higher number of samples will provide more accurate results.
int soil_level = 0;
int new_soil_level = 0;

int min_raw_value = 1450;                         //  Adjust these with the values you determined       
int max_raw_value = 2850;                         //  Adjust these with the values you determined

int water_upper;
int water_lower;
int H2O_Error = 0;
char data[192];

// UV sensor declarations // 

//int sun_type = 0;
UDOUBLE UV,ALS;
int sun_light_level = 0;
int new_sun_light_level = 0;
int sun_upper;
int sun_lower;
int UV_Error = 0;


// RGB LED declarations // 
int red_pin   = D7; 
int green_pin = D6;
int blue_pin  = D5;

int value;

class SmartPot {
  public:

    SmartPot() {
    }

    void setup() {

      Particle.function("Water", water_type);
      Particle.function("Sunlight", sun_type);
      
      }
};

SmartPot mySmartPot;

void setup() {

  mySmartPot.setup();
  //value = 300;

  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  pinMode(soil_pin, INPUT);

  delay(500);

  digitalWrite (red_pin, HIGH);
  digitalWrite (green_pin, HIGH);
  digitalWrite (blue_pin, HIGH);
  delay(100);


  // if(LTR390_Init() != 0){                          
  //   Serial.print("init err!!!");                   //  Waiting for sensot to initialise
  //   while(1); 
  //  }

  // LTR390_SetIntVal(5, 20);
  // pinMode(8, INPUT);                              // Interrupt pin

 digitalWrite (red_pin, LOW);
 delay(1000);

 digitalWrite (red_pin, HIGH);
 digitalWrite (green_pin, LOW);
 delay(1000);

 digitalWrite (green_pin, HIGH);
 digitalWrite (blue_pin, LOW);
 delay(1000);

 digitalWrite (blue_pin, HIGH);

 delay(100);

}

int water_type(String command) {                                // Exposing function/variable to the cloud
  
  if(command == "0") {            // "Tropical"
    soil_level = 0;
    return 0;
    
  } else if (command == "1") {    // "Sub-Tropical"
    soil_level = 1;
    return 1;

  } else if (command == "2") {    // "Succulent"
    soil_level = 2;
    return 2;
  }

  else return -1;
}

int sun_type(String command) {                                // Exposing function/variable to the cloud

  if(command == "0") {
    sun_light_level = 0;
    return 0;
    
  } else if (command == "1") {
    sun_light_level = 1;
    return 1;

  } else if (command == "2") {
    sun_light_level = 2;
    return 2;
  
  } else if (command == "3") {
    sun_light_level = 3;
    return 3;  
  }

  else return -1;
}


void soil() {

switch (soil_level) {

  case 0:                 // Plants that require less watering - Succulent
    water_upper = 20;
    water_lower = 5;
    break;
    
  case 1:
    water_upper = 40;     // Plants that require MODERATE watering - Most plants are covered in this category
    water_lower = 20;
    break;

  case 2:
    water_upper = 60;     // Plants that require more watering - Tropical
    water_lower = 20;
    break;

  default:
    water_upper = 0;
    water_lower = 0;
    break;
  }

int val_prev = 0;                                 //  temporary variables to determine  
int val_new = 0;                                  //  ..
int val_accum = 0;                                //  average value from multiple readings

ave_soil_val = 0;                               
soil_percentage = 0;
  
    for (int i=0; i<number_attempts; i++) {       //  Start taking measurements

        val = analogRead(soil_pin);
        val_new = val;
        val_accum = (val_new + val_prev);   

        val_prev = val_accum;                     // Accumilated value over all samples 
        delay(50);
    }

ave_soil_val = val_accum/number_attempts;                   //  Determine average value
//soil_percentage = map(ave_soil_val, 3700, 1875,0,100);    //  Map average value with pullup
soil_percentage = map(ave_soil_val, max_raw_value, min_raw_value,0,100);      //  Map average value

new_soil_level = soil_level;

  Serial.print(val);                               //  DEBUG
  Serial.println(" Single Read");                  //  DEBUG
  Serial.print(ave_soil_val);                      //  DEBUG
  Serial.println(" ACG");                          //  DEBUG
  Serial.print(soil_percentage);                   //  DEBUG
  Serial.println(" %");

  // Serial.print("Soil Moisture: ");                //  DEBUG
  // Serial.println(soil_level);                     //  DEBUG

  // Serial.print("Upper Limit: ");                  //  DEBUG
  // Serial.println(water_upper);                    //  DEBUG

  // Serial.print("Lower Limit: ");                  //  DEBUG
  // Serial.println(water_lower);                    //  DEBUG
}

void U_V() {
  
UV = LTR390_UVS();

Serial.print("UV = ");                            // DEBUG
Serial.println(UV);                               // DEBUG 
DEV_Delay_ms(500);

switch (sun_light_level) {

  case 0:                 // Plants that require full sun
    sun_upper = 6;
    sun_lower = 2;
    break;
         
  case 1:
    sun_upper = 2;       // Plants that require partial sun
    sun_lower = 1;
    break;
     
  case 2:
    sun_upper = 1;      // Plants that require full shade
    sun_lower = 0;
    break;

  case 3: 
    sun_upper = 1;      // Indoor Plants 
    sun_lower = 0;
    break;

  default:
    sun_upper = 0;
    sun_lower = 0;
    break;
  }

new_sun_light_level = sun_light_level;

// Serial.print("UV level: ");                  //  DEBUG
// Serial.println(UV);                          //  DEBUG

// Serial.print("UV Upper Limit: ");            //  DEBUG
// Serial.println(sun_upper);                   //  DEBUG

// Serial.print("UV Lower Limit: ");            //  DEBUG
// Serial.println(sun_lower);                   //  DEBUG

  // if(digitalRead(8) == 1){                //  Triggers interrupt if cindiction is met
  //   Serial.println("INT");
}

void Error_states () {

if ((UV > sun_upper) || (UV < sun_lower)) {
  UV_Error = 1;
    } else {
      UV_Error = 0;
    }

if (soil_percentage < water_lower) {
    digitalWrite (red_pin, LOW);                  // Red indicator when plant requires watering
    digitalWrite (blue_pin, HIGH);
    digitalWrite (green_pin, HIGH);
    H2O_Error = 1;

      } else if (soil_percentage > water_upper) {
        digitalWrite (blue_pin, LOW);             // Blue indicator when plant is overwatered
        digitalWrite (green_pin, HIGH);
        digitalWrite (red_pin, HIGH);
        H2O_Error = 1;

      } else {
        digitalWrite (red_pin, HIGH);
        digitalWrite (blue_pin, HIGH);
        digitalWrite (green_pin, LOW);
        H2O_Error = 0;
      }
}


void publish () {
    
    snprintf(data, sizeof(data) 
        , "{\"t\":\"%s\",\"new_sun_light_level\":%i,\"new_soil_level\":%i,\"sun_upper\":%i,\"sun_lower\":%i,\"water_upper\":%i,\"water_lower\":%i,\"H2O_Error\":%i,\"UV_Error\":%i}"
        , BLYNK_AUTH_TOKEN, UV,soil_percentage,sun_upper,sun_lower,water_upper,water_lower,H2O_Error,UV_Error
        );  
        
        Particle.publish("Send_smartpot_data", data, PRIVATE);
}


void loop() { 

unsigned long now = millis();
	 
  if ((now - lastTime) >= 300000) {
		lastTime = now;
    
    soil();
    U_V();
    Error_states ();
    publish();
	} 

  if ((new_soil_level != soil_level) || (new_sun_light_level != sun_light_level)) {
    soil();
    U_V();
    Error_states ();
    publish();
  }
}