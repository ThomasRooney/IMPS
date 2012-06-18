start:  lw $0 n1
        lw $1 n2
        bgt $0 $1 pn1
	addi $3 $3 n2

p:   out $3
     halt

pn1: addi $3 $3 n1 
     jmp p


n1: .fill 5
n2: .fill 6

