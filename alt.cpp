#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <algorithm>

std::random_device rd;
std::mt19937 rng(rd());
typedef std::unordered_map<unsigned, std::unordered_map<unsigned, unsigned>> hmap; 

void load_maps(hmap &volumeMap, hmap &closeMap, std::vector<size_t> &startingPos) {
	std::fstream file("volume_dict.txt");
	std::string line;
	while(std::getline(file, line)) {
		std::istringstream iss(line);
		size_t key1, key2, value;
		iss >> key1;
		iss >> key2;
		iss >> value;
		volumeMap[key1][key2] = value;
	}
	file = std::fstream("close_dict.txt");
	while(std::getline(file, line)) {
		std::istringstream iss(line);
		size_t key1, key2, value;
		iss >> key1;
		iss >> key2;
		iss >> value;
		closeMap[key1][key2] = value;
	}

	file = std::fstream("starting_pos.txt");
	while(std::getline(file, line)) {
		std::istringstream iss(line);
		size_t pos;
		while(iss >> pos) {
			startingPos.push_back(pos);
		}
	}
}

double evaluate_cycle(std::vector<size_t> &cycle, hmap& closeMap) {
	size_t startIdx;
	for(startIdx = 0; cycle[startIdx] != cycle.back(); ++startIdx);
	double fitness = (double) closeMap[cycle[startIdx]][cycle[++startIdx]] / 1.0e8;
	for(; startIdx < cycle.size() - 1; ++startIdx) {
		fitness *= ((double) closeMap[cycle[startIdx]][cycle[startIdx + 1]] / 1.0e8);
	}
	return fitness;
}

bool dfs(
		std::vector<size_t> &current,
		std::vector<std::pair<std::vector<size_t>, double>> &cycles,
		hmap &closeMap,
		size_t depth,
		std::unordered_set<size_t> visited) {
	
			
	for(size_t i = 0; i < current.size(); ++i) {
		std::cout << current[i] << " ";
	}
	std::cout << "\n";


	// if reached bottom, go out
	if(depth == 0) {
		return false;
	}

	// get current node
	size_t current_node = current.back();
	// if already visited
	if(visited.insert(current_node).second == false) {
		// calculate fitness of cycle
		double fitness = evaluate_cycle(current, closeMap);
		// if fitness doesn't suck, add it to cycles and return true
		if(fitness > 1.0) {
			cycles.push_back(std::make_pair(current, fitness));
			return true;
		}
	} else { // not visited
		// get all neighbours
		std::unordered_map<unsigned, unsigned> possible_jumpss = closeMap[current_node];
		
		// shuffle possible neighbours
		std::vector<size_t> possible_jumps;
		possible_jumps.reserve(closeMap[current_node].size());
		for(auto kv: closeMap[current_node]) {
			possible_jumps.push_back(kv.first);
		}
		std::shuffle(std::begin(possible_jumps), std::end(possible_jumps), rng);

		// visit each neighbour until cycle is found
		for(size_t index : possible_jumps) {
			current.push_back(index);
			bool cycle_found = dfs(current, cycles, closeMap, depth - 1, visited); 
			if(cycle_found) {
				return true;
			}
			// if not found, pop previous neighbour and continue loop
			current.pop_back();
		}
	}

	// if nothing was found return false;
	visited.erase(current_node);
	return false;
}

void find_cycles(std::vector<size_t> &startingPos, std::vector<std::pair<std::vector<size_t>, double>> &cycles, hmap &volumeMap, hmap &closeMap, size_t amount, size_t max_depth) {
	std::unordered_set<size_t> visited;
	std::vector<size_t> current;
	size_t dfs_amount = amount / startingPos.size();
	for(size_t i = 0; i < startingPos.size(); ++i) {
		current.clear();
		current.push_back(startingPos[i]);
		for(size_t j = 0; j < dfs_amount; ++j){
			std::cout << cycles.size() << "\n";
			size_t old_cycle_size = cycles.size();
			do {
				dfs(current, cycles, closeMap, max_depth, visited);
			} while(old_cycle_size == cycles.size());
		}
	}
}

int main() {
	rng.seed(time(NULL));
	size_t amount = 20;
	size_t max_depth = 5;

	hmap volumeMap;
	hmap closeMap;
	std::vector<size_t> startingPos;
	std::vector<std::pair<std::vector<size_t>, double>> cycles;

	load_maps(volumeMap, closeMap, startingPos);

	// nac cikluse (koliko, max dubina) DFS
	find_cycles(startingPos, cycles, volumeMap, closeMap, amount, max_depth);
	

	for(size_t i = 0; i < cycles.size(); ++i) {
		for(size_t j = 0; j < cycles[i].first.size(); ++j) {
			std::cout << cycles[i].first[j] << " ";
		}
		std::cout << " fitness=" << cycles[i].second <<"\n";
	}

	// evaluirat cikluse
	//evaluate_cycles();

	// [0 1 2 3 4 5 6 5 4 3]


}