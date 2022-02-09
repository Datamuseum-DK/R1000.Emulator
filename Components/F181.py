
'''
4-Bit Arithmetic Logic Unit
Fairchild DS009491 April 1988 Revised January 2004
http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html
'''

def ls181(s0, s1, s2, s3, m, cn, a3, a2, a1, a0, b3, b2, b1, b0):
    mode = str(s0) + str(s1) + str(s2) + str(s3) + "-" + str(m)
    a = (a3 << 3) | (a2 << 2) | (a1 << 1) | a0;
    b = (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
    cy = 1 - cn

    if mode == "0000-0":
        f = a + cy
    elif mode == "0000-1":
        f = ~a
    elif mode == "0001-0":
        f = a + (a & b) + cy
    elif mode == "0001-1":
        f = (~a) | b
    elif mode == "0010-0":
        f = a + (a & ~b) + cy
    elif mode == "0010-1":
        f = (~a) | (~b)
    elif mode == "0011-0":
        f = a + a + cy
    elif mode == "0011-1":
        f = 15
    elif mode == "0100-0":
        f = (a | ~b) + cy
    elif mode == "0100-1":
        f = (~a) & b
    elif mode == "0101-0":
        f = (a & b) + ((a | ~b) + cy)
    elif mode == "0101-1":
        f = b
    elif mode == "0110-0":
        f = cy + a - b - 1
    elif mode == "0110-1":
        f = a ^ b
    elif mode == "0111-0":
        f = a + (a | ~b) + cy
    elif mode == "0111-1":
        f = a | b
    elif mode == "1000-0":
        f = (a | b) + cy
    elif mode == "1000-1":
        f = ~(a | b)
    elif mode == "1001-0":
        f = a + b + cy
    elif mode == "1001-1":
        f = ~(a ^ b)
    elif mode == "1010-0":
        f = (a & ~b) + (a | b) + cy
    elif mode == "1010-1":
        f = ~b
    elif mode == "1011-0":
        f = a + (a | b) + cy
    elif mode == "1011-1":
        f = a | ~b
    elif mode == "1100-0":
        f = cy - 1
    elif mode == "1100-1":
        f = 0
    elif mode == "1101-0":
        f = (a & b) + cy - 1
    elif mode == "1101-1":
        f = a & b
    elif mode == "1110-0":
        f = (a & ~b) + cy - 1
    elif mode == "1110-1":
        f = a & (~b)
    elif mode == "1111-0":
        f = a + cy - 1
    elif mode == "1111-1":
        f = a

    g0 = not ((s3 and a0 and b0) or (s2 and a0 and not b0))
    g1 = not ((s3 and a1 and b1) or (s2 and a1 and not b1))
    g2 = not ((s3 and a2 and b2) or (s2 and a2 and not b2))
    g3 = not ((s3 and a3 and b3) or (s2 and a3 and not b3))
    p0 = not (a0 or (s0 and b0) or (s1 and not b0))
    p1 = not (a1 or (s0 and b1) or (s1 and not b1))
    p2 = not (a2 or (s0 and b2) or (s1 and not b2))
    p3 = not (a3 or (s0 and b3) or (s1 and not b3))
    
    aeqb = (f & 15) == 15
    p = (s3 and ((a & b) != 0)) or (s2 and (a & ~b) != 0)
        
    g = not (p3 or (g3 and p2) or (g3 and g2 and p1) or (g3 and g2 and g1 and p0))
        
    cn4 = (g0 and g1 and g2 and g3 and cn) or (not g)

    f += 16
    f &= 15
    return ((f >> 3) & 1, (f >> 2) & 1, (f >> 1) & 1, (f >> 0) & 1, int(aeqb), int(p), int(cn4), int(g))


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
        x = ls181(s0, s1, s2, s3, m, cn, a3, a2, a1, a0, b3, b2, b1, b0)
        lut = 0
        for i in x:
            lut += lut + i
        # print(cn, m, a0, b0, a1, b1, a2, b2, a3, b3, s0, s1, s2, s3, "->", *x, "%02x" % lut)
        yield lut

with open("F181_tbl.h", "w") as file:
    for i in zip(*(iter(make_lut()),) * 16):
        file.write(",".join("0x%02x" % x for x in i) + ",\n" )
