# encoding: utf-8

from random import randint

writer = open("feature.txt", "w")

sample_list = []
for _ in range(10000):
	fea_list = []
	for __ in range(50):
		fea_list.append(randint(0, 100)/100)
	fea_list.append(randint(0, 1))
	sample_list.append("\t".join(map(str, fea_list)))

writer.write("\n".join(sample_list))
writer.close()
