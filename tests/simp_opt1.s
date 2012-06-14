start:  bgt n1 n2 pn1
		addi $0 $1 n2
		
p: out $0
pn1: addi $0 $1 n1
	 jmp p
	 

n1: .fill 5
n2: .fill 6