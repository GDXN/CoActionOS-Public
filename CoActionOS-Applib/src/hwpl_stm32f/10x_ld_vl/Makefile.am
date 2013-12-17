

include ../../Files.am

lib_LTLIBRARIES = libapp_hwpl_stm32f10x_ld_vl.la
libapp_hwpl_stm32f10x_ld_vl_la_CFLAGS=-Os -I$(top_srcdir)/src -mlong-calls -D__CoActionOS__ -D__stm32f -DSTM32F10X_LD_VL
libapp_hwpl_stm32f10x_ld_vl_la_CXXFLAGS=-Os -I$(top_srcdir)/src -mlong-calls -D__CoActionOS__ -D__stm32f -DSTM32F10X_LD_VL \
	-fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-unwind-tables -ffunction-sections
libapp_hwpl_stm32f10x_ld_vl_la_include_HEADERS = $(HEADER_FILES)
libapp_hwpl_stm32f10x_ld_vl_la_LDFLAGS = -fno-rtti -fno-exceptions
libapp_hwpl_stm32f10x_ld_vl_la_includedir=$(includedir)/applib
libapp_hwpl_stm32f10x_ld_vl_la_SOURCES = $(SHARE_FILES) \
	$(HWPL_FILES)
	

	
