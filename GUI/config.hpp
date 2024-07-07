#include <gtk/gtk.h>

class Config {
public:
  //the default values are set my GTK3, not the class itself.
  //go to GUI.cpp to check out default config values.
  //everything here should be false.
  bool ESP = false;
  bool ESPbox = false;
  bool ESPname = false;
  bool ESPhealthbar = false;
  bool ESPhealthtext = false;

  bool AIM = false;
  float AIMsmooth = 0;

  bool BHOP = false;
};

inline Config* config = new Config;

static void esp_master_toggle() {
  config->ESP = !config->ESP;
}

static void esp_box_toggle() {
  config->ESPbox = !config->ESPbox; 
}

static void esp_name_toggle() {
  config->ESPname = !config->ESPname; 
}

static void esp_health_bar_toggle() {
  config->ESPhealthbar = !config->ESPhealthbar; 
}

static void esp_health_text_toggle() {
  config->ESPhealthtext = !config->ESPhealthtext; 
}

static void aim_master_toggle() {
  config->AIM = !config->AIM;
}

static void aim_smooth_slider(GtkRange* self) {
  config->AIMsmooth = gtk_range_get_value(self);
}

static void bhop_master_toggle() {
  config->BHOP = !config->BHOP;
}
