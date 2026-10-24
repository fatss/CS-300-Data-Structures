# CS-300-Data-Structures

SNHU CS 300 Data Structures and Algorithms Portfolio

This portfolio contains two pieces of work for ABCU's Computer Science
department. From Project One, it includes my run-time and memory analysis of
three data structures. From Project Two, it includes my working C++ advising
program. The program loads the course catalog and prints the courses in
alphanumeric order. It also shows any single course with its prerequisites.

## Files

```text
ProjectOne_Runtime_Analysis.docx   Project One: pseudocode for a vector, hash table,
                                   and binary search tree, with run-time and memory
                                   analysis and a recommendation
ProjectTwo.cpp                     Project Two: the advising program
courses.csv                        Sample course data for running the program
```

## Running Project Two

```sh
c++ -std=c++11 -Wall -Wextra -pedantic ProjectTwo.cpp -o advising
./advising
```

Choose option 1 and enter `courses.csv` to load the catalog. Option 2 prints
every course in alphanumeric order. Option 3 asks for a course number, such as
`CSCI400` or `csci400`, and prints its title and prerequisites. Option 9 exits.

Each row of the CSV file is `courseNumber,title[,prerequisite...]` with no
header row. The loader rejects the whole file if it has malformed rows,
duplicate course numbers, or prerequisites that are not in the file. If a
reload fails, the catalog that was already loaded is kept.

## Reflection

### What was the problem I was solving?

ABCU's academic advisors needed a program to help them answer students'
questions about Computer Science courses. The program had two jobs. It had to
print every course in the program in alphanumeric order. It also had to look
up any single course and show its title and prerequisites. In Project One I
wrote pseudocode for loading the course file into a vector, a hash table, and
a binary search tree. I then analyzed the run time and memory use of each one
and recommended one. In Project Two I built the working program in C++.

### How did I approach the problem, and why do data structures matter?

I began with what advisors would actually do: load the catalog once, then list
courses and look up individual courses many times. Then I worked out the
worst-case cost of each step for each data structure. A vector appends records
in amortized O(1) time, but finding one course takes O(n). It also has to be
sorted before the list can be printed. A hash table has O(1) expected lookup.
In the worst case, though, every key collides, so a lookup takes O(n) and
loading takes O(n²). It also has to be copied and sorted to print in order.
An unbalanced binary search tree prints in order for free with an in-order
traversal. But if the courses arrive already sorted, it becomes a chain, and
searching and loading degrade the same way.

That analysis is why data structures matter. All three structures store the
same records, yet the choice decides how fast and how predictable the program
is. In Project One I recommended the hash table for its fast expected lookup.
For the final program I chose a vector that is sorted once after loading. The
catalog rarely changes once it is loaded, so one O(n log n) sort pays for
everything after it. Printing the list becomes a plain O(n) loop, and every
lookup is an O(log n) binary search, even in the worst case. That is a better
fit than a hash table that has to sort a copy every time the list is printed.

### How did I overcome roadblocks?

Most roadblocks came from the data rather than from the algorithms. A
prerequisite can appear in the file before the course that defines it. So I
read every row first and only checked prerequisite references at the end. I
was also careful not to wipe out a good catalog when a reload failed. The new
file loads into a temporary vector, and it replaces the current catalog only
after all of it passes validation. Course numbers had to match no matter how
someone typed them, so I trimmed and uppercased them on both load and lookup.
Titles can contain commas, so the parser handles quoted fields too. Each time,
I tested the program with a small input built to trigger the problem. Then I
fixed the cause instead of patching the output.

### How has this work expanded my approach to designing software?

I now think about how data will be used before I decide how to store it.
Before this course, I would have picked whichever structure was easiest to
code. Now I ask which operations happen most often, which ones need to be
fast in the worst case, and whether the data changes after it is loaded. Then
I let those answers drive the design. Writing the pseudocode and the cost
tables first also helped. I could compare options on paper before writing any
code, and a bad design costs much less to drop at that stage.

### How has this work changed how I write maintainable, readable, and adaptable code?

I write small functions that each do one job. `trim`, `normalizeCourseNumber`,
`parseCsvRow`, `loadCourses`, `findCourse`, and the two print functions can
each be read, tested, and changed without touching the others. I use
descriptive names instead of abbreviations. I keep comments for the reasons
the code can't show on its own, such as why references are checked after
reading or why the vector stays sorted. I also rely on the standard library
(`std::sort`, `std::lower_bound`, `std::unordered_set`) instead of writing my
own versions, because those are well tested and other developers recognize
them. Finally, I keep validation and error messages specific, telling the user
which line is wrong and why. That makes the program easier to support. It is
also easier to adapt, for example to a larger catalog or a different storage
structure behind the same functions.
