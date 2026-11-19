# Panasonic heatpump (ESPHome + Home Assistant)

Files:

| File | Purpose |
|---|---|
| `../heatpump.yaml` | ESPHome config for ESP32-C3 + `kenavru/esphome_components` (`panasonic_heatpump`) |
| `lovelace-heatpump.yaml` | Lovelace dashboard (raw configuration) |

## ESPHome

Validated with ESPHome **2026.10.2** and the HeishaMon-parity branch
`feature/panasonic-heishamon-k-series` of `kenavru/esphome_components`
(commit `576739a` or newer).

It expects a `secrets.yaml` with `api_key`, `ota_password`, `wifi_ssid`,
`wifi_password` in the ESPHome config directory (not included here).

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/kenavru/esphome_components
      ref: feature/panasonic-heishamon-k-series
    components: [ panasonic_heatpump ]
```

Hardware: ESP32-C3, UART 9600 8E1 on GPIO3 (TX) / GPIO4 (RX), with an
ADUM1201 (or equivalent) level shifter between the 5 V CN-CNT bus and the ESP.

### K-series notes

- `set22` / `set23` (heater start/stop delta) are **signed**. This unit
  reports -10 K and 0 K. The heat pump clamps the start delta at **-10 K**:
  values below -10 (e.g. -11 or -14) are accepted transiently and then revert
  to -10, so -10 is the floor.
- `set21` (heater delay) is clamped at **60 min** on this unit: values above
  60 (e.g. 250) are accepted transiently and then revert to 60.
- `set37` / `set38` (bivalent advanced start/stop) accept -15 … 35 °C.
- `set46` (heater ON outdoor temperature) accepts -20 … 20 °C. This unit
  reports -20 °C, which is below the older HeishaMon documented lower bound.
- `Backup Heater Enable` (`top167`) is the master custom-menu switch and is
  **read-only** over CN-CNT. Disable it on the heat pump:
  *Installer / custom menu → Back-up heater → Disable*.
  On the K-series remote: from the main screen, **press and hold the Menu
  button for ~10 seconds** to open the *Custom menu*, then select
  *Back-up heater → Do not use*. If the menu is password-locked, reset the
  password to `0000` first (service manual 12.5.2). This setting also disables
  the frost-protection heater; Panasonic warns that de-ice can then stop and
  raise H75 if the heating setpoint is too low.
- `Room Heater State` / `DHW Heater State` (`set45` / `set44`) only allow or
  block the per-mode paths. They do not override the master.
- `Heater ON outdoor temp` only gates normal heating. Tank mode, force heater,
  defrost, anti-freeze and heat-exchanger protection bypass it.
- After disabling the master, the firmware can still energise the heater for
  defrost/anti-freeze/protection. Do not cut the heater supply or its breaker;
  Panasonic documents that as a cause of malfunction during de-ice.
- M-series-only values (heating-ON temperature/delay, efficiency stage times,
  extra pump, SG Ready power, model name) read as -128 / -5 / UNDEFINED on a
  K-series unit. The dashboard omits them.

## Dashboard

1. **Settings → Dashboards → Add dashboard → New dashboard from scratch**.
2. Open it, **Edit → ⋮ → Raw configuration editor**.
3. Replace the content with `lovelace-heatpump.yaml` and save.

Views:

- **Overview** — temperatures, heating, DHW, heater summary, COP/energy,
  controls, status.
- **Heater** — the gate hierarchy (master → per-mode → outdoor temp →
  force/defrost/anti-freeze), a 48 h causality graph, heater deltas/timers,
  DHW/sterilization timers, and the protection exceptions.
- **Diagnostics** — refrigerant circuit, performance, identification,
  installer/custom-menu options, bivalent/SG Ready, sensor calibration.

Entity IDs assume the ESPHome node name `heatpump` (e.g.
`sensor.heatpump_outside_temp`). If yours differ, check **Developer Tools →
States** and adjust.

The dashboard uses only built-in cards, no HACS dependencies. Optional
upgrades that work well with this config:

- **Mushroom** (`custom:mushroom-climate-card`, `mushroom-entity-card`) for a
  more compact control panel.
- **mini-graph-card** for combined temperature/power graphs.
- **apexcharts-card** for long-term COP and energy charts.
- **auto-entities** to list all diagnostic entities without maintaining the
  list by hand.
