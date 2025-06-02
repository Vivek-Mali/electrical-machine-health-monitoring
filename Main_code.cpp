#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Fuzzy.h>


#include <WiFi.h>

const float vibrationThreshold = 1.0; // Dynamic acceleration in g's
// Initialize components
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define ONE_WIRE_BUS 23
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define NUM_PZEMS 3
PZEM004Tv30 pzems[NUM_PZEMS];

int buzzer=13;


String recipientNumber[]={"+917411795792"};
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);


String CtrlJsonDataString;  

float treading=0;
float vreading=0;
float faultreading=0;
String webJSONoutputString;
String faultstate="NO";
// Fuzzy logic setup
Fuzzy *fuzzy = new Fuzzy();
FuzzyInput *temperatureInput = new FuzzyInput(1);
FuzzyInput *vibrationInput = new FuzzyInput(2);
FuzzyInput *currentInput = new FuzzyInput(3);
FuzzyOutput *faultOutput = new FuzzyOutput(1);

float baselineX = 0, baselineY = 0, baselineZ = 0;

float
    L1_voltage = 0,
    L1_current = 0,
    L1_power = 0,
    L1_energy = 0,
    L1_frequency = 0,
    L1_pf = 0,
    L2_voltage = 0,
    L2_current = 0,
    L2_power = 0,
    L2_energy = 0,
    L2_frequency = 0,
    L2_pf = 0,
    L3_voltage = 0,
    L3_current = 0,
    L3_power = 0,
    L3_energy = 0,
    L3_frequency = 0,
    L3_pf = 0;


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
//    Serial.println();
//    Serial.print("Connecting to ");
//    Serial.println(ssid);
   // displaycustommessage("Connecting WIFI..");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //Serial.print(".");
    }
    randomSeed(micros());
   
  siren();
  delay(2000);

//    Serial.println("");
//    Serial.println("WiFi connected");
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
}


int curtime=0;
int prevtime=0;

void setup() {
  Serial.begin(4800);
  lcd.init();
  lcd.backlight();
  sensors.begin();

  if (!accel.begin()) {
    Serial.println("Error: ADXL345 not detected. Check wiring!");
    while (1);
  }
  
  accel.setRange(ADXL345_RANGE_16_G);
  calibrateSensor();

  pzems[0] = PZEM004Tv30(&Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);
  pzems[1] = PZEM004Tv30(&Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x02);
  pzems[2] = PZEM004Tv30(&Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x03);

  // Initialize fuzzy sets
  setupFuzzyLogic();
 lcd.setCursor(0, 0);
  lcd.print("  Fuzzy  Logic  ");
  lcd.setCursor(0, 1);
  lcd.print("Fault Monitoring");
  delay(3000);
  lcd.clear();
  lcd.print("Initializing...");
  pinMode(buzzer,OUTPUT);
  delay(2000);
  lcd.clear();
  lcd.print("connecting wifi..");
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnect();
  siren();
  lcd.clear();
  lcd.print("Connected");

  lcd.clear();
  lcd.print("      Done      ");
  siren();

  delay(2000);
}

void loop() {
  // Read temperature

  
 if(!client.connected())
  {
    reconnect();
  }
  client.loop();


 curtime=millis();
 if((curtime-prevtime)>10000)
 {
  prevtime=curtime;


  generateWebJSONoutputString();



   // Serial.println("sent");

 }
  
  // Create a JSON document

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  treading=temperatureC;

 

// Read vibration - calculate average of 20 readings
  sensors_event_t event;
  float sumX = 0, sumY = 0, sumZ = 0;

  const int numReadings = 10; // Number of readings for averaging
  for (int i = 0; i < numReadings; i++) {
    accel.getEvent(&event);
    sumX += event.acceleration.x - baselineX;
    sumY += event.acceleration.y - baselineY;
    sumZ += event.acceleration.z - baselineZ;
    delay(10); // Small delay between readings to ensure consistent sampling
  }

  // Calculate averages
  float avgX = sumX / numReadings;
  float avgY = sumY / numReadings;
  float avgZ = sumZ / numReadings;

  // Calculate vibration magnitude
  float vibrationMagnitude = sqrt(sq(avgX) + sq(avgY) + sq(avgZ));

  vreading=vibrationMagnitude;

 
  

  // Read phase current
  float current = pzems[0].current();
  // Serial.println(temperatureC);
  //Serial.println(vibrationMagnitude);
 // Serial.println(current);

  // Set inputs for fuzzy logic
  fuzzy->setInput(1, temperatureC);
  fuzzy->setInput(2, vibrationMagnitude);
  fuzzy->setInput(3, current);

  // Perform fuzzification and defuzzification
  fuzzy->fuzzify();
  float faultLevel = fuzzy->defuzzify(1);

  faultreading=faultLevel;

  // Display results
  lcd.setCursor(0, 0);
  lcd.print("Fault Level:    ");
  lcd.setCursor(0, 1);
  lcd.print(faultLevel);
  lcd.print("    ");

  //Serial.println("Fuzzy Output:");
  //Serial.print("\tFault Level: ");
  //Serial.println(faultLevel);
  if(faultreading>6)
  {
    faultstate="YES";
     lcd.clear();
    lcd.print("FAULT ALERT");
    dangersiren();
    generateWebJSONoutputString();
    
   
    delay(1000);
    generateWebJSONoutputString();
  
    delay(3000);
    
    

    String msgtosend="FAULT ALERT. PLEASE ATTEND";
    sendsms(recipientNumber[0],msgtosend );
    delay(3000);
    dangersiren();
    dangersiren();
    generateWebJSONoutputString();
    
    
    delay(1000);
    siren();
    lcd.clear();
   
  }
  else
  {
    faultstate="NO";
  }

  
  
  delay(500);
}

void setupFuzzyLogic() {
  // Temperature fuzzy sets
  FuzzySet *lowTemp = new FuzzySet(0, 10, 10, 20);         // Low temperature
  FuzzySet *mediumTemp = new FuzzySet(15, 30, 30, 40);     // Medium temperature
  FuzzySet *highTemp = new FuzzySet(40, 45, 45, 50);       // High temperature
  temperatureInput->addFuzzySet(lowTemp);
  temperatureInput->addFuzzySet(mediumTemp);
  temperatureInput->addFuzzySet(highTemp);
  fuzzy->addFuzzyInput(temperatureInput);

  // Vibration fuzzy sets
  FuzzySet *lowVibration = new FuzzySet(0, 0.3, 0.3, 0.6);      // Low vibration
  FuzzySet *mediumVibration = new FuzzySet(0.6, 0.8, 0.8, 0.9); // Medium vibration
  FuzzySet *highVibration = new FuzzySet(0.9, 1.2, 1.2, 5.0); // High vibration
  vibrationInput->addFuzzySet(lowVibration);
  vibrationInput->addFuzzySet(mediumVibration);
  vibrationInput->addFuzzySet(highVibration);
  fuzzy->addFuzzyInput(vibrationInput);

  // Phase current fuzzy sets
  FuzzySet *lowCurrent = new FuzzySet(0, 0.5, 0.5, 1.0);         // Low current
  FuzzySet *normalCurrent = new FuzzySet(0.8, 1.5, 1.5, 2.5);    // Normal current
  FuzzySet *highCurrent = new FuzzySet(2.0, 3.0, 3.0, 5.0);      // High current
  currentInput->addFuzzySet(lowCurrent);
  currentInput->addFuzzySet(normalCurrent);
  currentInput->addFuzzySet(highCurrent);
  fuzzy->addFuzzyInput(currentInput);

  // Fault level fuzzy sets
  FuzzySet *noFault = new FuzzySet(0, 2, 2, 4);             // No fault
  FuzzySet *moderateFault = new FuzzySet(3, 5, 5, 7);       // Moderate fault
  FuzzySet *severeFault = new FuzzySet(6, 8, 8, 10);        // Severe fault
  faultOutput->addFuzzySet(noFault);
  faultOutput->addFuzzySet(moderateFault);
  faultOutput->addFuzzySet(severeFault);
  fuzzy->addFuzzyOutput(faultOutput);

  // Fuzzy rules

  // Rule 1: High temperature, high vibration, and high current -> Severe fault
  FuzzyRuleAntecedent *ifTempHighAndVibHigh = new FuzzyRuleAntecedent();
  ifTempHighAndVibHigh->joinWithAND(highTemp, highVibration);

  FuzzyRuleAntecedent *ifTempHighAndVibHighAndCurrHigh = new FuzzyRuleAntecedent();
  ifTempHighAndVibHighAndCurrHigh->joinWithAND(ifTempHighAndVibHigh, highCurrent);

  FuzzyRuleConsequent *thenSevereFault = new FuzzyRuleConsequent();
  thenSevereFault->addOutput(severeFault);

  FuzzyRule *rule1 = new FuzzyRule(1, ifTempHighAndVibHighAndCurrHigh, thenSevereFault);
  fuzzy->addFuzzyRule(rule1);

  // Rule 2: Medium temperature, medium vibration, and normal current -> Moderate fault
  FuzzyRuleAntecedent *ifTempMedAndVibMed = new FuzzyRuleAntecedent();
  ifTempMedAndVibMed->joinWithAND(mediumTemp, mediumVibration);

  FuzzyRuleAntecedent *ifTempMedAndVibMedAndCurrNormal = new FuzzyRuleAntecedent();
  ifTempMedAndVibMedAndCurrNormal->joinWithAND(ifTempMedAndVibMed, normalCurrent);

  FuzzyRuleConsequent *thenModerateFault = new FuzzyRuleConsequent();
  thenModerateFault->addOutput(moderateFault);

  FuzzyRule *rule2 = new FuzzyRule(2, ifTempMedAndVibMedAndCurrNormal, thenModerateFault);
  fuzzy->addFuzzyRule(rule2);

  // Rule 3: Low temperature, low vibration, and low current -> No fault
  FuzzyRuleAntecedent *ifTempLowAndVibLow = new FuzzyRuleAntecedent();
  ifTempLowAndVibLow->joinWithAND(lowTemp, lowVibration);

  FuzzyRuleAntecedent *ifTempLowAndVibLowAndCurrLow = new FuzzyRuleAntecedent();
  ifTempLowAndVibLowAndCurrLow->joinWithAND(ifTempLowAndVibLow, lowCurrent);

  FuzzyRuleConsequent *thenNoFault = new FuzzyRuleConsequent();
  thenNoFault->addOutput(noFault);

  FuzzyRule *rule3 = new FuzzyRule(3, ifTempLowAndVibLowAndCurrLow, thenNoFault);
  fuzzy->addFuzzyRule(rule3);

// Rule 4: High temperature and high vibration -> Moderate fault
FuzzyRuleAntecedent *ifTempHighAndVibHigh_Rule4 = new FuzzyRuleAntecedent();
ifTempHighAndVibHigh_Rule4->joinWithAND(highTemp, highVibration);

FuzzyRuleConsequent *thenModerateFault_Rule4 = new FuzzyRuleConsequent();
thenModerateFault_Rule4->addOutput(moderateFault);

FuzzyRule *rule4 = new FuzzyRule(4, ifTempHighAndVibHigh_Rule4, thenModerateFault_Rule4);
fuzzy->addFuzzyRule(rule4);

// Rule 5: High vibration alone -> Severe fault
FuzzyRuleAntecedent *ifVibHigh_Rule5 = new FuzzyRuleAntecedent();
ifVibHigh_Rule5->joinSingle(highVibration);

FuzzyRuleConsequent *thenSevereFault_Rule5 = new FuzzyRuleConsequent();
thenSevereFault_Rule5->addOutput(severeFault);

FuzzyRule *rule5 = new FuzzyRule(5, ifVibHigh_Rule5, thenSevereFault_Rule5);
fuzzy->addFuzzyRule(rule5);


// Rule 6: Medium temperature and high vibration -> Moderate fault
FuzzyRuleAntecedent *ifTempMedAndVibHigh_Rule6 = new FuzzyRuleAntecedent();
ifTempMedAndVibHigh_Rule6->joinWithAND(mediumTemp, highVibration);

FuzzyRuleConsequent *thenModerateFault_Rule6 = new FuzzyRuleConsequent();
thenModerateFault_Rule6->addOutput(moderateFault);

FuzzyRule *rule6 = new FuzzyRule(6, ifTempMedAndVibHigh_Rule6, thenModerateFault_Rule6);
fuzzy->addFuzzyRule(rule6);


  // Rule 7: High current alone -> Severe fault
  FuzzyRuleAntecedent *ifCurrHigh_Rule7 = new FuzzyRuleAntecedent();
  ifCurrHigh_Rule7->joinSingle(highCurrent);

  FuzzyRuleConsequent *thenSevereFault_Rule7 = new FuzzyRuleConsequent();
  thenSevereFault_Rule7->addOutput(severeFault);

  FuzzyRule *rule7 = new FuzzyRule(7, ifCurrHigh_Rule7, thenSevereFault_Rule7);
  fuzzy->addFuzzyRule(rule7);

  Serial.println("Fuzzy Logic Initialized Successfully.");
}


void calibrateSensor() {
  sensors_event_t event;
  int numSamples = 100;

  for (int i = 0; i < numSamples; i++) {
    accel.getEvent(&event);
    baselineX += event.acceleration.x;
    baselineY += event.acceleration.y;
    baselineZ += event.acceleration.z;
    delay(10);
  }

  baselineX /= numSamples;
  baselineY /= numSamples;
  baselineZ /= numSamples;

  Serial.println("Sensor calibrated!");
}

void siren()
{
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
  
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
  
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
}

void dangersiren()
{
  digitalWrite(buzzer,HIGH);
  delay(3000);
  digitalWrite(buzzer,LOW);
  delay(100);
}

void generateWebJSONoutputString() {

   StaticJsonDocument<2048> doc; 

 if (treading != DEVICE_DISCONNECTED_C) {
    

    doc["Temperature"] = treading;
  } else {
    
    doc["Temperature"] = -1; // Use -1 for error in temperature
  }

   doc["Vibration"] = vreading;
  if (vreading > vibrationThreshold) {
   
    doc["VibrationStatus"] = "Detected";
  } else {
    
    doc["VibrationStatus"] = "None";
  }

  doc["fault"] = faultstate;

   doc["fread"] = faultreading;

JsonArray pzemsData = doc.createNestedArray("PZEM_Modules");

  for (int i = 0; i < NUM_PZEMS; i++) {
    float voltage = pzems[i].voltage();
    float current = pzems[i].current();
    float power = pzems[i].power();
    float energy = pzems[i].energy();
    float frequency = pzems[i].frequency();
    float pf = pzems[i].pf();

    JsonObject pzemData = pzemsData.createNestedObject();
    pzemData["Module"] = i + 1;
    pzemData["Voltage"] = isnan(voltage) ? -1 : voltage; // Use -1 for error in numerical values
    pzemData["Current"] = isnan(current) ? -1 : current;
    pzemData["Power"] = isnan(power) ? -1 : power;
    pzemData["Energy"] = isnan(energy) ? -1 : energy;
    pzemData["Frequency"] = isnan(frequency) ? -1 : frequency;
    pzemData["PowerFactor"] = isnan(pf) ? -1 : pf;

   
  }

 
  serializeJson(doc, webJSONoutputString);
  Serial.println(webJSONoutputString);
  
}

void sendsms(String numberv, String message)
{
    Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  
  Serial.println("AT+CMGS=\""+numberv+"\""); 
  delay(1000);
  Serial.println(message);// The SMS text you want to send
  delay(100);
   Serial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
   siren();
}