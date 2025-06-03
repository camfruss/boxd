
## Overview

`boxd` 

bin packing problem NP-Hard significant business implications

aggressive pruning, early termination, multithreading 
minimize cost with fixed N box types

## Prerequisites

- Safari
- Develop > Developer Settings... > Allow remote automation

## Usage

## TODO

- [ ] data post-processing / clean-up / add filter args / sort dims
- [ ] C++ script
    - [ ] feasibility matrix
    - [ ] price calculation

```
|        | item 1 | item 2 | item 3 | ... | total |
|--------|--------|--------|--------|-----|-------|
| box 1  |   0    |   1    |   0    |  .  |   Σ   |
| box 2  |   1    |   1    |   0    |  .  |   Σ   |
| box 3  |   1    |   1    |   1    |  .  |   Σ   |
| ...    |   .    |   .    |   .    | ... |   Σ   |
| total  |   Σ    |   Σ    |   Σ    |  Σ  |       |
```

Simplifying Assumptions
- The minimum order quantity is 1 (vs. 25)
- For a given dimension X
    - if X <= 12, X mod 2 = 0 
    - if X > 12, X mod 4 = 0
- If an item can be shipped in an AxBxC box, it can also be shipped in a CxBxA box
    - sometimes boxes open from the side vs. top, resulting in a different dimension order

Invariants
- boxes are sorted $ -> $$$

Strategy
- keep track of location ... use long long so can restart if stopped
- brute force w/ small search space w/ threads
- generate subsets of size N (via indices ... use unsigned short)
- take any(item & subset)    TAKE | of all columns
     - if 0: terminate
- take MSB index, price++
- sum all prices
- keep priority queue of min 1000 prices
- std::array<uint8_fast_t, N> combination; float cost;


start with all at leftmost position
move rightmost to end, keep repeating until would need to move leftmost down 1
each thread's leftmost starts at different value ...

N set to 3 to start

## Alternative Strategies

- K-Means clustering

