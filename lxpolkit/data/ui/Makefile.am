NULL=

# GtkBuilder UI definition files
uidir=$(datadir)/lxpolkit/ui
ui_in_files= \
	lxpolkit.glade \
	$(NULL)
ui_DATA = $(ui_in_files:.glade=.ui)

CLEANFILES = \
	$(ui_DATA) \
	$(NULL)

# Purge GtkBuilder UI files
%.ui: %.glade
	cp $< $@
	$(top_srcdir)/src/xml-purge $@

EXTRA_DIST= \
	$(ui_DATA) \
	$(NULL)
