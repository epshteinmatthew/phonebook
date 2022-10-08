# phonebook
## very simple key value database in c

### Work in progress

phonebook is a key value database a lot like redis, working entierely in RAM. The central idea is that you do all database operations inside RAM, and commit to memory when the program ends.

## Basic Usage

- download the files: (jsmn.c, jsmn,h, phonebook.c, phonebook.h) from this repo and put them into your project.
- Include "phonebook.h" wherever you want to use the pb_ functions.
- compile with gcc: "gcc -I [PATH TO YOUR PROJECT DIRECTORY] [FILENAME].c phonebook.c jsmn.c"

## The API(these arent docs, justa breif description):

- pb_init() is a function that loads in the key value pairs from disk and into memory. this should be called before any other functions in the API
- pb_create() is a function that creates a key value pair in the database
- pb_lookup() is a functionlooks up an value in the database by key
- pb_hash() is a hash function that uses the [multiplicative hashing](https://www.youtube.com/watch?v=BmKMzAt2Gjc) method
- pb_remove() is a function that removes an entry in the database by its key
- pb_write() is a function that saves the key value pairs to disk. use this as a "save button" 

## credits

- @zserge for creating JSMN library, on which this code depends
- @weijuwang for helpful suggestions
- stack overflow for... assistance
