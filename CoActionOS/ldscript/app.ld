

MEMORY {
   flash : ORIGIN = 0xDE000000, LENGTH = 8M
   ram : ORIGIN = 0xDE800000, LENGTH = 8355840
   kernel : ORIGIN = 0xDEFF8000, LENGTH = 32K
}

__ram_size__ = 32K;

SECTIONS {

  . = 0;
  .text : {
  		 _text = .;
  		 *(.startup)
         *(.text)
         *(.init)
         *(.fini)
         *(.eh_frame)
         _ctors = .;
         KEEP(*(.init_array))
         KEEP(*(.ctors))
         _dtors = .;
         KEEP(*(.dtors))
         KEEP(*(.fini_array))
         _dtors_end = .;         
         *(.jcr)
         *(.rodata)
         *(.rodata*)
         *(.glue_7)
         *(.glue_7t)
         *(.text.*)
         
         FILL(0xFF);
         _text_size_align = .;
         . = ALIGN(256);
         . = ALIGN(512 * (_text_size_align > 2048) );
         . = ALIGN(1024 * (_text_size_align > 4096) );
         . = ALIGN(2048 * (_text_size_align > 8192) );
         . = ALIGN(4096 * (_text_size_align > 16384) );
         . = ALIGN(8192 * (_text_size_align > 32768) );
         . = ALIGN(16384 * (_text_size_align > 65536) );
         . = ALIGN(32768 * (_text_size_align > 131072) );
         . = ALIGN(65536 * (_text_size_align > 262144) );
   } >flash
   
  .ARM.extab : 
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > flash
	
  __exidx_start = .;
  .ARM.exidx   : { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } >flash
  __exidx_end = .;
         
  _etext = .;
  
  .data : {
         _data = .;
         __data_beg_src__ = _etext;
         *(.global_reent)
         *(.data)
         *(.data.*)
         } >ram AT>flash
  _edata = .;
  _bss = .;
  .bss : {
         *(.bss)
         *(.bss.*)
         } >ram
   . = ALIGN(4);
   _ebss = .;
}

PROVIDE ( _text_size = (_etext - _text) );
PROVIDE ( _data_size = (_edata - _data) );
PROVIDE ( _ctors_size = ((_dtors - _ctors)>>2) );
PROVIDE ( _dtors_size = ((_dtors_end - _dtors)>>2) );






