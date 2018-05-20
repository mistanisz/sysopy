#!/usr/bin/python3

import random
import sys

N = int(sys.argv[1])

L = [random.random() for _ in range(N*N)]
s = sum(L)
L = list(map(lambda x: x/s, L))

print(N)
print("\n".join(str(x) for x in L))
