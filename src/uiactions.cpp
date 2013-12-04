#include "uiactions.h"
#include "utils.h"
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <algorithm>

extern GtkWidget *g_main_window ;
extern GThread *g_uiThread;
extern Json::Value g_config;

extern GHashTable *g_ht_main;
extern gboolean g_enteruiloop;

using namespace std;

struct DelObj {
    void operator()(gpointer* data) {
        g_free(data);
    }
};
class ObjectInfo {
public:
    int refCnt;
    string id;
    string type;
    list<string> lines;
    ObjectInfo() {
        refCnt = 0;
        lines.clear();
    }
    void addLine(string line) {
        lines.push_back(line);
    };
    ~ObjectInfo() {
//        for_each(lines.begin(), lines.end(), DelObj());
        lines.clear();
    };
};
map<string, ObjectInfo*> obj2lines;

GtkTreeStore *treeStore = NULL;
GtkTreeView *treeView = NULL;
GtkTreeSelection *treeSelection = NULL;
GtkTreeViewColumn *treeColumn1 = NULL;

GtkTextView *textview_result = NULL;
GtkTextBuffer *textbuffer1 = NULL;
GtkScrolledWindow *scrolledwindow_result = NULL;
void initUIVars() {
    if(treeStore)
        return;
    treeView = GTK_TREE_VIEW(ste::GtkUtils::getWidget("treeview_objects", g_ht_main));
    treeColumn1 = GTK_TREE_VIEW_COLUMN(g_hash_table_lookup(g_ht_main, "treeviewcolumn1"));
    treeStore = GTK_TREE_STORE(g_hash_table_lookup(g_ht_main, "treestore_objects"));

    textview_result = GTK_TEXT_VIEW(ste::GtkUtils::getWidget("textview_result", g_ht_main));
    textbuffer1 = gtk_text_view_get_buffer(textview_result);
    scrolledwindow_result = GTK_SCROLLED_WINDOW(ste::GtkUtils::getWidget("scrolledwindow_result", g_ht_main));
}

void clearTextView() {
    if(gtk_text_buffer_get_char_count(textbuffer1) <= 0)
        return;
    GtkTextIter startIter, endIter;
    gtk_text_buffer_get_start_iter(textbuffer1, &startIter);
    gtk_text_buffer_get_end_iter(textbuffer1, &endIter);
    gtk_text_buffer_delete(textbuffer1, &startIter, &endIter);
}

void loadFileContent(string sFileName) {
        obj2lines.clear();
        gtk_tree_store_clear(treeStore);
        clearTextView();

        ifstream in(sFileName.c_str(), std::ios::in|std::ios::binary);
        if(!in.is_open()) {
            LOG_WARN("can't open file: %s", sFileName.c_str());
            return;
        }
        static int BUFSIZE = 1<<10;
        char buf[BUFSIZE];
        gboolean needLines = false;
        string lastKey;
        ObjectInfo *lastLines;
        static GRegex *reg = g_regex_new("\\s+object\\s+(\\S+),\\s*([^;]+)",
                                           (GRegexCompileFlags)0,
                                           (GRegexMatchFlags)0,
                                           NULL);
        static GRegex *regRefNum = g_regex_new("(\\d+)\\s*\\-\\>\\s*(\\d+)\\s*$",
                                           (GRegexCompileFlags)0,
                                           (GRegexMatchFlags)0,
                                           NULL);

        // read file content
        while(!in.eof()) {
            in.getline(buf, BUFSIZE);
            int type = 0;
            if(buf[0] == '#') {
                if(needLines) {
                    type = '#';
                    lastLines->addLine(buf);
                }
            } else if(buf[1] == '+') {
                if(buf[2] == '+') {
                    type = 2;
                } else if(buf[2] == ' ') {
                    type = 1;
                }
            } else if(buf[1] == '-') {
                if(buf[2] == '-') {
                    type = -2;
                } else if(buf[2] == ' ') {
                    type = -1;
                }
            }

            needLines = type != 0;
            if(type != 0 && type != '#') {
                GMatchInfo *mc = NULL;
                if(g_regex_match(reg, buf, (GRegexMatchFlags)0, &mc)) {
                    gchar* id = g_match_info_fetch(mc, 1);
                    gchar* objType = g_match_info_fetch(mc, 2);
                    if(type == 2) {
                        lastLines = new ObjectInfo();
                        lastLines->id = id;
                        lastLines->type = objType;
                        obj2lines[id] = lastLines;
                    } else {
                        lastLines = obj2lines[id];
                    }

                    gint cnt1=-1, cnt2 = -1;
                    GMatchInfo *mcRef = NULL;
                    if(g_regex_match(regRefNum, buf, (GRegexMatchFlags)0, &mcRef)) {
                            gchar* refCnt = g_match_info_fetch(mcRef, 1);
                            cnt1 = atoi(refCnt);
                            g_free(refCnt);
                            refCnt = g_match_info_fetch(mcRef, 2);
                            cnt2 = atoi(refCnt);
                            g_free(refCnt);
                    }
                    g_match_info_free(mcRef);

                    if(!lastLines) {
                        if(cnt1 >0 || cnt2 > 0) {
                            lastLines = new ObjectInfo();
                            lastLines->id = id;
                            lastLines->type = objType;
                            obj2lines[id] = lastLines;
                            lastLines->refCnt = cnt1;
                        } else {
                            SLOG_WARN("no create object and no ref match n->m: %s, %d->%d", buf, cnt1, cnt2);
                        }
                    }

                    if(lastLines) {
                        if(cnt2 > 0) {
                            lastLines->refCnt = cnt2;
                        } else if(type < 0) {
                            lastLines->refCnt--;
                        } else {
                            lastLines->refCnt++;
                        }
                        if(type == -2) {
//                            if(lastLines->refCnt>0) {
//                                LOG_WARN("destroy obj %s(%s), and ref %d > 0", id, objType, lastLines->refCnt);
//                            }
                            obj2lines.erase(id);
                            delete lastLines;
                            lastLines = NULL;
                            needLines = false;
                        } else {
                            lastLines->addLine(buf);
                        }
                        g_free(id);
                        g_free(objType);
                    }
                }
                g_match_info_free(mc);
                mc = NULL;
            }


        } // read file

        // add info to tree

        GtkTreeIter root;
        gtk_tree_store_append(treeStore, &root, NULL);
        gtk_tree_store_set(treeStore, &root, 0, "所有Ojbects", 1, "", -1);
        for(map<string, ObjectInfo*>::iterator it = obj2lines.begin(); it != obj2lines.end(); it++) {
            string key = it->first;
            string type = it->second->type;
            int refcnt = it->second->refCnt;
            char refcntStr[32];
            sprintf(refcntStr, "%d", refcnt);
            //type.append("(").append(key).append(")").append(refcntStr);
            GtkTreeIter itemAdd;
            gtk_tree_store_append(treeStore, &itemAdd, &root);
            gtk_tree_store_set(treeStore, &itemAdd, 0, type.c_str(), 1, key.c_str(), 2, refcnt, -1);
        }
}

ACTION_EXPORT void on_button_select_file_clicked(GtkButton *button, gpointer user_data)
{
    initUIVars();
//    SET_ENTRY_TEXT("entry_file", "", g_ht_main);

    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
                              GTK_WINDOW(g_main_window),
                              GTK_FILE_CHOOSER_ACTION_OPEN,
                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                              GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                              NULL);
    gint ret = gtk_dialog_run (GTK_DIALOG (dialog));
    if(ret != GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(dialog);
        return;
    }
//    if (ret == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        string sFileName(filename);
        g_free (filename);
        gtk_widget_destroy(dialog);
        SET_ENTRY_TEXT("entry_file", sFileName.c_str(), g_ht_main);

        loadFileContent(sFileName);


    }
}

void addStringToTxtview(string data) {
    static GtkTextView *textView = GTK_TEXT_VIEW(ste::GtkUtils::getWidget("textview_result", g_ht_main));
    static GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(textView);
    GtkTextIter endIter;
    gtk_text_buffer_get_end_iter(textbuffer, &endIter);
    gtk_text_buffer_insert(textbuffer, &endIter, data.c_str(), -1);

//    GtkScrolledWindow *scrollWindow = GTK_SCROLLED_WINDOW(getWidget("scrolledwindow_result", g_ht_main));
//    GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment(scrollWindow);
//    gtk_adjustment_set_value(vAdjustment, gtk_adjustment_get_upper(vAdjustment));
//    gtk_scrolled_window_set_vadjustment(scrollWindow, vAdjustment);
}

string objid;
string filter;
gboolean showMatch = false;
gboolean enableFilter = false;
void showSelected() {
    clearTextView();

    GRegex *reg = (enableFilter && filter.size() > 0) ? g_regex_new(filter.c_str(),
                                   (GRegexCompileFlags)0,
                                   (GRegexMatchFlags)0,
                                   NULL) : NULL;
    ObjectInfo *info = obj2lines[objid];
    if(info) {
        gboolean isMatch, show;
        for(list<string>::iterator it = info->lines.begin(); it!=info->lines.end(); it++) {
            show = true;
            if(enableFilter && reg) {
                isMatch = g_regex_match(reg, it->c_str(), (GRegexMatchFlags)0, NULL);
                show = showMatch ? isMatch : !isMatch;
            }
            if(show) {
                addStringToTxtview(*it);
                addStringToTxtview("\n");
            }
        }
    }

    if(reg) {
        g_regex_unref(reg);
    }
}

ACTION_EXPORT void on_button_filter_clicked(GtkButton *button, gpointer user_data)
{
    initUIVars();

    const gchar* filterStr = GET_ENTRY_TEXT("entry_filter", g_ht_main);
    GError *err = NULL;
    if(filterStr && *filterStr != 0) {
        GRegex *regtest = g_regex_new(filterStr,
                                           (GRegexCompileFlags)0,
                                           (GRegexMatchFlags)0,
                                           &err);

        if(regtest)
            g_regex_unref(regtest);
    }
    if(err) {
        LOG_GERROR("can't compile regex", err);
    } else {
        showMatch = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ste::GtkUtils::getWidget("checkbutton_match", g_ht_main)));
        filter.clear();
        filter.append(filterStr);
        showSelected();
    }


}
ACTION_EXPORT void on_treeview_selection_objects_changed(GtkTreeSelection *treeselection, gpointer user_data)
{
    initUIVars();
    GValue val = {0};
    GtkTreeIter iter;
    if(!gtk_tree_selection_get_selected(treeselection, (GtkTreeModel**)&treeStore, &iter))
        return;

    gtk_tree_model_get_value(GTK_TREE_MODEL(treeStore), &iter, 1, &val);

    string id((gchar*)val.data[0].v_pointer);
//    LOG_DEBUG("selected value: %s", id.c_str());

    if(id.size() > 0) {
        objid.clear();
        objid.append(id);
        showSelected();
    }

    g_value_unset(&val);
}

ACTION_EXPORT void on_entry_file_activate(GtkEntry *entry, gpointer  user_data)
{
    initUIVars();
    loadFileContent(GET_ENTRY_TEXT("entry_file", g_ht_main));
}
ACTION_EXPORT void on_entry_filter_activate(GtkEntry *entry, gpointer  user_data)
{
    on_button_filter_clicked(GTK_BUTTON(ste::GtkUtils::getWidget("button_filter", g_ht_main)), NULL);
}
ACTION_EXPORT void on_checkbutton_match_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    on_button_filter_clicked(GTK_BUTTON(ste::GtkUtils::getWidget("button_filter", g_ht_main)), NULL);
}
ACTION_EXPORT void on_checkbutton_enable_filter_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    enableFilter = gtk_toggle_button_get_active(togglebutton);
    gtk_widget_set_sensitive(ste::GtkUtils::getWidget("box_filter", g_ht_main), enableFilter);
    on_button_filter_clicked(GTK_BUTTON(ste::GtkUtils::getWidget("button_filter", g_ht_main)), NULL);
}




