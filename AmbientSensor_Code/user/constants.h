/******************************************************************************
* Copyright 2019 Alex M.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define MAC_BYTES  6
#define IPV4_BYTES 4
#define PORT_BYTES 2

//! 16-bit endian swap
#define BYTE_SWAP_16(x) (((x) << 8) | ((x) >> 8))

//! 32-bit endian swap
#define BYTE_SWAP_32(x) ((((x) & 0xFF000000U) >> 24) | (((x) & 0x00FF0000U) >> 8) | (((x) & 0x0000FF00U) << 8) | (((x) & 0x000000FFU) << 24))

#endif // _CONSTANTS_H_
