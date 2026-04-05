
# Table of Contents
- [Table of Contents](#table-of-contents)
- [Type Implementation](#type-implementation)
- [Types](#types)
  - [Primitive Types](#primitive-types)
  - [Compound Types](#compound-types)
    - [Tuple Types](#tuple-types)
    - [Object Types](#object-types)
- [Statements](#statements)
  - [Variable Definition and Initialization](#variable-definition-and-initialization)
  - [Function Definition and Invocation](#function-definition-and-invocation)
  - [Structure](#structure)
    - [Structure Definition](#structure-definition)
  - [Flow Control](#flow-control)
    - [](#)

# Type Implementation

For built-in types, the operators are mapped into different functions.

Operator mapping table:
| Operator | Mapped into |
|:--------:|:-----------:|
|   `+`    |  `__add__`  |
|   `-`    |  `__sub__`  |
|   `*`    |  `__mul__`  |
|   `/`    |  `__div__`  |
|   `%`    |  `__mod__`  |
|   `&`    |  `__and__`  |
|   `\|`   |  `__or__`   |
|   `!`    |  `__not__`  |

# Types

## Primitive Types

- Integer `let var_integer: Integer = 47`
  The value is alway owned by the variable, alway copied while passed through function.

- Float `let var_float: Float = 3.14`
  The value is alway owned by the variable, alway copied while passed through function.

- Boolean `let var_boolean: Boolean = true`
  The value is alway owned by the variable, alway copied while passed through function.

- String `let var_string: String = s"Hello, world!\n"`
- Blob `let var_blob: Blob = b"\xAA\x55\x1F\x1B\x1A"`

- Option `let var_possible_integer: Integer? = nil`

- Function `fn add(a: Integer, b: Integer) -> Integer { return a + b }`

- Array

- Structure

## Compound Types

### Tuple Types

### Object Types

# Statements

## Variable Definition and Initialization

- Variable definition
  ```mastack
  let length: Integer
  ```

- Variable definition and initialization
  ```mastack
  let PI: Float = 3.14
  ```

## Function Definition and Invocation

- Function definition
  ```mastack
  fn add(a: Integer, b: Integer) -> Integer {
      return a + b
  }
  ```

- Function Invocation
  ```mastack
  let c = a * add(b, c)
  ```

## Structure

### Structure Definition
```mastack
struct Person {
    let name: String
    let age: Integer

    init(name: String, age: Integer) {
        self.name = name
        self.age = age
    }

    fn to_string() -> String {
        return f"name: {self.name}, age : {self.age}"
    }
}
```

## Flow Control

###
