#!/usr/bin/env python3

buffer = [0,0]

for px_row in range(0,8):
  print(px_row)
  for px_col in range(0,64):
    print("   "+str(px_col))
    buffer.append(9)

f = open("bar.bin", "wb")
f.write(bytearray(buffer))
f.close()


buffer = [0,0]

for px_row in range(0,8):
  for px_col in range(0,64):
    if px_row <=3:
      buffer.append(1)
    else:
      buffer.append(0)

f = open("hbar.bin", "wb")
f.write(bytearray(buffer))
f.close()

buffer = [0,0]

for px_row in range(0,64):
  for px_col in range(0,8):
    if px_col <=4:
      buffer.append(6)
    else:
      buffer.append(0)

f = open("vbar.bin", "wb")
f.write(bytearray(buffer))
f.close()