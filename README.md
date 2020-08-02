![CPP project with CTest CI](https://github.com/came-project/cmd-template/workflows/CPP%20project%20with%20CTest%20CI/badge.svg)

# Command-line tool template (C++)

## Directory structure

* `third-party` : here third-party components reside. An example is `gflags` library.
* `include` : h files.
* `src` : cpp files.
* `test` : testing stuff. Initially empty.

## Building

CMake (at least 3.13) is used for build instructions generating.

## Continuous integration

Github-provided CI is used. See `.github/workflows/cpp_project.yml`.

## Clang format
A default clang format config is provided in `.clang-format`.
If LLVM package is installed in your system and its tools are in your PATH, then you can use an integration with Git, for example
```sh
git clang-format HEAD~1
```
format latest commit and store results as unstaged changes.
