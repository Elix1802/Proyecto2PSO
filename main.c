#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "Files/FileManager.h"
#include "DataStructs/Simulation.h"



static char *selected_file_path = NULL;
static char *generated_file_path = NULL;


static GtkWidget *ramOptArea = NULL;
static GtkWidget *ramAlgArea = NULL;
static int ramOptFrames[100];
static int ramAlgFrames[100];




typedef struct {
    GtkWidget *spinSeed;
    GtkWidget *cmbProcesses;
    GtkWidget *cmbOperations;
    GtkWidget *cmbAlgorithm;
    GtkWidget *lblAlgName;
    GtkWidget *btnDownload;
    GtkWidget *btnStartSimulation;
}AppWidgets;

static AppWidgets widgets;

static const char *dropdownText(GtkWidget *dropdown){
    GObject * item = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(dropdown));
    if(item == NULL){
        return NULL;
    }
    return gtk_string_object_get_string(GTK_STRING_OBJECT(item));
}

//Tabla/////////////////////////////////


typedef struct {
    int pageId;
    int pid;
    int loaded;     // 1 = en RAM
    int lAddr;
    int mAddr;     
    int dAddr;      
    int loadedT;   
    int mark;
} PageRow;

typedef struct {
    GtkWidget     *widget;   
    GtkWidget     *area;
    GtkAdjustment *vadj;
    const PageRow *rows;     
    int            count;
} PageTable;

static const char  *COL_TITLES[8] = {"PAGE ID", "PID", "LOADED", "L-ADDR", "M-ADDR", "D-ADDR", "LOADED-T", "MARK"};
static const double COL_FRAC[8]   = {0.14, 0.10, 0.12, 0.12, 0.12, 0.14, 0.14, 0.12};   

//Definicion de colores a utilizar
static const double PALETTE[][3] = {
    {0.78, 0.92, 0.78},   
    {1.00, 0.93, 0.65},   
    {0.99, 0.80, 0.84},  
    {0.78, 0.85, 0.97},   
    {0.98, 0.80, 0.62},   
    {0.86, 0.80, 0.94},   
};
#define PALETTE_SIZE ((int)(sizeof PALETTE / sizeof PALETTE[0]))

static PageTable *tableOpt = NULL;
static PageTable *tableAlg = NULL;

static void cellText(const PageRow *r, int col, char *buf, size_t n) {
    buf[0] = '\0';
    switch (col) {
        case 0: snprintf(buf, n, "%d", r->pageId); break;
        case 1: snprintf(buf, n, "%d", r->pid); break;
        case 2: if (r->loaded) snprintf(buf, n, "X"); break;
        case 3: snprintf(buf, n, "%d", r->lAddr); break;
        case 4: if (r->mAddr >= 0) snprintf(buf, n, "%d", r->mAddr); break;
        case 5: if (r->dAddr >= 0) snprintf(buf, n, "%d", r->dAddr); break;
        case 6: if (r->loadedT >= 0) snprintf(buf, n, "%ds", r->loadedT); break;
        case 7: if (r->mark) snprintf(buf, n, "X"); break;
    }
}

static void drawCenteredText(cairo_t *cr, const char *text, double x, double y, double w, double h) {
    cairo_font_extents_t fe;
    cairo_text_extents_t te;
    cairo_font_extents(cr, &fe);
    cairo_text_extents(cr, text, &te);
    cairo_move_to(cr, x + (w - te.x_advance) / 2.0, y + (h + fe.ascent - fe.descent) / 2.0);
    cairo_show_text(cr, text);
}

static void drawTable(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    PageTable *t = (PageTable *)data;
    double scroll = gtk_adjustment_get_value(t->vadj);

    double colX[8 + 1];
    colX[0] = 0;
    for (int c = 0; c < 8; c++) {
        colX[c + 1] = colX[c] + COL_FRAC[c] * width;
    }

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    int first = (int)(scroll / 22);
    int last  = (int)((scroll + height - 26) / 22);
    if (first < 0) first = 0;
    if (last >= t->count) last = t->count - 1;

    cairo_save(cr);
    cairo_rectangle(cr, 0, 26, width, height - 26);
    cairo_clip(cr);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);

    for (int i = first; i <= last; i++) {
        const PageRow *r = &t->rows[i];
        double y = 26 + i * 22 - scroll;

        const double *color = PALETTE[r->pid % PALETTE_SIZE];
        cairo_set_source_rgb(cr, color[0], color[1], color[2]);
        cairo_rectangle(cr, 0, y, width, 22);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0.15, 0.15, 0.15);
        for (int c = 0; c < 8; c++) {
            char buf[32];
            cellText(r, c, buf, sizeof buf);
            if (buf[0] != '\0') {
                drawCenteredText(cr, buf, colX[c], y, colX[c + 1] - colX[c], 22);
            }
        }
    }

    cairo_set_source_rgb(cr, 0.65, 0.65, 0.65);
    cairo_set_line_width(cr, 1);
    for (int i = first; i <= last; i++) {
        double y = 26 + (i + 1) * 22 - scroll - 0.5;
        cairo_move_to(cr, 0, y);
        cairo_line_to(cr, width, y);
    }
    cairo_stroke(cr);
    cairo_restore(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, width, 26);
    cairo_fill(cr);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 0, 0, 0);
    for (int c = 0; c < 8; c++) {
        drawCenteredText(cr, COL_TITLES[c], colX[c], 0, colX[c + 1] - colX[c], 26);
    }

    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
    for (int c = 1; c < 8; c++) {
        cairo_move_to(cr, colX[c] + 0.5, 0);
        cairo_line_to(cr, colX[c] + 0.5, height);
    }
    cairo_move_to(cr, 0, 26 - 0.5);
    cairo_line_to(cr, width, 26 - 0.5);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0.5, 0.5, width - 1, height - 1);
    cairo_stroke(cr);
}

static void updateScroll(PageTable *t, int height) {
    double viewport = (height > 26) ? height - 26 : 0;
    double content  = (double)t->count * 22;
    double upper    = (content > viewport) ? content : viewport;

    gtk_adjustment_configure(t->vadj, gtk_adjustment_get_value(t->vadj),
                             0, upper, 22, viewport * 0.9, viewport);
}

static void onTableResize(GtkDrawingArea *area, int width, int height, gpointer data) {
    updateScroll((PageTable *)data, height);
}

static gboolean onTableScroll(GtkEventControllerScroll *ctrl, double dx, double dy, gpointer data) {
    PageTable *t = (PageTable *)data;
    gtk_adjustment_set_value(t->vadj, gtk_adjustment_get_value(t->vadj) + dy * 22 * 3);
    return TRUE;
}

static PageTable *pageTableNew(void) {
    PageTable *t = g_new0(PageTable, 1);

    t->area = gtk_drawing_area_new();
    gtk_widget_set_hexpand(t->area, TRUE);
    gtk_widget_set_vexpand(t->area, TRUE);
    gtk_widget_set_size_request(t->area, 420, 300);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(t->area), drawTable, t, NULL);
    g_signal_connect(t->area, "resize", G_CALLBACK(onTableResize), t);

    t->vadj = gtk_adjustment_new(0, 0, 1, 22, 22 * 5, 1);
    g_signal_connect_swapped(t->vadj, "value-changed", G_CALLBACK(gtk_widget_queue_draw), t->area);

    GtkEventController *scroll = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect(scroll, "scroll", G_CALLBACK(onTableScroll), t);
    gtk_widget_add_controller(t->area, scroll);

    GtkWidget *bar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, t->vadj);

    t->widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(t->widget, TRUE);
    gtk_widget_set_vexpand(t->widget, TRUE);
    gtk_box_append(GTK_BOX(t->widget), t->area);
    gtk_box_append(GTK_BOX(t->widget), bar);
    return t;
}

static void pageTableSetRows(PageTable *t, const PageRow *rows, int count) {
    t->rows = rows;
    t->count = count;
    updateScroll(t, gtk_widget_get_height(t->area));
    gtk_widget_queue_draw(t->area);
}


static void on_create_file_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *w = (AppWidgets *)user_data;

    gtk_spin_button_update(GTK_SPIN_BUTTON(w->spinSeed));  
    unsigned int seed = (unsigned int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(w->spinSeed));

    const char *pText = dropdownText(w->cmbProcesses);
    const char *nText = dropdownText(w->cmbOperations);
    if (pText == NULL || nText == NULL) {
        g_printerr("Selecciona la cantidad de procesos y de operaciones\n");
        return;
    }
    int P = atoi(pText);
    int N = atoi(nText);

    InstructionList list;
    if (generateInstructions(seed, P, N, &list) != 0) {
        g_printerr("Error generando la secuencia\n");
        return;
    }

    g_mkdir_with_parents("Files/Sequences", 0755);

    char fileName[128];
    snprintf(fileName, sizeof fileName,
             "Files/Sequences/SecuenciaProcesos-S%u-P%d-N%d.txt", seed, P, N);
    createFile(fileName, &list);

    freeInstructionList(&list);

    if(!g_file_test(fileName, G_FILE_TEST_IS_REGULAR)){
        g_print("No se pudo crear el archivo %s\n", fileName);
        return;
    }

    g_free(generated_file_path);
    generated_file_path = g_strdup(fileName);

    if(w->btnDownload){
        gtk_widget_set_sensitive(w->btnDownload, TRUE);
    }
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
    GtkAlertDialog *alert = gtk_alert_dialog_new("✅ File created and ready to download");
    gtk_alert_dialog_show(alert, parent_window);
    g_object_unref(alert);
    printf("Archivo generado: %s (semilla: %u, P=%d, N=%d)\n", fileName, seed, P, N);

}

static void showMessage(GtkWindow *parent, const char *message) {
    GtkAlertDialog *alert = gtk_alert_dialog_new("%s", message);
    gtk_alert_dialog_show(alert, parent);
    g_object_unref(alert);
}

static void on_save_dialog_response(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GtkWindow *parent = GTK_WINDOW(user_data);
    GError *error = NULL;

    GFile *dest = gtk_file_dialog_save_finish(dialog, result, &error);
    if (dest) {
        GFile *src = g_file_new_for_path(generated_file_path);
        GError *copyError = NULL;

        if (g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &copyError)) {
            char *path = g_file_get_path(dest);
            char *msg = g_strdup_printf("Archivo guardado en:\n%s", path);
            showMessage(parent, msg);
            g_free(msg);
            g_free(path);
        } else {
            showMessage(parent, copyError->message);
            g_error_free(copyError);
        }

        g_object_unref(src);
        g_object_unref(dest);
    } else if (error) {
        if (!g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
            !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
            g_printerr("Error al elegir destino: %s\n", error->message);
        }
        g_error_free(error);
    }
    g_object_unref(dialog);
}

static void on_start_simulation_button_clicked(GtkButton *button, gpointer user_data) {

  
  GtkLabel *label = GTK_LABEL(user_data);
  
  AppWidgets *w = (AppWidgets *)user_data;

  if (selected_file_path == NULL){
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));
    GtkAlertDialog *alert = gtk_alert_dialog_new("⚠️ Please select or create a file");
    gtk_alert_dialog_show(alert, parent_window);
    g_object_unref(alert);

  
  }
  else{
    g_print("//////////////////Start Simulation//////////////");
    printf("-------Ejecutando-------------------\n");
    printf("selected_file_path: %s \n", selected_file_path);
    runFile(selected_file_path);
    g_print("//////////////////Simulation Finished//////////////");
  }
}

static void on_download_clicked(GtkButton *button, gpointer user_data) {
    if (generated_file_path == NULL) return;

    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Guardar secuencia generada");

    char *baseName = g_path_get_basename(generated_file_path);
    gtk_file_dialog_set_initial_name(dialog, baseName);   
    g_free(baseName);

    gtk_file_dialog_save(dialog, parent, NULL, on_save_dialog_response, parent);
}


static void on_algorithm_changed(GObject *dropdown, GParamSpec *pspec, gpointer user_data) {
    AppWidgets *w = (AppWidgets *)user_data;

    const char *name = dropdownText(GTK_WIDGET(dropdown));
    if (name == NULL) return;

    char text[64];
    snprintf(text, sizeof text, "RAM - %s", name);
    gtk_label_set_text(GTK_LABEL(w->lblAlgName), text);
}

static void on_file_dialog_response(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GtkLabel *label = GTK_LABEL(user_data);
    GError *error = NULL;

    GFile *file = gtk_file_dialog_open_finish(dialog, result, &error);
    if (file) {
        if (selected_file_path != NULL) {
            g_free(selected_file_path);
        }

        selected_file_path = g_file_get_path(file);
        char *filename = g_file_get_basename(file);
        


        gtk_label_set_text(label, filename);
		g_free(filename);
		
        g_object_unref(file);
    } else {
        if (error) {
            if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
                g_printerr("Error al seleccionar archivo: %s\n", error->message);
            }
            g_error_free(error);
        }
    }
    g_object_unref(dialog);
}

static void on_open_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *label = GTK_WIDGET(user_data);
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Seleccionar archivo");

    gtk_file_dialog_open(dialog, parent_window, NULL, on_file_dialog_response, label);
}
static void pidColor(int pid, double *r, double *g, double *b) {
    double h = pid * 0.61803398875;
    h -= (int)h;
    h *= 6.0;
    double s = 0.35, v = 0.95;
    int i = (int)h;
    double f = h - i;
    double p = v * (1 - s), q = v * (1 - s * f), t = v * (1 - s * (1 - f));

    switch (i % 6) {
        case 0:  *r = v; *g = t; *b = p; break;
        case 1:  *r = q; *g = v; *b = p; break;
        case 2:  *r = p; *g = v; *b = t; break;
        case 3:  *r = p; *g = q; *b = v; break;
        case 4:  *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

static void drawRamBar(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const int *frames = (const int *)data;

    for (int i = 0; i < 100; i++) {
        int x0 = (int)((double)i * width / 100 + 0.5);
        int x1 = (int)((double)(i + 1) * width / 100 + 0.5);

        if (frames[i] == 0) {
            cairo_set_source_rgb(cr, 1, 1, 1);
        } else {
            double r, g, b;
            pidColor(frames[i], &r, &g, &b);
            cairo_set_source_rgb(cr, r, g, b);
        }
        cairo_rectangle(cr, x0, 0, x1 - x0, height);
        cairo_fill(cr);
    }

    cairo_set_source_rgb(cr, 0.65, 0.65, 0.65);
    cairo_set_line_width(cr, 1);
    for (int i = 1; i < 100; i++) {
        int x = (int)((double)i * width / 100 + 0.5);
        cairo_move_to(cr, x + 0.5, 0);
        cairo_line_to(cr, x + 0.5, height);
    }
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0.5, 0.5, width - 1, height - 1);
    cairo_stroke(cr);
}

static gboolean onRamQueryTooltip(GtkWidget *widget, int x, int y, gboolean keyboard,
                                  GtkTooltip *tooltip, gpointer data) {
    const int *frames = (const int *)data;
    int width = gtk_widget_get_width(widget);
    if (width <= 0) return FALSE;

    int i = x * 100 / width;
    if (i < 0 || i >= 100) return FALSE;

    char text[64];
    if (frames[i] == 0) {
        snprintf(text, sizeof text, "Marco %d: libre", i);
    } else {
        snprintf(text, sizeof text, "Marco %d: proceso %d", i, frames[i]);
    }
    gtk_tooltip_set_text(tooltip, text);
    return TRUE;
}

static void ramBarSetup(GtkWidget *area, int *frames) {
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), drawRamBar, frames, NULL);
    gtk_widget_set_has_tooltip(area, TRUE);
    g_signal_connect(area, "query-tooltip", G_CALLBACK(onRamQueryTooltip), frames);
}

static void ramBarUpdate(GtkWidget *area, int *dest, const int *src) {
    for (int i = 0; i < 100; i++) {
        dest[i] = src[i];
    }
    gtk_widget_queue_draw(area);
}





static void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new_from_file("View/MMU.ui");
    
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    GtkWidget *open_button = GTK_WIDGET(gtk_builder_get_object(builder, "btnOpenFile"));
    GtkWidget *file_label = GTK_WIDGET(gtk_builder_get_object(builder, "lblDirectory"));
    GtkWidget *btnCreate = GTK_WIDGET(gtk_builder_get_object(builder,"btnCreateFile"));

    widgets.spinSeed = GTK_WIDGET(gtk_builder_get_object(builder,"spinSeed"));
    widgets.cmbProcesses = GTK_WIDGET(gtk_builder_get_object(builder, "cmbProcesses"));
    widgets.cmbOperations = GTK_WIDGET(gtk_builder_get_object(builder, "cmbOperations"));
    widgets.cmbAlgorithm = GTK_WIDGET(gtk_builder_get_object(builder, "cmbAlgorithm"));
    widgets.lblAlgName = GTK_WIDGET(gtk_builder_get_object(builder, "lblAlgName"));
    widgets.btnDownload = GTK_WIDGET(gtk_builder_get_object(builder, "btnDownloadRandomFile"));
    
    widgets.btnStartSimulation = GTK_WIDGET(gtk_builder_get_object(builder, "btnStartSimulation"));

    ramOptArea = GTK_WIDGET(gtk_builder_get_object(builder, "ramOptArea"));
    ramAlgArea = GTK_WIDGET(gtk_builder_get_object(builder, "ramAlgArea"));

    if(ramOptArea && ramAlgArea){
        ramBarSetup(ramOptArea, ramOptFrames);
        ramBarSetup(ramAlgArea, ramAlgFrames);

    }else{
        g_printerr("drawingAreas not found");
    }

    GtkWidget *boxOpt = GTK_WIDGET(gtk_builder_get_object(builder, "boxMmuOpt"));
    GtkWidget *boxAlg = GTK_WIDGET(gtk_builder_get_object(builder, "boxMmuAlg"));
    if (boxOpt && boxAlg) {
        tableOpt = pageTableNew();
        tableAlg = pageTableNew();
        gtk_box_append(GTK_BOX(boxOpt), tableOpt->widget);
        gtk_box_append(GTK_BOX(boxAlg), tableAlg->widget);

    } else {
        g_printerr("No se encontraron boxMmuOpt / boxMmuAlg en el .ui\n");
    }
    

    
    if(btnCreate){
        g_signal_connect(btnCreate, "clicked", G_CALLBACK(on_create_file_clicked), &widgets);
    }

    if(widgets.cmbAlgorithm){
        g_signal_connect(widgets.cmbAlgorithm, "notify::selected", G_CALLBACK(on_algorithm_changed), &widgets);
        on_algorithm_changed(G_OBJECT(widgets.cmbAlgorithm), NULL, &widgets);
    }

    if (open_button) {
        g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button_clicked), file_label);
    }
    
    if (widgets.btnStartSimulation) {
        g_signal_connect(widgets.btnStartSimulation, "clicked", G_CALLBACK(on_start_simulation_button_clicked), NULL);
    }
    
    if(widgets.btnDownload){
        gtk_widget_set_sensitive(widgets.btnDownload, FALSE);
        g_signal_connect(widgets.btnDownload, "clicked", G_CALLBACK(on_download_clicked), NULL);
    }

    
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_present(GTK_WINDOW(window));
    
    g_object_unref(builder);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
