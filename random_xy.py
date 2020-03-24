import random

file = open('test4_1.gen', 'w')

width = 512
height = 512
size = width * height

file.write(str(width) + ' ' + str(height) + '\n')

for i in range(int(size/4), int(size/2)):
    x = random.randint(0, width-1)
    y = random.randint(0, height-1)
    file.write(str(x) + ' ' + str(y) + '\n')

file.close()
