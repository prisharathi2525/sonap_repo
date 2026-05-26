"""
Simple data logger: thermistors + MAX30102 (IR/Red) + accelerometer (X, Y, Z)
Reads Arduino CSV: Tleft, Tright, IR, Red, AccX, AccY, AccZ
Outputs raw data to CSV.
"""

import serial
import time
import csv

# ---------------------- CONFIG ----------------------
PORT = "COM3"         # change if needed
BAUD = 115200
TOTAL_TIME = 120       # seconds

# ---------------------- CSV logging ----------------------
log_fname = f"session_log_{int(time.time())}.csv"
log_file = open(log_fname, "w", newline='')
log_writer = csv.writer(log_file)
log_writer.writerow(["t", "Tleft", "Tright", "IR", "Red", "AccX", "AccY", "AccZ"])

# ---------------------- Serial connect ----------------------
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
print(f"Connected to {PORT} @ {BAUD} baud")
print(f"Logging to {log_fname}")

# ---------------------- MAIN LOOP ----------------------
start = time.time()
print("\n=== Recording started ===")

try:
    while True:
        elapsed = time.time() - start
        if elapsed > TOTAL_TIME:
            break

        line = ser.readline().decode(errors='ignore').strip()
        if not line:
            continue

        parts = [s.strip() for s in line.split(",")]
        if len(parts) < 7:
            continue

        try:
            Tleft_v  = float(parts[0])
            Tright_v = float(parts[1])
            IR_v     = int(float(parts[2]))
            Red_v    = int(float(parts[3]))
            AX_v     = int(float(parts[4]))
            AY_v     = int(float(parts[5]))
            AZ_v     = int(float(parts[6]))
        except ValueError:
            continue

        log_writer.writerow([
            f"{elapsed:.3f}",
            f"{Tleft_v:.3f}",
            f"{Tright_v:.3f}",
            IR_v,
            Red_v,
            AX_v,
            AY_v,
            AZ_v
        ])

        print(f"t={elapsed:.1f}s | Tleft={Tleft_v:.2f} Tright={Tright_v:.2f} | "
              f"IR={IR_v} Red={Red_v} | AX={AX_v} AY={AY_v} AZ={AZ_v}")

finally:
    ser.close()
    log_file.close()
    print(f"\n=== Done. Data saved to {log_fname} ===")