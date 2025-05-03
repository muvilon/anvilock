# Anvilock (Unstable branch)

This branch aims to port Anvilock to a better and readable codebase using modern C++ (ideally cpp20)

> [!IMPORTANT]
> 
> Unstable branch as the name suggests, is VERY unstable!
> 
> Expect breaking changes with each passing commit.
> 

## Building

Firstly clone this branch recursively:

```bash 
git clone --branch unstable --recursive https://github.com/muvilon/Anvilock.git
```

Then build:

```bash 
cmake -S . -B build 
cmake --build build/
```

More docs on the porting process will be out soon.
