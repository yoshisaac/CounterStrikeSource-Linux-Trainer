#include "GUI.hpp"

#include "config.hpp" //Config class and setter functions

static void activate(GtkApplication* app, gpointer user_data) {
  GtkWidget* window;
  
  GtkWidget* ESPmaster;
  GtkWidget* ESPbox;
  GtkWidget* ESPskeleton;
  GtkWidget* ESPdot;
  GtkWidget* ESPname;
  GtkWidget* ESPhealthbar;
  GtkWidget* ESPhealthtext;
  GtkWidget* ESPboxcolor;
  GtkWidget* ESPskeletoncolor;
  GtkWidget* AIMmaster;

  GtkWidget* AIMsmooth;
  GtkWidget* AIMbone;

  GtkWidget* BHOPmaster;
  
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
  gtk_grid_attach(GTK_GRID(grid), ESPbox, 0, 2, 2, 1);

  ESPskeleton = gtk_check_button_new_with_label("Skeleton");
  g_signal_connect(ESPskeleton, "toggled", G_CALLBACK(esp_skeleton_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ESPskeleton), false);
  gtk_grid_attach(GTK_GRID(grid), ESPskeleton, 0, 3, 2, 1);

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

  ESPboxcolor = gtk_color_button_new();
  g_signal_connect(ESPboxcolor, "color-set", G_CALLBACK(esp_box_color), NULL);
  gtk_grid_attach(GTK_GRID(grid), ESPboxcolor, 0, 8, 2, 1);

  ESPskeletoncolor = gtk_color_button_new();
  g_signal_connect(ESPskeletoncolor, "color-set", G_CALLBACK(esp_skeleton_color), NULL);
  gtk_grid_attach(GTK_GRID(grid), ESPskeletoncolor, 0, 9, 2, 1);
  
  /* AIMBOT GUI configuration */
  AIMmaster = gtk_check_button_new_with_label("Aimbot Master Toggle");
  g_signal_connect(AIMmaster, "toggled", G_CALLBACK(aim_master_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AIMmaster), true);
  gtk_grid_attach(GTK_GRID(grid), AIMmaster, 2, 1, 2, 1);

  AIMsmooth = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 0.5);
  g_signal_connect(AIMsmooth, "value-changed", G_CALLBACK(aim_smooth_slider), AIMsmooth);
  gtk_grid_attach(GTK_GRID(grid), AIMsmooth, 2, 2, 2, 1);

  /* BHOP GUI configuration */
  BHOPmaster = gtk_check_button_new_with_label("Bhop Master Toggle");
  g_signal_connect(BHOPmaster, "toggled", G_CALLBACK(bhop_master_toggle), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(BHOPmaster), true);
  gtk_grid_attach(GTK_GRID(grid), BHOPmaster, 5, 1, 2, 1);
  
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
