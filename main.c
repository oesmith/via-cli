#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hidapi.h>

#include "commands.h"
#include "keycodes.h"

#define PACKET_SIZE 32

#define RAW_USAGE_PAGE 0xff60
#define RAW_USAGE_ID 0x61

#define CMD(device, data) command(device, data, sizeof(data))

#undef DEBUG


static unsigned char PACKET[PACKET_SIZE];

void dump_packet() {
#ifdef DEBUG
  printf("__ __ __ __ __ __ __ __\n");
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", PACKET[0], PACKET[1],
         PACKET[2], PACKET[3], PACKET[4], PACKET[5], PACKET[6], PACKET[7]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", PACKET[8], PACKET[9],
         PACKET[10], PACKET[11], PACKET[12], PACKET[13], PACKET[14],
         PACKET[15]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", PACKET[16], PACKET[17],
         PACKET[18], PACKET[19], PACKET[20], PACKET[21], PACKET[22],
         PACKET[23]);
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", PACKET[24], PACKET[25],
         PACKET[26], PACKET[27], PACKET[28], PACKET[29], PACKET[30],
         PACKET[31]);
#endif
}

unsigned char *command(hid_device *device, unsigned char *data, int len) {
  memset(PACKET, 0, PACKET_SIZE);
  memcpy(PACKET + 1, data, len);

  dump_packet();

  if (hid_write(device, PACKET, PACKET_SIZE) != PACKET_SIZE) {
    perror("Error writing request\n");
    return NULL;
  }

  if (hid_read_timeout(device, PACKET, PACKET_SIZE, 500) != PACKET_SIZE) {
    perror("Error reading response\n");
    return NULL;
  }

  dump_packet();

  return PACKET;
}

int help() {
  printf("Usage: via [command] [args...]\n");
  printf("\nBasic:\n");
  printf("- devices\n");
  printf("- version [vendor:product]\n");
  printf("- uptime [vendor:product]\n");
  printf("Keymap:\n");
  printf("- get_keycode [vendor:product] [layer] [row] [column]\n");
  printf("- set_keycode [vendor:product] [layer] [row] [column] [keycode]\n");
  printf("RGB:\n");
  printf("- get_rgb_brightness [vendor:product]\n");
  printf("- get_rgb_mode [vendor:product]\n");
  printf("- get_rgb_speed [vendor:product]\n");
  printf("- get_rgb_colour [vendor:product]\n");
  printf("- set_rgb_brightness [vendor:product] [brightness]\n");
  printf("- set_rgb_mode [vendor:product] [mode]\n");
  printf("- set_rgb_speed [vendor:product] [speed]\n");
  printf("- set_rgb_colour [vendor:product] [hue] [saturation]\n");
  return 0;
}

int devices() {
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
  return 0;
}

int version(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_get_protocol_version};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Version: %u\n", res[1] << 8 | res[2]);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int uptime(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_get_keyboard_value, id_uptime};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    uint32_t uptime = res[2] << 24 | res[3] << 16 | res[4] << 8 | res[5];
    printf("Uptime: %u\n", uptime);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int get_rgb_brightness(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_lighting_get_value, id_qmk_rgblight_brightness};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Brightness: %u\n", res[2]);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int get_rgb_mode(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_lighting_get_value, id_qmk_rgblight_effect};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Mode: %u\n", res[2]);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int get_rgb_speed(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_lighting_get_value, id_qmk_rgblight_effect_speed};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Speed: %u\n", res[2]);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int get_rgb_colour(hid_device *device) {
  if (device == NULL) {
    return 1;
  }

  unsigned char req[] = {id_lighting_get_value, id_qmk_rgblight_color};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Hue: %u\n", res[2]);
    printf("Saturation: %u\n", res[3]);
  }

  hid_close(device);
  return res != NULL ? 0 : 1;
}

int set_rgb_brightness(hid_device *device, unsigned char brightness) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_lighting_set_value, id_qmk_rgblight_color,
                         brightness};
  unsigned char *res = CMD(device, req);
  hid_close(device);
  return res != NULL ? 0 : 1;
}

int set_rgb_mode(hid_device *device, unsigned char mode) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_lighting_set_value, id_qmk_rgblight_effect, mode};
  unsigned char *res = CMD(device, req);
  hid_close(device);
  return res != NULL ? 0 : 1;
}

int set_rgb_speed(hid_device *device, unsigned char speed) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_lighting_set_value, id_qmk_rgblight_effect_speed,
                         speed};
  unsigned char *res = CMD(device, req);
  hid_close(device);
  return res != NULL ? 0 : 1;
}

int set_rgb_colour(hid_device *device, unsigned char hue,
                   unsigned char saturation) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_lighting_set_value, id_qmk_rgblight_color, hue,
                         saturation};
  unsigned char *res = CMD(device, req);
  hid_close(device);
  return res != NULL ? 0 : 1;
}

int get_keycode(hid_device *device, unsigned char layer, unsigned char row,
                unsigned char column) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_dynamic_keymap_get_keycode, layer, row, column};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Layer: %hhu Row: %hhu Column: %hhu\n", res[1], res[2], res[3]);
    unsigned short keycode = res[4] << 8 | res[5];
    char* keyname = (keycode < sizeof(qmk_keycodes)) ? qmk_keycodes[keycode] : "OTHER";
    printf("Keycode: 0x%hx (%s)\n", keycode, keyname);
  }
  hid_close(device);
  return res != NULL ? 0 : 1;
}

int set_keycode(hid_device *device, unsigned char layer, unsigned char row,
                unsigned char column, unsigned short keycode) {
  if (device == NULL) {
    return 1;
  }
  unsigned char req[] = {id_dynamic_keymap_set_keycode,
                         layer,
                         row,
                         column,
                         keycode >> 8,
                         keycode & 0xff};
  unsigned char *res = CMD(device, req);
  if (res != NULL) {
    printf("Layer: %hhu Row: %hhu Column: %hhu\n", res[1], res[2], res[3]);
    printf("Keycode: 0x%hx\n", res[4] << 8 | res[5]);
  }
  hid_close(device);
  return res != NULL ? 0 : 1;
}

hid_device *open_device(char *id) {
  unsigned short vendor_id, product_id;
  if (2 != sscanf(id, "%hx:%hx", &vendor_id, &product_id)) {
    printf("Cannot parse ID\n");
    return NULL;
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
    printf("No such device\n");
    return NULL;
  }

  hid_device *device = hid_open_path(device_info->path);
  if (device == NULL) {
    perror("Cannot open device\n");
    return NULL;
  }

  hid_free_enumeration(enumeration);

  return device;
}

unsigned char u8(char *arg) {
  unsigned char ret = 0;
  if (sscanf(arg, "%hhu", &ret) != 1) {
    fprintf(stderr, "Can't parse argument: %s\n", arg);
  }
  return ret;
}

unsigned short x16(char *arg) {
  unsigned short ret = 0;
  if (sscanf(arg, "%hx", &ret) != 1) {
    fprintf(stderr, "Can't parse argument: %s\n", arg);
  }
  return ret;
}

void cleanup() { hid_exit(); }

int main(int argc, char **argv) {
  if (0 > hid_init()) {
    perror("hid_init() failed.");
    return 1;
  }
  atexit(cleanup);

  memset(PACKET, 0, PACKET_SIZE);

  if (argc == 1) {
    return help();
  } else if (strcmp(argv[1], "devices") == 0) {
    return devices();
  } else if (strcmp(argv[1], "version") == 0 && argc == 3) {
    return version(open_device(argv[2]));
  } else if (strcmp(argv[1], "uptime") == 0 && argc == 3) {
    return uptime(open_device(argv[2]));
  } else if (strcmp(argv[1], "get_rgb_brightness") == 0 && argc == 3) {
    return get_rgb_brightness(open_device(argv[2]));
  } else if (strcmp(argv[1], "get_rgb_mode") == 0 && argc == 3) {
    return get_rgb_mode(open_device(argv[2]));
  } else if (strcmp(argv[1], "get_rgb_speed") == 0 && argc == 3) {
    return get_rgb_speed(open_device(argv[2]));
  } else if (strcmp(argv[1], "get_rgb_colour") == 0 && argc == 3) {
    return get_rgb_colour(open_device(argv[2]));
  } else if (strcmp(argv[1], "set_rgb_brightness") == 0 && argc == 4) {
    return set_rgb_brightness(open_device(argv[2]), u8(argv[3]));
  } else if (strcmp(argv[1], "set_rgb_mode") == 0 && argc == 4) {
    return set_rgb_mode(open_device(argv[2]), u8(argv[3]));
  } else if (strcmp(argv[1], "set_rgb_speed") == 0 && argc == 4) {
    return set_rgb_speed(open_device(argv[2]), u8(argv[3]));
  } else if (strcmp(argv[1], "set_rgb_colour") == 0 && argc == 5) {
    return set_rgb_colour(open_device(argv[2]), u8(argv[3]), u8(argv[4]));
  } else if (strcmp(argv[1], "get_keycode") == 0 && argc == 6) {
    return get_keycode(open_device(argv[2]), u8(argv[3]), u8(argv[4]),
                       u8(argv[5]));
  } else if (strcmp(argv[1], "set_keycode") == 0 && argc == 7) {
    return set_keycode(open_device(argv[2]), u8(argv[3]), u8(argv[4]),
                       u8(argv[5]), x16(argv[6]));
  } else {
    return help();
  }
}
