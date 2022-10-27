<h1 align="center">
  <br>
    <img src=./docs/hashtable.png alt="hashtable.png" width="100"></a>
  <br>
    Polymorphic Hashing
  <br>
</h1>

<h4 align="center">A C implementation of a polymorphic hash table.</h4>

<p align="center">
  <a href="#Features">Features</a> |
  <a href="#Overview">Overview</a> |
  <a href="#Design">Design</a>     |
  <a href="#Usage">Usage</a>       |
  <a href="#License">License</a>   |
</p>

# Features
- Creates a polymorphic hash table that can hash any type of data.
- Uses the [SDBM](www.cse.yorku.ca/~oz/hash.html) and [DJB2](www.cse.yorku.ca/~oz/hash.html) hashing algorithms.
- Implements a cuckoo hashing scheme to handle collisions when multiple elements have the same hashing value.

# Overview
Polymorphism is the concept of writing functions (or ADTs), without needing to specify which particular type is being used/stored. This project is an implementation of a polymorphic associative array using hashing. Both the key and data to be used by the hash function are of unknown types, so void pointers are used to store both of these. The user of the associative array (and not the ADT itself) is responsible for creating and maintaining such memory, and also ensuring it doesn’t change when in use by the associative array.

# Design
There are two implementations of this ADT - basic and cuckoo. 
## Basic
The basic implementation is a resizable hash table that uses [linear probing](https://en.wikipedia.org/wiki/Linear_probing) (a form of open [addressing](https://en.wikipedia.org/wiki/Open_addressing)) to deal with collisions. The initial size of the hash table can hold 17 key-value pairs and is resized by a scale factor of 2 when it reaches a load factor of 60 percent. Further setup parameters can be found in the [header file](realloc/specific.h).
## Cuckoo
 The basic idea of cuckoo hashing is to resolve collisions by using two hash functions instead of only one. This provides two possible locations in the hash table for each key. In this implementation, the hash table is split into two smaller tables of equal size, and each hash function provides an index into one of these two tables.

Cuckoo hashing uses two hash tables `T1` and `T2`. The first step involves examining if the hashed index in T1 is occupied; if it is not, the item is inserted at that cell. However, if the slot is occupied, the preoccupied item gets removed, and the new item inserted into this location; and the preoccupied item is moved into `T2`. This process continues until an empty location can be found to insert the key. To avoid the possible infinite iteration in the process loop, a MAX_LOOP is specified such that if the iterations exceeds the fixed threshold, the hash tables - both `T1` and `T2` are rehashed with newer hash functions and the insertion procedure repeats.

# Usage
Ensure you have [GCC](https://gcc.gnu.org) installed on your machine. Once you have done so, run the following command on your terminal to compile and run an example of the basic hash table:

```bash
make basic
```

and for an example of the cuckoo hash table implementation:

```bash
make cuckoo
```

# License

```
Copyright (c) 2021 Keane Fernandes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

