## Description
Simple DSL parser.

## Build
```sh
clang++ -std=c++20 main.cpp
```
### Dependencies
- `Boost.Parser==1.90.0`

## Grammar
```
S ::=
      GET <CONDITION>
    | ADD <DATA>
    | CHANGE TO <DATA> IF <CONDITION>
    | DELETE <CONDITION>
    
    CONDITION ::=
      <BASE_CONDITION>
    | <BASE_CONDITION> AND <CONDITION>
    | <BASE_CONDITION> OR  <CONDITION>
  
    BASE_CONDITION ::=
      property == value
    | property != value
    
    DATA ::=
      property = value
    | property = value, <DATA>
```
