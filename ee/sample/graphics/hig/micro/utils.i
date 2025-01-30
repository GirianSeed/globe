 
	.macro	struct	name
xyzzy	.assigna	0
	.endm

	.macro	vud	name
\name	.equ   		\&xyzzy
xyzzy	.assigna	\&xyzzy+1
	.endm

	.macro	endst	name
\name	.equ		\&xyzzy
xyzzy	.assigna	0
	.endm





