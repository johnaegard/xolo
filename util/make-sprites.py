#!/usr/bin/env python3

f = open("../bar.bin", "wb")

buffer = []

for px_row in range(0,8):
  print(px_row)
  for px_col in range(0,64):
    print("   "+str(px_col))
    buffer.append(9)

f.write(bytearray(buffer))
f.close()