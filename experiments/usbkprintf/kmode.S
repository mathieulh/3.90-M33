
	.set noreorder
	.set noat

	.global psplinkSetK1
	.ent    psplinkSetK1

psplinkSetK1:
	move $v0, $k1
	jr	 $ra
	move $k1, $a0

	.end psplinkSetK1
