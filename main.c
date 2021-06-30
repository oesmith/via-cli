#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <hidapi.h>

#include "commands.h"
#include "keycodes.h"

#define PACKET_SIZE 32

#define RAW_USAGE_PAGE 0xff60
#define RAW_USAGE_ID 0x61

#undef DEBUG

uint8_t packet[PACKET_SIZE];

hid_device *flag_device = NULL;
uint8_t flag_row = 0;
uint8_t flag_column = 0;
uint8_t flag_layer = 0;
uint8_t flag_brightness = 0;
uint8_t flag_mode = 0;
uint8_t flag_speed = 0;
uint8_t flag_hue = 0;
uint8_t flag_saturation = 0;
unsigned short flag_keycode = 0;

void dump_packet() {
#ifdef DEBUG
  printf("__ __ __ __ __ __ __ __\n");
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", packet[0], packet[1],
         packet[2], packet[3], packet[4], packet[5], packet[6], packet[7]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", packet[8], packet[9],
         packet[10], packet[11], packet[12], packet[13], packet[14],
         packet[15]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", packet[16], packet[17],
         packet[18], packet[19], packet[20], packet[21], packet[22],
         packet[23]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", packet[24], packet[25],
         packet[26], packet[27], packet[28], packet[29], packet[30],
         packet[31]);
#endif
}

void send(uint8_t *data, int len) {
  if (flag_device == NULL) {
    fprintf(stderr, "--device flag required.\n");
    exit(EXIT_FAILURE);
  }

  memset(packet, 0, PACKET_SIZE);
  memcpy(packet + 1, data, len);

  dump_packet();

  if (hid_write(flag_device, packet, PACKET_SIZE) != PACKET_SIZE) {
    perror("Error writing request\n");
    exit(EXIT_FAILURE);
  }

  if (hid_read_timeout(flag_device, packet, PACKET_SIZE, 500) != PACKET_SIZE) {
    perror("Error reading response\n");
    exit(EXIT_FAILURE);
  }

  dump_packet();
}

void help() {
  printf("Usage: via [command] [args...]\n");
  printf("\nCommands:\n");
  printf("  devices\n");
  printf("  version -d [vendor:product]\n");
  printf("  uptime -d [vendor:product]\n");
  printf("Keymap:\n");
  printf("  get_keycode -d [vendor:product] -l [layer] -r [row] -c [column]\n");
  printf("  set_keycode -d [vendor:product] -l [layer] -r [row] -c [column] -k [keycode]\n");
  printf("RGB:\n");
  printf("  get_rgb_brightness -d [vendor:product]\n");
  printf("  get_rgb_mode -d [vendor:product]\n");
  printf("  get_rgb_speed -d [vendor:product]\n");
  printf("  get_rgb_colour -d [vendor:product]\n");
  printf("  set_rgb_brightness -d [vendor:product] -b [brightness]\n");
  printf("  set_rgb_mode -d [vendor:product] -m [mode]\n");
  printf("  set_rgb_speed -d [vendor:product] -s [speed]\n");
  printf("  set_rgb_colour -d [vendor:product] -h [hue] -S [saturation]\n");
  printf("\nFlags:\n");
  printf("-d VENDOR:PRODUCT\n");
  printf("   Select a device to command. Use 'devices' to enumerate available devices.\n");
  printf("-b [brightness] (0-255, default: 0)\n");
  printf("   RGB brightness.\n");
  printf("-m [mode] (0-255, default: 0)\n");
  printf("   RGB effect mode.\n");
  printf("-s [speed] (0-255, default: 0)\n");
  printf("   RGB effect speed.\n");
  printf("-h [hue] (0-255, default: 0)\n");
  printf("   RGB colour hue.\n");
  printf("-S [saturation] (0-255, default: 0)\n");
  printf("   RGB colour saturation.\n");
  printf("-l [layer] (0-255, default: 0)\n");
  printf("   Key layer.\n");
  printf("-r [row] (0-255, default: 0)\n");
  printf("   Key row.\n");
  printf("-c [column] (0-255, default: 0)\n");
  printf("   Key column.\n");
  printf("-k [keycode] (0-255, default: 0)\n");
  printf("   Hexadecimal keycode.\n");
}

void devices() {
  struct hid_device_info *enumeration = hid_enumerate(0, 0);
  struct hid_device_info *device_info = enumeration;
  while (device_info != NULL) {
    if (device_info->usage_page == RAW_USAGE_PAGE &&
        device_info->usage == RAW_USAGE_ID) {
      wprintf(L"[%x:%x] %ls / %ls\n", device_info->vendor_id,
              device_info->product_id, device_info->manufacturer_string,
              device_info->product_string);
    }
    device_info = device_info->next;
  }
  hid_free_enumeration(enumeration);
}

void version() {
  send((uint8_t[]){id_get_protocol_version}, 1);
  printf("Version: %u\n", packet[1] << 8 | packet[2]);
}

void uptime() {
  send((uint8_t[]){id_get_keyboard_value, id_uptime}, 2);
  uint32_t uptime =
      packet[2] << 24 | packet[3] << 16 | packet[4] << 8 | packet[5];
  printf("Uptime: %u\n", uptime);
}

void get_rgb_brightness() {
  send((uint8_t[]){id_lighting_get_value, id_qmk_rgblight_brightness}, 2);
  printf("Brightness: %hhu\n", packet[2]);
}

void get_rgb_mode() {
  send((uint8_t[]){id_lighting_get_value, id_qmk_rgblight_effect}, 2);
  printf("Mode: %hhu\n", packet[2]);
}

void get_rgb_speed() {
  send((uint8_t[]){id_lighting_get_value, id_qmk_rgblight_effect_speed}, 2);
  printf("Speed: %hhu\n", packet[2]);
}

void get_rgb_colour() {
  send((uint8_t[]){id_lighting_get_value, id_qmk_rgblight_color}, 2);
  printf("Hue: %hhu\n", packet[2]);
  printf("Saturation: %hhu\n", packet[3]);
}

void set_rgb_brightness() {
  send((uint8_t[]){id_lighting_set_value, id_qmk_rgblight_color,
                   flag_brightness},
       3);
  printf("Brightness: %hhu\n", packet[2]);
}

void set_rgb_mode() {
  send((uint8_t[]){id_lighting_set_value, id_qmk_rgblight_effect, flag_mode},
       3);
  printf("Mode: %hhu\n", packet[2]);
}

void set_rgb_speed() {
  send((uint8_t[]){id_lighting_set_value, id_qmk_rgblight_effect_speed,
                   flag_speed},
       3);
  printf("Speed: %hhu\n", packet[2]);
}

void set_rgb_colour() {
  send((uint8_t[]){id_lighting_set_value, id_qmk_rgblight_color, flag_hue,
                   flag_saturation},
       4);
  printf("Hue: %hhu\n", packet[2]);
  printf("Saturation: %hhu\n", packet[3]);
}

void get_keycode() {
  send((uint8_t[]){id_dynamic_keymap_get_keycode, flag_layer, flag_row,
                   flag_column},
       4);
  printf("Layer: %hhu Row: %hhu Column: %hhu\n", packet[1], packet[2],
         packet[3]);
  unsigned short keycode = packet[4] << 8 | packet[5];
  char *keyname =
      (keycode < sizeof(qmk_keycodes)) ? qmk_keycodes[keycode] : "OTHER";
  printf("Keycode: 0x%hx (%s)\n", keycode, keyname);
}

void set_keycode() {
  send((uint8_t[]){id_dynamic_keymap_set_keycode, flag_layer, flag_row,
                   flag_column, flag_keycode >> 8, flag_keycode & 0xff},
       6);
  unsigned short keycode = packet[4] << 8 | packet[5];
  char *keyname =
      (keycode < sizeof(qmk_keycodes)) ? qmk_keycodes[keycode] : "OTHER";
  printf("Keycode: 0x%hx (%s)\n", keycode, keyname);
}

void open_device(char *id) {
  unsigned short vendor_id, product_id;
  if (2 != sscanf(id, "%hx:%hx", &vendor_id, &product_id)) {
    printf("Cannot parse ID\n");
    exit(EXIT_FAILURE);
  }

  struct hid_device_info *enumeration = hid_enumerate(0, 0);
  struct hid_device_info *device_info = enumeration;
  while (device_info != NULL) {
    if (device_info->vendor_id == vendor_id &&
        device_info->product_id == product_id &&
        device_info->usage_page == RAW_USAGE_PAGE &&
        device_info->usage == RAW_USAGE_ID) {
      break;
    }
    device_info = device_info->next;
  }

  if (device_info == NULL) {
    fprintf(stderr, "No such device: %s\n", id);
    exit(EXIT_FAILURE);
  }

  flag_device = hid_open_path(device_info->path);
  if (flag_device == NULL) {
    perror("Cannot open device\n");
    exit(EXIT_FAILURE);
  }

  hid_free_enumeration(enumeration);
}

void u8(char *arg, uint8_t *dest, char *name) {
  if (sscanf(arg, "%hhu", dest) != 1) {
    fprintf(stderr, "Invalid %s: %s\n", name, arg);
    exit(EXIT_FAILURE);
  }
}

void x16(char *arg, unsigned short *dest, char *name) {
  if (sscanf(arg, "%hx", dest) != 1) {
    fprintf(stderr, "Invalid %s: %s\n", name, arg);
    exit(EXIT_FAILURE);
  }
}

void cleanup() {
  if (flag_device != NULL) {
    hid_close(flag_device);
  }
  hid_exit();
}

int main(int argc, char **argv) {
  if (0 > hid_init()) {
    perror("hid_init() failed.");
    return 1;
  }
  atexit(cleanup);

  char *cmd = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "-d:m:s:b:h:S:r:c:l:k:")) != -1) {
    switch (opt) {
    case 1:
      if (cmd != NULL) {
        help();
        exit(EXIT_FAILURE);
      }
      cmd = optarg;
      break;
    case 'd':
      open_device(optarg);
      break;
    case 'm':
      u8(optarg, &flag_mode, "mode");
      break;
    case 's':
      u8(optarg, &flag_speed, "speed");
      break;
    case 'b':
      u8(optarg, &flag_brightness, "brightness");
      break;
    case 'h':
      u8(optarg, &flag_hue, "hue");
      break;
    case 'S':
      u8(optarg, &flag_saturation, "saturation");
      break;
    case 'r':
      u8(optarg, &flag_row, "row");
      break;
    case 'c':
      u8(optarg, &flag_column, "column");
      break;
    case 'l':
      u8(optarg, &flag_layer, "layer");
      break;
    case 'k':
      x16(optarg, &flag_keycode, "keycode");
      break;
    default:
      help();
      exit(EXIT_FAILURE);
    }
  }

  if (cmd == NULL || strcmp(cmd, "help") == 0) {
    help();
  } else if (strcmp(cmd, "devices") == 0) {
    devices();
  } else if (strcmp(cmd, "version") == 0) {
    version();
  } else if (strcmp(cmd, "uptime") == 0) {
    uptime();
  } else if (strcmp(cmd, "get_rgb_brightness") == 0) {
    get_rgb_brightness();
  } else if (strcmp(cmd, "get_rgb_mode") == 0) {
    get_rgb_mode();
  } else if (strcmp(cmd, "get_rgb_speed") == 0) {
    get_rgb_speed();
  } else if (strcmp(cmd, "get_rgb_colour") == 0) {
    get_rgb_colour();
  } else if (strcmp(cmd, "set_rgb_brightness") == 0) {
    set_rgb_brightness();
  } else if (strcmp(cmd, "set_rgb_mode") == 0) {
    set_rgb_mode();
  } else if (strcmp(cmd, "set_rgb_speed") == 0) {
    set_rgb_speed();
  } else if (strcmp(cmd, "set_rgb_colour") == 0) {
    set_rgb_colour();
  } else if (strcmp(cmd, "get_keycode") == 0) {
    get_keycode();
  } else if (strcmp(cmd, "set_keycode") == 0) {
    set_keycode();
  } else {
    help();
  }

  exit(EXIT_SUCCESS);
}
