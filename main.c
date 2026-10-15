#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "Files/FileManager.h"


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
}AppWidgets;

static AppWidgets widgets;

static const char *dropdownText(GtkWidget *dropdown){
    GObject * item = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(dropdown));
    if(item == NULL){
        return NULL;
    }
    return gtk_string_object_get_string(GTK_STRING_OBJECT(item));
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

static void on_download_clicked(GtkButton *button, gpointer user_data) {
    if (generated_file_path == NULL) return;

    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Guardar secuencia generada");

    char *baseName = g_path_get_basename(generated_file_path);
    gtk_file_dialog_set_initial_name(dialog, baseName);   // nombre sugerido
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
        printf("-------Ejecutando-------------------\n");
        printf("selected_file_path: %s \n", selected_file_path);
        runFile(selected_file_path);


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

    ramOptArea = GTK_WIDGET(gtk_builder_get_object(builder, "ramOptArea"));
    ramAlgArea = GTK_WIDGET(gtk_builder_get_object(builder, "ramAlgArea"));

    if(ramOptArea && ramAlgArea){
        ramBarSetup(ramOptArea, ramOptFrames);
        ramBarSetup(ramAlgArea, ramAlgFrames);

    }else{
        g_printerr("drawingAreas not found");
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
