# In-Memory Version Control System

**Creator Name:** Avaneesh R  
---

This is a model for an in-memory version control system like git. I have tried to make it as interactive and user-friendly as possible. Hope you will like it.

I have made most of the members of the classes I have used here of PUBLIC scope to facilitate easy access of variables.

Firstly I thought it would be very irritating if all the commands that are to be given are supposed to be in upper-case so I made a function (lower_case()) that compares the lower_case of input to command strings in the code therefore allowing any mix of case of letters while giving the command.

Then I structured my code based on 6 Major parts : TreeNode, Hashmaps(One for files and one for versions), MaxHeap data structure, File class, FileSystem class and the main() function.

My file class contains all the properties and details that any file that is created by the user must posses and the class FileSystem is like a database that stores all my files in one place and allows me to implement functions like recent_files() and biggest_trees().

Recent_files() and Biggest_Trees() are based on Max-Heap data stuctures that use two comparators based on which the sorting in the heap is defined.


Some Extra Features : 

    1. Create function is able to take multiple space separated arguments as files and create all of them at once.

    2. Type help at any point of time to get know the valid functions that you can use along with the arguments they take.

    3. Errors are tried to be made to point out exactly what the issue is.

    4. History of a file prints a list of all the operations done on the file along with the exact date and time of modification.

    5. My program is NOT case-sensitive as it will work even if you mix up the cases for commands (like Create,UpDATe,insert etc). 


# How to Run

## 🔹 On Unix-based Systems (Linux / macOS / Git Bash on Windows)

1. Open a Unix-based terminal (e.g., Git Bash or Linux terminal).  
2. Run the following commands to compile and execute the program:

```sh
chmod +x compile.sh
./compile.sh
```

If you are a windows user, then just run the file "run.bat".
