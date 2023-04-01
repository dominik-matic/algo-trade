import numpy as np
from collections import defaultdict


def convertToMatrix(data):
	closeDict = defaultdict(lambda: defaultdict(lambda:0))
	volumeDict = defaultdict(lambda: defaultdict(lambda:0))
	keys = set()
	for key in data:
		parsed_key = key.split('_')
		type = parsed_key[0]
		source, dest = parsed_key[1].split(',')

		keys.add(source)
		keys.add(dest)
		
		if type == 'volume':
			volumeDict[source][dest] = data[key]
		elif type == 'close':
			closeDict[source][dest] = data[key]
	
	keys = {k: i for i, k in enumerate(keys)}

	closeMatrix = np.zeros(shape=(len(keys), len(keys)))
	volumeMatrix = np.zeros(shape=(len(keys), len(keys)))

	for k1 in keys:
		for k2 in keys:
			closeMatrix[keys[k1]][keys[k2]] = closeDict[k1][k2]
	
	for k1 in keys:
		for k2 in keys:
			volumeMatrix[keys[k1]][keys[k2]] = volumeDict[k1][k2]
	
	
	with open("close.txt", "w") as file:
		for i in range(closeMatrix.shape[0]):
			file.write(f'{closeMatrix[i][0]}')
			for j in range(closeMatrix.shape[1]):
				file.write(f'{closeMatrix[i][j]} ')
			file.write('\n')
	
	with open("volume.txt", "w") as file:
		for i in range(volumeMatrix.shape[0]):
			file.write(f'{volumeMatrix[i][0]}')
			for j in range(volumeMatrix.shape[1]):
				file.write(f'{volumeMatrix[i][j]} ')
			file.write('\n')
	
	
		


if __name__ == '__main__':
	import requests
	data = requests.get(url='http://192.168.1.101:3000/getAllPairs').json()
	convertToMatrix(data)