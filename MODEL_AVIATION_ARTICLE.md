# Smart Electric Power at EAA KidVenture

**By David Siegler, AMA NE9N**  
*Circle Masters Control Line Club*  
*For submission to Model Aviation Magazine*

---

## 1,500 Flights in Seven Days

Every summer at EAA AirVenture in Oshkosh, Wisconsin, the Circle Masters Control Line Club runs introductory flights for kids at Pioneer Airport. We have been doing this for over 30 years. Modelers from across the country and around the world come to Oshkosh to help out. In just seven days, our crew gives more than 1,500 flights to kids who have never held a control handle before.

For years, we flew small Norvel .061 glow engines. They did the job, but running them all day was hard work. Every flight circle needed three people: one to prime and fuel, one to hold the starter battery and electric starter, and one to hold the plane. Finding parts, fuel, and people who knew how to tune small glow engines was getting harder every year. Refueling after every 90-second flight caused constant lines and delays.

We decided to switch to electric power. But off-the-shelf timers did not fit how we fly at Oshkosh. So over three summers of testing, we built our own setup: the **Cheesehead Timer**.

---

## One Pit Man, Seven Flights per Battery

Going electric changed everything on our flight line:

* **One-person pits:** We went from three pit crew members down to one. The pit man holds the plane, pushes a button on the fuselage, and hand-launches it.
* **Seven flights per pack:** We use a cheap 3S 2,200 mAh LiPo battery and a budget 2807-size brushless motor. That single battery gives us six to seven 90-second flights before we need to swap it out.
* **Fast turnarounds:** When a flight ends, the next kid takes the handle. The pit man hits the button, launches, and the plane is in the air in seconds.

---

## Launch Power and Training Speed

Our circles sit right next to active helicopter flight paths at Pioneer Airport. Passing rotor wash and summer wind gusts make for rough air down low.

We hand-launch every airplane. Here is the problem with training:
* If you launch at a slow training speed, the airplane struggles the first two laps to pick up airspeed and get tossed around by wind or rotor wash.
* If you run full throttle the whole time, the plane flies at 50 to 60 mph. That is way too fast for an 8-year-old beginner.
* If the instructor tries to hold a throttle lever while helping the student, their attention is divided.

The timer fixes this with two throttle stages:

1. **Full-power launch:** The motor starts at 100% power for the first couple laps. This gives the hand-launched plane enough thrust to pick up speed quickly, beat the wind, and lock in solid line tension.
2. **Slow cruise:** Once the plane is up and moving, the motor drops to a calm 65% power. The flight slows down so the student can relax and learn to fly level.
3. **The warning "burp":** One lap before flight time ends, the motor revs up for a quick blip. This tells the instructor and student that the motor is about to shut down.
4. **Smooth landing:** The motor ramps down slowly so the plane glides in for an easy touchdown.

---

## How the Software Works

The software is simple and works like a flight checklist. It steps through six basic stages:

1. **Wait:** The timer sits disarmed with a flashing light, waiting for the button press.
2. **Arming delay:** When you press the button, it gives a five-second countdown so the pit man can get set to launch.
3. **Takeoff ramp:** It ramps the motor to full power for the hand launch and holds it there for two laps.
4. **Fly:** It throttles back to cruise speed. While flying, the code reads the gyro sensor 50 times every second to watch for crashes or stunt maneuvers.
5. **Warning blip:** At the end of the flight, it gives a quick throttle "burp" as a landing heads-up.
6. **Ramp down:** It smoothly cuts the throttle so the plane glides in for a touchdown.

All speed settings, timing intervals, and IMU sensor configurations are fully programmable by the user. You can easily adjust launch power, cruise speed, countdown delays, flight duration, and sensor sensitivities (such as crash-detection cutoffs and stunt maneuver boost) to match your specific model and flying style.

The code talks to standard speed controllers using the same regular servo signals any RC receiver uses. It is written in simple C++, so anyone who has used an Arduino can open the file, read it, and change values directly.

---

## Protecting Motors in a Crash

When a glow engine hits the ground, the prop stops and the engine stalls. Electric motors do not do that.

If an electric plane hits the ground and the timer keeps running, the stalled motor pulls massive current. Within three or four seconds, the heat melts the copper wire windings and destroys the speed control (ESC).

To stop this from happening, we added an inexpensive MPU-6050 gyro sensor to the board:

* **Impact shutoff:** If the nose hits the ground, the sensor catches the shock and kills power in milliseconds. We do not burn up motors or ESCs anymore.
* **Stop shutoff:** If the plane stops spinning around the circle, the timer cuts power right away.
* **Slack line safety:** If a line breaks or the plane turns inward toward the circle, the motor shuts off immediately.

---

## The Idea from the Czech Republic

The idea for this timer came from a trip I took in 2017 to an indoor control line contest in the Czech Republic. Pilots were flying full aerobatic stunt patterns inside a gym on short 15-foot lines.

Their models flew very slowly on level laps. But the second the nose went up for a loop or a corner, the motor spooled up to give extra pull.

We added that same feature to our timer for sport and stunt flyers:

* **Corner boost:** When you pull up for a loop or a square corner, the gyro senses the move and adds throttle to keep line tension tight.
* **Pitch trim:** The timer adds power on vertical climbs and cuts power on dives. It acts just like the classic "4-2-4" engine break on a glow stunt engine.

---

## Simple Hardware and STEM Projects

The timer runs on a cheap, standard Arduino Nano board and a gyro module. We designed a small circuit board that everything plugs into, so there are no loose or messy wires.

Because the parts are cheap and the code is open-source, this is a great project for school STEM clubs and youth build sessions. Kids can build the board, plug it in, and see how sensors, software, and motors work on a real model airplane.

We also made a simple computer app. You can plug the timer into a laptop with a USB cable and change speeds, launch power, and flight times in seconds.

---

## What Comes Next

We are working on a few new features:
* Retract servo controls to pull landing gear up after takeoff and drop it before landing.
* Twin-engine setups with motor syncing and outboard thrust boost.
* Wireless setup from a phone in the pit.
* Voltage sensing to keep lap times steady as the battery runs down.

KidVenture works because modelers from all over show up to help. If you are coming to Oshkosh this summer, stop by Pioneer Airport. We can always use another set of hands on the flight line.

---

## Photo Captions

* **Photo 1 (`fig1_kidventure_line.jpg`):**  
  *Caption:* An instructor coaches a young first-time flyer at EAA KidVenture in Oshkosh. (13 words)

* **Photo 2 (`fig2_launch_sequence.jpg`):**  
  *Caption:* Full power at launch helps the hand-launched trainer pick up speed before dropping to cruise. (16 words)

* **Photo 3 (`fig3_carrier_board.jpg`):**  
  *Caption:* The circuit board plugs directly into the Arduino, gyro sensor, and speed control. (13 words)

* **Photo 4 (`fig4_battery_turnaround.jpg`):**  
  *Caption:* One 3S 2,200 mAh LiPo battery powers six to seven flights with quick pit turnarounds. (15 words)

* **Photo 5 (`fig5_motor_protection.jpg`):**  
  *Caption:* Built-in gyro sensors shut off the motor instantly during crashes to protect the electronics. (14 words)

* **Photo 6 (`fig6_stunt_operation.jpg`):**  
  *Caption:* The gyro adds power in corners and climbs to help sport and stunt flyers. (14 words)

* **Photo 7 (`fig7_stem_workbench.jpg`):**  
  *Caption:* The low-cost Arduino setup makes this an easy project for school STEM clubs. (13 words)

---

## Resources

* **Club Website:** Circle Masters Control Line Club ([http://www.circlemasters.com](http://www.circlemasters.com))
* **Author Contact:** David Siegler, AMA NE9N (`dwsiegler@gmail.com`)
* **Project Files & Code:** [https://github.com/ne9n/new-timer](https://github.com/ne9n/new-timer)
* **Setup:** 2807 brushless motor, 30A ESC, 3S 2,200 mAh LiPo battery, 8x4 prop
* **Electronics:** Arduino Nano, MPU-6050 gyro board, custom plug-in PCB
