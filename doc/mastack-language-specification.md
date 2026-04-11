
# 1. Table of Contents
- [1. Table of Contents](#1-table-of-contents)
- [2. Type Implementation](#2-type-implementation)
- [3. Types](#3-types)
  - [3.1. Primitive Types](#31-primitive-types)
  - [3.2. Compound Types](#32-compound-types)
    - [3.2.1. Tuple Types](#321-tuple-types)
    - [3.2.2. Object Types](#322-object-types)
- [4. Statements](#4-statements)
  - [4.1. Variable Definition and Initialization](#41-variable-definition-and-initialization)
  - [4.2. Function Definition and Invocation](#42-function-definition-and-invocation)
  - [4.3. Structure](#43-structure)
    - [4.3.1. Structure Definition](#431-structure-definition)
  - [4.4. Flow Control](#44-flow-control)

# 2. Type Implementation

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

# 3. Types

## 3.1. Primitive Types

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

## 3.2. Compound Types

### 3.2.1. Tuple Types

### 3.2.2. Object Types

# 4. Statements

## 4.1. Variable Definition and Initialization

- Variable definition
  ```mastack
  let length: Integer
  ```

- Variable definition and initialization
  ```mastack
  let PI: Float = 3.14
  ```

## 4.2. Function Definition and Invocation

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

## 4.3. Structure

### 4.3.1. Structure Definition
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

## 4.4. Flow Control
