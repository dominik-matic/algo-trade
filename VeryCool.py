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
	

def save_to_file(data):
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

	
	idx2key = {i: k for i, k in enumerate(keys)}
	key2idx = {k: i for i, k in enumerate(keys)}
	

	with open("key2idx.txt", "w") as f:
		for idx in idx2key:
			f.write(f'{idx}:{idx2key[idx]}\n')
	

	with open("close_dict.txt", "w") as f:
		for k1 in closeDict.keys():
			for k2 in closeDict[k1].keys():
				f.write(f'{key2idx[k1]},{key2idx[k2]},{closeDict[k1][k2]}\n')
	
	with open("volume_dict.txt", "w") as f:
		for k1 in volumeDict.keys():
			for k2 in volumeDict[k1].keys():
				if volumeDict[k1][k2] == 0:
					continue
				f.write(f'{key2idx[k1]},{key2idx[k2]},{volumeDict[k1][k2]}\n')	
		


if __name__ == '__main__':
	import requests
	data = requests.get(url='http://192.168.1.101:3000/getAllPairs').json()
	save_to_file(data)
	#convertToMatrix(data)