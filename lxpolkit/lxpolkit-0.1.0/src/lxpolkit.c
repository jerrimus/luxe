/*
 *      lxpolkit.c
 *
 *      Copyright 2010 PCMan <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <sys/types.h>
#include <unistd.h>

#include "lxpolkit-listener.h"

static GOptionEntry option_entries[] =
{
    { NULL }
};

void show_msg(GtkWindow* parent, GtkMessageType type, const char* msg)
{
    GtkWidget* dlg = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, msg);
    const char* title = NULL;
    switch(type)
    {
    case GTK_MESSAGE_ERROR:
        title = _("Error");
        break;
    case GTK_MESSAGE_INFO:
        title = _("Information");
        break;
    }
    if(title)
        gtk_window_set_title(GTK_WINDOW(dlg), title);
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

int main(int argc, char** argv)
{
    GError* err = NULL;
    PolkitAgentListener *listener;
    PolkitSubject* session;

    /* gettext support */
#ifdef ENABLE_NLS
    bindtextdomain ( GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR );
    bind_textdomain_codeset ( GETTEXT_PACKAGE, "UTF-8" );
    textdomain ( GETTEXT_PACKAGE );
#endif

    /* initialize GTK+ and parse the command line arguments */
    if( G_UNLIKELY( ! gtk_init_with_args( &argc, &argv, "", option_entries, GETTEXT_PACKAGE, &err ) ) )
    {
        g_print( "Error: %s\n", err->message );
        return 1;
    }

    listener = lxpolkit_listener_new();
    session = polkit_unix_session_new_for_process_sync(getpid(), NULL, NULL);

    if(!polkit_agent_register_listener(listener, session, NULL, &err))
    {
        /* show error msg */
        g_object_unref(listener);
        g_object_unref(session);
        show_msg(NULL, GTK_MESSAGE_ERROR, err->message);
        return 1;
    }

    gtk_main();

    g_object_unref(listener);
    g_object_unref(session);

	return 0;
}

