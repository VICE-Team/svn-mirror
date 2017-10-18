#include "vice.h"
#include <gtk/gtk.h>

#include "foo.h"

GtkWidget *foo_widget_create(void)
{
    GtkWidget *foo;

    foo = gtk_label_new("foo");
    gtk_widget_show(foo);
    return foo;
}
