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

### Setting log level

You can simply set the log level using `ANVLK_LOG_LEVEL` environment variable in the terminal.

The environment variable can be set to:

1. **ERROR**
2. **WARN**
3. **INFO**
4. **DEBUG**
5. **TRACE**

These levels are listed in priority and upon choosing a log level (say `X`), it will print logs of priority `X` **AND** above

```bash 
export ANVLK_LOG_LEVEL=INFO
./build/anvilock 

# For future changes 
ANVLK_LOG_LEVEL=debug ./build/anvilock
```

More docs on the porting process will be out soon.
