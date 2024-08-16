#include "GUI.hpp"

#include "config.hpp" //Config class and setter functions

static void activate(GtkApplication* app, gpointer user_data) {
  GtkWidget* window;
  
  GtkWidget* ESPmaster;
  GtkWidget* ESPbox;
  GtkWidget* ESPboxcolor;
  GtkWidget* ESPskeleton;
  GtkWidget* ESPskeletoncolor;
  GtkWidget* ESPdot;
  GtkWidget* ESPname;
  GtkWidget* ESPhealthbar;
  GtkWidget* ESPhealthtext;
  GtkWidget* ESPsnaplines;
  GtkWidget* ESPsnaplinescolor;
  
  GtkWidget* ESPcrosshair;
  GtkWidget* ESPcrosshaircolor;
  GtkWidget* ESPcrosshairRCS;
  GtkWidget* ESPcrosshairRCScolor;
  
  GtkWidget* AIMmaster;
  GtkWidget* AIMsmooth;
  GtkWidget* AIMrecoilcompensation;
  GtkWidget* AIMhitbox;
  
  GtkWidget* BHOPmaster;

  GtkWidget* ROmode; //read only mode
  
  GtkWidget* grid = gtk_grid_new();

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "GNU/Linux CS:S Cheat");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 350);

  
  /* ESP GUI configuration */
  gtk_container_add(GTK_CONTAINER(window), grid);

  ESPmaster = gtk_check_button_new_with_label("ESP Master Toggle");
  g_signal_connect(ESPmaster, "toggled", G_CALLBACK(esp_master_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPmaster), true);
  gtk_grid_attach(GTK_GRID(grid), ESPmaster, 0, 1, 2, 1);

  ESPbox = gtk_check_button_new_with_label("Box");
  g_signal_connect(ESPbox, "toggled", G_CALLBACK(esp_box_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPbox), true);
  gtk_grid_attach(GTK_GRID(grid), ESPbox, 0, 2, 1, 1);

  ESPboxcolor = gtk_color_button_new();
  g_signal_connect(ESPboxcolor, "color-set", G_CALLBACK(esp_box_color), NULL);
  GdkRGBA boxdefcolor; boxdefcolor.red = (230.f/255.f);
  boxdefcolor.green = (35.f/255.f); boxdefcolor.blue = (35.f/255.f);
  boxdefcolor.alpha = 1.0f;
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ESPboxcolor), &boxdefcolor);
  gtk_grid_attach(GTK_GRID(grid), ESPboxcolor, 1, 2, 1, 1);

  ESPskeleton = gtk_check_button_new_with_label("Skeleton");
  g_signal_connect(ESPskeleton, "toggled", G_CALLBACK(esp_skeleton_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPskeleton), false);
  gtk_grid_attach(GTK_GRID(grid), ESPskeleton, 0, 3, 1, 1);
   
  ESPskeletoncolor = gtk_color_button_new();
  g_signal_connect(ESPskeletoncolor, "color-set", G_CALLBACK(esp_skeleton_color), NULL);
  GdkRGBA skeletondefcolor; skeletondefcolor.red = 1.0f;
  skeletondefcolor.green = 1.0f; skeletondefcolor.blue = 1.0f;
  skeletondefcolor.alpha = 1.0f;
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ESPskeletoncolor), &skeletondefcolor);
  gtk_grid_attach(GTK_GRID(grid), ESPskeletoncolor, 1, 3, 1, 1);

  ESPdot = gtk_check_button_new_with_label("Head Dot");
  g_signal_connect(ESPdot, "toggled", G_CALLBACK(esp_dot_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPdot), false);
  gtk_grid_attach(GTK_GRID(grid), ESPdot, 0, 4, 2, 1);
  
  ESPname = gtk_check_button_new_with_label("Name");
  g_signal_connect(ESPname, "toggled", G_CALLBACK(esp_name_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPname), false);
  gtk_grid_attach(GTK_GRID(grid), ESPname, 0, 5, 2, 1);

  ESPhealthbar = gtk_check_button_new_with_label("Health Bar");
  g_signal_connect(ESPhealthbar, "toggled", G_CALLBACK(esp_health_bar_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPhealthbar), false);
  gtk_grid_attach(GTK_GRID(grid), ESPhealthbar, 0, 6, 2, 1);

  ESPhealthtext = gtk_check_button_new_with_label("Health Text");
  g_signal_connect(ESPhealthtext, "toggled", G_CALLBACK(esp_health_text_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPhealthtext), true);
  gtk_grid_attach(GTK_GRID(grid), ESPhealthtext, 0, 7, 2, 1);

  ESPsnaplines = gtk_check_button_new_with_label("Snap Lines");
  g_signal_connect(ESPsnaplines, "toggled", G_CALLBACK(esp_snaplines_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPsnaplines), false);
  gtk_grid_attach(GTK_GRID(grid), ESPsnaplines, 0, 8, 1, 1);

  ESPsnaplinescolor = gtk_color_button_new();
  g_signal_connect(ESPsnaplinescolor, "color-set", G_CALLBACK(esp_snaplines_color), NULL);
  GdkRGBA snaplinesdefcolor; snaplinesdefcolor.red = 0.0f;
  snaplinesdefcolor.green = 1.0f; snaplinesdefcolor.blue = 0.0f;
  snaplinesdefcolor.alpha = 1.0f;
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ESPsnaplinescolor), &snaplinesdefcolor);
  gtk_grid_attach(GTK_GRID(grid), ESPsnaplinescolor, 1, 8, 1, 1);

  ESPcrosshair = gtk_check_button_new_with_label("Crosshair");
  g_signal_connect(ESPcrosshair, "toggled", G_CALLBACK(esp_crosshair_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPcrosshair), false);
  gtk_grid_attach(GTK_GRID(grid), ESPcrosshair, 0, 9, 1, 1);

  ESPcrosshaircolor = gtk_color_button_new();
  g_signal_connect(ESPcrosshaircolor, "color-set", G_CALLBACK(esp_crosshair_color), NULL);
  GdkRGBA crosshairdefcolor; crosshairdefcolor.red = 1.0f;
  crosshairdefcolor.green = 1.0f; crosshairdefcolor.blue = 0.0f;
  crosshairdefcolor.alpha = 1.0f;
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ESPcrosshaircolor), &crosshairdefcolor);
  gtk_grid_attach(GTK_GRID(grid), ESPcrosshaircolor, 1, 9, 1, 1);

  ESPcrosshairRCS = gtk_check_button_new_with_label("Crosshair RCS");
  g_signal_connect(ESPcrosshairRCS, "toggled", G_CALLBACK(esp_crosshair_rcs_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPcrosshairRCS), false);
  gtk_grid_attach(GTK_GRID(grid), ESPcrosshairRCS, 0, 10, 1, 1);
  
  ESPcrosshairRCScolor = gtk_color_button_new();
  g_signal_connect(ESPcrosshairRCScolor, "color-set", G_CALLBACK(esp_crosshair_rcs_color), NULL);
  GdkRGBA crosshairRCSdefcolor; crosshairRCSdefcolor.red = 0.0f;
  crosshairRCSdefcolor.green = 1.0f; crosshairRCSdefcolor.blue = (240.f/255.f);
  crosshairRCSdefcolor.alpha = 1.0f;
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(ESPcrosshairRCScolor), &crosshairRCSdefcolor);
  gtk_grid_attach(GTK_GRID(grid), ESPcrosshairRCScolor, 1, 10, 1, 1);
  
  /* AIMBOT GUI configuration */
  AIMmaster = gtk_check_button_new_with_label("Aimbot Master Toggle");
  g_signal_connect(AIMmaster, "toggled", G_CALLBACK(aim_master_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AIMmaster), true);
  gtk_grid_attach(GTK_GRID(grid), AIMmaster, 2, 1, 2, 1);

  AIMsmooth = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 0.5);
  g_signal_connect(AIMsmooth, "value-changed", G_CALLBACK(aim_smooth_slider), AIMsmooth);
  gtk_grid_attach(GTK_GRID(grid), AIMsmooth, 2, 2, 2, 1);

  AIMrecoilcompensation = gtk_check_button_new_with_label("Recoil Compensation");
  g_signal_connect(AIMrecoilcompensation, "toggled", G_CALLBACK(aim_recoil_compensation_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AIMrecoilcompensation), false);
  gtk_grid_attach(GTK_GRID(grid), AIMrecoilcompensation, 2, 3, 2, 1);

  AIMhitbox = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(AIMhitbox), NULL, "Head");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(AIMhitbox), NULL, "Upper Body");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(AIMhitbox), NULL, "Lower Body");
  gtk_combo_box_set_active(GTK_COMBO_BOX(AIMhitbox), 0);
  g_signal_connect(AIMhitbox, "changed", G_CALLBACK(aim_hitbox_dropdown), NULL);
  gtk_grid_attach(GTK_GRID(grid), AIMhitbox, 2, 4, 2, 1);
  
  /* BHOP GUI configuration */
  BHOPmaster = gtk_check_button_new_with_label("Bhop Master Toggle");
  g_signal_connect(BHOPmaster, "toggled", G_CALLBACK(bhop_master_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(BHOPmaster), true);
  gtk_grid_attach(GTK_GRID(grid), BHOPmaster, 5, 1, 2, 1);

  // ROmode = gtk_check_button_new_with_label("Read Only Mode");
  // g_signal_connect(ROmode, "toggled", G_CALLBACK(read_only_toggle), NULL);
  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ROmode), false);
  // gtk_grid_attach(GTK_GRID(grid), ROmode, 5, 9, 2, 1);
  
  gtk_widget_show_all (window);
}

void* guiThread(void*) {
  GtkApplication* app;
  int status;
  app = gtk_application_new("org.gtk.csstrainer", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), 0, NULL); //thread halts here until GUI is closed
  g_object_unref(app);
  pthread_kill(pthread_self(), SIGTERM); //we reach this point once they press the X on the window
  return NULL;  
}
