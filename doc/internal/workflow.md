# Workflow setup

There are commands for automating building and running operations.
When you're under the project top directory, run the following in your terminal:

`source ./scripts/env.sh`

This will allow you to use the following commands:

- `build`:   Automatically builds the project.
- `run`: Automatically builds, and runs the built executable
- `clean`: Removes the `build/` directory.
- `test`: Performs tests, a routine part of our CI/CD and test-driven development process.