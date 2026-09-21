# Smart Electric Power at EAA KidVenture: 1,500 Flights with the Cheesehead Timer

**By David Siegler, AMA NE9N**  
*Circlemasters Control Line Club*  
*For submission to Model Aviation Magazine*

---

## The KidVenture Challenge

For more than 30 years, the **Circlemasters Control Line Club** has introduced young people to model aviation at the Experimental Aircraft Association (EAA) AirVenture fly-in in Oshkosh, Wisconsin. During a single week at **KidVenture**, our volunteer crew—joined by dedicated modelers from across the country and around the world who travel to Oshkosh to help—conducts **more than 1,500 introductory flights** for youngsters who have never touched a control handle before.

For decades, we flew small **Norvel .061 glow engines**. While they served us well, running them at high volume was labor-intensive. Each circle required a three-person ground crew: one to fuel and prime, one on the starter battery, and one to hold and launch the airplane. Managing needle valves, hot engines, and refueling after every flight created constant bottlenecks.

We needed a modern electric system that could streamline operations, reduce crew workload, and withstand the unique demands of the Oshkosh flight line. Over three summers of testing, we developed the **Cheesehead Timer**, an open-source Arduino-based controller (available on [GitHub](https://github.com/ne9n/new-timer)).

```
+-------------------------------------------------------------------------+
|                  EAA KIDVENTURE FLIGHT LINE AT A GLANCE                 |
|                                                                         |
|  • 1,500+ student flights per week                                      |
|  • Volunteer modelers from around the world assisting on the line       |
|  • 1 pit person per circle (reduced from 3 crew members)                |
|  • 7 flights per 3S 2,200 mAh LiPo battery pack                         |
|  • 90-second instructional flight blocks                                |
|  • Full-power launch to defeat helicopter rotor wash                    |
|  • Automatic step-down to a gentle instructional cruise speed           |
|  • Millisecond IMU crash cutoff to protect brushless motors             |
|  • Active MPU gyro boost & pitch trim for Sport & Stunt flying          |
+-------------------------------------------------------------------------+
```

---

## Two-Stage Throttle: Launch Power vs. Training Speed

The KidVenture flight circle sits near active full-scale helicopter operations. Passing rotor wash, gusty summer winds, and thick grass demand high power at takeoff to pull lines tight and establish clean control. 

However, full power is too fast for a beginner's reflexes. Once airborne, the model must slow down so the student can comfortably learn elevator control.

```
       +--------------------------------------------------------------+
       |                  TWO-STAGE FLIGHT THROTTLE PROFILE           |
       |                                                              |
 100%  |     /------------\ (Stage 1: Launch through rotor wash)      |
       |    /              \                                          |
       |   /                \                                         |
  65%  |  /                  \=======================\ (Stage 2: Cru.)|
       | /                                            \---\ (Burp)    |
       |/                                                  \_______   |
       +--------------------------------------------------------------+
         [ RAMP ] [ TAKEOFF ]      [ INSTRUCTIONAL CRUISE ]   [ LAND ]
```

The Cheesehead Timer automates this two-stage sequence:

1. **Stage 1 (Launch):** Commands the Electronic Speed Control (ESC) to 100% power, accelerating through grass and rotor wash to lock in immediate line tension.
2. **Stage 2 (Cruise):** Automatically throttles back after 2 to 3 seconds to a mild 65% training speed.
3. **The Warning "Burp":** Delivers a brief throttle burst one lap before shutdown, alerting the instructor and student that landing is imminent.
4. **Landing:** Ramps down power smoothly for a gentle touchdown.

---

## Ground Crew Efficiency: 7 Flights per Pack

Transitioning to this smart electric system transformed our pit operations:

* **One Pit Person:** The pit person sets the airplane on the ground, presses a single button, and steps clear. The timer handles the 5-second walkout delay, launch ramp, flight timing, and shutdown.
* **Seven Flights per Battery:** Using a standard 3S 2,200 mAh Lithium Polymer (LiPo) battery and a budget 2212-size brushless motor, we consistently get **seven 90-second flights per pack**. 
* **Fast Turnarounds:** As soon as one student finishes, the next steps up, the button is pressed, and the flight begins.

---

## Motor Protection: Preventing High-Stall Burnouts

Early in testing, we discovered a key difference between glow and electric power. When a glow engine hits the ground, the prop stops and the engine dies. With an electric setup, if the airplane strikes the turf and the timer keeps running, the motor stalls while drawing maximum current. Within three to four seconds, this high locked-rotor current melts the motor windings and destroys the ESC.

To solve this, we integrated an MPU-6050 six-axis Inertial Measurement Unit (IMU):

* **Pitch Delta Cutoff (`PitchEX`):** A sudden pitch spike ($>40^\circ$) from a ground impact cuts ESC throttle to zero within milliseconds, saving the motor and ESC.
* **Low-Yaw Cutoff (`YawLOW`):** If the model stops rotating in the circle, power cuts immediately to prevent propeller strikes on the grass.
* **Line-Break / In-Circle Turn Cutoff (`YawEX`):** If the airplane suffers a line break or turns inward toward the center of the circle, the anomalous yaw/roll rate triggers an immediate motor shutdown to protect participants, spectators, and equipment.

---

## Expanding to Sport and Stunt Operation

While engineered for training, the Cheesehead Timer is equally valuable for **sport flying** and **Precision Aerobatics (Stunt)**. In traditional glow stunt, pilots relied on the classic "4-2-4" engine break to deliver extra power in climbs and maneuvers. The timer recreates and enhances this behavior electronically using two built-in gyro algorithms:

* **Maneuver Power Boost (`maneuverBoost`):** When the pilot deflects the elevator for sharp square corners, inside/outside loops, or vertical eights, the MPU-6050 detects the high pitch rate and commands an instantaneous burst of throttle. This delivers extra thrust at the apex of the maneuver to maintain flying speed and line tension.
* **Sinusoidal Pitch Trim (`posTrim`):** The timer reads vertical pitch angle in real time, increasing throttle during vertical climbs to combat gravity, and reducing power on vertical dives to maintain uniform lap speeds across the entire flight profile.

---

## Hardware & Software Design

We chose low-cost, off-the-shelf components so any club or individual modeler can build the system:

* **Microcontroller:** Arduino Nano V3 (ATmega328P) or ESP32.
* **Custom Carrier PCB:** A small breakout board sockets the Arduino Nano and MPU-6050, providing direct headers for three status LEDs, the start button, and a standard 3-pin ESC servo connector.
* **ESC Control:** Uses the standard Arduino `Servo` library (`#include <Servo.h>`) to output standard 50 Hz Pulse Width Modulation (PWM) signals.
* **Desktop Workbench App:** A companion Python app (`desktop_app.py`) allows instructors and stunt pilots to adjust launch throttle, cruise speeds, maneuver boost gains, and safety thresholds via USB and save them directly to EEPROM.

```
+--------------------------------------------------------------------------+
|                       CHEESEHEAD TIMER HARDWARE MAP                      |
+--------------------------------------------------------------------------+
|  Controller:         Arduino Nano V3 (ATmega328P) or ESP32 Module        |
|  Attitude Sensor:    MPU-6050 6-Axis Gyro/Accelerometer Breakout ($I^2C$) |
|  Carrier Board:      Custom PCB with LED, Button, and Servo headers      |
|  ESC Interface:      Arduino Servo Library (50Hz PWM Output)             |
|  Power Package:      Budget 2212 Outrunner + 30A ESC + 3S 2,200 mAh LiPo |
|  Status LEDs:        Red (Idle/Error), Yellow (Armed), Green (Running)   |
+--------------------------------------------------------------------------+
```

---

## Future Enhancements

Development continues to make the system even more capable and user-friendly:

* **Automated Gyro Lap Counting & Lap-Time Logging:** Using the onboard yaw rate integration ($360^\circ = 1.0\text{ lap}$ count), future updates will add precise lap-by-lap timing analytics and automatic lap-based flight termination, guaranteeing identical flight experiences regardless of wind conditions.
* **Wireless Interface for Field Setup:** Integrating Bluetooth or Wi-Fi connectivity (via ESP32) will allow flight leaders to tweak flight times, launch thrust, and cruise speeds directly from a smartphone or tablet in the pit without plugging in cables.
* **Direct ESC Telemetry & Fault Control:** Direct digital two-way communication with the ESC will enable real-time monitoring of motor current, temperature, and fault codes to catch issues before hardware is damaged.
* **Active RPM Compensation:** As battery voltage gradually drops across seven flights on a single pack, closed-loop Revolutions Per Minute (RPM) compensation will dynamically adjust throttle to maintain constant propeller speed and identical lap times from the first flight to the last.
* **More Compact Hardware:** An integrated, all-in-one surface-mount Printed Circuit Board (PCB) is in development to combine the microcontroller, IMU, power regulation, and headers into an ultra-compact, featherweight footprint suitable for smaller sport models.

---

## Conclusion: A Global Community Effort

The success of the KidVenture training circle is made possible by the incredible aeromodeling community. Modelers from across the globe volunteer their time as pilots and ground crew each summer, united by a passion for sharing flight with the next generation. 

By automating launch thrust, reducing student flight speed, protecting motors from crash damage, and supporting advanced stunt and sport capabilities, the Cheesehead Timer offers a proven, accessible solution for every level of Control Line flying.

---

## Photo Captions

* **Photo 1 (`fig1_kidventure_line.jpg`):**  
  *Caption:* A volunteer instructor guides a first-time pilot at EAA KidVenture during a 90-second introductory flight. (16 words)

* **Photo 2 (`fig2_launch_sequence.jpg`):**  
  *Caption:* The trainer launches at full power to overcome grass drag and helicopter turbulence before stepping down to cruise speed. (20 words)

* **Photo 3 (`fig3_carrier_board.jpg`):**  
  *Caption:* The custom carrier board sockets an Arduino Nano and MPU-6050 sensor, simplifying connections to LEDs and the ESC lead. (20 words)

* **Photo 4 (`fig4_battery_turnaround.jpg`):**  
  *Caption:* One 3S 2,200 mAh LiPo battery powers seven consecutive 90-second flights, enabling quick, single-person pit turnarounds. (17 words)

* **Photo 5 (`fig5_motor_protection.jpg`):**  
  *Caption:* Millisecond IMU pitch cutoffs prevent motor burnout from high locked-rotor currents during accidental ground strikes. (16 words)

* **Photo 6 (`fig6_stunt_operation.jpg`):**  
  *Caption:* On sport and stunt models, the timer's MPU-6050 gyro delivers dynamic maneuver power boost and active pitch trim through aerobatics. (21 words)

---

## Specifications & Resources

* **Circlemasters Club Website:** [http://www.circlemasters.com](http://www.circlemasters.com)
* **Author Contact:** David Siegler, AMA NE9N (`dwsiegler@gmail.com`)
* **Source Code & Documentation:** [https://github.com/ne9n/new-timer](https://github.com/ne9n/new-timer)
* **Hardware:** Arduino Nano V3, MPU-6050 IMU, custom carrier PCB, 30A ESC, 3S 2,200 mAh LiPo
* **Software:** Arduino IDE / Arduino-CLI, Python 3.8+ (`customtkinter`, `pyserial`)

---

*This article is formatted according to Model Aviation submission standards, employing AP and AMA style conventions with all acronyms spelled out on first reference.*
