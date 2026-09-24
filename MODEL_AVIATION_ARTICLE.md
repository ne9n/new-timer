# Smart Electric Power at EAA KidVenture

**By David Siegler, AMA NE9N**  
*Circle Masters Control Line Club*  
*For submission to Model Aviation Magazine*

---

## 1,500 Flights in Seven Days

For more than 30 years, the Circle Masters Control Line Club has introduced youngsters to model aviation at the Experimental Aircraft Association (EAA) AirVenture fly-in in Oshkosh, Wisconsin. Operating at Pioneer Airport during **KidVenture**, our volunteer crew—joined by modelers from around the world who travel to Oshkosh to help—gives **over 1,500 introductory flights** in a single week. For most of these kids, it is the first time they have ever held a control handle.

For decades, we relied on small **Norvel .061 glow engines**. They flew well, but managing them at high volume was hard work. Every circle needed a three-person pit crew: one to fuel and prime, one on the battery and electric starter, and one to hold and launch the plane. Between finicky needle valves, hot engines, and constant refueling after every 90-second flight, the pits were always backed up.

We needed a clean, reliable electric setup that would cut pit workload and keep airplanes flying all day long. The result is the **Cheesehead Timer**, an open-source, low-cost controller that has transformed our flight line.

---

## Simple Pits: One Person, Seven Flights per Battery

Switching to smart electric power streamlined our entire operation:

* **One-Person Pit Crew:** Instead of three people juggling fuel and starters, a single pit man presses a button on the fuselage and hand-launches the airplane.
* **6 to 7 Flights per Charge:** Using an inexpensive 3S 2,200 mAh Lithium Polymer (LiPo) battery and a budget 2807-size brushless motor, each trainer flies **six to seven 90-second instructional flights** before we need to swap batteries.
* **Instant Turnaround:** As soon as one student lands, the next youngster steps to the handle. The pit man presses the button, launches, and the next flight is in the air in seconds.

---

## Launch Power vs. Training Speed

Our flight circles at Pioneer Airport are right next to active full-scale helicopter flight paths. Passing rotor wash and summer wind gusts create turbulent air close to the ground.

A beginner trainer needs plenty of thrust at takeoff to pull lines tight, overcome grass drag, and punch through helicopter downwash. But once airborne, full power is way too fast for a newcomer's reflexes.

The Cheesehead Timer automates this with an easy two-stage throttle profile:

1. **Full-Power Launch:** The motor starts at 100% power for two to three seconds, giving the plane an authoritative climbout with rock-solid line tension.
2. **Gentle Cruise:** Once the model reaches flying height, the timer automatically throttles back to an easy 65% training speed. The plane slows down to a comfortable pace where the student can relax and learn elevator control.
3. **The Warning "Burp":** One lap before flight time is up, the motor gives a brief power surge—an audible cue telling the instructor and student that landing is next.
4. **Smooth Touchdown:** Power ramps down gently for a smooth landing on the grass.

---

## Built-In Crash Protection

With a glow engine, hitting the ground simply stalls the propeller and stops the engine. Electric motors behave differently. If a trainer hits the turf and the timer keeps running, the stalled motor draws huge current. Within seconds, the heat melts the internal copper windings and destroys the Electronic Speed Controller (ESC).

To protect our equipment, we added an inexpensive gyro sensor (MPU-6050) to the board:

* **Instant Ground Shutoff:** If the nose pitches down into the turf, the sensor detects the sudden shock and shuts the motor off in milliseconds—saving the motor and ESC from burnout.
* **Low-Yaw Cutoff:** If the airplane stops circling, the controller immediately cuts power.
* **Line-Break Safety:** If a line snaps or the model turns inward toward the circle, power shuts down instantly.

---

## From Czech Gymnasium to Oshkosh Flight Line

The inspiration for this design came from an indoor Control Line stunt contest I attended in the Czech Republic in 2017. Pilots flew full aerobatic patterns inside a gymnasium on **short 15-foot lines**. 

Their airplanes flew slowly on level laps, but the moment the nose pitched up into a maneuver, the motor automatically spooled up to provide extra power through loops and corners.

We built that same capability into the Cheesehead Timer for **sport and stunt flyers**:

* **Maneuver Boost:** Senses when you pull up for loops or square corners and automatically adds throttle to maintain line tension and speed.
* **Pitch Trim:** Boosts power during vertical climbs and reduces power on dives, mimicking the classic "4-2-4" glow stunt run.

---

## Simple Hardware & STEM Learning

The timer is built around a standard, low-cost **Arduino** board and an attitude sensor, mounted on a small custom carrier board with simple plug-in connections for the start button, status lights, and ESC lead.

Because it is affordable, easy to assemble, and open-source, it makes a fantastic hands-on project for school STEM classes, youth groups, and club build sessions. Students get to explore basic electronics, sensor physics, and flight mechanics on a real flying airplane.

A free companion computer app connects to the timer through a USB cable, letting you change launch power, cruise speed, and flight times with a few clicks.

---

## Future Features & An Open Invitation

We continue to expand the project with features like automated retractable landing gear sequencing, twin-engine motor controls, wireless smartphone setup, and active propeller speed regulation as battery voltage drops.

Our success at KidVenture is thanks to the wonderful aeromodeling community. Modelers from across the country and around the world volunteer their time on our flight lines every summer. If you are heading to Oshkosh, come join us—we can always use more hands on the circle!

---

## Photo Captions

* **Photo 1 (`fig1_kidventure_line.jpg`):**  
  *Caption:* A volunteer instructor coaches a young first-time pilot on the KidVenture flight line at Oshkosh. (16 words)

* **Photo 2 (`fig2_launch_sequence.jpg`):**  
  *Caption:* Full power at launch punches through grass drag and helicopter rotor wash before stepping down to cruise. (18 words)

* **Photo 3 (`fig3_carrier_board.jpg`):**  
  *Caption:* The compact carrier board neatly connects the Arduino, gyro sensor, status lights, and speed control. (16 words)

* **Photo 4 (`fig4_battery_turnaround.jpg`):**  
  *Caption:* One inexpensive 3S 2,200 mAh LiPo battery powers six to seven flights with quick single-person turnarounds. (17 words)

* **Photo 5 (`fig5_motor_protection.jpg`):**  
  *Caption:* Built-in gyro sensors cut power in milliseconds during ground strikes, preventing burned brushless motor windings. (16 words)

* **Photo 6 (`fig6_stunt_operation.jpg`):**  
  *Caption:* For sport and stunt flying, the timer automatically adds power in maneuvers to mimic a classic engine run. (19 words)

* **Photo 7 (`fig7_stem_workbench.jpg`):**  
  *Caption:* Simple open-source Arduino hardware makes this timer a great hands-on project for school STEM programs and clubs. (18 words)

---

## Specifications & Resources

* **Club Website:** Circle Masters Control Line Club ([http://www.circlemasters.com](http://www.circlemasters.com))
* **Author Contact:** David Siegler, AMA NE9N (`dwsiegler@gmail.com`)
* **Project Files & Code:** [https://github.com/ne9n/new-timer](https://github.com/ne9n/new-timer)
* **Power Setup:** 2807-size brushless motor, 30A ESC, 3S 2,200 mAh LiPo battery, 8x4 propeller
* **Electronics:** Arduino Nano V3 (or ESP32), MPU-6050 sensor board, custom carrier PCB

---

*This article is formatted according to Model Aviation submission guidelines, using AP and AMA style conventions with acronyms spelled out on first reference.*
