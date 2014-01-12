noninitializedarray
===================

An implementation of an integer array in c++ that is not initialized on creation.

Typically, when creating an integer array in c++, the values will all be initialized to 0.
One would write for example:
```c++
int array[100];
```
Then the program will allocate 100*4 bytes (4 bytes per integer) of contiguous memory and then go through all values and set them to 0. So

```c++
std::cout << array[99]
```

will print 0. Theoretically, we may want a data structure for an array that can support:

 - initialize
 - set
 - get

All in constant O(1) time. This means that on initialization of an array of size n, one cannot go through all the values as set them to a default value. So when one has this data structure, it will need to allocate O(n) spaces of memory, but until values are set, they will hold the garbage values that were previously held. Of course, we do not want to let the user access garbage values, or rather we would want to tell the user when garbage values are being accessed.

Currently, on large arrays initialization takes O(n) time. Since all values are set to 0.

This implementation of an integer array has constant time initialization. While keeping constant time
access and modifications. It also checks whether you are accessing garbage values if you check a value that has not
been initialized.

In this case, the initialization of an array is so optimized by GCC that only under very contribed examples was I able to reach
the performance of a regular array.

## Data Structure

In order to avoid the O(n) initialization, I allocate the memory for the array without actually initializing it. Of course, this makes some values be "garbage". In order to make sure that the user never accesses garbage values, I split the array into three parts:

 - The first part saves the data. (arr)
 - The second part keeps the time on which that position was initialized (help1)
 - The third part keeps the index that was initialized index per time (help2)

Also, we keep a counter that is incremented everytime we set a value. Originally, the time is 0.

Therefore, arr[index] will have been initialized when 0 <= help1[index] < time (so the time is after the time when arr[index] was intiailized) and help2[help1[index]] == index (at the time of initialization of arr[index] the value initialized was arr[index]).

The first thing we notice is that at the begining. Since the time is 0, there could be nothing initialized. Since no matter what the value of help1[index] is, 0 <= help1[index] < 0 will evaluate to false. Afterwards, we know that everthing from 0 to time has been initialized. So help2[0], ..., help2[time] have been initialized (do not contain garbage values). So if it happens by chance that 0 <= help1[index] < time is true, then we know that help2[help1[index]] has been initialized. If at the time of initialization of arr[index] (the value of help1[index]) the value initialized was arr[index], then help2[help1[index]] == index, which garantees that we are not accessing garbage values. 

Note that if we only had one array (help1) that kept the time of the initialization, then we would need to initialize help1, still taking O(n) time. Otherwise, we cannot guarantee that the garbage values won't tell us that values where previously initialized when in fact they weren't.

So in order to set a value in the array, we first check whether its a garbage value with the method described above. If its not, then just set it. If it is, then we need to initialize it. We initialize it by incrementing the time, setting the value in the arr and then setting the help1 to be the time, and then the help2 at the time to be the index. 

Using this implementation, on each get operation, we are checking if its a garbage value and then returning, which is approximatly 4 operations, whereas a regular array will just take 1 operation to return the value. On each set operation, we check if its a garbage value (approximatly 3 operations) and if it isn't 3 more operations to increment the time and set the helper arrays, and then actually set the value. This means that 4 or 7 operations per set. Regular arrays use 1 operation per set. 

On initialize, we just allocate the memory (approximatly 1 operation) and then setting the time to 0. In a regular array, we would be taking time to allocate the memory and set each value to its default.

Another issue is that we are using about 3 times more space for this type of array than the regular array.

## Some implementation details

Since I always access arr[index] and help1[index], I interleave the two arrays. So I have one array (array) that contains arr and help1, and then another array that contains help2.

- arr[index] can be accessed as array[2*index]
- help1[index] can be accessed as array[2*index+1]
- help2[index] can be accessed as help2[index]

This way, arr[index] and help1[index] are next to each other in memory and so its fast to access this is at the expense of a multiplication by two and an addition, which can be implemented quickly by a bit shift. The other array, help2, doesn't need to be close to arr since we cannot predict the way we will be accessing the elements.

The data structure is implemented here:

```c++
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
```