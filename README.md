## Description

Just example of implementing grammar parsing with Boost.Parser. It parses
unexisting DSL to make requests to database.

## Build

### Dependencies
- C++20-compatible compiler
- Boost.Parser `==1.90.0`
- Magic Enum `==0.9.7-1`
- Graphviz `==14.1.4`

### Usage
To build you must have C++20 compatible compiler. To build write:
```sh
make
./parser
```

### Get AST image
```sh
dot -Tpng graph.dot -o graph.png
```

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
