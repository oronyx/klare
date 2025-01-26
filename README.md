Oronyx ![CI](https://github.com/oronyx/oronyx-lang/actions/workflows/build.yml/badge.svg)
====

Oronyx (/ˈoʊroʊnɪks) is a statically typed, general purpose programming language inspired by C++
and Rust. It is designed to be safe and expressive, as well as being compatible with the C abi.

## Example

```orx
import { printf } from 'cstdlib';

function main() -> i32
{
    printf("Hello from Oronyx!"\n);
    return 0;
}
```

## Status

Early development - not ready for use by any means.

## License

MIT License. See [license](LICENSE.txt) for more information.