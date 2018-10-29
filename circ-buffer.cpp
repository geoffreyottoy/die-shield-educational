/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *   KU Leuven - Technology Campus Gent,
 *   Gebroeders De Smetstraat 1,
 *   B-9000 Gent, Belgium
 *
 *         File: circ_buffer.cpp
 *      Created: 2018-10-22
 *       Author: Geoffrey Ottoy
 *
 *  Description: Implementation of a circular buffer. The buffer contains floats
 *               and also stores the average value.
 *
 * MIT License
 * 
 * Copyright (c) 2018 DRAMCO - KU Leuven
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "circ-buffer.h"

CircBuffer::CircBuffer(void){}

/* Initialize a circular buffer
 */
CircBufferStatus_t CircBuffer::init(uint8_t length){
	// check max length
	if(length > MAX_BUF_SIZE){
		return CB_ERROR;
	}
	
	// check if buffer has not yet been initalized
	if(this->buffer != NULL){
		return CB_NOT_NULL;
	}

	// allocate memory for the buffer
	this->buffer = (float*)malloc(sizeof(float)*length);
	if(this->buffer == NULL){
		return CB_NOT_INITIALIZED;
	}
	
	// initialize attributes
	this->length = length;
	this->head = 0;
	this->tail = 0;
	this->fill = 0;
	this->avg = 0;

	return CB_SUCCESS;
}

/* Reset the buffer
 */ 
void CircBuffer::reset(void){
	// re-init attributes (memory is not cleared)
	this->head = 0;
	this->tail = 0;
	this->fill = 0;
	this->avg = 0;
}

/* Returns true when the buffer is empty, and false otherwise.
 */
bool CircBuffer::isEmpty(void){
	// We define empty as head == tail
	return (this->head == this->tail);
}

/* Returns the number of values stored in the buffer.
 */
uint8_t CircBuffer::getFill(void){
	return this->fill;
}

/* Returns the average of all the values stored in the buffer.
 */
float CircBuffer::getAverage(void){
	return this->avg;
}

/* Put a value in the buffer
 */
CircBufferStatus_t CircBuffer::put(float data){
	// TODO: add computation of the buffer average
	
	// check if buffer is initalized
	if(this->buffer != NULL){
		// store last value (in case it gets "pushed out")
		float lastValue = this->buffer[this->tail];

		// store value at the head of the buffer
		this->buffer[this->head] = data;
		// move head
		this->head = (this->head + 1) % this->length;
		// increase number of elements
		this->fill++;

		// check if last byte has been overwritten (pushed out)
		if(this->head == this->tail){
			// move tail
			this->tail = (this->tail + 1) % this->length;
			// correct fill
			this->fill = this->length;
		}

		return CB_SUCCESS;
	}

	return CB_NOT_INITIALIZED;
}

/* Get a value from the buffer
 */
CircBufferStatus_t CircBuffer::get(float * data){
	// check for NULL pointer
	if(data == NULL){
		return CB_ARG_NULL;
	}
	// check if buffer is initalized
	if(this->buffer == NULL){
		return CB_NOT_INITIALIZED;
	}
	// check if buffer is empty
	if(!this->isEmpty()){
		// return last value in the buffer
		*data = this->buffer[this->tail];
		// move the tail
		this->tail = (this->tail + 1) % this->length;

		return CB_SUCCESS;
	}

	return CB_EMPTY;
}