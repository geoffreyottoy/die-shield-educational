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
 *  Description: Implementation of a circular buffer.
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

CircBuffer::CircBuffer(){}

/* Initialize a circular buffer (check size)
 */
circular_buf_status_t CircBuffer::init(uint8_t length){
    if(length > MAX_BUF_SIZE){
        return ERROR;
    }
	else{
		buffer = (float*)malloc(sizeof(float)*length);
		if(buffer == NULL){
			return ERROR;
		}
	}
    this->length = length;
    this->head = 0;
    this->tail = 0;
	this->fill = 0;
	this->sum = 0;
	this->avg = 0;
    
    return OK;
}

/* Reset a circular buffer
 */ 
void CircBuffer::reset(){
    this->head = 0;
	this->tail = 0;
	this->fill = 0;
	this->sum = 0;
	this->avg = 0;
}

/* Returns true when the buffer is empty
 */
bool CircBuffer::isEmpty(){
    // We define empty as head == tail
    return (this->head == this->tail);
}

uint8_t CircBuffer::getFill(){
	return this->fill;
}

float CircBuffer::getAverage(){
	return this->avg;
}

/* Put a byte in the buffer
 */
circular_buf_status_t CircBuffer::put(float data){
    if(this->buffer != NULL){
		float lastTail = this->buffer[this->tail];
		
        this->buffer[this->head] = data;
        this->head = (this->head + 1) % this->length;
		this->fill++;
		this->sum += data;
		
		// check if last byte has been overwritten
        if(this->head == this->tail){
			// move tail
            this->tail = (this->tail + 1) % this->length;
			// correct fill
			this->fill = this->length;
			// correct sum
			this->sum -= lastTail;
        }
		
		// update avg
		this->avg = (this->sum / this->fill);

        return OK;
    }
    
    return ERROR;
}

/* Get a byte from the buffer (don't need this)
 */
/*circular_buf_status_t circular_buf_get(circular_buf_t * cbuf, float * data){
    if(cbuf && data && !circular_buf_empty(*cbuf)){
        *data = cbuf->buffer[cbuf->tail];
        cbuf->tail = (cbuf->tail + 1) % cbuf->length;

        return OK;
    }
    
    return ERROR;
}*/