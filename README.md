# Shelly

Shelly is a simple shell implemented in C++. It provides basic functionality for command-line interaction.

**Warning!** Shelly is a work in progress and should not be used as your default shell at this time.

## Placeholders

**Adding custom placeholders is coming soon**

- `{cwd}`: Current working directory.
- `{username}`: Username of the current user.
- `{hostname}`: Hostname (PC name).

## Colors

To change the texts foreground or background colors you can use ansi escape codes
``\033[0m``
replace 0 with a color code.

## Built-in Commands

Shelly has the following built-in commands:

- `set prompt`: Sets the current prompt.
- `cd`: Changes the current directory.
- `exit`: Exits the shell.

Feel free to contribute to the development of Shelly by submitting pull requests or reporting any issues you encounter.
