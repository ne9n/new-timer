import customtkinter as ctk
import serial
import serial.tools.list_ports
import threading
import subprocess
import time
import queue

# Set the appearance mode and default color theme
ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

class SerialMonitorApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("Cheesehead Timer Serial Monitor")
        self.geometry("800x600")

        # Serial connection variables
        self.serial_port = None
        self.is_connected = False
        self.baudrate = 19200
        self.read_thread = None
        self.msg_queue = queue.Queue()

        self._build_ui()
        self.update_ports()
        self.after(100, self.process_queue)

    def _build_ui(self):
        # Configure grid layout (1 column, 4 rows)
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(2, weight=1)

        # --- Top Frame (Controls) ---
        self.top_frame = ctk.CTkFrame(self, height=60, corner_radius=0)
        self.top_frame.grid(row=0, column=0, sticky="ew")

        # Port Selector
        self.port_var = ctk.StringVar(value="")
        self.port_combobox = ctk.CTkComboBox(self.top_frame, variable=self.port_var, width=150)
        self.port_combobox.pack(side="left", padx=10, pady=10)

        # Refresh Ports Button
        self.refresh_btn = ctk.CTkButton(self.top_frame, text="Refresh Ports", width=100, command=self.update_ports)
        self.refresh_btn.pack(side="left", padx=10, pady=10)

        # Connect / Disconnect Button
        self.connect_btn = ctk.CTkButton(self.top_frame, text="Connect", width=100, command=self.toggle_connection)
        self.connect_btn.pack(side="left", padx=10, pady=10)

        # Flash Button
        self.flash_btn = ctk.CTkButton(self.top_frame, text="Flash Firmware", width=120, fg_color="#D35B5B", hover_color="#C25353", command=self.flash_firmware)
        self.flash_btn.pack(side="right", padx=10, pady=10)

        # --- File Selection Frame ---
        self.file_frame = ctk.CTkFrame(self, height=40, corner_radius=0, fg_color="transparent")
        self.file_frame.grid(row=1, column=0, sticky="ew", padx=10, pady=(10, 0))

        self.file_path_var = ctk.StringVar(value="")
        self.file_entry = ctk.CTkEntry(self.file_frame, textvariable=self.file_path_var, placeholder_text="Select a .hex file to flash, or leave empty to flash from source...")
        self.file_entry.pack(side="left", fill="x", expand=True, padx=(0, 10))

        self.select_file_btn = ctk.CTkButton(self.file_frame, text="Browse...", width=80, command=self.select_file)
        self.select_file_btn.pack(side="right")

        # --- Middle Frame (Terminal Display) ---
        self.terminal_textbox = ctk.CTkTextbox(self, state="disabled", font=("Consolas", 12))
        self.terminal_textbox.grid(row=2, column=0, sticky="nsew", padx=10, pady=(10, 0))

        # --- Bottom Frame (Input) ---
        self.bottom_frame = ctk.CTkFrame(self, height=50, corner_radius=0, fg_color="transparent")
        self.bottom_frame.grid(row=3, column=0, sticky="ew", padx=10, pady=10)

        self.input_entry = ctk.CTkEntry(self.bottom_frame, placeholder_text="Enter command to send...")
        self.input_entry.pack(side="left", fill="x", expand=True, padx=(0, 10))
        self.input_entry.bind("<Return>", lambda event: self.send_command())

        self.send_btn = ctk.CTkButton(self.bottom_frame, text="Send", width=80, command=self.send_command)
        self.send_btn.pack(side="right")

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
        self.after(100, self.process_queue)

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
        
        # Run flashing in a separate thread so UI doesn't freeze
        threading.Thread(target=self._run_flash, args=(port, was_connected), daemon=True).start()

    def _run_flash(self, port, reconnect_after):
        try:
            file_to_flash = self.file_path_var.get().strip()
            if file_to_flash:
                import os
                user_home = os.path.expanduser("~")
                avrdude_path = os.path.join(user_home, ".platformio", "packages", "tool-avrdude", "avrdude.exe")
                avrdude_conf = os.path.join(user_home, ".platformio", "packages", "tool-avrdude", "avrdude.conf")
                
                # Check if avrdude exists in the platformio path, otherwise fall back to global avrdude
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
                # Using platformio command line to upload
                # Assuming pio is in the system PATH
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
            # Re-enable buttons in the main thread
            self.after(0, lambda: self.flash_btn.configure(state="normal"))
            self.after(0, lambda: self.connect_btn.configure(state="normal"))
            
            if reconnect_after:
                self.after(1000, self.connect)

if __name__ == "__main__":
    app = SerialMonitorApp()
    app.mainloop()
