#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

struct instructions{
	char type;
	int address;

};
long long direct_mapped(int cache_size, int cache_line_size, vector<instructions> instr, ofstream & outfile){
	int cache_entries = cache_size / cache_line_size;
	
	//Initialize cache entries
	vector<vector<long long> > cache (cache_entries, vector<long long> (cache_line_size, -1)); 
		
	//std::ifstream infile (input_file_name);
//	std::string line;
	//char instruction_type;
	long long cache_line_index, base, cache_index;
	long long cache_hit = 0;
	//while(std::getline(infile, line)){
	long long refrences = 0;
	for(long long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr.address / cache_line_size;
		int cache_index = base % cache_entries;
		int cache_line_index = instr.address % cache_line_size;
		
		//cache missed
		if(cache[cache_index][cache_line_index] != instr.address){

			//populate the cache line
			for(int i = 0; i < cache_line_size; i++){
				cache[cache_index][i] = base*cache_line_size+i;
			}


		}else{
			cache_hit++;
			
		}
	}
	//infile.close();
	cout<<"cache_hits : " << cache_hit<<endl;
	outfile << hits << "," << refrences << "; " << endl;
	return cache_hit;
}
int main(int argc, char *argv[]){
	int address;
	char type;
	long long ref = 0;
	ifstream infile(argv[1]);
	ofstream outfile(argv[2]);
	vector<instructions> instr;
	while(infile >> tpe>>std::hex >> address){
		instr.push_back(instr{type, address});
		ref++;
		
	}

	direct_mapped(1024, 32, instr, outfile);
	direct_mapped(4096, 32, instr, outfile);
	direct_mapped(16384, 32, instr, outfile);
	direct_mapped(32768, 32, instr, outfile);

}
