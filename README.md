# Shelly

Shelly is a simple shell implemented in C++. It provides basic functionality for command-line interaction.

**Warning!** Shelly is a work in progress and should not be used as your default shell at this time.

## Placeholders

**Adding custom placeholders is coming soon**

- `{cwd}`: Current working directory.
- `{username}`: Username of the current user.
- `{hostname}`: Hostname (PC name).

## Colors

``\033[Xm]``
replace 'X' with a color code

| Color Name | Foreground Color Code | Background Color Code |
| :--------- | :-------------------- | :-------------------- |
| Black      | `30`                  | `40`                  |
| Red        | `31`                  | `41`                  |
| Green      | `32`                  | `42`                  |
| Yellow     | `33`                  | `43`                  |
| Blue       | `34`                  | `44`                  |
| Magenta    | `35`                  | `45`                  |
| Cyan       | `36`                  | `46`                  |
| White      | `37`                  | `47`                  |
| Default    | `39`                  | `49`                  |
| Reset      | `0`                   | `0`                   |

## Built-in Commands

Shelly has the following built-in commands:

- `set prompt`: Sets the current prompt.
- `cd`: Changes the current directory.
- `exit`: Exits the shell.

Feel free to contribute to the development of Shelly by submitting pull requests or reporting any issues you encounter.
