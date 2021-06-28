# via-cli
Command-line interface for managing VIA keyboard firmware

## Usage

```
Usage: via [command] [args...]

Basic:
- devices
- version [vendor:product]
- uptime [vendor:product]
Keymap:
- get_keycode [vendor:product] [layer] [row] [column]
- set_keycode [vendor:product] [layer] [row] [column] [keycode]
RGB:
- get_rgb_brightness [vendor:product]
- get_rgb_mode [vendor:product]
- get_rgb_speed [vendor:product]
- get_rgb_colour [vendor:product]
- set_rgb_brightness [vendor:product] [brightness]
- set_rgb_mode [vendor:product] [mode]
- set_rgb_speed [vendor:product] [speed]
- set_rgb_colour [vendor:product] [hue] [saturation]
```
