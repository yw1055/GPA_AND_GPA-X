/**********************************************************
*******************  this is a modified version of GPA AND GPA-X  **********************************
**********************************************************/



#pragma once
#include <limits>
#include <ostream>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <bitset>
#include <unordered_map>

#include <bitset>

using namespace std;

class PancakePuzzle {
public:
    typedef double Cost;
    static constexpr Cost COST_MAX = std::numeric_limits<Cost>::max();

    class State {
    public:
        State() {}

        State(std::vector<unsigned int> b, int l) : ordering(b), label(l) {
            generateKey();
        }

        std::vector<unsigned int> getOrdering() const { return ordering; }

        friend std::ostream& operator<<(std::ostream& stream,
                const PancakePuzzle::State& state) {
            for (int r = 0; r < state.getOrdering().size(); r++) {
                stream << std::setw(3) << (int)state.getOrdering()[r] << " ";
            }
            return stream;
        }

        bool operator==(const State& state) const {
            return ordering == state.getOrdering();
        }

        bool operator!=(const State& state) const {
            return ordering != state.getOrdering();
        }

        void generateKey() {
            /*
                    FNV-1a
            */
            unsigned long long offset_basis = 0xCBF29CE484222325;
            unsigned long long fnv_prime = 0x100000001B3;
            for (unsigned int value : ordering) {
                offset_basis ^= value;
                offset_basis *= fnv_prime;
            }
            theKey = offset_basis;
        }

        unsigned long long key() const { return theKey; }

        std::string toString() const {
            std::string s = "";
            for (int r = 0; r < ordering.size(); r++) {
                s += std::to_string((int)ordering[r]) + " ";
            }
            return s + "\n";
        }

        int getLabel() const { return label; }

        void markStart() { label = 0; }

        void dumpToProblemFile(ofstream& f) {
            f << ordering.size() << "\n";

            f << "starting positions for pancake :\n";

            for (int r = 0; r < ordering.size(); r++) {
                f << ordering[r] << "\n";
        }

        f << "end positions pancake:\n";

        for (int i = 1; i <= ordering.size(); i++) {
            f << i << "\n";
        }
    }

    private:
		std::vector<unsigned int> ordering;
		int label;
		unsigned long long theKey = -1;
    };

    struct HashState {
        std::size_t operator()(const State& s) const { return s.key(); }
    };

    PancakePuzzle(std::istream& input) {
        // Get the size of pancake
        string line;
        getline(input, line);
        stringstream ss(line);
        ss >> size;

        // Skip the next line
        getline(input, line);
        std::vector<unsigned int> rows(size, 0);

        startOrdering = rows;
        endOrdering = rows;

        for (int i = 0; i < size; ++i) {
            getline(input, line);
            startOrdering[i] = stoi(line);
        }
        // Skip the next line
        getline(input, line);

        for (int i = 0; i < size; ++i) {
            getline(input, line);
            endOrdering[i] = stoi(line);
        }

        puzzleVariant = 0; // Default
        startState = State(startOrdering, 0);

	}
    bool isGoal(const State& s) const {
		if (s.getOrdering() == endOrdering)
		{
			return true;
		}

		return false;
	}

	Cost gapHeuristic(const State& state, int X_) const {
		// Using gap heuristic from - Landmark Heuristics for the Pancake Problem
		// Where add 1 to heuristic if the adjacent sizes of the pancakes differs more than 1
		// The GAP-X heuristics exclude gaps that include pancakes among the smallest X in size, X = 0 denotes the regular gap heuristic
		int size = state.getOrdering().size();
		int plate = size + 1;
		int sum = 0;

		for (int i = 1; i < size; ++i ){
			int x =  state.getOrdering()[i - 1];
			int y = state.getOrdering()[i];
			if( X_ == 0 || (x>X_ && y > X_)) // GPA AND GAP-X
			{
			    int dif = x - y; 
			    if (dif > 1 || dif < -1){
					++sum; 
			    }
			}

		}

		int x =  state.getOrdering()[size - 1];
		int dif = x - plate;
		if (dif > 1 || dif < -1){
		       ++sum;
		}
		return sum;
	}

    void flipOrdering(std::vector<State>& succs,
            std::vector<unsigned int> ordering,
            int loc) const {
        int start = 0;
        int end = loc;
        while (start < end) {
            std::swap(ordering[start++], ordering[end--]);// one can also use std::reverse(pancakes.begin(), pancakes.begin() + loc + 1);
        }
        succs.push_back(State(ordering, loc));
    }

    std::vector<State> successors(const State& state) const {
		std::vector<State> successors;
        for (int i = size - 1; i > 0; --i){
			// Don't allow inverse actions, to cut down on branching factor
			if (state.getLabel() == i) continue; 

			flipOrdering(successors, state.getOrdering(), i);
        }
		return successors;
	}

    std::vector<State> predecessors(const State& state) const {
		std::vector<State> predecessors;
        for (int i = size - 1; i > 0; --i){
			flipOrdering(predecessors, state.getOrdering(), i);
        }
		return predecessors;
	}

	const State getStartState() const {
		return startState;
	}
	bool validatePath(queue<int> path)
    {
        std::vector<unsigned int> board = startOrdering;

        std::vector<State> successors;

        while (!path.empty())
        {
            int action = path.front();
            int start = 0;
			int end = path.front();
			while(start < end){
				std::swap(board[start++], board[end--]);
			}
            path.pop();

        }

        if (board == endOrdering)
            return true;
        return false;
    }


    std::vector<unsigned int> startOrdering;
	std::vector<unsigned int> endOrdering;
    
    State startState;

    int size;
	double expansionCounter;
    string expansionPolicy;
};
