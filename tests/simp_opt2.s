start:  addi $4 $0 n9
    jmp waste
end:  addi $2 $20 72
	  out $2
	  addi $2 $20 101
	  out $2
	  addi $2 $20 121
	  out $2
		
n1: .fill 1
n2: .fill 68
n9: .fill 1000

waste: addi $3 $3 1
       bgt $3 $9 end
