	.rodata

	.globl bgData
bgData:
	.incbin "../data/background"

	.globl loadingImg
loadingImg:
	.incbin "../data/loading"

	.globl errorImg
errorImg:
	.incbin "../data/error"



#include "asm.h"

	.text
	.section .stub

	.extern _start
	.globl  _stub_start
_stub_start:
	lis   r3, 0x0011
	ori   r3, r3, 0x0C64
	mtspr HID0, r3
	isync

	li    r4, 0x2000
	mtmsr r4

	ori r3, r3, 0xC000
	mtspr HID0, r3
	isync

	li r0, 0
	mtspr IBAT0U, r0
	mtspr IBAT1U, r0
	mtspr IBAT2U, r0
	mtspr IBAT3U, r0
	mtspr IBAT4U, r0
	mtspr IBAT5U, r0
	mtspr DBAT0U, r0
	mtspr DBAT1U, r0
	mtspr DBAT2U, r0
	mtspr DBAT3U, r0
	mtspr DBAT4U, r0
	mtspr DBAT5U, r0
	isync

	lis  r0, 0x8000
	mtsr 0,  r0
	mtsr 1,  r0
	mtsr 2,  r0
	mtsr 3,  r0
	mtsr 4,  r0
	mtsr 5,  r0
	mtsr 6,  r0
	mtsr 7,  r0
	mtsr 8,  r0
	mtsr 9,  r0
	mtsr 10, r0
	mtsr 11, r0
	mtsr 12, r0
	mtsr 13, r0
	mtsr 14, r0
	mtsr 15, r0
	isync

	li  r3, 2
	lis r4, 0x8000
	ori r4, r4, 0x1FFF
	mtspr DBAT0L, r3
	mtspr DBAT0U, r4
	isync
	mtspr IBAT0L, r3
	mtspr IBAT0U, r4
	isync

	addis r3, r3, 0x1000
	addis r4, r4, 0x1000
	mtspr DBAT4L, r3
	mtspr DBAT4U, r4
	isync
	mtspr IBAT4L, r3
	mtspr IBAT4U, r4
	isync

	li  r3, 0x2A
	lis r4, 0xC000
	ori r4, r4, 0x1FFF
	mtspr DBAT1L, r3
	mtspr DBAT1U, r4
	isync
	mtspr IBAT1L, r3
	mtspr IBAT1U, r4
	isync

	addis r3, r3, 0x1000
	addis r4, r4, 0x1000
	mtspr DBAT2L, r3
	mtspr DBAT2U, r4
	isync
	mtspr IBAT2L, r3
	mtspr IBAT2U, r4
	isync

	li r3, 0
	mfmsr  r3
	ori    r3, r3, MSR_DR|MSR_IR
	mtsrr1 r3

	lis r3, _start@h
	ori r3, r3, _start@l
	mtsrr0 r3

	rfi
