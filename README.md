# via-cli
Command-line interface for managing VIA keyboard firmware

## Usage

```
Usage: via [command] [args...]

Commands:
  devices
  version -d [vendor:product]
  uptime -d [vendor:product]
Keymap:
  get_keycode -d [vendor:product] -l [layer] -r [row] -c [column]
  set_keycode -d [vendor:product] -l [layer] -r [row] -c [column] -k [keycode]
RGB:
  get_rgb_brightness -d [vendor:product]
  get_rgb_mode -d [vendor:product]
  get_rgb_speed -d [vendor:product]
  get_rgb_colour -d [vendor:product]
  set_rgb_brightness -d [vendor:product] -b [brightness]
  set_rgb_mode -d [vendor:product] -m [mode]
  set_rgb_speed -d [vendor:product] -s [speed]
  set_rgb_colour -d [vendor:product] -h [hue] -S [saturation]
  dump_keymap -d [vendor:product] -L [layers] -R [rows] -C [cols]
  reset_keymap -d [vendor:product]

Flags:
-d VENDOR:PRODUCT
   Select a device to command. Use 'devices' to enumerate available devices.
-b [brightness] (0-255, default: 0)
   RGB brightness.
-m [mode] (0-255, default: 0)
   RGB effect mode.
-s [speed] (0-255, default: 0)
   RGB effect speed.
-h [hue] (0-255, default: 0)
   RGB colour hue.
-S [saturation] (0-255, default: 0)
   RGB colour saturation.
-l [layer] (0-255, default: 0)
   Key layer.
-r [row] (0-255, default: 0)
   Key row.
-c [column] (0-255, default: 0)
   Key column.
-k [keycode] (0-255, default: 0)
   Hexadecimal keycode.
-L [layer count] (0-255, default 0)
   Number of layers in keymap.
-R [row count] (0-255, default 0)
   Number of rows in keymap.
-C [column count] (0-255, default 0)
   Number of columns in keymap.
```
