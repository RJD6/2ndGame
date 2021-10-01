.export _background_pal
.export _background_rle

;;{pal:"nes",layout:"nes"};;
_background_pal:
 .byte $0F,$11,$14,$35,$0F,$01,$21,$30
 .byte $0F,$1A,$11,$3C,$0F,$11,$00,$39
;;
;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_background_rle:
	.byte $02,$00,$02,$29,$0d,$00,$02,$0a,$0f,$00,$02,$2c,$0e,$00,$02,$05
	.byte $80,$00,$02,$07,$90,$02,$03,$80,$80,$00,$02,$07,$90,$02,$08,$80
	.byte $02,$02,$0d,$80,$02,$0c,$0e,$00,$02,$04,$90,$02,$02,$80,$02,$03
	.byte $90,$80,$02,$15,$00,$02,$05,$0f,$80,$80,$90,$80,$02,$09,$0f,$80
	.byte $02,$0a,$00,$02,$03,$90,$80,$02,$03,$00,$02,$03,$80,$02,$12,$00
	.byte $02,$03,$80,$02,$03,$00,$02,$03,$80,$02,$13,$00,$02,$02,$80,$02
	.byte $04,$00,$02,$04,$80,$02,$19,$00,$02,$04,$10,$11,$12,$00,$00,$80
	.byte $02,$0e,$00,$0f,$80,$02,$05,$00,$02,$02,$80,$20,$22,$23,$00,$02
	.byte $02,$80,$02,$07,$0d,$80,$80,$90,$0d,$00,$02,$02,$80,$02,$05,$00
	.byte $00,$80,$80,$24,$25,$80,$02,$06,$0e,$80,$02,$08,$00,$02,$02,$90
	.byte $90,$80,$02,$02,$00,$02,$02,$80,$02,$02,$90,$80,$02,$0e,$90,$00
	.byte $02,$05,$90,$90,$0f,$80,$02,$15,$00,$80,$00,$02,$04,$80,$90,$80
	.byte $02,$0b,$90,$02,$04,$80,$02,$02,$90,$00,$02,$07,$90,$80,$90,$80
	.byte $02,$18,$00,$02,$02,$0d,$00,$90,$90,$80,$02,$15,$0f,$00,$02,$07
	.byte $90,$80,$80,$0e,$80,$02,$0c,$0d,$80,$02,$03,$90,$00,$02,$08,$80
	.byte $02,$09,$0f,$80,$02,$09,$90,$90,$00,$02,$09,$80,$02,$06,$00,$80
	.byte $02,$05,$90,$80,$90,$02,$07,$00,$02,$07,$80,$80,$00,$80,$80,$90
	.byte $80,$90,$80,$90,$80,$02,$02,$90,$02,$08,$00,$02,$0e,$90,$80,$02
	.byte $06,$90,$02,$04,$80,$90,$02,$02,$0e,$00,$02,$06,$0d,$00,$02,$02
	.byte $0f,$00,$02,$02,$80,$02,$07,$90,$02,$06,$00,$02,$11,$80,$02,$06
	.byte $0d,$90,$02,$04,$00,$02,$08,$90,$02,$13,$00,$00,$0f,$00,$02,$07
	.byte $90,$02,$0c,$00,$90,$02,$0e,$80,$0c,$0c,$01,$0c,$01,$02,$03,$0c
	.byte $0c,$01,$02,$04,$0c,$0c,$01,$02,$02,$0c,$01,$0c,$02,$02,$01,$01
	.byte $0c,$01,$01,$0c,$0c,$01,$02,$3f,$00,$30,$cc,$00,$00,$5c,$30,$00
	.byte $04,$c5,$a5,$2f,$05,$3f,$05,$47,$fc,$bf,$aa,$ae,$ef,$ff,$ff,$f7
	.byte $0c,$cb,$fe,$fe,$3f,$cf,$ff,$37,$0c,$cc,$ff,$02,$04,$07,$00,$3c
	.byte $7f,$ff,$ff,$57,$75,$31,$fc,$ff,$02,$04,$f3,$f0,$0f,$02,$06,$0f
	.byte $02,$00
 ;;