	lw	0	2	mcand
	lw	0	3	mpliar
	lw	0	4	mask
	lw	0	5	negone
	lw	0	6	mask
	sw	0	6	1000
	nand	2	4	7
	beq	7	5	start
	add	0	3	1
start	add	4	4	4
	nand	2	4	7
	beq	5	7	start
	add	3	3	1
	beq	7	6	done
	beq	0	0	start
done	halt
mcand	.fill	32766
mpliar	.fill	10383
mask	.fill	1
begin	.fill	start
max	.fill	15
negone	.fill	-1