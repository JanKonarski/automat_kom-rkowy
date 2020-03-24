plik = open('test2_1.gen', 'w')
width = 10000
height = 10000

plik.write(str(width) + ' ' + str(height) + '\n')

for x in range(width):
    for y in range(height):
        plik.write(str(x) + ' ' + str(y) + '\n')

plik.close()
