.include "x16.inc"

.org $080D
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

   jmp start

; VRAM Addresses
; https://docs.google.com/spreadsheets/d/1n0DPc4DzMAWshT9GZvgzJAs2BIdy6EfK9pPbRWDD-3A/edit?usp=sharing

VRAM_vert_sprite_base     = $0000
VRAM_horiz_sprite_base    = $0400

;
; VERA CONFIGS
;
; 640x4?0
VERA_pixel_scale = $80
; enable both layers + sprites + vga
VERA_mode = %01000001
; 64 x 64 tile map, 16 color mode, 1bpp
VERA_tile_config = %01010000
; USE CHANNEL 0 for input
VERA_channel = %11111110

vertical_filename:
.byte "vbar.bin"
end_vertical_filename:
VERT_FILENAME_LENGTH = end_vertical_filename - vertical_filename

horiz_filename:
.byte "hbar.bin"
end_horiz_filename:
HORIZ_FILENAME_LENGTH = end_horiz_filename - horiz_filename

; sprite configs 
BPP8_MASK                = %10000000
VERT_X                   = 144
VERT_Y                   = 250
VERT_8x64                = %11000000
SPRITE_COLMASK_Z3_NOFLIP = %00001100

HORIZ_X                  = 260
HORIZ_Y                  = 180
HORIZ_64x8               = %00110000

default_irq_vector: .addr 0

start:
   ; channel select
   lda VERA_ctrl
   and #VERA_channel
   sta VERA_ctrl

   ; resolution
   lda #VERA_pixel_scale
   sta VERA_dc_hscale
   sta VERA_dc_vscale

   ; disable display during setup
   stz VERA_dc_video
   
   ; load bins to VRAM
   VRAM_LOAD_FILE vertical_filename, VERT_FILENAME_LENGTH, VRAM_vert_sprite_base
   VRAM_LOAD_FILE horiz_filename, HORIZ_FILENAME_LENGTH, VRAM_horiz_sprite_base

   ; light up the sprite
   VERA_SET_ADDR VRAM_sprattr, 1

   ; set sprite frame address
   lda #<(VRAM_vert_sprite_base >> 5)
   sta VERA_data0
   lda #>(VRAM_vert_sprite_base >> 5)
   ora #BPP8_MASK
   sta VERA_data0

   ; position
   lda #<VERT_X
   sta VERA_data0
   lda #>VERT_X
   sta VERA_data0
   lda #<VERT_Y
   sta VERA_data0
   lda #>VERT_Y
   sta VERA_data0

   ; attributes
   lda #(SPRITE_COLMASK_Z3_NOFLIP)
   sta VERA_data0
   ;size
   lda #(VERT_8x64)
   sta VERA_data0



  ; HORIZ


   ; VRAM_LOAD_FILE horiz_filename, HORIZ_FILENAME_LENGTH, VRAM_horiz_sprite_base
   ; VERA_SET_ADDR (VRAM_sprattr+8), 1
   
; set sprite frame address
   lda #<(VRAM_horiz_sprite_base >> 5)
   sta VERA_data0
   lda #>(VRAM_horiz_sprite_base >> 5)
   ora #BPP8_MASK
   sta VERA_data0

   ; ; position
   lda #<HORIZ_X
   sta VERA_data0
   lda #>HORIZ_X
   sta VERA_data0
   lda #<HORIZ_Y
   sta VERA_data0
   lda #>HORIZ_Y
   sta VERA_data0

   ; ; attributes
   lda #(SPRITE_COLMASK_Z3_NOFLIP)
   sta VERA_data0
   ;size
   lda #(HORIZ_64x8)
   sta VERA_data0

   ; reenable display
   lda #VERA_mode
   sta VERA_dc_video
   endless:
   jmp endless

   ; backup default RAM IRQ vector
;    lda IRQVec
;    sta default_irq_vector
;    lda IRQVec+1
;    sta default_irq_vector+1

;    ; overwrite RAM IRQ vector with custom handler address
;    sei ; disable IRQ while vector is changing
;    lda #<custom_irq_handler
;    sta IRQVec
;    lda #>custom_irq_handler
;    sta IRQVec+1
;    lda #VSYNC_BIT ; make VERA only generate VSYNC IRQs
;    sta VERA_ien
;    cli ; enable IRQ now that vector is properly set

; custom_irq_handler:
;    lda VERA_isr
;    and #VSYNC_BIT
;    beq @continue ; non-VSYNC IRQ, no tick update
;    ; stuff that happens during irq

; @continue:
;    ; continue to default IRQ handler
;    jmp (default_irq_vector)
;    ; RTI will happen after jump