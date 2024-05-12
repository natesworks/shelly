# Shelly

Shelly is a simple shell implemented in C++. It provides basic functionality for command-line interaction.

**Warning!** Shelly is a work in progress and should not be used as your default shell at this time.

## Built-in commands

Shelly provides the following built-in commands.

- `set`: Allows changing different settings.
- `cd`: Changes the current directory
- `echo`: Writes text to the console
- `help`: Shows a help message
- `exit`: Exits the shell

## Prompt

The prompt is the text before the input field where you enter the command.

**Setting prompt**

To change the prompt you can use `set prompt $ `

Replace `$` with your actual prompt.

## Placeholders

Placeholders are a symbolic representation of something that will later be replaced with it's actual value later by the shell. This can be used in your prompt or welcome message.

- `{cwd}`: Current working directory.
- `{username}`: Username of the current user.
- `{hostname}`: Hostname (PC name).
## Colors

`\033[Xm`

You can use the \\033[Xm sequence to adjust the foreground and background colors of your prompt or welcome message text. Simply substitute 'X' with the appropriate color code from the provided table:

| Color Name | Foreground Color Code | Background Color Code |
| :--------- | :-------------------- | :-------------------- |
| Black      | 30                    | 40                    |
| Red        | 31                    | 41                    |
| Green      | 32                    | 42                    |
| Yellow     | 33                    | 43                    |
| Blue       | 34                    | 44                    |
| Magenta    | 35                    | 45                    |
| Cyan       | 36                    | 46                    |
| White      | 37                    | 47                    |
| Default    | 39                    | 49                    |
| Reset      | 0                     | 0                     |

## Welcome message

The welcome message is the message sent when you start the shell.

**Setting welcome message**

To change the welcome message you can use `set welcome Hello world!`

Replace `Hello world!`with your actual prompt.
