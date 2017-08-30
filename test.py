import pycuckoo

i=0
s= "qualcunovolosoprailnidodelcuculo"

while True:
    solutions = pycuckoo.cuckoo(i,s)
    
    if len(solutions)>0:
        break

    i=i+1

# for sol in solutions:
#     print ("SOLUTION:")
#     for nonce in sol:
#         print(hex(nonce),end=' ')

print(pycuckoo.verify(solutions[0],s,i))