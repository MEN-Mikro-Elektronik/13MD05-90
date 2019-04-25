/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: assem68k.c
 *      Project: HwBug
 *
 *      $Author: ub $
 *        $Date: 2003/05/26 10:05:46 $
 *    $Revision: 1.1 $
 *
 *  Description: Line-by-Line assembler and disassembler
 *                      
 *                      
 *     Required: - 
 *     Switches: - 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *	disassemble( argc, argv )
 *	assemble( argc, argv )
 *  
 *---------------------------------------------------------------------------
 * Copyright (c) 1993-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
 /*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/assem68k.c,v 1.1 2003/05/26 10:05:46 ub Exp $";
#include "hwbug.h"

u_int8 assembler_buffer[1024];

disassemble( argc, argv )
int argc;
char **argv;
{
	extern int control_c;
	char textbuf[100], *text;
	u_int32 adr, endadr, count = 40, prot_adr;
	static u_int32 last_adr, temp_adr;
		
	if( argc == 0 ){
		adr = temp_adr;
	} else if( argc == 1 ){
		adr = last_adr;
	} else if (argc >= 2 ){
		if( *argv[1] == 'T' )
			adr = (u_int32)assembler_buffer;
		else {	
			if( make_hex( argv[1], &adr ) == -1 )
				return 1;
		}
		if( argc > 2 )
			if( make_hex( argv[2], &count ) == -1 )
				return 1;
		last_adr = temp_adr = adr;
	}		
	prot_adr = adr;

	endadr = adr + count;	
	os_permit_access( prot_adr, count );

	while( adr < endadr && control_c == 0 ){
		printf("%08x    ", adr );

		text = textbuf;
		adr = _disasm( adr, &text);
		*text = '\0';
		printf( "%s\n", textbuf );
	}
	temp_adr = adr;

	os_protect_access( prot_adr, count );

	return 0;
}			
	
assemble( argc, argv )
int argc;
char **argv;
{
	extern int control_c;
	extern char *get_line();
	char textbuf[100], *text;
	u_int32 adr, newadr, prot_adr;
	char *inline;
	int exitflg = 0;
	
	if( argc < 2 ) return 1;

	if( *argv[1] == 'T' )
		adr = (u_int32)assembler_buffer;
	else {
		if( make_hex( argv[1], &adr ) == -1 )
			return 1;
	}		
	prot_adr = adr;

	os_permit_access( prot_adr, 0x1000 );

	do {
		printf( "%08x    ", adr );
		text = textbuf;
		newadr = _disasm( adr, &text);			/* disassemble first */
		*text = '\0';
		printf( "%s\n         ", textbuf );
		
		inline = get_line( "-> " );		/* get input line */
		switch( *inline ){
		case '\0':	adr = newadr; break;
		case 'Q': 	  
		case '.':	exitflg = 1; break;
		default :
			inline[strlen(inline)] = 0xd;

			newadr = _assem( adr, inline );
			if( newadr == adr )
				printf("\n*** bad input, try again.");
			else {
				adr = newadr;
			}
			break;
		} 
		printf("\n");

	} while( !exitflg );			

	os_protect_access( prot_adr, 0x1000 );

	return 0;
}			

_asm("
* _disasm( void *adr, char **bufp )
_disasm:
	movem.l	d1-d7/a0-a6,-(sp)
	move.l	d0,a4
	move.l	d1,a0
	move.l	a0,-(sp)
	move.l	(a0),a6
	bsr		EXTDISS
	movea.l	(sp)+,a0
	move.l	a6,(a0)
	move.l	a3,d0
	movem.l	(sp)+,d1-d7/a0-a6
	rts	

* _assem( void *adr, char *intext )
_assem:
	movem.l	d1-d7/a0-a6,-(sp)
	move.l	d0,a4
	move.l	d1,a6
	bsr		EXTASS
	move.l	a3,d0
	movem.l	(sp)+,d1-d7/a0-a6
	rts	

*----------------------------------------------------------------------
* assembler
* Input:	a4 = address to start assembling
*			a6 = input text
* Output:	a3 = next address to assemble
*
EXTASS:		DC.L 	$60000C92 			

*----------------------------------------------------------------------
* disassembler
* Input:	a4 = address to start disassembling
*			a6 = buffer for text
* Output:	a3 = next address to disassemble
*			a6 = end of text
*
EXTDISS: 	DC.L 	$60200006 			
			DC.W 	$0088,$007E,$0084,$01D2 
			DC.W 	$00B0,$00F2,$009E,$010C 
			DC.W 	$014E,$01A0,$014E,$010C 
			DC.W 	$014E,$01AC,$01A0,$264C 
			DC.W 	$3E14,$764F,$7C80,$544B 
			DC.W 	$4240,$1014,$E808,$E308 
			DC.W 	$303B,$00CC,$41FA,$0004 
			DC.W 	$4EB0,$0000,$6000,$03DC 
			DC.W 	$7646,$3C3C,$8000,$6100 
			DC.W 	$02E8,$0807,$0008,$6638 
			DC.W 	$0C47,$07FF,$6318,$0C47 
			DC.W 	$08C0,$6212,$341B,$C47C 
			DC.W 	$001F,$B23C,$0007,$634C 
			DC.W 	$C43C,$0007,$6046,$763C 
			DC.W 	$6100,$02E8,$6700,$0164 
			DC.W 	$B23C,$003C,$6636,$5A41 
			DC.W 	$4A06,$6B00,$0156,$602C 
			DC.W 	$2601,$6100,$02C0,$C741 
			DC.W 	$0C01,$0007,$631E,$0C01 
			DC.W 	$000F,$6218,$5047,$D23C 
			DC.W 	$0020,$7C40,$0887,$0006 
			DC.W 	$6702,$DC86,$0807,$0007 
			DC.W 	$6602,$C741,$6000,$02B6 
			DC.W 	$3C3C,$0080,$6006,$7C40 
			DC.W 	$6002,$4246,$6100,$026A 
			DC.W 	$2601,$2400,$6100,$026C 
			DC.W 	$3E3C,$1000,$6000,$0296 
			DC.W 	$45FA,$07F1,$7646,$1407 
			DC.W 	$6100,$026A,$3C3C,$8000 
			DC.W 	$4E75,$6100,$0244,$6100 
			DC.W 	$026A,$6700,$0014,$2601 
			DC.W 	$6100,$0252,$C741,$2403 
			DC.W 	$163C,$0040,$6000,$0266 
			DC.W 	$3C3C,$8000,$B23C,$0007 
			DC.W 	$6300,$02C6,$B23C,$000F 
			DC.W 	$6200,$02BE,$5047,$5101 
			DC.W 	$2601,$123C,$0043,$301B 
			DC.W 	$6000,$02AE,$7243,$1007 
			DC.W 	$6708,$3C3C,$00C0,$4880 
			DC.W 	$6006,$3C3C,$0080,$301B 
			DC.W 	$4207,$6000,$0294,$6100 
			DC.W 	$01E8,$2601,$6100,$01FE 
			DC.W 	$6100,$0208,$672A,$0807 
			DC.W 	$0008,$6728,$4A06,$6700 
			DC.W 	$003E,$C343,$0C01,$0007 
			DC.W 	$630E,$0C01,$000F,$6214 
			DC.W 	$4A06,$6B00,$0004,$5003 
			DC.W 	$DE06,$3C3C,$8000,$6004 
			DC.W 	$3C3C,$8040,$6000,$01E6 
			DC.W 	$6100,$01A6,$2601,$6100 
			DC.W 	$01BC,$6100,$01C6,$6734 
			DC.W 	$0807,$0008,$672A,$0C03 
			DC.W 	$0008,$6D1C,$0C03,$000F 
			DC.W 	$621C,$D23C,$0020,$D63C 
			DC.W 	$0018,$5087,$0C47,$B108 
			DC.W 	$660E,$5141,$5143,$6008 
			DC.W 	$BE7C,$B100,$6602,$C741 
			DC.W 	$6000,$01A2,$5081,$7C40 
			DC.W 	$0887,$0008,$67F2,$E34E 
			DC.W 	$60EE,$45FA,$0601,$7247 
			DC.W 	$764F,$7C40,$4E75,$6100 
			DC.W 	$0172,$6716,$6100,$0138 
			DC.W 	$2601,$6100,$0158,$C741 
			DC.W 	$0807,$0005,$6700,$FF00 
			DC.W 	$6006,$6100,$012C,$7C40 
			DC.W 	$6000,$0162,$0807,$0008 
			DC.W 	$6600,$00B4,$0C47,$483F 
			DC.W 	$6300,$00CE,$0C47,$487F 
			DC.W 	$6300,$00CE,$0C47,$48FF 
			DC.W 	$6364,$0C47,$4AF9,$6300 
			DC.W 	$00B8,$0C47,$4AFF,$6334 
			DC.W 	$0C47,$4CFF,$6350,$0C47 
			DC.W 	$4E3F,$6396,$0C47,$4E4F 
			DC.W 	$6300,$00AE,$0C47,$4E5F 
			DC.W 	$6300,$0020,$0C47,$4E6F 
			DC.W 	$6300,$0016,$0C47,$4E72 
			DC.W 	$650A,$677C,$0C47,$4E77 
			DC.W 	$6200,$0086,$724F,$6040 
			DC.W 	$7642,$3C3C,$8040,$6100 
			DC.W 	$00AE,$5041,$0807,$0003 
			DC.W 	$6602,$C741,$0C41,$004F 
			DC.W 	$6626,$723C,$6022,$6100 
			DC.W 	$00A0,$7C40,$0887,$0006 
			DC.W 	$6702,$E34E,$0C01,$0007 
			DC.W 	$630E,$7644,$341B,$0807 
			DC.W 	$000A,$6704,$C741,$C540 
			DC.W 	$B23C,$0020,$6300,$00B6 
			DC.W 	$B23C,$0027,$6200,$00AE 
			DC.W 	$5203,$6000,$00A8,$6100 
			DC.W 	$0068,$3C3C,$8040,$2601 
			DC.W 	$6100,$007A,$0C07,$00BF 
			DC.W 	$6304,$E30E,$5001,$60D0 
			DC.W 	$3C3C,$8040,$723C,$60C8 
			DC.W 	$6100,$0070,$6700,$0014 
			DC.W 	$6100,$003E,$6000,$0076 
			DC.W 	$6100,$0036,$3001,$7246 
			DC.W 	$60AE,$3C3C,$8000,$6100 
			DC.W 	$0028,$0C47,$4A00,$64A0 
			DC.W 	$7C40,$7641,$0807,$000A 
			DC.W 	$6796,$C741,$0887,$0009 
			DC.W 	$668E,$4286,$608A,$3207 
			DC.W 	$C27C,$0007,$9E41,$4E75 
			DC.W 	$3207,$C27C,$003F,$9E41 
			DC.W 	$4E75,$6100,$0010,$3807 
			DC.W 	$C87C,$01C0,$9E44,$E64C 
			DC.W 	$D244,$4E75,$3207,$C27C 
			DC.W 	$0E00,$9E41,$E049,$E249 
			DC.W 	$4E75,$3C3C,$00C0,$CC47 
			DC.W 	$0C06,$00C0,$6704,$9E46 
			DC.W 	$7801,$4E75,$43FA,$033E 
			DC.W 	$6138,$6136,$2046,$1C07 
			DC.W 	$E04F,$2449,$4A19,$6AFC 
			DC.W 	$5489,$BE19,$6616,$BC11 
			DC.W 	$6612,$5789,$6100,$09BC 
			DC.W 	$6708,$4843,$4841,$2C08 
			DC.W 	$4E75,$5689,$5289,$4A19 
			DC.W 	$6BDE,$4A21,$66D4,$6000 
			DC.W 	$FE32,$C343,$3801,$4841 
			DC.W 	$4241,$B83C,$0040,$641A 
			DC.W 	$B83C,$0037,$630C,$983C 
			DC.W 	$0039,$6C02,$5204,$5004 
			DC.W 	$6004,$E64C,$5204,$09C1 
			DC.W 	$4E75,$323C,$FFFF,$4E75 
			DC.W 	$343C,$0F00,$C447,$9E42 
			DC.W 	$E04A,$6188,$224A,$4A1A 
			DC.W 	$66FC,$528A,$4285,$1A12 
			DC.W 	$41FA,$0974,$D1C5,$5488 
			DC.W 	$B418,$66FA,$5748,$95CA 
			DC.W 	$4E75,$1CD9,$57CC,$FFFC 
			DC.W 	$538E,$1CD8,$1CD8,$6600 
			DC.W 	$0006,$5284,$538E,$5584 
			DC.W 	$600E,$7807,$2A0A,$67E2 
			DC.W 	$1CDA,$5BCC,$FFF8,$538E 
			DC.W 	$3E06,$6B1E,$1CFC,$002E 
			DC.W 	$7A42,$EC0E,$670E,$7A57 
			DC.W 	$5306,$6708,$7A4C,$5306 
			DC.W 	$6702,$7A53,$1CC5,$3C07 
			DC.W 	$5544,$1CFC,$0020,$51CC 
			DC.W 	$FFFA,$0C03,$004F,$670E 
			DC.W 	$C741,$C540,$6108,$1CFC 
			DC.W 	$002C,$C343,$C142,$5101 
			DC.W 	$6D00,$006A,$5101,$6D00 
			DC.W 	$006A,$5101,$6D00,$0070 
			DC.W 	$5101,$6D00,$007A,$5101 
			DC.W 	$6D00,$006E,$5101,$6D00 
			DC.W 	$0076,$5101,$6D00,$007C 
			DC.W 	$5301,$6D00,$00AE,$6700 
			DC.W 	$00BE,$5501,$6D00,$00BE 
			DC.W 	$6700,$00DC,$5B01,$6D00 
			DC.W 	$00E8,$6700,$0104,$5501 
			DC.W 	$6D00,$0178,$6700,$018A 
			DC.W 	$5501,$6D00,$0192,$6700 
			DC.W 	$01A2,$5541,$6D00,$0190 
			DC.W 	$6700,$00EC,$5541,$6D00 
			DC.W 	$01D2,$4E75,$1CFC,$0044 
			DC.W 	$6004,$1CFC,$0041,$D23C 
			DC.W 	$0038,$1CC1,$4E75,$1CFC 
			DC.W 	$0028,$61EE,$6000,$004E 
			DC.W 	$1CFC,$002D,$60F0,$61EE 
			DC.W 	$1CFC,$002B,$4E75,$301B 
			DC.W 	$1F01,$6100,$00B8,$121F 
			DC.W 	$60DC,$301B,$1F01,$4880 
			DC.W 	$6100,$00AA,$121F,$1CFC 
			DC.W 	$0028,$61BE,$1CFC,$002C 
			DC.W 	$4241,$122B,$FFFE,$E859 
			DC.W 	$6100,$FF3C,$1CFC,$002E 
			DC.W 	$7057,$4A41,$6A00,$0004 
			DC.W 	$704C,$1CC0,$1CFC,$0029 
			DC.W 	$4E75,$7853,$301B,$48C0 
			DC.W 	$1F04,$6100,$00A6,$1CFC 
			DC.W 	$002E,$1CDF,$4E75,$784C 
			DC.W 	$201B,$60EC,$301B,$6114 
			DC.W 	$1CFC,$0028,$6100,$0004 
			DC.W 	$60D2,$1CFC,$0050,$1CFC 
			DC.W 	$0043,$4E75,$48C0,$D08C 
			DC.W 	$5480,$6000,$0076,$301B 
			DC.W 	$4880,$61F0,$1CFC,$0028 
			DC.W 	$61E0,$1CFC,$002C,$6090 
			DC.W 	$1CFC,$0023,$4280,$4A06 
			DC.W 	$670E,$6A06,$201B,$6000 
			DC.W 	$0052,$301B,$6000,$004C 
			DC.W 	$528B,$101B,$6000,$0044 
			DC.W 	$4A00,$6602,$5000,$1CFC 
			DC.W 	$0023,$4880,$48C0,$7810 
			DC.W 	$4A80,$6A06,$4480,$1CFC 
			DC.W 	$002D,$1CFC,$0024,$307C 
			DC.W 	$FFFE,$224E,$2A00,$4840 
			DC.W 	$4245,$4845,$8AC4,$3005 
			DC.W 	$4840,$3A00,$8AC4,$3005 
			DC.W 	$4845,$610A,$4A80,$66E4 
			DC.W 	$4E75,$7810,$60D4,$4A45 
			DC.W 	$6728,$7230,$BA3C,$0009 
			DC.W 	$6302,$7237,$DA01,$2045 
			DC.W 	$220E,$9289,$224E,$5389 
			DC.W 	$5381,$6B06,$1299,$5589 
			DC.W 	$60F6,$5289,$528E,$1285 
			DC.W 	$4E75,$5248,$BA48,$66D2 
			DC.W 	$4E75,$7853,$4A46,$6600 
			DC.W 	$0008,$1CFC,$0043,$7843 
			DC.W 	$1CC4,$1CFC,$0052,$4E75 
			DC.W 	$1CFC,$0055,$1CFC,$0053 
			DC.W 	$1CFC,$0050,$4E75,$48C0 
			DC.W 	$D08C,$5480,$609C,$780F 
			DC.W 	$3200,$E251,$E350,$51CC 
			DC.W 	$FFFA,$72FF,$5241,$B23C 
			DC.W 	$000F,$6224,$E248,$64F4 
			DC.W 	$6100,$0022,$E248,$6410 
			DC.W 	$5241,$E248,$65FA,$5349 
			DC.W 	$1CFC,$002D,$6100,$000E 
			DC.W 	$5241,$1CFC,$002F,$60D4 
			DC.W 	$534E,$4E75,$48A7,$C000 
			DC.W 	$6100,$FDCC,$4C9F,$0003 
			DC.W 	$4E75,$264C,$4280,$301B 
			DC.W 	$6000,$FF48,$4142,$4344 
			DC.W 	$8311,$77C1,$0083,$BB77 
			DC.W 	$C108,$4144,$448F,$5174 
			DC.W 	$D000,$8F1E,$72D1,$008D 
			DC.W 	$526D,$D0C0,$8F63,$6A06 
			DC.W 	$0041,$4444,$418D,$526D 
			DC.W 	$D0C0,$4144,$4449,$8F63 
			DC.W 	$6A06,$0041,$4444,$518F 
			DC.W 	$6360,$5000,$8F62,$6050 
			DC.W 	$0041,$4444,$588F,$1176 
			DC.W 	$D100,$8FBB,$76D1,$0841 
			DC.W 	$4E44,$8F41,$74C0,$008F 
			DC.W 	$1E72,$C100,$8F64,$6A02 
			DC.W 	$008F,$686A,$023C,$414E 
			DC.W 	$4449,$8F64,$6A02,$008F 
			DC.W 	$686A,$023C,$4153,$4C8F 
			DC.W 	$6160,$E100,$8F11,$72E1 
			DC.W 	$208F,$0323,$E1C0,$4153 
			DC.W 	$528F,$6160,$E000,$8F11 
			DC.W 	$72E0,$208F,$0323,$E0C0 
			DC.W 	$4243,$4847,$8163,$6908 
			DC.W 	$4081,$1371,$0140,$4243 
			DC.W 	$4C52,$8163,$6908,$8081 
			DC.W 	$1371,$0180,$4253,$4554 
			DC.W 	$8163,$6908,$C081,$1371 
			DC.W 	$01C0,$4254,$5354,$8163 
			DC.W 	$6908,$0081,$1371,$0100 
			DC.W 	$4200,$1103,$990F,$0660 
			DC.W 	$0043,$484B,$8541,$7B41 
			DC.W 	$8043,$4C52,$8F03,$7242 
			DC.W 	$0043,$4D50,$8F51,$74B0 
			DC.W 	$008D,$526D,$B0C0,$8F63 
			DC.W 	$6A0C,$008F,$CC76,$B108 
			DC.W 	$434D,$5041,$8D52,$6DB0 
			DC.W 	$C043,$4D50,$498F,$636A 
			DC.W 	$0C00,$434D,$504D,$8FCC 
			DC.W 	$76B1,$0844,$4956,$5385 
			DC.W 	$417B,$81C0,$4449,$5655 
			DC.W 	$8541,$7B80,$C044,$4385 
			DC.W 	$0FAF,$FEFE,$4442,$0010 
			DC.W 	$0089,$1F04,$50C8,$454F 
			DC.W 	$528F,$1372,$B100,$8F63 
			DC.W 	$6A0A,$008F,$686A,$0A3C 
			DC.W 	$454F,$5249,$8F63,$6A0A 
			DC.W 	$008F,$686A,$0A3C,$4558 
			DC.W 	$4789,$1137,$C140,$8912 
			DC.W 	$37C1,$8089,$2137,$C180 
			DC.W 	$8922,$37C1,$4045,$5854 
			DC.W 	$8D01,$A148,$804A,$4D50 
			DC.W 	$8907,$234E,$C04A,$5352 
			DC.W 	$8907,$234E,$804C,$4541 
			DC.W 	$8972,$7941,$C04C,$494E 
			DC.W 	$4B89,$2631,$4E50,$8902 
			DC.W 	$2F4E,$504C,$534C,$8F61 
			DC.W 	$60E1,$088F,$1172,$E128 
			DC.W 	$8F03,$23E3,$C04C,$5352 
			DC.W 	$8F61,$60E0,$088F,$1172 
			DC.W 	$E028,$8F03,$23E2,$C04D 
			DC.W 	$4F56,$458F,$539B,$1000 
			DC.W 	$8748,$5E44,$C087,$8323 
			DC.W 	$40C0,$8929,$1E4E,$6089 
			DC.W 	$9221,$4E68,$8D52,$9B10 
			DC.W 	$004D,$4F56,$4541,$8D52 
			DC.W 	$9B10,$004D,$4F56,$454D 
			DC.W 	$8DA7,$4648,$808D,$AB46 
			DC.W 	$4880,$8D7A,$444C,$808D 
			DC.W 	$CA44,$4C80,$4D4F,$5645 
			DC.W 	$508D,$1D59,$0188,$8DD1 
			DC.W 	$5B01,$084D,$4F56,$4551 
			DC.W 	$8961,$8E70,$004D,$554C 
			DC.W 	$5385,$417B,$C1C0,$4D55 
			DC.W 	$4C55,$8541,$7BC0,$C04E 
			DC.W 	$4243,$4483,$0323,$4800 
			DC.W 	$4E45,$478F,$0372,$4400 
			DC.W 	$4E45,$4758,$8F03,$7240 
			DC.W 	$004E,$4F50,$8100,$234E 
			DC.W 	$714E,$4F54,$8F03,$7246 
			DC.W 	$004F,$528F,$4174,$8000 
			DC.W 	$8F1E,$7281,$008F,$636A 
			DC.W 	$0000,$8F68,$6A00,$3C4F 
			DC.W 	$5249,$8F63,$6A00,$008F 
			DC.W 	$686A,$003C,$5045,$4189 
			DC.W 	$0723,$4840,$5245,$5345 
			DC.W 	$5481,$0023,$4E70,$524F 
			DC.W 	$4C8F,$6160,$E118,$8F11 
			DC.W 	$72E1,$388F,$0323,$E7C0 
			DC.W 	$524F,$528F,$6160,$E018 
			DC.W 	$8F11,$72E0,$388F,$0323 
			DC.W 	$E6C0,$524F,$584C,$8F61 
			DC.W 	$60E1,$108F,$1172,$E130 
			DC.W 	$8F03,$23E5,$C052,$4F58 
			DC.W 	$528F,$6160,$E010,$8F11 
			DC.W 	$72E0,$308F,$0323,$E4C0 
			DC.W 	$5254,$4581,$0023,$4E73 
			DC.W 	$5254,$5281,$0023,$4E77 
			DC.W 	$5254,$5381,$0023,$4E75 
			DC.W 	$5342,$4344,$8311,$7781 
			DC.W 	$0083,$BB77,$8108,$5354 
			DC.W 	$4F50,$8106,$204E,$7253 
			DC.W 	$5542,$8F51,$7490,$008F 
			DC.W 	$1E72,$9100,$8D52,$6D90 
			DC.W 	$C08F,$636A,$0400,$5355 
			DC.W 	$4241,$8D52,$6D90,$C053 
			DC.W 	$5542,$498F,$636A,$0400 
			DC.W 	$5355,$4251,$8F63,$6051 
			DC.W 	$008F,$6260,$5100,$5355 
			DC.W 	$4258,$8F11,$7691,$008F 
			DC.W 	$BB76,$9108,$5357,$4150 
			DC.W 	$8901,$2348,$4053,$0010 
			DC.W 	$0083,$0323,$50C0,$5441 
			DC.W 	$5383,$0323,$4AC0,$5452 
			DC.W 	$4150,$8106,$B64E,$4054 
			DC.W 	$5241,$5056,$8100,$234E 
			DC.W 	$7654,$5354,$8F03,$724A 
			DC.W 	$0055,$4E4C,$4B89,$0221 
			DC.W 	$4E58,$0000,$4343,$520C 
			DC.W 	$5553,$508D,$5352,$004C 
			DC.W 	$528E,$6100,$0132,$7204 
			DC.W 	$4A06,$6B06,$6100,$04F6 
			DC.W 	$7202,$E149,$4841,$D2BC 
			DC.W 	$003C,$0800,$4E75,$BD08 
			DC.W 	$6600,$014E,$BD08,$670A 
			DC.W 	$538E,$4A28,$FFFF,$6600 
			DC.W 	$0140,$4281,$1010,$01C1 
			DC.W 	$C03C,$00C0,$B006,$6600 
			DC.W 	$0130,$4E75,$0801,$0006 
			DC.W 	$6714,$92BC,$0210,$0030 
			DC.W 	$0C1E,$002B,$6708,$538E 
			DC.W 	$92BC,$0008,$0008,$4E75 
			DC.W 	$3001,$E648,$D07C,$0011 
			DC.W 	$01C1,$4240,$1801,$0C16 
			DC.W 	$002F,$670A,$0C16,$002D 
			DC.W 	$671C,$4841,$4E75,$09C0 
			DC.W 	$528E,$6100,$0128,$6600 
			DC.W 	$00E8,$03C0,$1801,$223C 
			DC.W 	$4000,$0200,$60D8,$528E 
			DC.W 	$6100,$0112,$6600,$00D2 
			DC.W 	$B801,$6302,$C941,$03C0 
			DC.W 	$B204,$57C9,$FFFA,$60DE 
			DC.W 	$4280,$0C16,$0023,$6700 
			DC.W 	$FF48,$6100,$00BC,$6784 
			DC.W 	$6100,$00EA,$679A,$41FA 
			DC.W 	$FF2C,$7002,$BD08,$6700 
			DC.W 	$FF4E,$5688,$538E,$51C8 
			DC.W 	$FFF4,$0C16,$002D,$673E 
			DC.W 	$0C16,$000D,$672C,$614E 
			DC.W 	$6100,$008E,$6726,$2200 
			DC.W 	$48C1,$B081,$6620,$4A06 
			DC.W 	$6A10,$41FA,$FCE5,$B3C8 
			DC.W 	$6714,$41FA,$FCE5,$B3C8 
			DC.W 	$670C,$223C,$0238,$0100 
			DC.W 	$6002,$7201,$4E75,$223C 
			DC.W 	$0439,$0100,$4E75,$528E 
			DC.W 	$6156,$534E,$66C0,$524E 
			DC.W 	$0801,$0006,$6742,$92BC 
			DC.W 	$0208,$0020,$4E75,$3F3C 
			DC.W 	$0001,$4281,$538E,$0C2E 
			DC.W 	$002D,$0001,$6604,$4257 
			DC.W 	$528E,$528E,$6100,$03EC 
			DC.W 	$4A57,$6704,$D280,$6002 
			DC.W 	$9280,$5257,$0C16,$002B 
			DC.W 	$67E8,$4257,$0C16,$002D 
			DC.W 	$67E0,$425F,$2001,$4E75 
			DC.W 	$264C,$2E4D,$4280,$4E75 
			DC.W 	$0C16,$0028,$662C,$528E 
			DC.W 	$6132,$6760,$0C16,$002A 
			DC.W 	$670C,$0C1E,$0050,$66E0 
			DC.W 	$0C1E,$0043,$66DA,$223C 
			DC.W 	$023A,$0200,$615A,$6604 
			DC.W 	$08C1,$0010,$0C1E,$0029 
			DC.W 	$66C6,$4E75,$4281,$0C16 
			DC.W 	$0044,$671C,$7208,$0C16 
			DC.W 	$0041,$6714,$0C16,$0053 
			DC.W 	$6620,$0C2E,$0050,$0001 
			DC.W 	$6618,$720F,$548E,$6010 
			DC.W 	$528E,$D216,$923C,$0030 
			DC.W 	$6D96,$0C1E,$0037,$6290 
			DC.W 	$4205,$4E75,$4841,$D2BC 
			DC.W 	$0220,$0040,$610A,$66B4 
			DC.W 	$D2BC,$0008,$0000,$60AC 
			DC.W 	$0C16,$002C,$663C,$528E 
			DC.W 	$2801,$2200,$4880,$6100 
			DC.W 	$02F2,$61A0,$6600,$FF62 
			DC.W 	$E949,$E149,$0C16,$002E 
			DC.W 	$6616,$528E,$0C1E,$0057 
			DC.W 	$670E,$0C2E,$004C,$FFFF 
			DC.W 	$6600,$FF46,$08C1,$000B 
			DC.W 	$D200,$3001,$2204,$E349 
			DC.W 	$4204,$4E75,$2A4F,$264C 
			DC.W 	$0C1E,$0020,$67FA,$538E 
			DC.W 	$4287,$204E,$43FA,$FA0E 
			DC.W 	$B30E,$67FC,$7C01,$5349 
			DC.W 	$4A11,$6700,$00F0,$6A38 
			DC.W 	$538E,$0C16,$002E,$6618 
			DC.W 	$528E,$7803,$101E,$E34E 
			DC.W 	$B03B,$480A,$670A,$51CC 
			DC.W 	$FFF6,$6024,$534C,$5742 
			DC.W 	$0C16,$0020,$672A,$0C16 
			DC.W 	$000D,$6724,$4A11,$6710 
			DC.W 	$6ABE,$5A89,$2C48,$60F4 
			DC.W 	$4A19,$6AFC,$5389,$60EC 
			DC.W 	$6000,$FECE,$548A,$51CC 
			DC.W 	$00B2,$60EC,$1E12,$60B2 
			DC.W 	$0C1E,$0020,$67FA,$538E 
			DC.W 	$3F06,$0806,$0000,$6602 
			DC.W 	$E40E,$ED0E,$7601,$6100 
			DC.W 	$FDE8,$0C16,$002C,$660A 
			DC.W 	$528E,$2601,$2400,$6100 
			DC.W 	$FDD8,$0C1E,$000D,$66C0 
			DC.W 	$3806,$3C1F,$3F04,$3806 
			DC.W 	$C819,$6706,$610C,$6600 
			DC.W 	$00A8,$5889,$4A11,$6AA8 
			DC.W 	$60EC,$780F,$C811,$E34C 
			DC.W 	$3A01,$CA7B,$4074,$670C 
			DC.W 	$1811,$E80C,$E34C,$3A03 
			DC.W 	$CA7B,$4066,$4E75,$4600 
			DC.W 	$0154,$0000,$4849,$024C 
			DC.W 	$5303,$4343,$0443,$5305 
			DC.W 	$4E45,$0645,$5107,$5643 
			DC.W 	$0856,$5309,$504C,$0A4D 
			DC.W 	$490B,$4745,$0C4C,$540D 
			DC.W 	$4754,$0E4C,$450F,$5352 
			DC.W 	$0152,$4100,$4240,$4244 
			DC.W 	$5289,$1819,$1019,$45FB 
			DC.W 	$00BE,$538E,$BD0A,$6600 
			DC.W 	$FF44,$BD0A,$6700,$FF46 
			DC.W 	$538E,$4A2A,$FFFF,$6700 
			DC.W 	$FF3C,$538A,$6000,$FF2E 
			DC.W 	$0001,$0002,$0004,$01FA 
			DC.W 	$0FFA,$0FFE,$0800,$07C8 
			DC.W 	$1000,$2000,$4000,$0020 
			DC.W 	$0010,$0040,$01F8,$0300 
			DC.W 	$3C1F,$4841,$4843,$5289 
			DC.W 	$1819,$E34C,$DE19,$E14F 
			DC.W 	$1E19,$41FA,$0004,$4EB0 
			DC.W 	$4000,$200B,$908C,$4E75 
			DC.W 	$DE03,$7C80,$0801,$0018 
			DC.W 	$6704,$908B,$5580,$2200 
			DC.W 	$4A06,$6B0A,$6712,$4881 
			DC.W 	$48C1,$B081,$6710,$6100 
			DC.W 	$0120,$36C7,$6000,$002C 
			DC.W 	$4881,$6100,$0116,$DE01 
			DC.W 	$36C7,$4E75,$C741,$6002 
			DC.W 	$4201,$C23C,$00C7,$36C7 
			DC.W 	$D32B,$FFFF,$E049,$670C 
			DC.W 	$0C01,$0002,$6704,$26C0 
			DC.W 	$4E75,$36C0,$4E75,$2601 
			DC.W 	$4280,$6100,$00F0,$323C 
			DC.W 	$0200,$6000,$0082,$0C01 
			DC.W 	$0007,$6302,$C741,$E65B 
			DC.W 	$E659,$C63C,$0007,$8601 
			DC.W 	$4201,$E759,$E75B,$606E 
			DC.W 	$C343,$C142,$1601,$4201 
			DC.W 	$EE4E,$ED4E,$B63C,$0020 
			DC.W 	$634E,$B63C,$0027,$6248 
			DC.W 	$780F,$4843,$E352,$E253 
			DC.W 	$51CC,$FFFA,$3403,$4843 
			DC.W 	$6036,$C741,$C540,$EE4E 
			DC.W 	$ED4E,$6030,$E74E,$6028 
			DC.W 	$2602,$B6BC,$0000,$0008 
			DC.W 	$6200,$FCF6,$0883,$0003 
			DC.W 	$6012,$4206,$1601,$4201 
			DC.W 	$600E,$E34E,$4206,$DE46 
			DC.W 	$6010,$4206,$C343,$C142 
			DC.W 	$DE46,$600A,$DE06,$C63C 
			DC.W 	$00C7,$C23C,$00C7,$4244 
			DC.W 	$1801,$E189,$EB49,$E849 
			DC.W 	$E64C,$ED4C,$D244,$DE41 
			DC.W 	$E089,$4201,$C540,$C741 
			DC.W 	$6100,$FF34,$C343,$C540 
			DC.W 	$6000,$FF32,$2602,$4883 
			DC.W 	$48C3,$B682,$670A,$B4BC 
			DC.W 	$0000,$00FF,$6200,$FC92 
			DC.W 	$C67C,$00FF,$60C0,$E31E 
			DC.W 	$E34E,$ED0E,$EB4E,$DE46 
			DC.W 	$60B4,$EE4E,$ED4E,$609C 
			DC.W 	$2200,$48C1,$B280,$6600 
			DC.W 	$FC70,$4E75,$B0BC,$0000 
			DC.W 	$FFFF,$63F6,$60EA,$0801 
			DC.W 	$0019,$6600,$FC5C,$61EC 
			DC.W 	$36C0,$4E75,$2200,$C27C 
			DC.W 	$000F,$B280,$6600,$FC4A 
			DC.W 	$6000,$FECC,$1A1E,$BA3C 
			DC.W 	$0027,$6762,$E188,$D005 
			DC.W 	$60F2,$200C,$0C1E,$002A 
			DC.W 	$6754,$4280,$7810,$0C2E 
			DC.W 	$0024,$FFFF,$6714,$5D84 
			DC.W 	$0C2E,$0026,$FFFF,$670A 
			DC.W 	$0C2E,$0027,$FFFF,$67CC 
			DC.W 	$534E,$4285,$1A1E,$9A7C 
			DC.W 	$0030,$BA7C,$0009,$630A 
			DC.W 	$BA7C,$0010,$6300,$FBFA 
			DC.W 	$5F45,$BA84,$6216,$3F00 
			DC.W 	$4840,$C0C4,$4840,$4240 
			DC.W 	$2040,$301F,$C0C4,$D085 
			DC.W 	$D088,$60CE,$534E,$4E75 
			DC.W 	$0000,$0000,$0000,$0000 
			DC.W 	$0000,$0000,$0000,$0000 
			align 
");


