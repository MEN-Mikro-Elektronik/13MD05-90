/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file cha_ut.c
 *       \author dieter.pfeuffer@men.de
 *        $Date: 2008/03/31 11:31:45 $
 *    $Revision: 1.3 $
 *
 *        \brief Basic Unit Test for Chameleon library
 *
 *     Required: libraries: usr_oss
 *     \switches (none)
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: cha_ut.c,v $
 * Revision 1.3  2008/03/31 11:31:45  DPfeuffer
 * completely revised to support 'pluggable bridges'
 *
 * Revision 1.2  2005/05/09 10:10:28  dpfeuffer
 * OSS_BusToPhysAddr() added
 *
 * Revision 1.1  2005/04/29 14:45:28  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*--------------------------------------*/
/*    INCLUDES                          */
/*--------------------------------------*/
#include <stdlib.h>
#include <math.h>

#include "cha_ut.h"

#include <MEN/chameleon.h>


/*--------------------------------------*/
/*	DEFINES			            		*/
/*--------------------------------------*/
#define CHAMOFF__BUS0_TO_BUS1	0x12345678
#define CHAMOFF__BUS0_TO_BUS1_ILLTBL	0x12344321
#define CHAMOFF__BUS1_TO_BUS0	0x87654321
#define CHAMOFF__BUS1_TO_BUS2	0x87654567

#define MAX_TABLE_SIZE 4096

/*--------------------------------------*/
/*	MACROS       	                    */
/*--------------------------------------*/
#define CHABITS(val, nbr, shift) \
 ( ((val) & (   (0xffffffff >> (32-(nbr)))   )) << (shift) )

#define UT_PRINT(_x_) printf _x_

#define UT_ASSERT(_expr) \
 if( !(_expr) ){\
   UT_PRINT((" *** UT_ASSERT assertion failed %s line %d: %s\n", \
   __FILE__, __LINE__, #_expr ));\
   goto ABORT;\
 }

/*--------------------------------------*/
/*	GLOBALS     	                    */
/*--------------------------------------*/
/* OSS_MemGet(), OSS_MemFree() supervision: */
int32	G_allocBytes;	/* number of allocated bytes */
int32	G_allocCalls;	/* number of outstanding free calls */

/*---------------------------------------------------------------------------*/
/*--------------------------  ChamTblV0 example-1  --------------------------*/
/*---------------------------------------------------------------------------*/
u_int16 G_ChamTblV0_Ex1[] = {
	/* header */
	0x4B02, 0x0000,
	0xABCD, 0x0000,

	/* unit #0 */
	0x0005, 0x0000,
	0x0000, 0x0000,
	/* unit #1 */
	0x0034, 0x0000,
	0x0041, 0x0000,
	/* unit #2 */
	0x0045, 0x0000,
	0x0082, 0x0000,
	/* unit #3 */
	0x0091, 0x0000,
	0x00C3, 0x0000,
	/* unit #4 */
	0x007C, 0x0000,
	0x0104, 0x0000,
	/* unit #5 */
	0x0089, 0x0000,
	0x0145, 0x0000,
	/* unit #6 */
	0x02a0, 0x0000,
	0x0006, 0x0000,
	/* unit #7 */
	0x0216, 0x0000,
	0x4180, 0x0000,
	/* unit #8 */
	0x0067, 0x0000,
	0x81C0, 0x0000,
	/* unit #9 */
	0x025a, 0x0000,
	0xc200, 0x0000,

	/* end */
	0x03f1, 0x0000,
	0x0000, 0x0000};

/*---------------------------------------------------------------------------*/
/*----------------------  ChamTblV2 example-2 (F206N)  ----------------------*/
/*---------------------------------------------------------------------------*/
/* table for bus #0 (Wishbone bus) */
u_int32 G_ChamTblV2_Ex2Bus0[] = {
/* ====== HEADER (20 byte) ====== */
/* 0x00      val  nbr shift */
( CHABITS(   0x0,   8,   24) |	/* bus-type [31:24] : WB */
  CHABITS(  0x41,   8,    8) |	/* model    [15: 8] : A */
  CHABITS(   0x0,   8,    0) ),	/* revision [ 7: 0] */
/* 0x04 */
0x0000abce,	/* magic-word */
/* 0x08 */
0x36303246,	/* file  3..0 : 602F */
/* 0x0c */
0x4930304e,	/* file  7..4 : I00N */
/* 0x10 */
0x32303043,	/* file 11..8 : 200C */
/* --- UNIT (16 byte) --- : IRQ Controller */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   101,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000600,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : Flash-Interface */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    45,  10,   18) |	/* device   [27:18] : 16z045 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000100,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : SRAM */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     2,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000000,	/* offset */
/* 0x0c */
0x00040000,	/* size */
/* --- UNIT (16 byte) --- : BitBlitter */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    46,  10,   18) |	/* device   [27:18] : 16z046 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     8,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x1,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     5,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000200,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : DISP */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    44,  10,   18) |	/* device   [27:18] : 16z044 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0xa,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     5,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000300,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : SDRAM for DISP */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    43,  10,   18) |	/* device   [27:18] : 16z043 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x8,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     5,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     1,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00200000,	/* offset */
/* 0x0c */
0x00100000,	/* size */
/* --- UNIT (16 byte) --- : UART */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    38,  10,   18) |	/* device   [27:18] : 16z038 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(  0x1a,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x2,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000400,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : CAN */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    29,  10,   18) |	/* device   [27:18] : 16z029 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x7,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x3,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     2,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000500,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : IDE */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   102,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x6,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x4,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000000,	/* offset */
/* 0x0c */
0x00010000,	/* size */
/* --- UNIT (16 byte) --- : PCI */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   103,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(    13,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x0,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     3,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000000,	/* offset */
/* 0x0c */
0x00010000,	/* size */
/* --- end (4 byte) --- */
0xffffffff,
/* -------------------- */
/* behind the table (4 byte) */
0xdeadbeef,
/* fill space to next table with 0xff (68 bytes) */
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff};

/* table for bus #1 (Avalone bus) */
u_int32 G_ChamTblV2_Ex2Bus1[] = {
/* ====== HEADER (20 byte) ====== */
/* 0x00      val  nbr shift */
( CHABITS(   0x1,   8,   24) |	/* bus-type [31:24] : Avalon */
  CHABITS(  0x41,   8,    8) |	/* model    [15: 8] : A */
  CHABITS(   0x0,   8,    0) ),	/* revision [ 7: 0] */
/* 0x04 */
0x0000abce,	/* magic-word */
/* 0x08 */
0x36303246,	/* file  3..0 : 602F */
/* 0x0c */
0x4930304e,	/* file  7..4 : I00N */
/* 0x10 */
0x32303043,	/* file 11..8 : 200C */
/* --- UNIT (16 byte) --- : IRQ Controller */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   105,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     3,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00004000,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- BAR (12 byte) --- */
/* 0x00      val  nbr shift */
( CHABITS(     3,   4,   28) |	/* dtype    [31:28] */
  CHABITS(     0,  25,    3) |  /* reserved [27: 3] */
  CHABITS(     1,   3,    0) ), /* ba count [ 2: 0] */
/* 0x04 */
0x00000000,	/* BA0 */
/* 0x08 */
0x10000000,	/* BSIZE0 */
/* --- UNIT (16 byte) --- : Flash for NAND */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    45,  10,   18) |	/* device   [27:18] : 16z045 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     5,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     1,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00008000,	/* offset */
/* 0x0c */
0x00001000,	/* size */
/* --- UNIT (16 byte) --- : Flash for CAN */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    45,  10,   18) |	/* device   [27:18] : 16z045 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     2,   6,    9) |	/* group    [14: 9] */
  CHABITS(     2,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00009000,	/* offset */
/* 0x0c */
0x00002000,	/* size */
/* --- BRIDGE (20 byte) : Bus#1[BAR0]-->Bus#0[BAR0] (AV2WB) --- */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   106,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x8,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x1,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     0,   6,   12) |	/* group    [17:12] */
  CHABITS(     0,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS1_TO_BUS0,	/* cham-offset - will be replaced later */
/* 0x0c */
0x0000a000,	/* offset */
/* 0x10 */
0x00020000,	/* size */
/* --- BRIDGE (20 byte) : Bus#1[BAR0]-->Bus#0[BAR3] (AV2WB) --- */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   107,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x6,   6,   12) |	/* variant  [17:12] */
  CHABITS(  0x1a,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x2,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     0,   6,   12) |	/* group    [17:12] */
  CHABITS(     3,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS1_TO_BUS0,	/* cham-offset - will be replaced later */
/* 0x0c */
0x00050000,	/* offset */
/* 0x10 */
0x00010000,	/* size */
/* --- UNIT (16 byte) --- : SDRAM for NAND */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    43,  10,   18) |	/* device   [27:18] : 16z043 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x7,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     1,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000000,	/* offset */
/* 0x0c */
0x00100000,	/* size */
/* --- UNIT (16 byte) --- : SDRAM for CAN */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    43,  10,   18) |	/* device   [27:18] : 16z043 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x7,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     2,   6,    9) |	/* group    [14: 9] */
  CHABITS(     2,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00100000,	/* offset */
/* 0x0c */
0x00100000,	/* size */
/* --- UNIT (16 byte) --- : IRQ Controller #2 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   108,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     7,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     4,   6,    9) |	/* group    [14: 9] */
  CHABITS(     2,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00004100,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : NAND */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   109,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     3,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x3,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     1,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x03000000,	/* offset */
/* 0x0c */
0x00010000,	/* size */
/* --- CPU (12 byte) : NIOS #1 --- */
/* 0x00      val  nbr shift */
( CHABITS(     2,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   110,  10,   18) |	/* device   [27:18] : tbd*/
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     3,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(   0x0,   3,    0) ),	/* reserved	[ 2: 0] */
/* 0x08 */
0x00008000,	/* boot-addr */
/* --- CPU (12 byte) : NIOS #2 --- */
/* 0x00      val  nbr shift */
( CHABITS(     2,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   111,  10,   18) |	/* device   [27:18] : tbd*/
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     4,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(   0x0,   6,    0) ),	/* reserved	[ 2: 0] */
/* 0x08 */
0x00009000,	/* boot-addr */
/* --- end (4 byte) --- */
0xffffffff,
/* -------------------- */
/* behind the table (4 byte) */
0xdeadbeef
};

/*---------------------------------------------------------------------------*/
/*-------------------  ChamTblV2 example-3 (bridge test)  -------------------*/
/*---------------------------------------------------------------------------*/
/* table for bus #0 (Wishbone bus) */
u_int32 G_ChamTblV2_Ex3Bus0[] = {
/* ====== HEADER (20 byte) ====== */
/* 0x00      val  nbr shift */
( CHABITS(   0x0,   8,   24) |	/* bus-type [31:24] : WB */
  CHABITS(  0x58,   8,    8) |	/* model    [15: 8] : X */
  CHABITS(  0x3f,   8,    0) ),	/* revision [ 7: 0] */
/* 0x04 */
0x0000abce,	/* magic-word */
/* 0x08 */
0x36303246,	/* file  3..0 : 321F */
/* 0x0c */
0x4930304e,	/* file  7..4 : I00N */
/* 0x10 */
0x32303043,	/* file 11..8 : 200C */
/* --- UNIT (16 byte) --- : Flash-Interface #0 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    45,  10,   18) |	/* device   [27:18] : 16z045 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     3,   6,    9) |	/* group    [14: 9] */
  CHABITS(    22,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00000000,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- BRIDGE (20 byte) : Bus#0[BAR4]-->Bus#ILL[BAR1] (WB2AV) - to ill table --- */
/* Note: This bridge will be counted, even though the destination bus is unplugged (ill table)! */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   104,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x6,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x4,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     3,   6,   12) |	/* group    [17:12] */
  CHABITS(     1,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     22,  6,    3) |	/* instance [ 8: 3] */
  CHABITS(     4,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS0_TO_BUS1_ILLTBL,	/* cham-offset - will be replaced later */
/* 0x0c */
0x00002000,	/* offset */
/* 0x10 */
0x00010000,	/* size */
/* --- BRIDGE (20 byte) : Bus#0[BAR4]-->Bus#1[BAR0] (WB2AV) --- */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   104,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x6,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x4,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     0,   6,   12) |	/* group    [17:12] */
  CHABITS(     0,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     4,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS0_TO_BUS1,	/* cham-offset - will be replaced later */
/* 0x0c */
0x00022000,	/* offset */
/* 0x10 */
0x00010000,	/* size */
/* --- UNIT (16 byte) --- : SRAM #0 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     1,   6,    9) |	/* group    [14: 9] */
  CHABITS(     0,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00001000,	/* offset */
/* 0x0c */
0x00000800,	/* size */
/* --- UNIT (16 byte) --- : SRAM #1 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     2,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00002000,	/* offset */
/* 0x0c */
0x00000400,	/* size */
/* --- BRIDGE (20 byte) : Bus#0[BAR4]-->Bus#1[BAR1] (WB2AV) --- */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   104,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x6,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x4,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     1,   6,   12) |	/* group    [17:12] */
  CHABITS(     1,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     4,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS0_TO_BUS1,	/* cham-offset - will be replaced later */
/* 0x0c */
0x00002000,	/* offset */
/* 0x10 */
0x00010000,	/* size */
/* --- end (4 byte) --- */
0xffffffff,
/* -------------------- */
/* behind the table (4 byte) */
0xdeadbeef};

/* table for bus #1 (Avalone bus) */
u_int32 G_ChamTblV2_Ex3Bus1[] = {
/* before the table (16 byte) */
0xdeadbeef,0xdeadbeef,0xdeadbeef,0xdeadbeef,
/* ====== HEADER (20 byte) ====== */
/* 0x00      val  nbr shift */
( CHABITS(   0x1,   8,   24) |	/* bus-type [31:24] : Avalon */
  CHABITS(  0x58,   8,    8) |	/* model    [15: 8] : X */
  CHABITS(  0x3f,   8,    0) ),	/* revision [ 7: 0] */
/* 0x04 */
0x0000abce,	/* magic-word */
/* 0x08 */
0x36303246,	/* file  3..0 : 321F */
/* 0x0c */
0x4930304e,	/* file  7..4 : I00N */
/* 0x10 */
0x32303043,	/* file 11..8 : 200C */
/* --- BAR (12 byte) --- */
/* 0x00      val  nbr shift */
( CHABITS(     3,   4,   28) |	/* dtype    [31:28] */
  CHABITS(     0,  25,    3) |  /* reserved [27: 3] */
  CHABITS(     2,   3,    0) ), /* ba count [ 2: 0] */
/* 0x04 */
0xba000000,	/* BA0 */
/* 0x08 */
0x00010000,	/* BSIZE0 */
/* 0x0c */
0xba100000,	/* BA1 */
/* 0x10 */
0x00020000,	/* BSIZE1 */
/* --- UNIT (16 byte) --- : Flash-Interface #1 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    45,  10,   18) |	/* device   [27:18] : 16z045 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     1,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00100000,	/* offset */
/* 0x0c */
0x00000100,	/* size */
/* --- UNIT (16 byte) --- : SRAM #1 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     1,   6,    9) |	/* group    [14: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00110000,	/* offset */
/* 0x0c */
0x00000500,	/* size */
/* --- UNIT (16 byte) --- : SRAM #2 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     4,   6,    9) |	/* group    [14: 9] */
  CHABITS(     22,  6,    3) |	/* instance [ 8: 3] */
  CHABITS(     1,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00120000,	/* offset */
/* 0x0c */
0x00004000,	/* size */
/* --- BRIDGE (20 byte) : Bus#1[BAR1]-->Bus#0[BAR0] (AV2WB) --- */
/* Note: This bridge will be skipped, because Bus#0[BAR0] already scanned! */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   106,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x8,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x1,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     4,   6,   12) |	/* group    [17:12] */
  CHABITS(     0,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(    22,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     1,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS1_TO_BUS0,	/* cham-offset - will be replaced later */
/* 0x0c */
0x0000a000,	/* offset */
/* 0x10 */
0x00008000,	/* size */
/* --- BRIDGE (20 byte) : Bus#1[BAR0]-->Bus#2[BAR0] (WB2AV) --- */
/* 0x00      val  nbr shift */
( CHABITS(     1,   4,   28) |	/* dtype    [31:28] */
  CHABITS(   104,  10,   18) |	/* device   [27:18] : tbd */
  CHABITS(   0x0,   6,   12) |	/* variant  [17:12] */
  CHABITS(   0x6,   6,    6) |	/* revision [11: 6] */
  CHABITS(   0x4,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  14,   18) |  /* reserved [31:18] */
  CHABITS(     1,   6,   12) |	/* group    [17:12] */
  CHABITS(     0,   3,    9) |	/* dbar     [11: 9] */
  CHABITS(     1,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ),	/* bar      [ 2: 0] */
/* 0x08 */
CHAMOFF__BUS1_TO_BUS2,	/* cham-offset - will be replaced later */
/* 0x0c */
0x00002000,	/* offset */
/* 0x10 */
0x00010000,	/* size */
/* --- end (4 byte) --- */
0xffffffff,
/* -------------------- */
/* behind the table (4 byte) */
0xdeadbeef
};

/* table for bus #2 */
u_int32 G_ChamTblV2_Ex3Bus2[] = {
/* ====== HEADER (20 byte) ====== */
/* 0x00      val  nbr shift */
( CHABITS(   0x3,   8,   24) |	/* bus-type [31:24] : ? */
  CHABITS(  0x58,   8,    8) |	/* model    [15: 8] : X */
  CHABITS(  0x3f,   8,    0) ),	/* revision [ 7: 0] */
/* 0x04 */
0x0000abce,	/* magic-word */
/* 0x08 */
0x36303246,	/* file  3..0 : 321F */
/* 0x0c */
0x4930304e,	/* file  7..4 : I00N */
/* 0x10 */
0x32303043,	/* file 11..8 : 200C */
/* --- BAR (12 byte) --- */
/* 0x00      val  nbr shift */
( CHABITS(     3,   4,   28) |	/* dtype    [31:28] */
  CHABITS(     0,  25,    3) |  /* reserved [27: 3] */
  CHABITS(     1,   3,    0) ), /* ba count [ 2: 0] */
/* 0x04 */
0xba000000,	/* BA0 */
/* 0x08 */
0x00010000,	/* BSIZE0 */
/* --- UNIT (16 byte) --- : SRAM #1 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     22,  6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00110000,	/* offset */
/* 0x0c */
0x00000500,	/* size */
/* --- UNIT (16 byte) --- : SRAM #2 */
/* 0x00      val  nbr shift */
( CHABITS(     0,   4,   28) |	/* dtype    [31:28] */
  CHABITS(    24,  10,   18) |	/* device   [27:18] : 16z024 */
  CHABITS(     0,   6,   12) |	/* variant  [17:12] */
  CHABITS(     2,   6,    6) |	/* revision [11: 6] */
  CHABITS(  0x3f,   6,    0) ),	/* irq      [ 5: 0] */
/* 0x04      val  nbr shift */
( CHABITS(     0,  17,   15) |	/* reserved [31:15] */
  CHABITS(     0,   6,    9) |	/* group    [14: 9] */
  CHABITS(     2,   6,    3) |	/* instance [ 8: 3] */
  CHABITS(     0,   3,    0) ), /* bar      [ 2: 0] */
/* 0x08 */
0x00120000,	/* offset */
/* 0x0c */
0x00004000,	/* size */
/* --- end (4 byte) --- */
0xffffffff,
/* -------------------- */
/* behind the table (4 byte) */
0xdeadbeef
};

#define GROUPS		6

/** property V2 */
typedef struct{
	u_int16	busId;
	u_int16	tableNbr;
	u_int16	unitNbr;
	u_int16	bridgeNbr;
	u_int16	cpuNbr;
	u_int16	inst22Nbr;
	u_int32 groupField[GROUPS];
	u_int32 groupFieldFlagBrgAll[GROUPS];
} PROPV2;

/* Test 1.1: V2-API / V2-Tbl: F206N bus #0 (PCI) */
PROPV2 G_PropV2_Ex2Bus0 = {
	0,	/* bus number */
	1,	/* tables */
	10,	/* units */
	0,	/* bridges */
	0,	/* cpus */
	0,  /* nbr of instances=22 */
  /* 0  1  2  3  4  5 : group */
	{6, 0, 1, 0, 0, 3} ,	
  /* 0  1  2  3  4  5 : group for _FF_BRGALL flag */
	{6, 0, 1, 0, 0, 3} };	

/* Test 1.2: V2-API / V2-Tbl: F206N bus #1 (Avalon) */
PROPV2 G_PropV2_Ex2Bus1 = {
	0,	/* bus number */
	2,	/* tables */
	15,	/* units (7 on AV + 8 bridged on WB) */
	2,	/* bridges */
	2,	/* cpus */
	0,  /* nbr of instances=22 */
  /* 0  1  2  3  4  5 : group */
	{7, 3, 3, 2, 2, 2} ,	
  /* 0  1  2  3  4  5 : group for _FF_BRGALL flag */
	{10, 3, 3, 2, 2, 2} };	

/* Test 1.3: V2-API / V2-Tbl: BridgeEx bus #0 (PCI) */
PROPV2 G_PropV2_Ex3Bus0 = {
	0,	/* bus number */
	3,	/* tables */
	8,	/* units */
	4,	/* bridges */
	0,	/* cpus */
	4,  /* nbr of instances=22 */
  /* 0  1  2  3  4  5 : group */
	{5, 4, 0, 2, 1, 0} ,	
  /* 0  1  2  3  4  5 : group for _FF_BRGALL flag */
	{7, 7, 0, 2, 1, 0} };	

/* Test 1.4: V2-API / V0-Dev */
PROPV2 G_PropV2_Ex1Bus0 = {
	0,	/* bus number */
	1,	/* tables */
	10,	/* units */
	0,	/* bridges */
	0,	/* cpus */
	0,  /* nbr of instances=22 */
  /*  0  1  2  3  4  5 : group */
	{10, 0, 0, 0, 0, 0} ,	
  /* 0  1  2  3  4  5 : group for _FF_BRGALL flag */
	{10, 0, 0, 0, 0, 0} };	

/** property V0 */
typedef struct{
	char	variant;
	u_int16	revision;
	u_int16	nbrOfMods[10];
} PROPV0;	

/* Test 2.1: V0-API / V0-Dev */
PROPV0 G_PropV0_Ex1 = {
	'K',	/* variant */
	0x02,	/* revision */
	{
	1,	/* 16z022nbr */
	1,	/* 16z024nbr */
	0	/* 16z045nbr */
	}
};	

/* Test 2.2: V0-API / V2-Tbl: F206N bus #0 (PCI) */
PROPV0 G_PropV0_Ex2Bus0 = {
	'A',	/* variant */
	0x00,	/* revision */
	{
	0,	/* 16z022nbr */
	1,	/* 16z024nbr */
	1	/* 16z045nbr */
	}
};	

/* Test 2.3: V0-API / V2-Tbl: BridgeEx bus #0 (PCI) */
PROPV0 G_PropV0_Ex3Bus0 = {
	'X',	/* variant */
	0x3f,	/* revision */
	{
	0,	/* 16z022nbr, modCode=0x0009 */	
	3,	/* 16z024nbr, modCode=0x0006 */
		/* There are 6 instances of 16z024 but only 3 are found because
		 * Cham_V0 API requires unique instance numbers. This requirement is
		 * not always granted by Cham_V2 tables!
	2	/* 16z045nbr, modCode=0x001f */
	}
};	

u_int32 *G_chaTblBus0Bar0P;

/* units to search */
static struct {
	u_int16		i;
	u_int16		modCode;
	char	*name;
} G_modCodeTbl[] = {
	{  0, CHAMELEON_16Z022_GPIO,	"16Z022_GPIO"	},
	{  1, CHAMELEON_16Z024_SRAM,	"16Z024_SRAM"	},
	{  2, CHAMELEON_16Z045_FLASH,	"16Z045_FLASH"	},
	{ 10, 0xffff,					NULL }	/* end of table */
};


/*--------------------------------------*/
/*	PROTOTYPES     	                    */
/*--------------------------------------*/
int32 UtChamV2Api( void );
int32 DoChamV2Calls(
	CHAM_FUNCTBL		fTbl,
	CHAMELEONV2_HANDLE	*chah,
	PROPV2				*prop );

int32 UtChamV0Api( void );
int32 DoChamV0Calls(
	CHAMELEON_HANDLE	*chah,
	PROPV0				*prop );

/**********************************************************************/
/** Main function
 */
int main( int argc, char *argv[])
{
	u_int32		*tblBuf = NULL;
	int32		ret = -1;

	printf("**************************************************\n");
	printf("       cha_ut - Unit-Test for Chameleon-Lib       \n");
	printf("**************************************************\n");

	G_allocBytes = 0;
	G_allocCalls = 0;

	/*------------------------------+
	|  Alloc alligned buffer        |
	+------------------------------*/
	if( !(tblBuf = malloc(MAX_TABLE_SIZE)) )
		goto ABORT;

	/* 16 byte alligned? */
	if( 0x0 == ((u_int32)tblBuf & 0xf) )
		G_chaTblBus0Bar0P = tblBuf;
	else{
		/* allign it */
		G_chaTblBus0Bar0P = (u_int32*)((u_int8*)tblBuf
			+ (0x10 - ((u_int32)tblBuf & 0xf)));
	}

	if( UtChamV2Api() )
		goto ABORT;

	if( UtChamV0Api() )
		goto ABORT;

	printf("\nRemaining allocated bytes from OSS_MemGet(): %d\n", G_allocBytes);
	printf("Outstanding OSS_MemFree() calls            : %d\n", G_allocCalls);
	UT_ASSERT( 0 == G_allocBytes );
	UT_ASSERT( 0 == G_allocCalls );

	printf("**************************************************\n");
	printf("              Unit-Test   P A S S E D             \n");
	printf("**************************************************\n");

	ret = 0;

ABORT:
	if( ret ){
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		printf("              Unit-Test    F A I L E D            \n");
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}

	if( tblBuf )
		free( tblBuf);

	return ret;
}

/**********************************************************************/
/** Unit test for chameleon V2 API
 */
int32 UtChamV2Api( void )
{
	CHAMELEONV2_HANDLE	*chah;
	CHAM_FUNCTBL		fTbl;
	u_int32				*chaTblBus1P=NULL;
	u_int32				*chaTblBus2P=NULL;
	u_int32				*sP = NULL;
	u_int32				*dP = NULL;
	int32				ret = -1;

	/*------------------------------+
	|  CHAM_InitMem                 |
	+------------------------------*/
	UT_ASSERT( 0 == CHAM_InitMem( &fTbl ) );

	printf("\n");
	printf("========================================================\n");
	printf("===== Test 1.1: V2-API / V2-Tbl: F206N bus #0 (PCI) ====\n");
	printf("========================================================\n");

	/*------------------------------+
	|  CHAM - InitPci               |
	+------------------------------*/
	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex2Bus0;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV2_Ex2Bus0 ) )
		*dP++ = *sP++;

	UT_ASSERT( 0 ==
		fTbl.InitPci(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			0,		/* pciFunc*/
			&chah) );

	if( DoChamV2Calls( fTbl, chah, &G_PropV2_Ex2Bus0 ) )
		goto ABORT;

	/*------------------------------+
	|  CHAM - Term                  |
	+------------------------------*/
	fTbl.Term( &chah );
	UT_ASSERT( 0 == chah );

	printf("\n");
	printf("========================================================\n");
	printf("=== Test 1.2: V2-API / V2-Tbl: F206N bus #1 (Avalon) ===\n");
	printf("========================================================\n");

	/*------------------------------+
	|  Alloc buffer                 |
	+------------------------------*/
	if( !(chaTblBus1P = malloc(MAX_TABLE_SIZE)) )
		goto ABORT;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex2Bus1;
	dP = chaTblBus1P;
	
	while( dP-chaTblBus1P < sizeof( G_ChamTblV2_Ex2Bus1 ) )
		/* set cham-offset in bridge */
		if( *sP == CHAMOFF__BUS1_TO_BUS0 ){
			*dP++ = (u_int32)G_chaTblBus0Bar0P;
			*sP++;
		}
		else
			*dP++ = *sP++;

	/*------------------------------+
	|  CHAM - InitInside            |
	+------------------------------*/
	UT_ASSERT( 0 ==
		fTbl.InitInside(
			NULL,			/* osh */
			chaTblBus1P,	/* tblAddr */
			&chah) );

	if( DoChamV2Calls( fTbl, chah, &G_PropV2_Ex2Bus1 ) )
		goto ABORT;

	/*------------------------------+
	|  CHAM - Term                  |
	+------------------------------*/
	fTbl.Term( &chah );
	UT_ASSERT( 0 == chah );

	printf("\n");
	printf("========================================================\n");
	printf("=== Test 1.3: V2-API / V2-Tbl: BridgeEx bus #0 (PCI) ===\n");
	printf("========================================================\n");

	/*------------------------------+
	|  Alloc buffer                 |
	+------------------------------*/
	if( !(chaTblBus2P = malloc(MAX_TABLE_SIZE)) )
		goto ABORT;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus2;
	dP = chaTblBus2P;
	
	while( dP-chaTblBus2P < sizeof( G_ChamTblV2_Ex3Bus2 ) )
		*dP++ = *sP++;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus1;
	dP = chaTblBus1P;
	
	while( dP-chaTblBus1P < sizeof( G_ChamTblV2_Ex3Bus1 ) )
		/* set cham-offset to WB-table in bridge on AV-bus */
		if( *sP == CHAMOFF__BUS1_TO_BUS0 ){
			*dP++ = (u_int32)G_chaTblBus0Bar0P - (0x2000 + 0x10);
			*sP++;
		}
		else if( *sP == CHAMOFF__BUS1_TO_BUS2 ){
			*dP++ = (u_int32)chaTblBus2P - (0x22000 + 0x10);
			*sP++;
		}
		else
			*dP++ = *sP++;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus0;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV2_Ex3Bus0 ) )
		/* set cham-offset to AV-table in bridge on WB-bus */
		if( *sP == CHAMOFF__BUS0_TO_BUS1 ){
			*dP++ = (u_int32)chaTblBus1P;
			*sP++;
		}
		else if( *sP == CHAMOFF__BUS0_TO_BUS1_ILLTBL ){
			*dP++ = (u_int32)chaTblBus1P + 0x10;	/* simulate ill table */
			*sP++;
		}
		else
			*dP++ = *sP++;

	/*------------------------------+
	|  CHAM - InitPci               |
	+------------------------------*/
	UT_ASSERT( 0 ==
		fTbl.InitPci(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			0,		/* pciFunc*/
			&chah) );

	if( DoChamV2Calls( fTbl, chah, &G_PropV2_Ex3Bus0 ) )
		goto ABORT;

	/*------------------------------+
	|  CHAM - TableIdent            |
	+------------------------------*/
	{
	CHAMELEONV2_TABLE	tbl;
	
	/* first table (at root) must has busId=0 */
	UT_ASSERT( 0 == fTbl.TableIdent( chah, 0, &tbl ) );
	UT_ASSERT( tbl.busId == 0x0000 );
	
	/* second table must has busId=2 because bridge to empty bus */
	UT_ASSERT( 0 == fTbl.TableIdent( chah, 1, &tbl ) );
	UT_ASSERT( tbl.busId == 0x2000 );
	}

	/*------------------------------+
	|  CHAM - Term                  |
	+------------------------------*/
	fTbl.Term( &chah );
	UT_ASSERT( 0 == chah );

	printf("\n");
	printf("========================================================\n");
	printf("============== Test 1.4: V2-API / V0-Dev ===============\n");
	printf("========================================================\n");

	/*------------------------------+
	|  CHAM - InitPci               |
	+------------------------------*/
	/* fill alligned buffer */
	sP = (u_int32*)G_ChamTblV0_Ex1;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV0_Ex1 ) )
		*dP++ = *sP++;

	UT_ASSERT( 0 ==
		fTbl.InitPci(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			0,		/* pciFunc*/
			&chah) );

	if( DoChamV2Calls( fTbl, chah, &G_PropV2_Ex1Bus0 ) )
		goto ABORT;

	/*------------------------------+
	|  CHAM - Term                  |
	+------------------------------*/
	fTbl.Term( &chah );
	UT_ASSERT( 0 == chah );

	ret = 0;

ABORT:
	if( chaTblBus1P )
		free( chaTblBus1P );

	if( chaTblBus2P )
		free( chaTblBus2P );

	return ret;
}

/**********************************************************************/
/** DoChamV2Calls function
 */
int32 DoChamV2Calls(
	CHAM_FUNCTBL		fTbl,
	CHAMELEONV2_HANDLE	*chah,
	PROPV2				*prop )
{
	u_int32		i, ret;

	/*------------------------------+
	|  CHAM - Info                  |
	+------------------------------*/
	{
	CHAMELEONV2_INFO	info;

	UT_ASSERT( 0 ==
		fTbl.Info( chah, &info ) );

	printf("\nCHAMELEONV2_INFO:\n"
		"  chaRev=%d, busId=0x%04x,\n"
		"  tableNbr=%d, unitNbr=%d, bridgeNbr=%d, cpuNbr=%d\n",
		info.chaRev, info.busId,
		info.tableNbr, info.unitNbr, info.bridgeNbr, info.cpuNbr );

	for( i=0; i<6; i++ )
		printf("BAR#%i: addr=0x%x, size=0x%x, type=%s\n", i,
			info.ba[i].addr, info.ba[i].size,
			  (info.ba[i].type == -1) ? "unused" :
			( (info.ba[i].type == 0) ? "mem mapped" : "io mapped") );
	
	UT_ASSERT( info.busId	  == prop->busId );
	UT_ASSERT( info.tableNbr  == prop->tableNbr );
	UT_ASSERT( info.unitNbr   == prop->unitNbr );
	UT_ASSERT( info.bridgeNbr == prop->bridgeNbr );
	UT_ASSERT( info.cpuNbr    == prop->cpuNbr );
	}

	/*------------------------------+
	|  CHAM - TableIdent            |
	+------------------------------*/
	{
	CHAMELEONV2_TABLE	tbl;
	for( i=0; i<1000; i++ ){

		if( CHAMELEONV2_NO_MORE_ENTRIES == (ret = fTbl.TableIdent( chah, i, &tbl )) )
			break;

		UT_ASSERT( 0 == ret );

		printf("\nCHAMELEONV2_HEADER #%d:\n"
			"  busType=0x%02x, busId=0x%04x, model=%c, revision=0x%02x,\n"
			"  file=%s, magicWord=0x%04x, reserved=0x%04x\n",
			i,
			tbl.busType, tbl.busId, tbl.model, tbl.revision,
			tbl.file, tbl.magicWord, tbl.reserved );
	}
	}

	/*------------------------------+
	|  CHAM - UnitIdent             |
	+------------------------------*/
	{
	CHAMELEONV2_UNIT	unit;
	for( i=0; i<1000; i++ ){

		if( CHAMELEONV2_NO_MORE_ENTRIES == (ret = fTbl.UnitIdent( chah, i, &unit )) )
			break;

		UT_ASSERT( 0 == ret );

		printf("\nCHAMELEONV2_UNIT #%d:\n"
			"  TYPE: devId=%03d, variant=0x%02x, revision=0x%02x,\n"
			"  LOC : busId=0x%04x, instance=%d, group=0x%02x,\n"
			"  RES : interrupt=0x%02x, bar=%d, offset=0x%08x,\n"
			"        size=0x%08x, addr=0x%08x, reserved=0x%08x\n",
			i,
			unit.devId, unit.variant, unit.revision,
			unit.busId, unit.instance, unit.group,
			unit.interrupt,	unit.bar, unit.offset,
			unit.size, unit.addr, unit.reserved );
	}
	}

	/*------------------------------+
	|  CHAM - BridgeIdent           |
	+------------------------------*/
	{
	CHAMELEONV2_BRIDGE	brg;
	for( i=0; i<1000; i++ ){

		if( CHAMELEONV2_NO_MORE_ENTRIES == (ret = fTbl.BridgeIdent( chah, i, &brg )) )
			break;

		UT_ASSERT( 0 == ret );

		printf("\nCHAMELEONV2_BRIDGE #%d:\n"
			"  TYPE: devId=%03d, variant=0x%02x, revision=0x%02x,\n"
			"  LOC : busId=0x%04x, instance=%d, group=0x%02x, nextBus=0x%04x,\n"
			"  RES : interrupt=0x%02x, bar=%d, offset=0x%08x,\n"
			"        size=0x%08x, addr=0x%08x\n",
			i,
			brg.devId, brg.variant, brg.revision,
			brg.busId, brg.instance, brg.group, brg.nextBus,
			brg.interrupt,	brg.bar, brg.offset, brg.size, brg.addr );
	}
	}

	/*------------------------------+
	|  CHAM - CpuIdent              |
	+------------------------------*/
	{
	CHAMELEONV2_CPU	cpu;
	for( i=0; i<1000; i++ ){

		if( CHAMELEONV2_NO_MORE_ENTRIES == (ret = fTbl.CpuIdent( chah, i, &cpu )) )
			break;

		UT_ASSERT( 0 == ret );

		printf("\nCHAMELEONV2_CPU #%d:\n"
			"  TYPE: devId=%03d, variant=0x%02x, revision=0x%02x,\n"
			"  LOC : busId=0x%04x, instance=%d, group=0x%02x,\n"
			"  RES : interrupt=0x%02x, bootAddr=0x%01x, reserved=0x%08x\n",
			i,
			cpu.devId, cpu.variant, cpu.revision,
			cpu.busId, cpu.instance, cpu.group,
			cpu.interrupt,	cpu.bootAddr, cpu.reserved );
	}
	}

	/*------------------------------+
	|  CHAM - InstanceFind          |
	+------------------------------*/
	{
	CHAMELEONV2_UNIT	unit;
	CHAMELEONV2_BRIDGE	brg;
	CHAMELEONV2_CPU		cpu;
	CHAMELEONV2_FIND	find;
	u_int8				run;

	for ( run=0; run<2; run++ ){

		/* search for groups */
		find.devId		= -1;
		find.variant	= -1;
		find.instance	= -1;
		find.busId		= -1;
		find.bootAddr	= -1;

		if( run==1 )
			find.flags		= CHAMELEONV2_FF_BRGALL;
		else
			find.flags		= 0;

		printf("\nSearching for groups with flags=0x%x\n", find.flags);

		/* find groups */
		for( find.group=0; find.group<GROUPS; find.group++ ){

			printf("Searching for group %d:\n", find.group);

			i=0;
			do{
				ret = fTbl.InstanceFind( chah, i, find,
								&unit, &brg, &cpu );
				
				switch( ret ){
					case CHAMELEONV2_UNIT_FOUND:
					printf(
						"  Match #%d - Unit: devId=%03d, busId=0x%04x, instance=%d, bar=%d\n",
						i, unit.devId, unit.busId, unit.instance, unit.bar );
					i++;
					break;

					case CHAMELEONV2_BRIDGE_FOUND:
					printf(
						"  Match #%d - Bridge: devId=%03d, busId=0x%04x, instance=%d, nextBus==0x%04x\n",
						i, brg.devId, brg.busId, brg.instance, brg.nextBus );
					i++;
					break;

					case CHAMELEONV2_CPU_FOUND:
					printf(
						"  Match #%d - Cpu:  devId=%03d, busId=0x%04x, instance=%d\n",
						i, cpu.devId, cpu.busId, cpu.instance );
					i++;
					break;
					
					case CHAMELEONV2_NO_MORE_ENTRIES:
						break;
					
					default:
						UT_ASSERT( !ret );
				}

			} while( ret != CHAMELEONV2_NO_MORE_ENTRIES );

			if( i==0 )
				printf("  No match\n");

			if( run==1 ){
				UT_ASSERT( prop->groupFieldFlagBrgAll[find.group] == i );
			}
			else{
				UT_ASSERT( prop->groupField[find.group] == i );
			}
		}/* find groups */
	}/* run */

	/* search for one instance */
	find.devId		= -1;
	find.variant	= -1;
	find.instance	= 22;
	find.busId		= -1;
	find.group		= -1;
	find.bootAddr	= -1;

	printf("\nSearching for instance %d:\n", find.instance);

	i=0;
	do{
		ret = fTbl.InstanceFind( chah, i, find,
						&unit, &brg, &cpu );
		
		switch( ret ){
			case CHAMELEONV2_UNIT_FOUND:
			printf(
				"  Match #%d - Unit: devId=%03d, busId=0x%04x, group=%d, bar=%d\n",
				i, unit.devId, unit.busId, unit.group, unit.bar );
			i++;
			break;

			case CHAMELEONV2_BRIDGE_FOUND:
			printf(
				"  Match #%d - Bridge: devId=%03d, busId=0x%04x, group=%d, nextBus==0x%04x\n",
				i, brg.devId, brg.busId, brg.group, brg.nextBus );
			i++;
			break;

			case CHAMELEONV2_CPU_FOUND:
			printf(
				"  Match #%d - Cpu:  devId=%03d, busId=0x%04x, group=%d\n",
				i, cpu.devId, cpu.busId, cpu.group );
			i++;
			break;
			
			case CHAMELEONV2_NO_MORE_ENTRIES:
				break;
			
			default:
				UT_ASSERT( !ret );
		}

	} while( ret != CHAMELEONV2_NO_MORE_ENTRIES );

	if( i==0 )
		printf("  No match\n");

	UT_ASSERT( prop->inst22Nbr == i );
	}

	return 0;

ABORT:
	return -1;
}

/**********************************************************************/
/** Unit test for chameleon V0 API
 */
int32 UtChamV0Api( void )
{
	CHAMELEON_HANDLE	*chah;
	u_int32				*sP = NULL;
	u_int32				*dP = NULL;
	int32				ret = -1;
	u_int32				*chaTblBus1P=NULL;
	u_int32				*chaTblBus2P=NULL;

	printf("\n");
	printf("========================================================\n");
	printf("============== Test 2.1: V0-API / V0-Dev ===============\n");
	printf("========================================================\n");

	/*------------------------------+
	|  ChameleonInit                |
	+------------------------------*/
	/* fill alligned buffer */
	sP = (u_int32*)G_ChamTblV0_Ex1;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV0_Ex1 ) )
		*dP++ = *sP++;	

	UT_ASSERT( 0 ==
		ChameleonInit(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			&chah) );

	if( DoChamV0Calls( chah, &G_PropV0_Ex1 ) )
		goto ABORT;

	/*------------------------------+
	|  ChameleonTerm                |
	+------------------------------*/
	ChameleonTerm( &chah );
	UT_ASSERT( 0 == chah );

	printf("\n");
	printf("========================================================\n");
	printf("==== Test 2.2: V0-API / V2-Tbl: F206N bus #0 (PCI) =====\n");
	printf("========================================================\n");

	/*------------------------------+
	|  ChameleonInit                |
	+------------------------------*/
	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex2Bus0;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV2_Ex2Bus0 ) )
		*dP++ = *sP++;

	UT_ASSERT( 0 ==
		ChameleonInit(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			&chah) );

	if( DoChamV0Calls( chah, &G_PropV0_Ex2Bus0 ) )
		goto ABORT;

	/*------------------------------+
	|  ChameleonTerm                |
	+------------------------------*/
	ChameleonTerm( &chah );
	UT_ASSERT( 0 == chah );


	printf("\n");
	printf("========================================================\n");
	printf("=== Test 2.3: V0-API / V2-Tbl: BridgeEx bus #0 (PCI) ===\n");
	printf("========================================================\n");
	
	/*------------------------------+
	|  Alloc buffer                 |
	+------------------------------*/
	if( !(chaTblBus2P = malloc(MAX_TABLE_SIZE)) )
		goto ABORT;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus2;
	dP = chaTblBus2P;
	
	while( dP-chaTblBus2P < sizeof( G_ChamTblV2_Ex3Bus2 ) )
		*dP++ = *sP++;

	if( !(chaTblBus1P = malloc(MAX_TABLE_SIZE)) )
		goto ABORT;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus1;
	dP = chaTblBus1P;
	
	while( dP-chaTblBus1P < sizeof( G_ChamTblV2_Ex3Bus1 ) )
		/* set cham-offset to WB-table in bridge on AV-bus */
		if( *sP == CHAMOFF__BUS1_TO_BUS0 ){
			*dP++ = (u_int32)G_chaTblBus0Bar0P - 0x2010;
			*sP++;
		}
		else if( *sP == CHAMOFF__BUS1_TO_BUS2 ){
			*dP++ = (u_int32)chaTblBus2P - (0x22000 + 0x10);
			*sP++;
		}
		else
			*dP++ = *sP++;

	/* fill alligned buffer */
	sP = G_ChamTblV2_Ex3Bus0;
	dP = G_chaTblBus0Bar0P;
	
	while( dP-G_chaTblBus0Bar0P < sizeof( G_ChamTblV2_Ex3Bus0 ) )
		/* set cham-offset to AV-table in bridge on WB-bus */
		if( *sP == CHAMOFF__BUS0_TO_BUS1 ){
			*dP++ = (u_int32)chaTblBus1P;
			*sP++;
		}
		else if( *sP == CHAMOFF__BUS0_TO_BUS1_ILLTBL ){
			*dP++ = (u_int32)chaTblBus1P + 0x10;	/* simulate ill table */
			*sP++;
		}
		else
			*dP++ = *sP++;

	UT_ASSERT( 0 ==
		ChameleonInit(
			NULL,	/* osh */
			1,		/* pciBus */
			10,		/* pciDev */
			&chah) );

	if( DoChamV0Calls( chah, &G_PropV0_Ex3Bus0 ) )
		goto ABORT;

	/*------------------------------+
	|  ChameleonTerm                |
	+------------------------------*/
	ChameleonTerm( &chah );
	UT_ASSERT( 0 == chah );

	ret = 0;

ABORT:
	if( chaTblBus1P )
		free( chaTblBus1P );

	if( chaTblBus2P )
		free( chaTblBus2P );

	return ret;
}

/**********************************************************************/
/** DoChamV0Calls function
 */
int32 DoChamV0Calls(
	CHAMELEON_HANDLE	*chah,
	PROPV0				*prop )
{
	u_int32		i, ret;

	/*------------------------------+
	|  ChameleonGlobalIdent         |
	+------------------------------*/
	{
	char	variant;
	int32		revision;

	UT_ASSERT( 0 ==
		ChameleonGlobalIdent( chah, &variant, &revision ) );

	printf("ChameleonGlobalIdent:\n"
		"  variant=%c, revision=0x%02x\n", variant, revision);

	UT_ASSERT( prop->variant == variant );
	UT_ASSERT( prop->revision == revision );
	}

	/*------------------------------+
	|  ChameleonUnitIdent           |
	+------------------------------*/
	{
	CHAMELEON_UNIT unit;
	for( i=0; i<1000; i++ ){

		if( CHAMELEON_OUT_OF_RANGE == (ret = ChameleonUnitIdent( chah, i, &unit )) )
			break;

		UT_ASSERT( 0 == ret );

		printf("CHAMELEON_UNIT #%d:\n"
			"  TYPE: modCode=0x%04x, revision=0x%02x,\n"
			"  LOC : instance=%d\n"
			"  RES : interrupt=0x%02x, bar=%d, offset=0x%08x,\n"
			"        addr=0x%08x\n",
			i,
			unit.modCode, unit.revision,
			unit.instance,
			unit.interrupt,	unit.bar, unit.offset,
			unit.addr );
	}
	}

	/*------------------------------+
	|  ChameleonUnitFind            |
	+------------------------------*/
	{
	CHAMELEON_UNIT	unit;
	int32 n;

	for( n=0; G_modCodeTbl[n].name; n++ ){

		printf("Searching %s (0x%04x):\n",
			G_modCodeTbl[n].name, G_modCodeTbl[n].modCode);

		i=0;
		do{
			ret = ChameleonUnitFind( chah, G_modCodeTbl[n].modCode,
				i, &unit );
			
			switch( ret ){
				case 0:
				printf(
					"  Unit #%d: modCode=0x%04x, instance=%d,\n"
					"            bar=%d, offset=0x%08x, addr=0x%08x\n",
					i, unit.modCode, unit.instance, unit.bar,
					unit.offset, unit.addr );
					i++;
				break;
				
				case CHAMELEON_UNIT_NOT_FOUND:
				printf(
					"  No (more) units!\n");
					break;
				
				default:
					UT_ASSERT( !ret );
			}

		} while( ret != CHAMELEON_UNIT_NOT_FOUND );

		UT_ASSERT( prop->nbrOfMods[G_modCodeTbl[n].i] == i );

	}/* for */

	}

	return 0;

ABORT:
	return -1;
}


