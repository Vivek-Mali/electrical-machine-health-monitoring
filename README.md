# Description 
This project presents a comprehensive system for monitoring the health of three-phase induction motors using fuzzy logic and real-time sensor data. It leverages ESP32, PZEM modules, and sensors to continuously track voltage, current, temperature, and vibration. The system intelligently classifies the motor’s condition into Normal, Warning, or Critical using fuzzy logic algorithms.

# Key features include

-Sensor Integration: Real-time voltage, current, power, temperature, and vibration monitoring

-IoT Dashboard: MQTT-based data transmission for remote visualization

-GSM Alerts: SMS notifications on critical motor faults

-Local Display: LCD for on-site monitoring

-Fuzzy Logic: Smart fault classification that handles sensor uncertainty

-Predictive Maintenance: Reduces downtime and improves operational reliability

This system is ideal for industrial automation, predictive maintenance, and smart motor control.

#Hardware Used

- ESP32 Board

- PZEM-004T Power Monitoring Modules

- DS18B20 Temperature Sensor

- ADXL345 Vibration Sensor

- GSM Module (SIM800L)

- LCD 16*2 Display

- Buzzer
 
## Software/Tools

- Arduino IDE

- Easy EDA

- Visual Studio Code

- MQTT Broker

## Documentation
📄[Report.pdf](https://github.com/user-attachments/files/20546798/Final_Report.PDF)

📄[EMHM_Paper.pdf](https://github.com/user-attachments/files/20547039/1741240310_H2080.pdf)

## Published Paper link
https://www.ijarse.com/images/fullpdf/1741240310_H2080.pdf

## Firmware
##fuzzy_logic_config
1. WiFi Connection Setup 

Input: 

• SSID (WiFi name) 

• Password (WiFi password) 

Output: 

• ESP32 connected to the WiFi network 

• Connection status displayed on the LCD and serial monitor 

Steps: 

-Initialize the serial communication (Serial.begin) for debugging.

-Initialize the LCD screen and display a message ("Connecting to WiFi..."). 

-Call WiFi.begin(ssid, password) to start the WiFi connection. 

-Check WiFi.status() in a loop: 

-If not connected, print a message ("Connecting...") to the serial monitor and wait 1 second.

-Once connected, print "WiFi Connected" to the serial monitor.

-Display "WiFi Connected" on the LCD. 

3. MQTT Setup and Reconnection 

Input: 

• MQTT server address 

• MQTT port 

Output: 

• Active MQTT connection 

• Data sent to the MQTT broker 

Steps: 

-Initialize the PubSubClient with the MQTT server and port.

-In the main loop, check client.connected(): 

o If the client is disconnected: 

▪ Attempt to reconnect using client.connect("Client_ID"). 

▪ Print success or failure messages to the serial monitor. 

▪ Wait 2 seconds before retrying if the connection fails. 

-Once connected, call client.loop() to maintain the connection. 

3. Fuzzy Logic Setup 

Input: 

• Voltage and current readings from sensors 

• Fuzzy rules for fault evaluation 

Output: 

• Fuzzy fault level: "Normal," "Warning," or "Critical" 

Steps: 

-Define fuzzy input variables: 

o Voltage: Define sets for "Low Voltage," "Normal Voltage," and "High Voltage." 

o Current: Define sets for "Low Current," "Normal Current," and "High Current." 

Define fuzzy output variables: 

o Fault Level: Define sets for "Normal," "Warning," and "Critical." 

Create fuzzy rules: 

o Rule 1: If Voltage is Low and Current is Low, Fault Level is Normal. 

o Rule 2: If Voltage is Normal and Current is Normal, Fault Level is Warning. 

o Rule 3: If Voltage is High or Current is High, Fault Level is Critical. 

Initialize the fuzzy system with these inputs, outputs, and rules.

4. Sensor Data Acquisition (PZEM Sensors) 

Input: 

• RX and TX pins for each PZEM sensor 

Output: 

• Voltage, Current, and Power readings for each phase 

Steps: 

-Initialize three PZEM sensor objects, each representing a phase. 

-Read sensor data using: 

o pzem.voltage() for voltage. 

o pzem.current() for current. 

o pzem.power() for power. 

-Check for invalid data (e.g., -1 returned by the library) and handle errors appropriately. 

-Store the readings for further processing.

5. Fuzzy Logic Evaluation 

Input: 

• Voltage and current readings for each phase 

• Predefined fuzzy rules 

Output: 

• Fault Level (Numerical and Categorized as "Normal," "Warning," or "Critical") 

Steps: 

-Pass the voltage and current readings for a phase to the fuzzy system using setInput().

-Run the fuzzification process to evaluate the inputs based on defined fuzzy sets. 

-Apply the fuzzy rules to derive a fault level. 

-Defuzzify the output to compute a numerical fault level. 

-Categorize the fault level: 

o <50: Normal 

o 50-80: Warning 

o 80: Critical

6. Display Fault Level on LCD 

Input: 

• Fault Level 

• Sensor readings (Voltage, Current, Power) 

Output: 

• Real-time fault status displayed on the LCD 

Steps: 

-Clear the LCD display.

-Set the cursor to the desired position.

-Display the fault level (Normal, Warning, or Critical) on the first line. 

-Optionally, display voltage, current, and power values for each phase on subsequent lines. 

7. Publish Data to MQTT 

Input: 

• Sensor readings for each phase 

• Fault Level 

Output: 

• JSON payload sent to the MQTT broker 

Steps: 

-Format the sensor data and fault level into a JSON string.

-Use client.publish(topic, payload) to send the data to the MQTT broker. 

-Print success or failure messages to the serial monitor. 

-Retry publishing if the transmission fails.

## Hardware

Schematic

![Schematic](https://github.com/user-attachments/assets/28fdbcbf-9543-4db0-be51-224fd444404e)

PCB_Layout

![PCB_Layout](https://github.com/user-attachments/assets/872cf9c7-7c6f-4402-aca3-df571846770d)

Block_Diagram

![Block_Diagram](https://github.com/user-attachments/assets/17613bca-b42b-4d8e-86e1-91048929f7ed)

## Flowchart

![Flowchart](https://github.com/user-attachments/assets/317adad7-0a54-44be-903f-af3c3b61b788)

Working_Diagram

![Working_Diagram](https://github.com/user-attachments/assets/3eae65de-7f79-40eb-a4a2-f370262f156d)

# Images

Model

![Model](https://github.com/user-attachments/assets/4d576fdf-10e7-493b-8d28-26a191406039)

Snapshot

![Snapshot](https://github.com/user-attachments/assets/287fb5db-8346-462d-87fa-2fcbf8747f8b)

Alert Msg

![Alert msg](https://github.com/user-attachments/assets/37f1d385-f16d-4944-be4a-bf46f1b91535)

# Demo Video

https://drive.google.com/file/d/1OLE9NDAUVFJ6cNfOJ8RM70nyzRCLB4V3/view?usp=sharing

https://drive.google.com/file/d/1PW4mfnNMRa0na9L72kXbzkxOPEjYd0Wv/view?usp=sharing

# Conclusions 
The development of an advanced fault detection and monitoring system for three-phase induction motors using fuzzy logic 
and IoT technologies has demonstrated a robust and effective solution for ensuring operational reliability. By integrating 
sensors for real-time data acquisition, fuzzy logic for precise fault classification, and IoT platforms for remote monitoring, the 
system enables proactive maintenance and reduces downtime. The inclusion of GSM alerts ensures timely intervention, while 
the LCD display provides local insights into motor health. This holistic approach not only addresses the limitations of 
traditional fault detection methods but also enhances the overall efficiency and lifespan of induction motors. The project 
exemplifies the potential of combining advanced algorithms and modern communication technologies to meet the needs of 
industries reliant on motor-driven systems. 


