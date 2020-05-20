FULL_LIBEXECDIR=$(libexecdir)

lxpolkit.desktop.in : lxpolkit.desktop.in.in
		cat lxpolkit.desktop.in.in | sed 's,@FULL_LIBEXECDIR@,$(FULL_LIBEXECDIR),' > $@

NULL=

SUBDIRS= \
	ui \
	$(NULL)

# FIXME: install this to /etc/xdg/autostart
desktopdir=$(sysconfdir)/xdg/autostart
desktop_in_files = \
	lxpolkit.desktop.in.in \
	$(NULL)
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST= \
	$(desktop_DATA) \
	$(NULL)
