/*
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

author: parker holloway - parkerh@xilinx.com
*/

#include "custom_type_container.h"
#include <cstdio>
#include "sds_lib.h"

using namespace std;

// Allocate memory for the sortables that live in aligned
// or contiguous memory in the container class and the generic
// pointers such that the custom type objects can be passed by
// reference into an accelerator
custom_type_container::custom_type_container() {
	int size = MAX_SIZE;
	this->custom_type_array = (uint64_t*) sds_alloc(sizeof(uint64_t) * size);
	this->x_accel = (int*) sds_alloc(sizeof(int) * size);
	this->y_accel = (int*) sds_alloc(sizeof(int) * size);
	return;
}

custom_type_container::~custom_type_container() {
	// Currently, this statement does nothing as there
	// is no memory to deallocate inside of the custom
	// type. This could be important for more complex
	// implementations!
	for (int i = 0; i < this->array_size; i++) {
		delete (custom_type*)this->custom_type_array[i];
	}

	delete[] this->custom_type_array;
	return;
}

// Add in a new object to the custom container
void custom_type_container::append(int x, int y) {
	// If the array is full, we could reallocate memory
	// or, since this is a simple example, print an overflow
	// message
	if (this->array_size == MAX_SIZE) {
		printf("overflow error\n");
		return;
	}

	this->x_accel[this->array_size] = x;
	this->y_accel[this->array_size] = y;

	this->custom_type_array[this->array_size] = (uint64_t) new custom_type((this->x_accel + this->array_size), (this->y_accel + this->array_size));
	this->array_size++;

	return;
}

// Quick function to visualize state of the container
void custom_type_container::print_container() {
	for (int i = 0; i < this->array_size; i++){
		((custom_type*)this->custom_type_array[i])->class_operation();
	}
}

// Return ptrs to sds_alloc'ed memory that can be used with accelerators
int *custom_type_container::get_x(){
	int *copy_x = (int*) sds_alloc(sizeof(int) * this->array_size);

	for (int i = 0; i < this->array_size; i++) {
		copy_x[i] = this->x_accel[i];
	}

	return copy_x;
}

// ""
int *custom_type_container::get_y(){
	return this->y_accel;
}

// ""
uint64_t *custom_type_container::get_array(){
	return this->custom_type_array;
}

// ""
int custom_type_container::get_size(){
	return this->array_size;
}
