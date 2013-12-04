#ifndef UIACTIONS_H
#define UIACTIONS_H
#include "utils.h"

#define ACTION_EXPORT extern "C" G_MODULE_EXPORT

ACTION_EXPORT void on_button_select_file_clicked(GtkButton *button, gpointer user_data);
ACTION_EXPORT void on_checkbutton_match_toggled(GtkToggleButton *togglebutton, gpointer user_data);
ACTION_EXPORT void on_button_filter_clicked(GtkButton *button, gpointer user_data);
ACTION_EXPORT void on_treeview_selection_objects_changed(GtkTreeSelection *treeselection, gpointer user_data);

ACTION_EXPORT void on_entry_file_activate(GtkEntry *entry, gpointer  user_data);
ACTION_EXPORT void on_entry_filter_activate(GtkEntry *entry, gpointer  user_data);
ACTION_EXPORT void on_checkbutton_enable_filter_toggled(GtkToggleButton *togglebutton, gpointer user_data);


#endif // UIACTIONS_H
