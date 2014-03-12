CFLAGS=-Os -I$(top_srcdir)/src -mlong-calls -D__CoActionOS__ -D__lpc17xx
CXXFLAGS=-Os -I$(top_srcdir)/src -mlong-calls -D__CoActionOS__ -D__lpc17xx

include ../Files.am

lib_LTLIBRARIES = libapp_hwpl_lpc17xx.la
libapp_hwpl_lpc17xx_la_include_HEADERS = $(HEADER_FILES)
libapp_hwpl_lpc17xx_la_includedir=$(includedir)/applib
libapp_hwpl_lpc17xx_la_SOURCES = $(SHARE_FILES) \
	$(HWPL_FILES) \
	$(top_srcdir)/src/hwpl/usbfifo_c.c
	

	
