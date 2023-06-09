#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <utility>
#include <random>
#include <getopt.h>
#include <algorithm>
#include <chrono>
#include <queue>

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

void dfs(
		std::vector<size_t> &current,
		std::vector<std::pair<std::vector<size_t>, double>> &cycles,
		hmap &closeMap,
		size_t depth,
		std::unordered_set<size_t> visited) {
	
	if(depth == 0) {
		return;
	}

	size_t current_node = current.back();
	if(visited.insert(current_node).second == false) {
		double fitness = evaluate_cycle(current, closeMap);
		if(fitness > 1.0) {
			cycles.push_back(std::make_pair(current, fitness));
		}
	} else {
		std::unordered_map<unsigned, unsigned> possible_jumps = closeMap[current_node];
		std::uniform_int_distribution<size_t> dist(0, possible_jumps.size() - 1);
		size_t index = dist(rng);
		for(auto &jump : possible_jumps) {
			if(index-- != 0) {
				continue;
			}
			current.push_back(jump.first);
			dfs(current, cycles, closeMap, depth - 1, visited);
			current.pop_back();
		}
	}
	visited.erase(current_node);
}

void find_cycles(std::vector<size_t> &startingPos, std::vector<std::pair<std::vector<size_t>, double>> &cycles, hmap &volumeMap, hmap &closeMap, size_t amount, size_t max_depth, size_t time_limit) {
	std::unordered_set<size_t> visited;
	std::vector<size_t> current;
	
	std::shuffle(std::begin(startingPos), std::end(startingPos), rng);

	size_t dfs_amount = std::max(amount / startingPos.size(), 1UL);
	bool running = true;
	auto start = std::chrono::high_resolution_clock::now();

	for(size_t i = 0; i < startingPos.size() && running; ++i) {
		current.clear();
		visited.clear();
		current.push_back(startingPos[i]);
		for(size_t j = 0; j < dfs_amount && running; ++j){
			size_t old_cycle_size = cycles.size();
			do {
				dfs(current, cycles, closeMap, max_depth, visited);
				auto now = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
				if(duration.count() > time_limit) {
					running = true;
				}
			} while(old_cycle_size == cycles.size() && running);
		}
	}
}

void write_output(std::vector<std::pair<std::vector<size_t>, double>> &cycles) {
	std::fstream file("out.txt", std::ios::out);
	file << "[";
	for(size_t i = 0; i < cycles.size(); ++i) {
		file << "([" << cycles[i].first[0];
		for(size_t j = 1; j < cycles[i].first.size(); ++j) {
			file << ", " << cycles[i].first[j];
		}
		file << "], " << cycles[i].second << ")";
		if(i != cycles.size() - 1) {
			file << ", ";
		}
	}
	file << "]";
}

bool bfs(hmap& closeMap, size_t start, std::vector<std::vector<size_t>> &paths_back, size_t destination) {
	std::cout << "bfs\n";
	std::unordered_set<size_t> visited;
	std::vector<size_t> current;
	std::queue<size_t> q;
	q.push(start);
	//visited.insert(start);
	//current.push_back(start);

	while(q.size() != 0) {
		
		size_t node = q.front(); q.pop();
		current.push_back(node);
		if(node == destination) {
			paths_back.push_back(current);
			return true;
		}
		std::unordered_map<unsigned, unsigned> neighbours_map = closeMap[node];
		std::vector<size_t> neighbours;
		for(auto kv : closeMap[node]) {
			neighbours.push_back(kv.first);
		}
		std::shuffle(std::begin(neighbours), std::end(neighbours), rng);

		for(size_t neigh : neighbours) {
			if(visited.insert(neigh).second == true) {
				q.push(neigh);
			}
		}
	}
	return false;
}

void find_ways_back(std::vector<size_t>& startingPos, hmap& closeMap, std::vector<std::vector<size_t>> &paths_back, size_t amount) {
	size_t destination = startingPos[0];
	for(size_t i = 1; i < startingPos.size(); ++i) {
		for(size_t j = 0; j < amount; ++j) {
			bool success;
			do {
				success = bfs(closeMap, startingPos[i], paths_back, destination);
			} while(!success);
		}
	}
}

void write_to_out(std::vector<std::vector<size_t>> paths_back) {
	std::fstream file("out.txt", std::ios::out);
	file << "[";
	for(size_t i = 0; i < paths_back.size(); ++i) {
		file << "[" << paths_back[i][0];
		for(size_t j = 1; j < paths_back[i].size(); ++j) {
			file << ", " << paths_back[i][j];
		}
		file << "]";
		if(i != paths_back.size() - 1) {
			file << ", ";
		}
	}
	file << "]";
}

int main(int argc, char* argv[]) {
	size_t amount = 5;
	size_t max_depth = 100;
	size_t time_limit = 1000;
	bool go_back = false;
	for(;;) {
		switch(getopt(argc, argv, "n:d:t:b")) {
			case -1:
				break;
			case 'n':
				amount = std::atoi(optarg);
				continue;
			case 'd':
				max_depth = std::atoi(optarg);
				continue;
			case 't':
				time_limit = std::atoi(optarg);
				continue;
			case 'b':
				go_back = true;
				continue;
		}
		break;
	}

	rng.seed(time(NULL));
	
	hmap volumeMap;
	hmap closeMap;
	std::vector<size_t> startingPos;
	std::vector<std::pair<std::vector<size_t>, double>> cycles;

	load_maps(volumeMap, closeMap, startingPos);

	if(go_back) {
		std::vector<std::vector<size_t>> paths_back;
		find_ways_back(startingPos, closeMap, paths_back, amount);
		write_to_out(paths_back);
		return 0;
	} else {
		// nac cikluse (koliko, max dubina) DFS
		find_cycles(startingPos, cycles, volumeMap, closeMap, amount, max_depth, time_limit);

		std::sort(cycles.begin(), cycles.end(), [](auto &left, auto &right) {
		return left.second > right.second;
		});

		write_output(cycles);

	}

	
	return 0;
}