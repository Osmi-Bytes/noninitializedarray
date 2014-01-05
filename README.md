noninitializedarray
===================

An implementation of an integer array in c++ that is not initialized on creation.

Typically, when creating an integer array in c++, the values will all be initialized to 0.
On large arrays, this means that initialization takes O(n) time. 

This implementation of an integer array has constant time initialization. While keeping constant time
access and modifications. It also checks whether you are accessing garbage values if you check a value that has not
been initialized.

In this case, the initialization of an array is so optimized by GCC that only under very contribed examples was I able to reach
the performance of a regular array.

## Implementation

In order to avoid the O(n) initialization, I allocate the memory for the array without actually initializing it. Ofcourse, this makes some values be "garbage". In order to make sure that the user never accesses garbage values, I split the array into three parts:

 - The first part saves the data. (arr)
 - The second part keeps the time on which that position was initialized (help1)
 - The third part keeps the index that was initialized index per time. (help2)

Therefore, arr[index] will have been initialized, when help1[index] <= time and help2[help1[index]] == index

In fact, since I always access arr[index] and help1[index], I interleave the three arrays. So I have one array (array) that contains all three.

arr[index] can be accessed as array[3*index]
help1[index] can be accessed as array[3*index+1]
help2[index] can be accessed as array[3*index+2]