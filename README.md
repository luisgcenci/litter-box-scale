# Litter box scale

A scale that sits under the cats' litter box and logs its weight, so you can
see visits, track each cat's weight and spot when something seems off.

```
4 load cells → HX711 → XIAO ESP32-C3 ──Wi-Fi/MQTT──► Mosquitto → ingest.py → TimescaleDB → Grafana
```

## What's where

| Folder      | What                                                         |
|-------------|--------------------------------------------------------------|
| `firmware/` | PlatformIO project for the board (reads the scale, sends grams) |
| `backend/`  | Docker Compose: Mosquitto, TimescaleDB, Grafana              |
| `bridge/`   | Python scripts: `ingest.py` saves readings to the database   |
| `cad/`      | FreeCAD design files                                         |

## Run it

**1. Start the backend**

```bash
cd backend
cp .env.example .env        # set the passwords
docker compose up -d
```

The `readings` table is created on first start (`init.sql`).
Grafana is at http://localhost:3000 (user `admin`).

**2. Calibrate the scale**

> First time only, or after changing the load cells, the board or the wiring.
> Already have a `CALIBRATION_FACTOR` in `main.cpp`? Skip to step 3.

1. Set `KNOWN_WEIGHT_G` in `firmware/src/calibrate.cpp` to something you've weighed.
2. Upload the calibration sketch and open the monitor:
   ```bash
   cd firmware
   pio run -e xiao-calibrate -t upload && pio device monitor
   ```
3. Follow the prompts, then copy the printed factor into `CALIBRATION_FACTOR` in `firmware/src/main.cpp`.

**3. Flash the board**

```bash
cd firmware
cp include/secrets.example.h include/secrets.h   # Wi-Fi + your computer's IP
pio run -e xiao -t upload
pio device monitor
```

The scale tares itself at startup, so keep it empty when you power it on.

**4. Start saving readings**

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install -r bridge/requirements.txt
cp bridge/.env.example bridge/.env              # same Postgres password as backend/.env
python bridge/ingest.py
```

## MQTT topics

| Topic          | Payload                                  |
|----------------|------------------------------------------|
| `cat/readings` | weight in grams, e.g. `4213.5`, every second |
| `cat/status`   | `online` / `offline` (retained)          |

## Wiring (XIAO ESP32-C3)

| HX711 | XIAO |
|-------|------|
| DT    | D2   |
| SCK   | D4   |
| VCC   | 3V3  |
| GND   | GND  |
