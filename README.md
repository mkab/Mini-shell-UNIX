# Mini-shell-UNIX
A simple shell program written in C language.

Prerequisites

Minishell requires the following:
-  An installed GNU Readline library.
-  Use the following command to install it to your system.

command: sudo apt-get install libreadline6 libreadline6-dev

Note: Installation of Minishell also installs the readline library with it. So do not worry if you can’t install the library yourself.

Installation in 3 steps

Step 1:  Go to a Linux shell and login as the system administrator root:  sudo su (and type your password)

Step 2:  Browse to where the sources files are located in your compute.

Step 3: Type 'make' or 'make install' to set up the program to your system.
That’s all. Minishell is now installed and ready to use. To use Minishell simply type the command minishell on your Linux shell. 

Note: In order for the makefile to work, all the header files, sources files and makefile should be on the same folder.
      You can also launch Minishell from your Linux terminal irrespective of your current/working directory (it's added to the global path)

Uninstallation:
To uninstall Minishell, in your Linux terminal, go to the extracted Minishell folder and type: 

"make uninstall".

Please check the manual guide for more details.
