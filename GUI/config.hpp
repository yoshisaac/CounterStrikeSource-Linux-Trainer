#include <gtk/gtk.h>

class Config {
public:
  //the default values are set by GTK3, not the class itself.
  //go to GUI.cpp to check out default config values.
  //everything here should be false. Except colors.
  bool ESP = false;
  bool ESPbox = false;
  int ESPboxcolor[4] = {230, 35, 35, 255}; //red, green, blue, alpha
  bool ESPskeleton = false;
  int ESPskeletoncolor[4] = {255, 255, 255, 255};
  bool ESPname = false;
  bool ESPhealthbar = false;
  bool ESPhealthtext = false;
  bool ESParmorbar = false;
  bool ESParmortext = false;
  bool ESPsnaplines = false;
  int ESPsnaplinescolor[4] = {0, 255, 0, 255};
  bool ESPsnaplineaimbot = false;
  int ESPsnaplineaimbotcolor[4] = {255, 255, 255, 255};
  bool ESPcrosshair = false;
  int ESPcrosshaircolor[4] = {255, 255, 0, 255};
  bool ESPcrosshairRCS = false;
  int ESPcrosshairRCScolor[4] = {0, 255, 240, 255};
  
  bool AIM = false;
  float AIMsmooth = 0;
  bool AIMrecoilcompensation = false;
  int AIMhitbox = 0;
  
  bool BHOP = false;

  bool ROmode = false;
};


inline Config* config = new Config;

static void esp_master_toggle() {
  config->ESP = !config->ESP;
}

static void esp_box_toggle() {
  config->ESPbox = !config->ESPbox; 
}

static void esp_box_color(GtkColorButton *color_button) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    //GdkRGBA has ranges between 0.0 and 1.0, and we need to convert them to standard 0-255 for X11
    config->ESPboxcolor[0] = (int)(255 * color.red);
    config->ESPboxcolor[1] = (int)(255 * color.green);
    config->ESPboxcolor[2] = (int)(255 * color.blue);
    config->ESPboxcolor[3] = (int)(255 * color.alpha);
}

static void esp_skeleton_toggle() {
  config->ESPskeleton = !config->ESPskeleton; 
}

static void esp_skeleton_color(GtkColorButton *color_button) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    config->ESPskeletoncolor[0] = (int)(255 * color.red);
    config->ESPskeletoncolor[1] = (int)(255 * color.green);
    config->ESPskeletoncolor[2] = (int)(255 * color.blue);
    config->ESPskeletoncolor[3] = (int)(255 * color.alpha);
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

static void esp_armor_bar_toggle() {
  config->ESParmorbar = !config->ESParmorbar;   
}

static void esp_armor_text_toggle() {
  config->ESParmortext = !config->ESParmortext; 
}

static void esp_snaplines_toggle() {
  config->ESPsnaplines = !config->ESPsnaplines;
}

static void esp_snaplines_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    config->ESPsnaplinescolor[0] = (int)(255 * color.red);
    config->ESPsnaplinescolor[1] = (int)(255 * color.green);
    config->ESPsnaplinescolor[2] = (int)(255 * color.blue);
    config->ESPsnaplinescolor[3] = (int)(255 * color.alpha);
}

static void esp_snaplineaimbot_toggle() {
  config->ESPsnaplineaimbot = !config->ESPsnaplineaimbot;
}

static void esp_snaplineaimbot_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    config->ESPsnaplineaimbotcolor[0] = (int)(255 * color.red);
    config->ESPsnaplineaimbotcolor[1] = (int)(255 * color.green);
    config->ESPsnaplineaimbotcolor[2] = (int)(255 * color.blue);
    config->ESPsnaplineaimbotcolor[3] = (int)(255 * color.alpha);
}

static void esp_crosshair_toggle() {
  config->ESPcrosshair = !config->ESPcrosshair;
}

static void esp_crosshair_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    config->ESPcrosshaircolor[0] = (int)(255 * color.red);
    config->ESPcrosshaircolor[1] = (int)(255 * color.green);
    config->ESPcrosshaircolor[2] = (int)(255 * color.blue);
    config->ESPcrosshaircolor[3] = (int)(255 * color.alpha);
}

static void esp_crosshair_rcs_toggle() {
  config->ESPcrosshairRCS = !config->ESPcrosshairRCS;
}

static void esp_crosshair_rcs_color(GtkColorButton *color_button, gpointer user_data) {
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), &color);

    config->ESPcrosshairRCScolor[0] = (int)(255 * color.red);
    config->ESPcrosshairRCScolor[1] = (int)(255 * color.green);
    config->ESPcrosshairRCScolor[2] = (int)(255 * color.blue);
    config->ESPcrosshairRCScolor[3] = (int)(255 * color.alpha);
}

static void aim_master_toggle() {
  config->AIM = !config->AIM;
}

static void aim_smooth_slider(GtkRange* self) {
  config->AIMsmooth = gtk_range_get_value(self);
}

static void aim_recoil_compensation_toggle() {
  config->AIMrecoilcompensation = !config->AIMrecoilcompensation;
}

static void aim_hitbox_dropdown(GtkComboBox* combo) {
  config->AIMhitbox = gtk_combo_box_get_active(combo);
}

static void bhop_master_toggle() {
  config->BHOP = !config->BHOP;
}

static void read_only_toggle() {
  config->ROmode = !config->ROmode;
}
