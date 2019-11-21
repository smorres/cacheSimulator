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
struct info{
	int data;
	int count;
};

long long directMapped(int cacheSize, vector<instructions> instr, ofstream & outfile){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	vector<vector<long long> > cache (entries, vector<long long> (32, -1));

	long long lineIndex, base, cacheIndex;
	long long hit = 0;
	long long refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % entries;
		int lineIndex = instr[i].address % 32;
		
		//if there is a cache miss
		if(cache[cacheIndex][lineIndex] != instr[i].address){
			//add to the cache line
			for(int i = 0; i < 32; i++){
				cache[cacheIndex][i] = base*32+i;
			}
		}else{
		//else we have a hit
			hit++;
		}
	}
	//cout<<"hits : " << hit<<endl;
	//cout<<"refrences :" << refrences<<endl;
	if(cacheSize != 32768){
		outfile << hit << "," << refrences << "; ";
	}
	else{
		outfile << hit << "," << refrences << "; "<<endl;
	}
	return hit;
}
long long setAssociative(int cacheSize, vector<instructions> instr, ofstream & outfile, int numWays){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	long long base, cacheIndex;
	long long hit = 0;
	long long refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;
		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				hit++;
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){
			//if miss
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
		}
		
	}
	outfile << hit << "," << refrences << "; ";
	cout<<"set assoc hits: " << hit<<endl;	
	return hit;
}

long long fullAssociativeLRU(int cacheSize, vector<instructions> instr, ofstream & outfile){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	int numWays = 512;
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	long long base, cacheIndex;
	long long hit = 0;
	long long refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;
		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				hit++;
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){
			//if miss
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
		}
		
	}
	outfile << hit << "," << refrences << "; ";
	cout<<"fully assoc lru: " << hit<<endl;	
	return hit;
}

long long fullAssociativeHotCold(int cacheSize, vector<instructions> instr, ofstream & outfile){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	int numWays = 512;
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	long long base, cacheIndex;
	long long hit = 0;
	long long refrences = 0;
	int hotColdTree[511] = {0};
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;
		for(int j= 0; j < numWays; j ++){
			
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				int treeIndex = j+511;
				hit++;
				hadHit = true;
				while(treeIndex>0){
					if(treeIndex%2 == 0){
						//right traverse
						treeIndex = (treeIndex-2)/2;
						if(hotColdTree[treeIndex] == 0){
							hotColdTree[treeIndex] = 1;
						}
					}
					else{
						treeIndex = (treeIndex-1)/2;
						if(hotColdTree[treeIndex] == 1){
							hotColdTree[treeIndex] = 0;
						}
					}
				}
	
			}
		}
		if(!hadHit){
			//had a miss
			int treeIndex = 0;
			while(511>treeIndex){
				if(hotColdTree[treeIndex] == 0){
					//right traverse
					treeIndex = 2*treeIndex+2;
				}
				else{
					//left traverse
					treeIndex = 2*treeIndex+1;
				}
			}
			int newJ = treeIndex - 511;
			//now flip
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			while(treeIndex>0){
				if(treeIndex%2 == 0){
					//right traverse
					treeIndex = (treeIndex-2)/2;
				}
				else{
					//left traverse
					treeIndex = (treeIndex-1)/2;
				}
				if(hotColdTree[treeIndex] == 0){
					hotColdTree[treeIndex] = 1;
				}else{
					hotColdTree[treeIndex] = 0;
				}
			}

		}
		
	}
	outfile << hit << "," << refrences << "; ";
	cout<<"fully assoc hotCold: " << hit<<endl;	
	return hit;
}

long long setAssociativeNoAlloc(int cacheSize, vector<instructions> instr, ofstream & outfile, int numWays){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	long long base, cacheIndex;
	long long hit = 0;
	long long refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;

		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				hit++;
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){
			 //the instr is a store instr
			//ou dont do anthing
			//just continue
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			if(instr[i].type == 'S'){
				continue;
			}
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
		}
		
	}
	outfile << hit << "," << refrences << "; ";
	cout<<"no alloc hits: " << hit<<endl;	
	return hit;
}
long long setAssociativeWithNextLinePrefetching(int cacheSize, vector<instructions> instr, ofstream & outfile, int numWays){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	int base, cacheIndex;
	int hit = 0;
	int refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;
		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				hit++;
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){//miss
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
		}
		base = (instr[i].address+32) / 32;
		cacheIndex = base % (entries/numWays);
		hadHit = false;
		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address+32 >> 5)/(entries/numWays)){
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			cache[newJ][cacheIndex].data = ((instr[i].address+32) >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
		}
		
	}
	outfile << hit << "," << refrences << "; "<<endl;
	cout<<"next line next line hits: " << hit<<endl;	
	return hit;
}
long long setAssociativePrefetchOnMiss(int cacheSize, vector<instructions> instr, ofstream & outfile, int numWays){
	
	int entries = cacheSize / 32; //32 is line size, if we want this to be variable we can take lineSize as an input
	vector<vector<info>> cache (numWays, vector<info> ((entries/numWays), {-1, 0}) );
	int base, cacheIndex;
	int hit = 0;
	int refrences = 0;
	
	for(unsigned long i = 0; i < instr.size(); i++){
		refrences++;
		int base = instr[i].address / 32;
		int cacheIndex = base % (entries/numWays);
		bool hadHit = false;
		for(int j= 0; j < numWays; j ++){
			if(cache[j][cacheIndex].data == (instr[i].address >> 5)/(entries/numWays)){
				hit++;
				hadHit = true;
				cache[j][cacheIndex].count = i;
			}
		}
		if(!hadHit){//miss
			int min = cache[0][cacheIndex].count;
			int newJ=0;
			for(int j = 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == -1){
					newJ = j;
					break;
				}
				if(cache[j][cacheIndex].count <= min){
					min = cache[j][cacheIndex].count;
					newJ = j;
					
				}
				
			}			
			cache[newJ][cacheIndex].data = (instr[i].address >> 5)/(entries/numWays);
			cache[newJ][cacheIndex].count = i;
			
			base = (instr[i].address+32) / 32;
			cacheIndex = base % (entries/numWays);
			hadHit = false;
			for(int j= 0; j < numWays; j ++){
				if(cache[j][cacheIndex].data == (instr[i].address+32 >> 5)/(entries/numWays)){
					hadHit = true;
					cache[j][cacheIndex].count = i;
				}
			}
			if(!hadHit){
				min = cache[0][cacheIndex].count;
				newJ=0;
				for(int j = 0; j < numWays; j ++){
					if(cache[j][cacheIndex].data == -1){
						newJ = j;
						break;
					}
					if(cache[j][cacheIndex].count <= min){
						min = cache[j][cacheIndex].count;
						newJ = j;
					
					}
				
				}			
				cache[newJ][cacheIndex].data = ((instr[i].address+32) >> 5)/(entries/numWays);
				cache[newJ][cacheIndex].count = i;
			}
		
		}
	
		
	}
	outfile << hit << "," << refrences << "; "<<endl;
	cout<<"next line preMiss hits: " << hit<<endl;	
	return hit;
}

int main(int argc, char *argv[]){
	int address;
	char type;
	long long ref = 0;
	ifstream infile(argv[1]);
	ofstream outfile(argv[2]);
	vector<instructions> instr;
	while(infile >> type >> std::hex >> address){
		instr.push_back(instructions{type, address});
		ref++;
	}

	directMapped(1024, instr, outfile);
	directMapped(4096, instr, outfile);
	directMapped(16384, instr, outfile);
	directMapped(32768, instr, outfile);
	setAssociative(16384,instr, outfile, 2);
	setAssociative(16384,instr, outfile, 4);
	setAssociative(16384,instr, outfile, 8);
	setAssociative(16384,instr, outfile, 16);
	//fullAssociativeLRU(16384, instr, outfile);
	fullAssociativeHotCold(16384, instr, outfile);
	setAssociativeNoAlloc(16384, instr, outfile, 2);
	setAssociativeNoAlloc(16384, instr, outfile, 4);
	setAssociativeNoAlloc(16384, instr, outfile, 8);
	setAssociativeNoAlloc(16384, instr, outfile, 16);
	setAssociativeWithNextLinePrefetching(16384,instr, outfile, 2);
	setAssociativeWithNextLinePrefetching(16384,instr, outfile, 4);
	setAssociativeWithNextLinePrefetching(16384,instr, outfile, 8);
	setAssociativeWithNextLinePrefetching(16384,instr, outfile, 16);
	setAssociativePrefetchOnMiss(16384,instr, outfile, 2);
	setAssociativePrefetchOnMiss(16384,instr, outfile, 4);
	setAssociativePrefetchOnMiss(16384,instr, outfile, 8);
	setAssociativePrefetchOnMiss(16384,instr, outfile, 16);
	

}
