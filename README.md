# CppSwiftyList

<img src="https://github.com/AlexRoar/CppSwiftyList/raw/main/Images/SpeedTest.png" width="100px">

When you work with lists, its contents get messed up and operations are no longer efficient. Here, I implemented list with two modes: optimized and non-optimized. 
If the list is optimized, then operations can be divided in two parts:

## Not loosing optimisation
- pushBack
- popBack
- set
- get
- search
- remove
- resize
- swap
- clear
- checkUp

## Loosing optimisation
- pushFront
- popFront

List can be optimized by explicitly calling `list->optimize()` this will alter list's shape (without altering logic sequence)^ so that operations can be optimized again.

## Modes difference 
In optimized mode all operations time is not so different from these on regular array.

In de-optimized mode linear complexity in several operations appear.

| Operation name | optimized | non-optimized |
|----------------|:---------:|:-------------:|
| set            |    O(1)   |      O(n)     |
| get            |    O(1)   |      O(n)     |
| pushFront      |    O(1)   |      O(1)     |
| pushBack       |    O(1)   |      O(1)     |
| popFront       |    O(1)   |      O(1)     |
| popBack        |    O(1)   |      O(1)     |
| search         |    O(n)   |      O(n)     |
| remove         |    O(1)   |      O(n)     |
| resize         |  O(logn)  |    O(logn)    |
| swap           |    O(1)   |      O(n)     |
| clear          |    O(1)   |      O(1)     |
| optimize       |    O(1)   |      O(n)     |
| deOptimize     |    O(n)   |      O(n)     |
| checkUp        |    O(n)   |      O(n)     |
| print          |    O(n)   |      O(n)     |
