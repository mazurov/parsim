# Strings Parsers C++ Library 

<img src="https://github.com/downloads/mazurov/parsim/logo.png"/>

The goal of this library is to have a extendable **headers only** C++ library  
for converting strings that represent the python types (primitive types, lists, 
dictionaries, sets) to the corresponding C++ types.

The library depends only on Boost headers.

```C++
#inlcude "parsim.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;

// You can use any type composed by supported containers and primitive types
map<int, vector<string> > result;

try {
  parse_("{1: ['Hello', 'Google'], 2: ['Hello', 'Github']", result);
} catch(parsim::expectation_failure e) {
  cout << "Expected: " << e.what_.tag << endl;
  cout << "Found: " << string(e.first, e.second) << endl;
}

```

## Strings syntax

### Supported types

#### Primitive types

* **integers**: _10_ , _2e+2_ , 
* **floats**: _1.9_ , _2.009_
* **chars**: _'a'_
* **strings**: _"Hello \"Google\"!"_ , _'Hello "Guthub"!'_

#### Containers

The containers can hold values of primitive type or hold other containers. 

* **list**: [_value_, _value_, ...]
* **dictionary**: {_key_: _value_, _key_: _value_, ...}
* **pairs**: (_first_, _second_), (_first_: _second_)
* **tuples**: (_valueOfType1_, _valueOfType2_, _valueOfType3_)


### Comments

```C++
{ 10: "value" /* My  comment */, 20: "another value"} // End of line
```

```python
[10, 20, 30] # Python's  comment

```
## Where it is used?
[CERN Gaudi framework][Gaudi]
[Gaudi]: http://svnweb.cern.ch/world/wsvn/gaudi/Gaudi/trunk/GaudiKernel/GaudiKernel/GrammarsV2.h
