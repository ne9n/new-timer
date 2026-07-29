import customtkinter as ctk
import serial
import serial.tools.list_ports
import threading
import subprocess
import time
import queue
import os
import re
import tkinter as tk
from tkinter import messagebox

# Set the appearance mode and default color theme
ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

STATE_NAMES = {
    0: "WAIT",
    1: "ARMED",
    2: "TAKEOFF_RAMP",
    3: "TAKEOFF",
    4: "FLY",
    5: "BURP",
    6: "RDYLAND",
    7: "RAMPDWN"
}

STATE_COLORS = {
    0: "#808080", # WAIT - Gray
    1: "#E65100", # ARMED - Orange
    2: "#F57F17", # TAKEOFF_RAMP - Gold
    3: "#00E676", # TAKEOFF - Bright Green
    4: "#00B0FF", # FLY - Cyan
    5: "#FF1744", # BURP - Bright Red
    6: "#AA00FF", # RDYLAND - Purple
    7: "#616161"  # RAMPDWN - Dark Gray
}

# Match telemetry pattern: ! speed:120 pitch:2 roll:0 yaw:15 boost:5 leds:R0 Y1 G1 btn:0 state:4
TELEMETRY_PATTERN = re.compile(
    r"^!\s*speed:(-?\d+)\s+pitch:(-?\d+)\s+roll:(-?\d+)\s+yaw:(-?\d+)\s+boost:(-?\d+)\s+leds:R([01])\s+Y([01])\s+G([01])\s+btn:([01])\s+state:(-?\d+)"
)

class SerialMonitorApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("Cheesehead Timer Dashboard & Serial Monitor")
        self.geometry("920x720")

        # Serial connection variables
        self.serial_port = None
        self.is_connected = False
        self.baudrate = 19200
        self.read_thread = None
        self.msg_queue = queue.Queue()

        # Telemetry plot data buffers (max 80 samples)
        self.max_samples = 80
        self.pitch_data = []
        self.roll_data = []
        self.yaw_data = []
        self.speed_data = []

        self._build_ui()
        self._create_menu()
        self.update_ports()
        self.after(100, self.process_queue)

    def _build_ui(self):
        # Configure grid layout (1 column, 3 rows)
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        # --- Top Frame (Connection & Telemetry Controls) ---
        self.top_frame = ctk.CTkFrame(self, height=50, corner_radius=0)
        self.top_frame.grid(row=0, column=0, sticky="ew")

        # Port Selector
        self.port_var = ctk.StringVar(value="")
        self.port_combobox = ctk.CTkComboBox(self.top_frame, variable=self.port_var, width=140)
        self.port_combobox.pack(side="left", padx=8, pady=10)

        # Refresh Ports Button
        self.refresh_btn = ctk.CTkButton(self.top_frame, text="Refresh", width=80, command=self.update_ports)
        self.refresh_btn.pack(side="left", padx=5, pady=10)

        # Connect / Disconnect Button
        self.connect_btn = ctk.CTkButton(self.top_frame, text="Connect", width=90, command=self.toggle_connection)
        self.connect_btn.pack(side="left", padx=5, pady=10)

        # Toggle Telemetry Stream Button
        self.telemetry_btn = ctk.CTkButton(
            self.top_frame, text="Toggle Telemetry Stream (!)", width=170, fg_color="#2E7D32", hover_color="#1B5E20", command=self.toggle_telemetry_stream
        )
        self.telemetry_btn.pack(side="left", padx=10, pady=10)

        # Flash Button
        self.flash_btn = ctk.CTkButton(self.top_frame, text="Flash Firmware", width=120, fg_color="#D35B5B", hover_color="#C25353", command=self.flash_firmware)
        self.flash_btn.pack(side="right", padx=10, pady=10)

        # --- Middle Tab View (Dashboard vs Terminal) ---
        self.tabview = ctk.CTkTabview(self)
        self.tabview.grid(row=1, column=0, sticky="nsew", padx=10, pady=5)

        self.dashboard_tab = self.tabview.add("📊 Live Telemetry Dashboard")
        self.terminal_tab = self.tabview.add("💻 Serial Terminal & Flasher")

        self._build_dashboard_tab()
        self._build_terminal_tab()

        # --- Bottom Frame (Input) ---
        self.bottom_frame = ctk.CTkFrame(self, height=50, corner_radius=0, fg_color="transparent")
        self.bottom_frame.grid(row=2, column=0, sticky="ew", padx=10, pady=5)

        self.input_entry = ctk.CTkEntry(self.bottom_frame, placeholder_text="Enter command to send (e.g. '?', '!', 'M 1.5', 'E 30')...")
        self.input_entry.pack(side="left", fill="x", expand=True, padx=(0, 10))
        self.input_entry.bind("<Return>", lambda event: self.send_command())

        self.send_btn = ctk.CTkButton(self.bottom_frame, text="Send", width=80, command=self.send_command)
        self.send_btn.pack(side="right")

    def _build_dashboard_tab(self):
        self.dashboard_tab.grid_columnconfigure(0, weight=1)
        self.dashboard_tab.grid_rowconfigure(1, weight=1)

        # --- Gauges Frame ---
        self.gauges_frame = ctk.CTkFrame(self.dashboard_tab)
        self.gauges_frame.grid(row=0, column=0, sticky="ew", padx=5, pady=5)

        # State Badge
        self.state_label = ctk.CTkLabel(
            self.gauges_frame, text="State: UNKNOWN", font=("Segoe UI", 13, "bold"), fg_color="#424242", corner_radius=6, width=150, height=32
        )
        self.state_label.pack(side="left", padx=8, pady=10)

        # Speed Gauge
        self.speed_val_label = ctk.CTkLabel(self.gauges_frame, text="Speed: 0 / 180", font=("Segoe UI", 11, "bold"))
        self.speed_val_label.pack(side="left", padx=(10, 4), pady=10)
        self.speed_progress = ctk.CTkProgressBar(self.gauges_frame, width=100, progress_color="#00E676")
        self.speed_progress.pack(side="left", padx=(0, 8), pady=10)
        self.speed_progress.set(0)

        # Boost Gauge
        self.boost_val_label = ctk.CTkLabel(self.gauges_frame, text="Boost: 0", font=("Segoe UI", 11, "bold"))
        self.boost_val_label.pack(side="left", padx=(8, 4), pady=10)
        self.boost_progress = ctk.CTkProgressBar(self.gauges_frame, width=70, progress_color="#FF9100")
        self.boost_progress.pack(side="left", padx=(0, 8), pady=10)
        self.boost_progress.set(0)

        # Angles Label
        self.angles_label = ctk.CTkLabel(self.gauges_frame, text="P: 0° | R: 0° | Y: 0°", font=("Consolas", 12, "bold"))
        self.angles_label.pack(side="left", padx=10, pady=10)

        # Hardware Indicators (LEDs & Button)
        self.led_frame = ctk.CTkFrame(self.gauges_frame, fg_color="transparent")
        self.led_frame.pack(side="right", padx=10, pady=10)

        self.led_r_badge = ctk.CTkLabel(self.led_frame, text="R", width=22, height=22, fg_color="#333333", corner_radius=11, font=("Segoe UI", 10, "bold"))
        self.led_r_badge.pack(side="left", padx=2)

        self.led_y_badge = ctk.CTkLabel(self.led_frame, text="Y", width=22, height=22, fg_color="#333333", corner_radius=11, font=("Segoe UI", 10, "bold"))
        self.led_y_badge.pack(side="left", padx=2)

        self.led_g_badge = ctk.CTkLabel(self.led_frame, text="G", width=22, height=22, fg_color="#333333", corner_radius=11, font=("Segoe UI", 10, "bold"))
        self.led_g_badge.pack(side="left", padx=2)

        self.btn_badge = ctk.CTkLabel(self.led_frame, text="BTN", width=30, height=22, fg_color="#333333", corner_radius=5, font=("Segoe UI", 9, "bold"))
        self.btn_badge.pack(side="left", padx=(5, 2))

        # --- Real-Time Plot Canvas ---
        self.plot_frame = ctk.CTkFrame(self.dashboard_tab)
        self.plot_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
        self.plot_frame.grid_columnconfigure(0, weight=1)
        self.plot_frame.grid_rowconfigure(1, weight=1)

        # Canvas Header / Legend
        self.legend_frame = ctk.CTkFrame(self.plot_frame, fg_color="transparent", height=28)
        self.legend_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=(5, 0))

        ctk.CTkLabel(self.legend_frame, text="📈 Telemetry Real-Time Plot:", font=("Segoe UI", 12, "bold")).pack(side="left", padx=5)
        ctk.CTkLabel(self.legend_frame, text="━ Pitch (X)", font=("Segoe UI", 11, "bold"), text_color="#00E5FF").pack(side="left", padx=10)
        ctk.CTkLabel(self.legend_frame, text="━ Roll (Y)", font=("Segoe UI", 11, "bold"), text_color="#FF9100").pack(side="left", padx=10)
        ctk.CTkLabel(self.legend_frame, text="━ Yaw (Z)", font=("Segoe UI", 11, "bold"), text_color="#FFEA00").pack(side="left", padx=10)
        ctk.CTkLabel(self.legend_frame, text="━ Throttle Speed", font=("Segoe UI", 11, "bold"), text_color="#00E676").pack(side="left", padx=10)

        self.clear_plot_btn = ctk.CTkButton(self.legend_frame, text="Clear Plot", width=70, height=22, fg_color="#424242", hover_color="#616161", command=self.clear_plot)
        self.clear_plot_btn.pack(side="right", padx=5)

        self.plot_canvas = tk.Canvas(self.plot_frame, bg="#121212", highlightthickness=0)
        self.plot_canvas.grid(row=1, column=0, sticky="nsew", padx=10, pady=8)
        self.plot_canvas.bind("<Configure>", lambda e: self._draw_plot())

    def _build_terminal_tab(self):
        self.terminal_tab.grid_columnconfigure(0, weight=1)
        self.terminal_tab.grid_rowconfigure(1, weight=1)

        # File Selection Frame
        self.file_frame = ctk.CTkFrame(self.terminal_tab, height=40, corner_radius=0, fg_color="transparent")
        self.file_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=(5, 0))

        self.file_path_var = ctk.StringVar(value="")
        self.file_entry = ctk.CTkEntry(self.file_frame, textvariable=self.file_path_var, placeholder_text="Select a .hex file to flash, or leave empty to flash from source...")
        self.file_entry.pack(side="left", fill="x", expand=True, padx=(0, 10))

        self.select_file_btn = ctk.CTkButton(self.file_frame, text="Browse...", width=80, command=self.select_file)
        self.select_file_btn.pack(side="right")

        # Terminal Textbox
        self.terminal_textbox = ctk.CTkTextbox(self.terminal_tab, state="disabled", font=("Consolas", 11))
        self.terminal_textbox.grid(row=1, column=0, sticky="nsew", padx=10, pady=5)

    def _create_menu(self):
        self.menu_bar = tk.Menu(self)
        self.config(menu=self.menu_bar)

        # Help Menu
        self.help_menu = tk.Menu(self.menu_bar, tearoff=0)
        self.menu_bar.add_cascade(label="Help", menu=self.help_menu)
        self.help_menu.add_command(label="Manual", command=self.show_manual)
        self.help_menu.add_command(label="About", command=self.show_about)

    def show_manual(self):
        manual_window = ctk.CTkToplevel(self)
        manual_window.title("Cheesehead Timer Manual")
        manual_window.geometry("700x800")
        manual_window.attributes("-topmost", True)

        textbox = ctk.CTkTextbox(manual_window, font=("Segoe UI", 12))
        textbox.pack(fill="both", expand=True, padx=20, pady=20)

        manual_path = os.path.join(os.path.dirname(__file__), "manual.md")
        if os.path.exists(manual_path):
            with open(manual_path, "r", encoding="utf-8") as f:
                content = f.read()
            textbox.insert("0.0", content)
        else:
            textbox.insert("0.0", f"Error: manual.md not found at {manual_path}")
        
        textbox.configure(state="disabled")

    def show_about(self):
        about_text = "Cheesehead Timer OSH26\n\nVersion: 1.1.0\nAuthor: Dave Siegler\nProject: Control Line Throttle Sequencer Dashboard"
        messagebox.showinfo("About", about_text)

    def select_file(self):
        filename = ctk.filedialog.askopenfilename(
            title="Select Firmware",
            filetypes=[("Hex Files", "*.hex"), ("All Files", "*.*")]
        )
        if filename:
            self.file_path_var.set(filename)

    def update_ports(self):
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if ports:
            self.port_combobox.configure(values=ports)
            if self.port_var.get() not in ports:
                self.port_var.set(ports[0])
        else:
            self.port_combobox.configure(values=[])
            self.port_var.set("")

    def toggle_connection(self):
        if self.is_connected:
            self.disconnect()
        else:
            self.connect()

    def toggle_telemetry_stream(self):
        if self.is_connected and self.serial_port and self.serial_port.is_open:
            self.serial_port.write("!\n".encode('utf-8'))
            self.log_to_terminal("> !\n")

    def connect(self):
        port = self.port_var.get()
        if not port:
            self.log_to_terminal("No port selected.\n")
            return

        try:
            self.serial_port = serial.Serial(port, self.baudrate, timeout=1)
            self.is_connected = True
            self.connect_btn.configure(text="Disconnect", fg_color="#D35B5B", hover_color="#C25353")
            self.port_combobox.configure(state="disabled")
            self.log_to_terminal(f"Connected to {port} at {self.baudrate} baud.\n")
            
            # Start read thread
            self.read_thread = threading.Thread(target=self.read_from_port, daemon=True)
            self.read_thread.start()
        except serial.SerialException as e:
            self.log_to_terminal(f"Error connecting to {port}: {e}\n")

    def disconnect(self):
        self.is_connected = False
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
        
        self.connect_btn.configure(text="Connect", fg_color=["#3a7ebf", "#1f538d"], hover_color=["#325882", "#14375e"])
        self.port_combobox.configure(state="normal")
        self.log_to_terminal("Disconnected.\n")

    def read_from_port(self):
        while self.is_connected and self.serial_port and self.serial_port.is_open:
            try:
                if self.serial_port.in_waiting > 0:
                    data = self.serial_port.readline().decode('utf-8', errors='replace')
                    if data:
                        self.msg_queue.put(data)
            except Exception as e:
                self.msg_queue.put(f"\n[Error reading from serial: {e}]\n")
                break
            time.sleep(0.01)

    def process_queue(self):
        while not self.msg_queue.empty():
            msg = self.msg_queue.get()
            self.log_to_terminal(msg)
            self._parse_telemetry(msg)
        self.after(50, self.process_queue)

    def _parse_telemetry(self, line):
        line_str = line.strip()
        match = TELEMETRY_PATTERN.match(line_str)
        if match:
            speed = int(match.group(1))
            pitch = int(match.group(2))
            roll = int(match.group(3))
            yaw = int(match.group(4))
            boost = int(match.group(5))
            r = int(match.group(6))
            y = int(match.group(7))
            g = int(match.group(8))
            btn = int(match.group(9))
            st_id = int(match.group(10))

            # Buffer plot data
            self.pitch_data.append(pitch)
            self.roll_data.append(roll)
            self.yaw_data.append(yaw)
            self.speed_data.append(speed)

            if len(self.pitch_data) > self.max_samples:
                self.pitch_data.pop(0)
                self.roll_data.pop(0)
                self.yaw_data.pop(0)
                self.speed_data.pop(0)

            # Update dashboard indicators
            st_name = STATE_NAMES.get(st_id, "UNKNOWN")
            st_color = STATE_COLORS.get(st_id, "#424242")
            self.state_label.configure(text=f"State: {st_name} ({st_id})", fg_color=st_color)

            self.speed_val_label.configure(text=f"Speed: {speed} / 180")
            self.speed_progress.set(min(1.0, max(0.0, speed / 180.0)))

            self.boost_val_label.configure(text=f"Boost: {boost}")
            self.boost_progress.set(min(1.0, max(0.0, boost / 180.0)))

            self.angles_label.configure(text=f"P: {pitch}° | R: {roll}° | Y: {yaw}°")

            self.led_r_badge.configure(fg_color="#FF1744" if r == 1 else "#333333")
            self.led_y_badge.configure(fg_color="#FFEA00" if y == 1 else "#333333")
            self.led_g_badge.configure(fg_color="#00E676" if g == 1 else "#333333")
            self.btn_badge.configure(fg_color="#29B6F6" if btn == 1 else "#333333")

            self._draw_plot()

    def clear_plot(self):
        self.pitch_data.clear()
        self.roll_data.clear()
        self.yaw_data.clear()
        self.speed_data.clear()
        self._draw_plot()

    def _draw_plot(self):
        w = self.plot_canvas.winfo_width()
        h = self.plot_canvas.winfo_height()
        if w < 20 or h < 20:
            return

        self.plot_canvas.delete("all")

        # Grid lines
        mid_y = h / 2
        self.plot_canvas.create_line(0, mid_y, w, mid_y, fill="#333333", dash=(4, 4))
        self.plot_canvas.create_line(0, mid_y - h / 4, w, mid_y - h / 4, fill="#222222", dash=(2, 4))
        self.plot_canvas.create_line(0, mid_y + h / 4, w, mid_y + h / 4, fill="#222222", dash=(2, 4))

        # Y-axis labels
        self.plot_canvas.create_text(18, 12, text="+180°", fill="#757575", font=("Consolas", 8))
        self.plot_canvas.create_text(18, mid_y - 5, text="0°", fill="#757575", font=("Consolas", 8))
        self.plot_canvas.create_text(18, h - 12, text="-180°", fill="#757575", font=("Consolas", 8))

        def map_coords(data_list, val_min=-180, val_max=180):
            if not data_list:
                return []
            pts = []
            dx = w / max(1, self.max_samples - 1)
            for i, val in enumerate(data_list):
                x = i * dx
                norm = (val - val_min) / float(val_max - val_min)
                y = h - (norm * h)
                y = max(2, min(h - 2, y))
                pts.extend([x, y])
            return pts

        # Render curves
        if len(self.pitch_data) >= 2:
            self.plot_canvas.create_line(map_coords(self.pitch_data), fill="#00E5FF", width=2)
        if len(self.roll_data) >= 2:
            self.plot_canvas.create_line(map_coords(self.roll_data), fill="#FF9100", width=2)
        if len(self.yaw_data) >= 2:
            self.plot_canvas.create_line(map_coords(self.yaw_data), fill="#FFEA00", width=2)
        if len(self.speed_data) >= 2:
            self.plot_canvas.create_line(map_coords(self.speed_data, val_min=0, val_max=180), fill="#00E676", width=2)

    def log_to_terminal(self, text):
        self.terminal_textbox.configure(state="normal")
        self.terminal_textbox.insert("end", text)
        self.terminal_textbox.see("end")
        self.terminal_textbox.configure(state="disabled")

    def send_command(self):
        if not self.is_connected or not self.serial_port or not self.serial_port.is_open:
            self.log_to_terminal("[Not connected. Cannot send command.]\n")
            return
            
        cmd = self.input_entry.get()
        if cmd:
            self.log_to_terminal(f"> {cmd}\n")
            self.serial_port.write(f"{cmd}\n".encode('utf-8'))
            self.input_entry.delete(0, "end")

    def flash_firmware(self):
        port = self.port_var.get()
        if not port:
            self.log_to_terminal("[Error: No port selected for flashing.]\n")
            return
            
        was_connected = self.is_connected
        if self.is_connected:
            self.disconnect()
            
        self.log_to_terminal("\n--- Starting Firmware Flash ---\n")
        self.flash_btn.configure(state="disabled")
        self.connect_btn.configure(state="disabled")
        
        threading.Thread(target=self._run_flash, args=(port, reconnect_after), daemon=True).start()

    def _run_flash(self, port, reconnect_after):
        try:
            file_to_flash = self.file_path_var.get().strip()
            if file_to_flash:
                user_home = os.path.expanduser("~")
                avrdude_path = os.path.join(user_home, ".platformio", "packages", "tool-avrdude", "avrdude.exe")
                avrdude_conf = os.path.join(user_home, ".platformio", "packages", "tool-avrdude", "avrdude.conf")
                
                exe_path = avrdude_path if os.path.exists(avrdude_path) else "avrdude"
                conf_path = avrdude_conf if os.path.exists(avrdude_conf) else "avrdude.conf"
                
                cmd = [
                    exe_path,
                    "-C", conf_path,
                    "-v", "-p", "atmega328p", "-c", "arduino", "-P", port, "-b", "115200", "-D",
                    f"-Uflash:w:{file_to_flash}:i"
                ]
                self.msg_queue.put(f"Flashing custom file: {file_to_flash}\n")
            else:
                self.msg_queue.put("Flashing from source via PlatformIO...\n")
                cmd = ["pio", "run", "-t", "upload", "-e", "nanoatmega328", "--upload-port", port]

            process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1
            )
            
            for line in process.stdout:
                self.msg_queue.put(line)
                
            process.wait()
            
            if process.returncode == 0:
                self.msg_queue.put("\n--- Flash Successful ---\n")
            else:
                self.msg_queue.put(f"\n--- Flash Failed (Exit code {process.returncode}) ---\n")
                
        except Exception as e:
            self.msg_queue.put(f"\n[Error executing PlatformIO: {e}]\n")
            self.msg_queue.put("[Make sure PlatformIO Core (CLI) is installed and in your PATH]\n")
            
        finally:
            self.after(0, lambda: self.flash_btn.configure(state="normal"))
            self.after(0, lambda: self.connect_btn.configure(state="normal"))
            
            if reconnect_after:
                self.after(1000, self.connect)

if __name__ == "__main__":
    app = SerialMonitorApp()
    app.mainloop()
