noninitializedarray
===================

An implementation of an integer array in c++ that is not initialized on creation.

## Introduction

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

All in constant O(1) time. This means that on initialization of an array of size n, one cannot go through all the values as set them to a default value. So when the data structure is initialized, it will need to allocate O(n) spaces of memory, but until values are set, they will hold "garbage values" that were previously held in those memory addresses. Of course, we do not want to let the user access these garbage values, or rather we would want to tell the user when garbage values are being accessed.

Currently, on large arrays initialization takes O(n) time. Since all values are set to 0. That is, all values are initialized to 0 on creation. The idea to make this better is to have a "lazy" array. One which initializes the values only when it needs to. We are going to need some helper variables and arrays that will keep track of which values have been initialized and which haven't - but these values also will start out with garbage values!

Using the lazy approach, we can achieve an implementation of that has constant time initialization while keeping constant time
access and modifications. We will also check whether the user is accessing garbage values.

## A step towards the solution

Lets take the following data structure. It will support the operations:

 - initialize
 - set
 - get

We are going to have one array that holds the data. Let call that arr. And one array that holds the values that have been initialized; we'll call that initialized, and finally a counter, which will keep the time. So,

```
initialize(n)
```

will initialize the data structure to hold n integers. On initialize, we will:

 - allocate n spaces of integer memory (4*n bytes) for arr
 - allocate n spaces of integer memory for initialized
 - set count = 0

So we haven't initialize any value of arr or initialized, so all the values instead of the counter will be garbage values. With initialized, we are going to keep track of the elements that we initialize. So if we initialized arr[134] first, then initialized[0] = 134. If directly after that we initialize arr[421], then initialized[1] = 421. With this implementation, initialize takes O(1) time.

Then on the set(i, value) operation, we check whether arr[i] is a garbage value. If it is, then we initialize it with value, set the initialized[count] to i, and increment the count. If its not a garbage value, we can just set arr[i] = value. On get(i), we just check whether arr[i] is a garbage value. If it isn't we can return it, otherwise we alert the user.

Whats the down side to this implementation? If I asked you whether arr[523] is a garbage value, you would need to go through all the values from 0 to count-1 of initialized to check if any of those locations is 523. If is it, then arr[523] has been initialized; if it isn't, then arr[523] is a garbage value. We can initialize up to n elements, so while initializing is O(1) time; set and get require you to check whether something is a garbage value, which takes O(n) in the worst case!

There is also one advantage to this method, we always know that the values initialized[0], initialized[1], ..., initialized[count-1] have been initialized, so when we access them, they are never garbage values.

So far, we have built a data structure that is O(1) time initialization and O(n) time get and set. Not completly what we wanted, but now we have a different problem that is easier to solve.

## Data Structure

The last problem that we have is that in order to check whether a value has been initialized, we need to check the whole "timeline" by checking every value of the initialized array. In order to fix this, we can have an additional array, we can call this array time, such that time[i] is the count when arr[i] was initialized. So now instead of checking through initialized[0], ..., initialized[count - 1], we just need to check initialized[time[i]]. How do we know that the values aren't garbage values? Well, first of all, 0 <= time[i] < count if we hope to have arr[i] initialized, and that means that time[i] is between 0 and count-1. And so we are guaranteed that initialized[time[i]] is not a garbage value. Indeed if initialized[time[i]] == i, then we know that arr[i] has been initialized, regardless of whether or not time[i] was a garbage value. This is the crux of the data structure.

So we will have three arrays:

 - The first array saves the data. (arr)
 - The second array saves the elements that have been initialized. (initialized)
 - The third array saves the time (by the count) of when values where initialized. (time)

Also, we keep a counter to keep the time that increases each time we initialize a new value. 

On initialize(n):

 - allocate n spaces of integer memory for arr
 - allocate n spaces of integer memory for initialized
 - allocate n spaces of integer memory for time
 - set count = 0

As before, this takes O(1) time, which is what we wanted. In order to check whether a value in arr is a garbage value, we can have this function:

```c++
bool isGarbage(int i){
	int initializedTime = time[i];
	if(initializedTime >= count || initializedTime < 0 || initialized[initializedTime] != i) {
		return true;
	}
	return false;
}
```

Now, checking for a garbage value takes O(1) time. This allows us to have constant time get and set as well.

```c++
int get(int i){
	if(!isGarbage(i)){
		return arr[i];
	}
	else {
		std::cout << "trying to access garbage values!";
		throw "error";
	}
}
```

Set follows a similar pattern, we just need to update the arrays and the counter.

```c++
void set(int i, int value){
	if(isGarbage(i)){
		time[i] = count;
		initialized[count] = i;
		count++;
	}
	arr[i] = value;
}
```

## Some implementation details

On each get operation, we are checking if its a garbage value and then returning, which is approximatly 4 operations, whereas a regular array will just take 1 operation to return the value. On each set operation, we check if its a garbage value (approximatly 3 operations) and if it isn't 3 more operations to increment the time and set the helper arrays, and then actually set the value. This means that 4 or 7 operations per set. Regular arrays use 1 operation per set. 

On initialize, we just allocate the memory (approximatly 1 operation) and then setting the time to 0. In a regular array, we would be taking time to allocate the memory and set each value to its default. Another issue is that we are using about 3 times more space for this type of array than the regular array.

Since I always access arr[index] and time[index], I interleave the two arrays. So I have one array (array) that contains arr and time, and then another array that contains initialized.

- arr[index] can be accessed as array[2*index]
- time[index] can be accessed as array[2*index+1]
- initialized[index] can be accessed as initialized[index]

This way, arr[index] and time[index] are next to each other in memory and so its fast to access this is at the expense of a multiplication by two and an addition, which can be implemented quickly by a bit shift. The other array, initialized, doesn't need to be close to arr since we cannot predict the way we will be accessing the elements.

The data structure is implemented here:

```c++
class NArray { 

private:
	int* array;
	int* initialized;
	int count;

	bool isGarbage(int i){
		int initializedTime = array[(i << 1)+1];
		if(initializedTime >= count || initializedTime < 0 || initialized[initializedTime] != i) {
			return true;
		}
		return false;
	}

public:
	NArray(int n){				
		array = (int*) malloc(n*8);
		initialized = (int*) malloc(n*4);
		count = 0;
	}
	int get(int i){
		if(!isGarbage(i)){
			return array[(i << 1)];
		}
		else {
			std::cout << "trying to access garbage values!";
			throw "error";
		}
	}
 	void set(int i, int value){
		if(isGarbage(i)){
			array[(i << 1)+1] = count;
			initialized[count] = i;
			count++;
		}
		array[(i << 1)] = value;
	}
};
```