# Shelly

Shelly is a simple shell implemented in C++. It provides basic functionality for command-line interaction.

**Warning!** Shelly is a work in progress and should not be used as your default shell at this time.

## Placeholders

Placeholders are temporary text which are later replaced. They can be used in the prompt or welcome message.

**Adding custom placeholders is coming soon**

- `{cwd}`: Current working directory.
- `{username}`: Username of the current user.
- `{hostname}`: Hostname (PC name).

## Colors

``\033[Xm``
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
- `set welcome`: Sets the current welcome message.
- `disable welcome`: Disables the welcome feature.
- `cd`: Changes the current directory.
- `exit`: Exits the shell.

## TODO

Things that I will do before final release.

- Custom placeholders
- Add aliases
- Add startup commands
- Add command block
- Error handling
- Code cleanup

## Command blocks

**Warning!** Command blocks haven't been added yet!

Command blocks can be used in your prompt or welcome message to get the output of a command.

For example:

`Today is: [date -I]` will display for example `Today is: 2024-05-04`. 

Feel free to contribute to the development of Shelly by submitting pull requests or reporting any issues you encounter.
