# Cheesehead Timer Flowcharts

Here are the flowcharts detailing the core logic and state progression of the Arduino firmware.

## Main Execution Loop

This flowchart outlines the main `loop()` function in `cheeshead_timer.ino`, showing the order of operations for reading sensors, updating outputs, and processing the state machine.

```mermaid
graph TD
    Start([loop]) --> CheckGyro{"gyroEn?"}
    CheckGyro -- Yes --> ReadGyro[speedGyro:<br>Read MPU, Calc Trim & Boost]
    CheckGyro -- No --> UpdateLED
    ReadGyro --> UpdateLED[updateLED:<br>Update LED states]
    UpdateLED --> GetDips[getDips:<br>Read DIP switches]
    GetDips --> UpdateBtn[updateButton:<br>Process button inputs]
    UpdateBtn --> Telemetry[telemetryUpdate:<br>Print Serial Telemetry]
    Telemetry --> CheckRunState{"run_state == true?"}
    
    CheckRunState -- Yes --> AdvanceState[check_state:<br>Advance to next state<br>if timer expired]
    AdvanceState --> ApplySpeed[speedState:<br>Apply throttle and<br>crash checks for current state]
    ApplySpeed --> TermCtrl
    
    CheckRunState -- No --> TermCtrl[term_ctrl:<br>Process serial menu commands]
    TermCtrl --> Start
```

## Speed State Machine

This state diagram outlines the `speed_state` machine found in `state_machine.cpp`. It details the throttle behavior and transition conditions for each flight phase.

```mermaid
stateDiagram-v2
    [*] --> WAIT: run_state = true
    
    WAIT --> ARMED: Timer (1s)
    note right of WAIT
        Throttle = 25 (Quick Burp)
        Clear crash flags
    end note
    
    ARMED --> TAKEOFF_RAMP: Timer (ArmTime)
    note right of ARMED
        Throttle = 0
    end note
    
    TAKEOFF_RAMP --> TAKEOFF: Timer (accelTime)
    note right of TAKEOFF_RAMP
        Increment Throttle to MAX_SPEED
    end note
    
    TAKEOFF --> FLY: Timer (5s)
    note right of TAKEOFF
        Throttle = MAX_SPEED
        Check Crash (Pitch/Yaw)
    end note
    
    FLY --> BURP: Lap Count >= LapLimit - 10
    FLY --> RDYLAND: Timer (FlyTime)
    note right of FLY
        Throttle = FlySpeed + AutoIncrease
        Check Crash (Pitch/Yaw/Low)
    end note
    
    BURP --> RDYLAND: Timer (0.5s)
    note right of BURP
        Throttle = BurpMax
    end note
    
    RDYLAND --> RAMPDWN: Timer (2s)
    note right of RDYLAND
        Throttle = FlySpeed
    end note
    
    RAMPDWN --> [*]: Timer (3s)
    note right of RAMPDWN
        Throttle = 0
        run_state = false
    end note

    state Crash_Shutdown_Logic {
        direction LR
        Exceed_Thresholds --> run_state_false
        run_state_false: run_state = false & Throttle = 0
    }
```

## Lap Logic (Gyro)

This flowchart outlines the lap counting and shutdown logic processed inside `read_giro()` in `gyro.cpp`.

```mermaid
graph TD
    Start([read_giro]) --> CheckTime{"Time since last<br>sample >= 100ms?"}
    CheckTime -- No --> End([Return])
    CheckTime -- Yes --> CalcDelta[Calculate Y-axis delta]
    CalcDelta --> Accumulate[Accumulate Y degrees]
    Accumulate --> CheckAccum["abs(Accumulated Y)<br>>= 36 degrees?"]
    CheckAccum -- Yes --> IncLap[Increment LapCount<br>by 0.1 laps]
    CheckAccum -- No --> EvalPitchYaw[Evaluate Pitch/Yaw<br>Hysteresis]
    
    IncLap --> CheckLimit{"LapCount >=<br>LapLimit?"}
    CheckLimit -- Yes --> Shutdown[run_state = false<br>Throttle = 0]
    CheckLimit -- No --> CheckBurp{"LapLimit >= 10 &<br>Approaching Limit?"}
    
    CheckBurp -- Yes --> TriggerBurp[Set state to BURP]
    CheckBurp -- No --> ConsumeY[Subtract 36 degrees<br>from Accumulated Y]
    
    ConsumeY --> EvalPitchYaw
    Shutdown --> EvalPitchYaw
    TriggerBurp --> EvalPitchYaw
    
    EvalPitchYaw --> UpdateLast[Update last angle & time]
    UpdateLast --> End
```

## Gyro Axis Mapping and Usage

This block diagram illustrates how the raw X, Y, and Z inputs from the MPU6050 are dynamically mapped to Pitch, Roll, and Yaw logical axes, and how each axis drives specific features like lap counting, throttle adjustments, and crash detection.

```mermaid
graph TD
    %% MPU Input and Mapping
    MPU[MPU6050 Sensor] -->|Raw X, Y, Z| Config{Axis Mapping Configuration<br>TimerSetup.axisPitch/Roll/Yaw}
    
    %% Mapped Axes
    Config -->|axisPitch| Pitch[Pitch Angle]
    Config -->|axisRoll| Roll[Roll Angle]
    Config -->|axisYaw| Yaw[Yaw Angle]

    %% Pitch Usage
    Pitch --> PitchDelta[Delta Angle Threshold<br>abs dx >= pitchThresh]
    PitchDelta -->|Exceeds Limit| PitchEX[Crash Detection: PitchEX<br>run_state = false]
    
    Pitch --> PosTrim[Positional Trim<br>posTrim]
    PosTrim -->|Sine wave adjustment| Throttle[Throttle Adjustment]
    
    Pitch --> ManeuverBoost[Maneuver Boost<br>maneuverBoost]
    ManeuverBoost -->|Absolute Magnitude| Throttle

    %% Roll Usage
    Roll --> Accumulate[Accumulate Delta Degrees]
    Accumulate -->|Every 36 degrees| Laps[Lap Count += 0.1]
    Laps -->|Approaching Limit| Burp[Trigger BURP State]
    Laps -->|Limit Reached| LapShutdown[Lap Limit Exceeded<br>run_state = false]

    %% Yaw Usage
    Yaw --> YawRate[Calculate Yaw Rate<br>deg/sec]
    YawRate -->|Exceeds Limit| YawEX[Crash Detection: YawEX<br>run_state = false]
    
    Yaw --> YawAbs[Absolute Yaw Angle]
    YawAbs -->|< Low Threshold| YawLOW[Low Yaw Detection: YawLOW<br>run_state = false]

    %% Subgraphs for Organization
    subgraph Hardware & Mapping
        MPU
        Config
    end

    subgraph Pitch Logic
        Pitch
        PitchDelta
        PitchEX
        PosTrim
        ManeuverBoost
    end

    subgraph Roll Logic
        Roll
        Accumulate
        Laps
        Burp
        LapShutdown
    end

    subgraph Yaw Logic
        Yaw
        YawRate
        YawEX
        YawAbs
        YawLOW
    end
```
