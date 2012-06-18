                  jmp start

black:            .fill 0
white:            .fill 255	
width:            .fill 64
height:           .fill 64
pixel:            .fill 8

char_P:           .fill 80
char_6:           .fill 54
char_4:           .fill 52
char_space:       .fill 32
char_2:           .fill 50
char_5:           .fill 53


colour_change:    beq  $7  $20  b_to_w          - changes the colour
                  beq  $7  $21  w_to_b 

b_to_w:           lw  $7  $0  white
                  lw  $8  $0  white
                  lw  $9  $0  white
                  jr  $31

w_to_b:           lw  $7  $0  black
                  lw  $8  $0  black
                  lw  $9  $0  black                   
                  jr  $31

print_pixel:      out $7
                  out $8
                  out $9
                  jr  $31                     - back to rowpix

start:            lw  $7  $0  white           - white is the first colour
                  lw  $8  $0  white
                  lw  $9  $0  white
                  lw  $4  $0  width           - register 4 = width
                  lw  $5  $0  height          - register 5 = height
                  lw  $6  $0  pixel           - register 6 = pixel size
                  lw  $14 $0  char_P          - loads P into $14 
                  lw  $15 $0  char_6          - loads character 6 into $15 
                  lw  $16 $0  char_4          - loads character 4 to $16
                  lw  $18 $0  char_2          - loads char 2
                  lw  $19 $0  char_5          - loads char 5
                  lw  $17 $0  char_space      - load space character to $17	 
                  lw  $20 $0  black
                  lw  $21 $0  white

                  addi $10 $0  0              - register 10 = col
                  addi $11 $0  0              - register 11 = col_pix
                  addi $12 $0  0              - register 12 = row
                  addi $13 $0  0              - register 13 = row_pix  
                    
                  out $14
                  out $15                     - outputs P6
                  out $17
                  out $15
                  out $16                     - print width
                  out $17
                  out $15
                  out $16                     - print height
                  out $17
                  out $18
                  out $19                     - prints 255
                  out $19
                  out $17

for_col:          blt  $10 $4  end            - branches and for loops

for_col_pix:      blt  $11 $6  end_col_pix    - branch if end of pixel

for_row:          blt  $12 $5  end_row	

for_row_pix:      blt  $13 $6  end_row_pix
                  jal  print_pixel            - prints pixel out
                  addi $13 $13 1              - row_pix++ 
                  jmp  for_row_pix            - loop row_pix

end_row_pix:      jal  colour_change          - changes the colour
                  addi $12 $12 1              - row++
                  jmp  for_row                - loop row

end_row:          jal colour_change           - corrects colour change of last iteration  
                  addi $11 $11 1              - col_pix++		
                  jmp  for_col_pix            - loop col_pix

end_col_pix:      addi $10 $10 1              - col++
                  jmp  for_col                - loop col

end:              halt
