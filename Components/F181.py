
'''
4-Bit Arithmetic Logic Unit
Fairchild DS009491 April 1988 Revised January 2004
http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html
'''

def stage0(s0, s1, s2, s3, a, b):
    prop = not (
        (a)
        or (b and s0)
        or (s1 and not b)
    )
    gen = not(
        (not b and s2 and a)
        or (a and b and s3)
    )
    return 0 + prop, 0 + gen

def ls181(cn, m, a3, a2, a1, a0, b3, b2, b1, b0, s3, s2, s1, s0):
   p0, g0 = stage0(s0, s1, s2, s3, a0, b0)
   p1, g1 = stage0(s0, s1, s2, s3, a1, b1)
   p2, g2 = stage0(s0, s1, s2, s3, a2, b2)
   p3, g3 = stage0(s0, s1, s2, s3, a3, b3)
   m = not m
   x1 = not (cn and m)
   x2 = m and p0
   x3 = m and g0 and cn
   x4 = m and p1
   x5 = m and p0 and g1
   x6 = m and cn and g0 and g1
   x7 = m and p2
   x8 = m and p1 and g2
   x9 = m and p0 and g1 and g2
   x10 = m and cn and g1 and g2
   x11 = not (g0 and g1 and g2 and g3)
   x12 = not (cn and g0 and g1 and g2 and g3)
   x13 = p0 and g1 and g2 and g3
   x14 = p1 and g2 and g3
   x15 = p2 and g3
   x16 = p3
   y1 = p0 ^ g0
   y2 = not(x2 or x3)
   y3 = p1 ^ g1
   y4 = not(x4 or x5 or x6)
   y5 = p2 ^ g2
   y6 = not(x7 or x8 or x9 or x10)
   y7 = p3 ^ g3
   y8 = not(x13 or x14 or x15 or x16)
   f0 = x1 ^ y1
   f1 = y2 ^ y3
   f2 = y4 ^ y5
   f3 = y6 ^ y7
   go = x11
   cn4 = (not x12) or (not y8)
   p = y8
   aeqb = f0 and f1 and f2 and f3
   return (
       #p0, g0, p1, g1, p2, g2, p3, g3, '-',
       #x1 + 0, y1 + 0, y2 + 0, y3 + 0, y4 + 0, y5 + 0, y6 + 0, y7 + 0, x12 + 0, y8 + 0, '-',
       f3 + 0, f2 + 0, f1 + 0, f0 + 0, aeqb + 0, p + 0, cn4 + 0, go + 0
   )

def make_lut():
    for n in range(1 << 14):
        cn = (n >> 13) & 1
        m = (n >> 12) & 1
        a3 = (n >> 11) & 1
        a2 = (n >> 10) & 1
        a1 = (n >> 9) & 1
        a0 = (n >> 8) & 1
        b3 = (n >> 7) & 1
        b2 = (n >> 6) & 1
        b1 = (n >> 5) & 1
        b0 = (n >> 4) & 1
        s3 = (n >> 3) & 1
        s2 = (n >> 2) & 1
        s1 = (n >> 1) & 1
        s0 = (n >> 0) & 1
        x = ls181(cn, m, a3, a2, a1, a0, b3, b2, b1, b0, s3, s2, s1, s0)
        lut = 0
        for i in x:
            lut += lut + i
        # print(cn, m, a0, b0, a1, b1, a2, b2, a3, b3, s0, s1, s2, s3, "->", *x, "%02x" % lut)
        yield lut

with open("F181_tbl.h", "w") as file:
    for i in zip(*(iter(make_lut()),) * 16):
        # print(type(i), i)
        file.write(",".join("0x%02x" % x for x in i) + ",\n" )

tbl = []
for i in make_lut():
    tbl.append(i)
# s 0001 m 0 cn 0 a 0000 b 0011 idx 31 f 0100 = 0 p 0 cn 1 g 0
print(
    ls181(
        cn=1,
        m=1, 
        a3=1, a2=0, a1=1, a0=0,
        b3=0, b2=0, b1=1, b0=0, 
        s3=1, s2=1, s1=0, s0=1,
    )
)
#idx = int("0" + "0" + "11111000" + "1111", 2)
#print("%x" % idx, bin(256 + tbl[idx])[3:])
