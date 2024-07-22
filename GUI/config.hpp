#include <gtk/gtk.h>

class Config {
public:
  //the default values are set by GTK3, not the class itself.
  //go to GUI.cpp to check out default config values.
  //everything here should be false.
  bool ESP = false;
  bool ESPbox = false;
  bool ESPskeleton = false;
  bool ESPdot = false;
  bool ESPname = false;
  bool ESPhealthbar = false;
  bool ESPhealthtext = false;
  int ESPboxcolor[4] = {255, 255, 255, 255}; //red, green, blue, alpha
  int ESPskeletoncolor[4] = {255, 255, 255, 255};

  bool AIM = false;
  float AIMsmooth = 0;
  float AIMbone = 0;
  
  bool BHOP = false;
};


inline Config* config = new Config;

static void esp_master_toggle() {
  config->ESP = !config->ESP;
}

static void esp_box_toggle() {
  config->ESPbox = !config->ESPbox; 
}

static void esp_skeleton_toggle() {
  config->ESPskeleton = !config->ESPskeleton; 
}

static void esp_dot_toggle() {
  config->ESPdot = !config->ESPdot; 
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

static void esp_box_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    //GdkRGBA has ranges between 0.0 and 1.0, and we need to convert them to standard 0-255 for X11
    config->ESPboxcolor[0] = (int)(255 * color.red);
    config->ESPboxcolor[1] = (int)(255 * color.green);
    config->ESPboxcolor[2] = (int)(255 * color.blue);
    config->ESPboxcolor[3] = (int)(255 * color.alpha);
}

static void esp_skeleton_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    //GdkRGBA has ranges between 0.0 and 1.0, and we need to convert them to standard 0-255 for X11
    config->ESPskeletoncolor[0] = (int)(255 * color.red);
    config->ESPskeletoncolor[1] = (int)(255 * color.green);
    config->ESPskeletoncolor[2] = (int)(255 * color.blue);
    config->ESPskeletoncolor[3] = (int)(255 * color.alpha);
}

static void aim_master_toggle() {
  config->AIM = !config->AIM;
}

static void aim_smooth_slider(GtkRange* self) {
  config->AIMsmooth = gtk_range_get_value(self);
}

static void aim_bone_slider(GtkRange* self) {
  config->AIMbone = gtk_range_get_value(self);
}

static void bhop_master_toggle() {
  config->BHOP = !config->BHOP;
}
