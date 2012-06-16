                  jmp start

black:            .fill 0
white:            .fill 255	
width:            .fill 64
height:           .fill 64
pixel:            .fill 8

char_P:           .fill 80
char_6:           .fill 54
char_4:           .fill 52
char_space:       .fill 20
max_colour:       .fill 255


- use registers $4, $5, $6 for width, height, size
- use registers $7, $8, $9 for pixel colours RGB
- use register  $10 for col
- use register  $11 for col_pix
- use register  $12 for row
- use register  $13 for row_pix
- register $14 for P
- register $15 for 6
- register $16 for 4
- register $17 for space




  

b_to_w:
                  lw  $7  $0  white
                  lw  $8  $0  white
                  lw  $9  $0  white
                  jmp end_row_pix    

w_to_b:
                  lw  $7  $0  black
                  lw  $8  $0  black
                  lw  $9  $0  black
                  jmp end_row_pix                     


start:            lw  $1  $0  white           - white is the first colour
                  lw  $4  $0  width           - register 4 = width
                  lw  $5  $0  height          - register 5 = height
                  lw  $6  $0  pixel           - register 6 = pixel size
                  lw  $14 $0  char_P          - loads P into $14 
                  lw  $15 $0  char_6          - loads character 6 into $15 
                  lw  $16 $0  char_4          - loads character 4 to $16
                  lw  $17 $0  char_space      - load space character to $17	

                  addi $10 $0  1              - register 10 = col
                  addi $11 $0  1              - register 11 = col_pix
                  addi $12 $0  1              - register 12 = row
                  addi $13 $0  1              - register 13 = row_pix	
                  addi $19 $0  255            - register 13 = 255	           

                  out $14
                  out $15                     - outputs P6
                  out $17

                  out $15
                  out $16                     - print width
                  out $17

                  out $15
                  out $16                     - print height
                  out $17

                  lw  $18 max_colour
                  out $18                     - prints 255
                  out $17

print_pixel:      out $7
                  out $8
                  out $9
                  jr $31                      - back to rowpix

for_col:          ble  $10 $4  end            - branches and for loops
for_col_pix:      ble  $11 $6  colour_change  - branch if end of pixel
for_row:          ble  $12 $5  end_row		
for_row_pix:      ble  $13 $6  colour_change
                  jal  print_pixel            - prints pixel out
                  addi $13 $13 1              - row_pix++ 
                  jmp  for_row_pix            - loop row_pix


change_colour:    beq  $7  $0  b_to_w         - changes the colour
                  beq  $7  $19 w_to_b

end_row_pix:      addi $12 $12 1              - row++
                  jmp  for_row                - loop row
end_row:          jal  colour_change          - corrects colour change of last iteration
                  addi $11 $11 1- col_pix++		
                  jmp  for_col_pix            - loop col_pix
end_col_pix:      addi $10 $10 1              - col++
                  jmp  for_col                - loop col
end:              halt
