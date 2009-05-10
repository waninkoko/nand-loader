/*
 * Linkscript for Wii
 */

OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpc");
OUTPUT_ARCH(powerpc:common);
EXTERN(_stub_start);
ENTRY(_stub_start);

SECTIONS
{
	/* DOL header (from TITANIK's GC docs) */
	/* default base address */
	off = 0;

	.header	: AT(off)
	{
		/*    0000-001B  Text[0..6] sections File Positions */
		LONG(stub_file_start);
		LONG(text_file_start);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*    001C-0047  Data[0..10] sections File Positions */
		LONG(data_file_start);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*    0048-0063  Text[0..6] sections Mem Address */
		LONG(stub_mem_start);
		LONG(text_mem_start);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*    0064-008F  Data[0..10] sections Mem Address */
		LONG(data_mem_start);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*    0090-00AB  Text[0..6] sections Sizes */
		LONG(stub_mem_size);
		LONG(text_mem_size);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*    00AC-00D7  Data[0..10] sections Sizes */
		LONG(data_mem_size);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);

		/*	00D8  BSS Mem address
			00DC  BSS Size */
		LONG(bss_mem_start);
		LONG(bss_mem_size);

		/*    00E0  Entry Point */
		LONG(ABSOLUTE(_stub_start));
	}

	. = 0x00003400;
	off += 0x100;

	stub_mem_start = .;
	stub_file_start = off;

	.stub : AT(off)
	{
		KEEP (*(.stub))
		. = 0x400;
	} = 0

	stub_mem_size = . - stub_mem_start;
	off += stub_mem_size;

	. = 0x81330000;

	text_mem_start = .;
	text_file_start = off;

	.init : AT(off)
	{
		KEEP (*crt0.o(*.init))
		KEEP (*(.init))
	}

	.text :
	{
		*(.text)
		*(.text.*)
		/* .gnu.warning sections are handled specially by elf32.em.  */
		*(.gnu.warning)
		*(.gnu.linkonce.t.*)
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	.fini :
	{
		KEEP (*(.fini))
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	PROVIDE (__etext = .);
	PROVIDE (_etext = .);
	PROVIDE (etext = .);

	text_mem_size = . - text_mem_start;
	off += text_mem_size;

	data_mem_start = .;
	data_file_start = off;

	.data    : AT(off)
	{
		*(.rodata) *(.rodata.*) *(.gnu.linkonce.r.*)
		*(.rodata1)
		*(.sdata2) *(.sdata2.*) *(.gnu.linkonce.s2.*)
		*(.sbss2) *(.sbss2.*) *(.gnu.linkonce.sb2.*)
		PROVIDE (__preinit_array_start = .);
		*(.preinit_array)
		PROVIDE (__preinit_array_end = .);
		PROVIDE (__init_array_start = .);
		*(.init_array)
		PROVIDE (__init_array_end = .);
		PROVIDE (__fini_array_start = .);
		*(.fini_array)
		PROVIDE (__fini_array_end = .);

		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d.*)
		SORT(CONSTRUCTORS)
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	.data1   : { *(.data1) }
	.tdata	  : { *(.tdata .tdata.* .gnu.linkonce.td.*) }
	.tbss		  : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) }
	.gcc_except_table : { *(.gcc_except_table) }
	.fixup          : { *(.fixup) }
	.got1           : { *(.got1) }
	.got2           : { *(.got2) }
	.dynamic       : { *(.dynamic) }

	.ctors   :
	{
	/*	gcc uses crtbegin.o to find the start of
		the constructors, so we make sure it is
		first.  Because this is a wildcard, it
		doesn't matter if the user does not
		actually link against crtbegin.o; the
		linker won't look for a file to match a
		wildcard.  The wildcard also means that it
		doesn't matter which directory crtbegin.o
		is in.  */

		KEEP (*crtbegin.o(.ctors))

    /*	We don't want to include the .ctor section from
		from the crtend.o file until after the sorted ctors.
		The .ctor section from the crtend file contains the
		end of ctors marker and it must be last */

		KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	.dtors         :
	{
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	.jcr            : { KEEP (*(.jcr)) }
	.got		  : { *(.got.plt) *(.got) }


	/*	We want the small data sections together, so single-instruction offsets
		can access them all, and initialized data all before uninitialized, so
		we can shorten the on-disk segment size.  */

	.sdata     :
	{
		*(.sdata)
		*(.sdata.*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
	}

	_edata = .;
	PROVIDE (edata = .);

	data_mem_size = . - data_mem_start;
	off += data_mem_size;
	
	bss_mem_start = .;
	bss_file_start = off;

	.sbss : AT(off)
	{
		__sbss_start = .;
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		*(.dynsbss)
		*(.sbss)
		*(.sbss.*)
		*(.gnu.linkonce.sb.*)
		*(.scommon)
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);
		. = ALIGN(32);   /* REQUIRED. LD is flaky without it. */
		__sbss_end = .;
	}

	.bss       :
	{
		__bss_start = .;
		PROVIDE (__bss_start = .);
		*(.dynbss)
		*(.bss)
		*(.bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
	/*	Align here to ensure that the .bss section occupies space up to
		_end.  Align after .bss to ensure correct alignment even if the
		.bss section disappears because there are no input sections.  */

		. = ALIGN(32);

		PROVIDE (__bss_end = .);
		__bss_end = .;
	}
 
 	bss_mem_size = . - bss_mem_start;

	_end = .;
	PROVIDE(end = .);
	/* Stabs debugging sections.  */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	/*	DWARF debug sections.
		Symbols in the DWARF debugging sections are relative to the beginning
		of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	/* These must appear regardless of  .  */
}

__isIPL = 0;
__stack_addr = (__bss_start + SIZEOF(.bss) + 0x20000 + 7) & (-8);
__stack_end = (__bss_start + SIZEOF(.bss));
__intrstack_addr = (__stack_addr + 0x4000);
__intrstack_end = (__stack_addr);
__Arena1Lo = (__intrstack_addr + 31) & (-32);
__Arena1Hi = (0x816ffff0);
__Arena2Lo = (0x90002000);
__Arena2Hi = (0x933E0000);

__gxregs = (__Arena1Hi + 31) & (-32);
__ipcbufferLo = (0x933e0000);
__ipcbufferHi = (0x93400000);

/* for backward compatibility with old crt0 */
PROVIDE (__stack = (0x816ffff0));

PROVIDE(__isIPL = __isIPL);
PROVIDE(__text_start = text_mem_start);
PROVIDE(__text_fstart = text_file_start);
PROVIDE(__data_start = data_mem_start);
PROVIDE(__data_fstart = data_file_start);
PROVIDE(__bss_start = bss_mem_start);
PROVIDE(__bss_fstart = bss_file_start);
PROVIDE(__stack_addr = __stack_addr);
PROVIDE(__stack_end = __stack_end);
PROVIDE(__intrstack_addr = __intrstack_addr);
PROVIDE(__intrstack_end = __intrstack_end);
PROVIDE(__Arena1Lo = __Arena1Lo);
PROVIDE(__Arena1Hi = __Arena1Hi);
PROVIDE(__Arena2Lo = __Arena2Lo);
PROVIDE(__Arena2Hi = __Arena2Hi);
PROVIDE(__ipcbufferLo = __ipcbufferLo);
PROVIDE(__ipcbufferHi = __ipcbufferHi);
PROVIDE(__gxregs = __gxregs);
