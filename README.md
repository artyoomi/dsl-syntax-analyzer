## Description
Simple DSL parser.

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
