# dir5
in *NIX OSes, it will search a parent directory, output the first occurrence of a search-string in files
in the parent directory tree and print surrounding text  and a line number.

the following will search all files internally starting with /usr/local/ for instances of "stdio.h" and will print out 200 
characters before and 200 characters after the first instance found in each file:

./dir5 "/usr/local" "stdio.h" 200

