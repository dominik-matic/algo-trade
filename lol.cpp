#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <random>


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

void dfs(
		std::vector<size_t> &current,
		std::vector<std::vector<size_t>> &cycles,
		hmap &closeMap,
		size_t &remaining,
		size_t depth,
		std::unordered_set<size_t> visited) {
	/*
	for(size_t i = 0; i < current.size(); ++i) {
		std::cout << current[i] << " ";
	}
	std::cout << "\t" << cycles.size() << " " << remaining << "\n";
	*/
	if(depth == 0 || remaining == 0) {
		return;
	}

	size_t current_node = current.back();
	if(visited.insert(current_node).second == false) {
		cycles.push_back(current);
		--remaining;
	} else {
		std::unordered_map<unsigned, unsigned> possible_jumps = closeMap[current_node];
		std::uniform_int_distribution<size_t> dist(0, possible_jumps.size() - 1);
		size_t index = dist(rng);
		for(auto &jump : possible_jumps) {
			if(index-- != 0) {
				continue;
			}
			current.push_back(jump.first);
			dfs(current, cycles, closeMap, remaining, depth - 1, visited);
			current.pop_back();
		}
	}
	visited.erase(current_node);
}

void find_cycles(std::vector<size_t> &startingPos, std::vector<std::vector<size_t>> &cycles, hmap &volumeMap, hmap &closeMap, size_t amount, size_t max_depth) {
	std::unordered_set<size_t> visited;
	std::vector<size_t> current;
	size_t dfs_amount = amount / startingPos.size();
	for(size_t i = 0; i < startingPos.size(); ++i) {
		current.clear();
		current.push_back(startingPos[i]);
		for(size_t j = 0; j < dfs_amount; ++j){
			dfs(current, cycles, closeMap, dfs_amount, max_depth, visited);
		}
	}
}

void evaluate_cycle(std::vector<size_t> cycle, hmap& closeMap) {
	size_t startIdx;
	for(startIdx = 0; cycle[startIdx] != cycle.back(); ++startIdx);
	for(; start)
}

int main() {
	rng.seed(time(NULL));
	size_t amount = 1000;
	size_t max_depth = 100;

	hmap volumeMap;
	hmap closeMap;
	std::vector<size_t> startingPos;
	std::vector<std::vector<size_t>> cycles;

	load_maps(volumeMap, closeMap, startingPos);

	// nac cikluse (koliko, max dubina) DFS
	find_cycles(startingPos, cycles, volumeMap, closeMap, amount, max_depth);
	

	for(size_t i = 0; i < cycles.size(); ++i) {
		for(size_t j = 0; j < cycles[i].size(); ++j) {
			std::cout << cycles[i][j] << " ";
		}
		std::cout << "\n";
	}

	// evaluirat cikluse
	//evaluate_cycles();

	// [0 1 2 3 4 5 6 5 4 3]


}