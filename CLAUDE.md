# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Purpose

This is a language experimentation project. The goal is to explore programming languages through hands-on experiments that reveal non-obvious, unique, or surprising behaviors — things you don't learn from textbooks.

## How It Works

When the user asks to do a new experiment (e.g., "do a new experiment in Python" or "new experiment in Kotlin on coroutines"):

1. Read all existing experiment files and `INSIGHTS.md` in the `<language>/` folder to understand what's already covered.
2. If the user specified a topic, create an experiment on that topic (but still avoid repeating existing insights).
3. If no topic specified, pick a topic that is NOT already covered by existing experiments.
4. Create the experiment as a standalone, runnable script in `<language>/`.
5. Run the experiment and capture actual output.
6. Append the new insight to `<language>/INSIGHTS.md`.

If the language folder doesn't exist yet, create it.

## Rules for Experiments

- **No web apps, no frameworks** — pure language experiments only.
- Each experiment file should be self-contained and runnable independently.
- Name experiments descriptively: `floating_point_trap.py`, `mutable_default_gotcha.py`, etc.
- Every experiment must include comments explaining what's happening and why it's surprising.
- Focus on behaviors that differ from other languages or defy common assumptions.
- Run the experiments and capture actual output in INSIGHTS.md.
- **Never repeat** an experiment or insight that already exists in the language folder.

## Running Experiments

- C: `gcc -std=c17 -o /tmp/c_exp c/<file>.c && /tmp/c_exp`
- C++: `g++ -std=c++17 -o /tmp/cpp_exp cpp/<file>.cpp && /tmp/cpp_exp`
- Go: `go run go/<file>.go`
- Java: `javac java/<file>.java -d /tmp/java_exp && java -cp /tmp/java_exp <ClassName>`
- JavaScript: `node javascript/<file>.js`
- Kotlin: `kotlinc kotlin/<file>.kt -include-runtime -d /tmp/kt_exp.jar && java -jar /tmp/kt_exp.jar`
- Python: `python3 python/<file>.py`
- Ruby: `ruby ruby/<file>.rb`
- Rust: `rustc rust/<file>.rs -o /tmp/rust_exp && /tmp/rust_exp`
- Swift: `swiftc swift/<file>.swift -o /tmp/swift_exp && /tmp/swift_exp`
- Other languages: use standard toolchain for that language.

## INSIGHTS.md Format

Each insight should have:
- **What**: the experiment (brief code snippet or reference to file)
- **Expected**: what most people would assume
- **Actual**: what actually happens
- **Why**: the language internals that cause this behavior
