/** 

	This is an implementation of an array that has constant time initialization and updates.

	Erik Waingarten 

	- Designing the data structure was part of a problem set question 
	in the MIT class 6.854 Advanced Algorithms taught by David Karger

*/

#include <iostream>
#include <boost/random.hpp>
#include <ctime>

class NArray { 

private:
	int* array;
	int* help2;
	int counter;

	bool isgarbage(int index) {
		int h = array[(index << 1)+1];
		if(h < 0 || h > counter || help2[h] != index) {
			return true;
		}
		return false;
	}

public:
	NArray(int n){				
		array = (int*) malloc(n*8);
		help2 = (int*) malloc(n*4);
		counter = 0;
	}
	int get(int index) {
		if(!isgarbage(index)){
			return array[(index << 1)];
		}
		std::cout << "ERROR: Trying to access garbage values" << std::endl;
		throw "ERROR";
	}
	void set(int index, int value) {
		int position = (index << 1);
		if(!isgarbage(index)){
			array[position] = value;
		} else {
			array[position] = value;
			array[position+1] = ++counter;
			help2[counter] = index;
		}
	}
};

void test_correctness() {

	int n = 1000000;
	int MAXVALUE = 1000000;

	NArray arr(n);
	int insertsvalues[n];

	std::cout << "Inserting " << n << " random values into the array in order..." <<  std::endl;

	for(int i = 0; i < n; i++){
		int randomnum = rand() % MAXVALUE;
		insertsvalues[i] = randomnum;
		arr.set(i, randomnum);
	}

	std::cout << "Counting Errors..."<<std::endl;
	int errors = 0;
	for(int i = 0; i < n; i++){
		if(insertsvalues[i] != arr.get(i)){
			errors++;
		}
	}

	std::cout << "Number of Errors: " << errors << std::endl;
}

void test_performance() {
	int n = 100000000;
	std::cout << "Initialize an array of size " << n << " and modify 100 elements" << std::endl;

	int part = 100;
	int insertsindexes[part];
	for(int i = 0; i < part; i++){
		int randomnum = rand() % n;
		insertsindexes[i] = randomnum;
	}


	clock_t begin = clock();
  	int* array1 = new int[n];
  	for(int i = 0; i < part; i++) {
  		array1[insertsindexes[i]] = rand();
  	}
  	for(int i = 0; i < part; i++){
  		array1[insertsindexes[i]];
  	}
  	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  	std::cout << "Time it took for array: " << elapsed_secs << std::endl;

  	begin = clock();
  	NArray array2(n);
  	for(int i = 0; i < part; i++) {
  		array2.set(insertsindexes[i], rand());
  	}
  	for(int i = 0; i < part; i++){
  		array2.get(insertsindexes[i]);
  	}
  	end = clock();
  	elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  	std::cout << "Time it took for NArray: " << elapsed_secs << std::endl;

}

int main(int argc, char** argv) {
	
	test_correctness();
	test_performance();
	return 0;
}