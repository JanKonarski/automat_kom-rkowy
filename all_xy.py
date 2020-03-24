import random

file = open('test5_1.gen', 'w')

width = 512
height = 512

file.write(str(width) + ' ' + str(height) + '\n')

for x in range(0, width):
    for y in range(0, height):
        file.write(str(x) + ' ' + str(y) + '\n');

file.close()
