;Copyright (C) 1997-2001 ZSNES Team ( zsknight@zsnes.com / _demo_@zsnes.com )
;
;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;as published by the Free Software Foundation; either
;version 2 of the License, or (at your option) any later
;version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

%include "macros.mac"

EXTSYM AddSub256,InitVesa2,cbitmode,cvidmode,makepal
EXTSYM scanlines,selcA000,vesa2_bits,vesa2_bpos,vesa2_clbit
EXTSYM vesa2_gpos,vesa2_rpos,vesa2_usbit,vesa2_x,vesa2_y
EXTSYM vesa2selec,InitVesa12,videotroub

%ifdef __MSDOS__
EXTSYM cscopymodeq,cscopymodex
%endif

NEWSYM InitVidAsmStart


SECTION .bss

NEWSYM clearfilter, resw 1
NEWSYM res640, resb 1
NEWSYM res480, resb 1


SECTION .text

NEWSYM dosinitvideo2
    jmp dosinitvideo.initmodeq256

;*******************************************************
; InitVideo
;*******************************************************
NEWSYM dosinitvideo
    mov byte[cbitmode],0
    mov byte[res640],0
    mov byte[res480],0
    cmp byte[cvidmode],0
    je near .initmodeq224
    cmp byte[cvidmode],1
    je near .initmodeq240
    cmp byte[cvidmode],2
    je near .initmodeq256
    cmp byte[cvidmode],3
    je near .initmodex224
    cmp byte[cvidmode],4
    je near .initmodex240
    cmp byte[cvidmode],5
    je near .initmodex256
    cmp byte[cvidmode],6
    je near .initvesa12640x480x16
    cmp byte[cvidmode],7
    je near .initvesa2320x240x8
    cmp byte[cvidmode],8
    je near .initvesa2320x240x16
    cmp byte[cvidmode],9
    je near .initvesa2320x480x8
    cmp byte[cvidmode],10
    je near .initvesa2320x480x16
    cmp byte[cvidmode],11
    je near .initvesa2512x384x8
    cmp byte[cvidmode],12
    je near .initvesa2512x384x16
    cmp byte[cvidmode],13
    je near .initvesa2640x480x8
    cmp byte[cvidmode],14
    je near .initvesa2640x480x16
    ret

%include "dos/vga.inc"


;*******************************************************
; InitModeQ 224             Sets up 256x224 chained mode
;*******************************************************

.initmodeq224
%ifdef __MSDOS__
    SetVGAMode .Mode256x224c
    call cscopymodeq
%endif
    call makepal
    ret

;*******************************************************
; InitModeQ 240             Sets up 256x240 chained mode
;*******************************************************

.initmodeq240
%ifdef __MSDOS__
    SetVGAMode .Mode256x240c
    call cscopymodeq
%endif
    call makepal
    ret

;*******************************************************
; InitModeQ 256             Sets up 256x256 chained mode
;*******************************************************

.initmodeq256
%ifdef __MSDOS__
    cmp byte[scanlines],1
    jne .noscanlines
    jmp .scanlines
.noscanlines
    SetVGAMode .Mode256x256c
    jmp .done    
.scanlines
    SetVGAMode .Mode256x256cs
    jmp .done    
.done
    call cscopymodeq
%endif
    call makepal
    ret


;*******************************************************
; InitModeX 224           Sets up 320x224 unchained mode
;*******************************************************

.initmodex224
%ifdef __MSDOS__
    SetVGAMode .Mode320x224
    call cscopymodex
%endif
    call makepal
    ret

;*******************************************************
; InitModeX 240           Sets up 320x240 unchained mode
;*******************************************************

.initmodex240
%ifdef __MSDOS__
    SetVGAMode .Mode320x240
    call cscopymodex
%endif
    call makepal
    ret

;*******************************************************
; InitModeX 256           Sets up 320x256 unchained mode
;*******************************************************

.initmodex256
%ifdef __MSDOS__
    SetVGAMode .Mode320x256
    call cscopymodex
%endif
    call makepal
    ret


;*******************************************************
; InitVESA2 320x240x8           Set up Linear 320x240x8b
;*******************************************************

.initvesa2320x240x8
    mov byte[cbitmode],0
    mov word[vesa2_x],320
    mov word[vesa2_y],240
    mov byte[vesa2_bits],8
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble
    ret
.notrouble
    call makepal
    ; clear screen (320*240 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,320*240
.loop
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loop
    pop es
    ret

;*******************************************************
; InitVESA2 320x240x16         Set up Linear 320x240x16b
;*******************************************************

.initvesa2320x240x16
    mov byte[cbitmode],1
    mov word[vesa2_x],320
    mov word[vesa2_y],240
    mov byte[vesa2_bits],16
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble2
    ret
.notrouble2
    ; clear screen (320*240*2 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,320*240*2
.loopb
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopb
    pop es
    ret

;*******************************************************
; InitVESA2 640x480x8           Set up Linear 640x480x8b
;*******************************************************

.initvesa2640x480x8
    mov byte[res640],1
    mov byte[res480],1
    mov word[vesa2_x],640
    mov word[vesa2_y],480
    mov byte[vesa2_bits],8
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble3
    ret
.notrouble3
    call makepal
    ; clear screen (640*480 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,640*480
.loopc3
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopc3
    pop es
    ret

;*******************************************************
; InitVESA2 640x480x16         Set up Linear 640x480x16b
;*******************************************************

.initvesa2640x480x16
    mov byte[res640],1
    mov byte[res480],1
    mov byte[cbitmode],1
    mov word[vesa2_x],640
    mov word[vesa2_y],480
    mov byte[vesa2_bits],16
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble4
    ret
.notrouble4
    ; clear screen (640*480*2 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,640*480*2
.loopd3
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopd3
    pop es
    ret

;*******************************************************
; InitVESA2 320x480x8           Set up Linear 320x480x8b
;*******************************************************

.initvesa2320x480x8
    mov byte[res480],1
    mov word[vesa2_x],320
    mov word[vesa2_y],480
    mov byte[vesa2_bits],8
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble5
    ret
.notrouble5
    call makepal
    ; clear screen (320*480 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,320*480
.loopc
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopc
    pop es
    ret

;*******************************************************
; InitVESA2 320x480x16         Set up Linear 320x480x16b
;*******************************************************

.initvesa2320x480x16
    mov byte[res480],1
    mov byte[cbitmode],1
    mov word[vesa2_x],320
    mov word[vesa2_y],480
    mov byte[vesa2_bits],16
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble6
    ret
.notrouble6
    ; clear screen (320*480*2 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,320*480*2
.loopd
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopd
    pop es
    ret

;*******************************************************
; InitVESA2 512x384x8           Set up Linear 512x384x8b
;*******************************************************

.initvesa2512x384x8
    mov byte[res640],2
    mov word[vesa2_x],512
    mov word[vesa2_y],384
    mov byte[vesa2_bits],8
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble7
    ret
.notrouble7
    call makepal
    ; clear screen (512*384 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,512*384
.loope
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loope
    pop es
    ret

;*******************************************************
; InitVESA2 512x384x16         Set up Linear 512x384x16b
;*******************************************************

.initvesa2512x384x16
    mov byte[res640],2
    mov byte[cbitmode],1
    mov word[vesa2_x],512
    mov word[vesa2_y],384
    mov byte[vesa2_bits],16
    call InitVesa2
    cmp byte[videotroub],1
    jne .notrouble8
    ret
.notrouble8
    ; clear screen (512*384*2 bytes)
    push es
    mov ax,[vesa2selec]
    mov es,ax
    mov edi,0
    mov ecx,512*384*2
.loopf
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopf
    pop es
    ret


;*******************************************************
; InitVESA1.2 640x480x16              Set up 640x480x16b
;*******************************************************

.initvesa12640x480x16
    mov byte[res640],1
    mov byte[cbitmode],1
    mov word[vesa2_x],640
    mov word[vesa2_y],480
    mov byte[vesa2_bits],16
    call InitVesa12
    ret

    ; clear screen (640*480*2 bytes)
    push es
    mov ax,[selcA000]
    mov es,ax
    mov ebx,9
    xor edx,edx
.loopbanks
    
    xor edi,edi
    mov ecx,16384
.loopg
    mov byte[es:edi],0
    inc edi
    dec ecx
    jnz .loopg
    dec ebx
    jnz .loopbanks

    mov ecx,6144
    pop es
    ret

NEWSYM InitVidAsmEnd
