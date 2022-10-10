/*
    This file is part of Phonebook.

    Phonebook is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    Phonebook is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <jsmn.h>   //json
#include <stdio.h>  //inputs and outputs
#include <string.h> //string functions like strcat() and strcpy()
#include <malloc.h> //memory allocation
#include <time.h>
#include <stdlib.h>

#pragma once


#define PB_GENERIC_SUCCESS 0
#define PB_GENERIC_FAILURE 1
#define JSON_PARSE_FAILURE 2
#define PB_CREATE_FAILURE 3
#define PB_WRITE_FAILURE 4
#define PB_REMOVE_FAILURE 5
#define PB_HASH_FAILURE 6
#define PB_LOOKUP_FAILURE NULL


typedef struct arr_entry
{
    char *key;
    char *value;
} arr_entry;

typedef struct phonebook_entry
{
    unsigned int hash;
    int datalen;
    arr_entry *data;
} phonebook_entry;

typedef struct phonebook
{
    phonebook_entry *holder;
    int size;
    unsigned int prime;
    unsigned int b;
    unsigned int a;
    int created;

} phonebook;



/**
 * @brief a function that concatenates several string. see strcat() for reference
 *
 * @param strs an array of strings you want to concat. use format (const char *[]){} for the array
 * @return a concatenated string consisting of the strings you passed in
 * @note this returns a string that has been memory allocated, you are responsible for freeing it
 */
char *strs_cat(const char **strs);



/**
 * @brief a multiplicative hash function.
 *
 * @param key the string that should be encoded
 * @param phonebook the database your are creating a hash in
 * @return the key, encoded, and in integer form. returns NULL if the database hasnt been intialized.
 */
unsigned int pb_hash(char *key, phonebook phonebook);



/**
 * @brief A function that looks up an entry in the database by key
 *
 * @param key the string you want to look up
 * @param phonebook the database your are looking up  an entry in
 * @return the array entry looked up. if the lookup fails, NULL is returned
 */
struct arr_entry *pb_lookup(char *key, phonebook phonebook);




/**
 * @brief adds a entry to the database
 *
 * @param key the key by which this new entry will be acessed
 * @param value the value stored in the new entry
 * @param phonebook the database your are adding an entry to
 * @return an int status code, either PB_GENERIC_SUCCESS or PB_CREATE_FAILURE
 */
int pb_create(char *key, char *value, phonebook phonebook);



/**
 * @brief
 *
 * @param key the string that is the key of the entry you want to remove
 * @param phonebook the database your are removing an entry from
 * @return a statuscode, PB_GENERIC SUCCESS for success and PB_GENERIC_FAILURE for failure
 */
int pb_remove(char *key, phonebook phonebook);



/**
 * @brief a function that initializes the database. should be called before any other pb_ functions
 *
 * @param phonebook the (POINTER TO) the database your are initalizing
 * @param filename the name of the .pb file you are reading from. .pb files should be in JSON format.
 * @return a statuscode, PB_GENERIC_SUCCESS for success and JSON_PARSE_FAILURE or PB_CREATE FAILURE for failures
 */
int pb_init(phonebook *phonebook, char *filename);



/**
 * @brief a function that writes a phonebook database to disk
 *
 * @param phonebook the database you are writing to disk
 * @param filename the name of the .pb file you are writing to. .pb files should be in JSON format.
 * @return a status code, PB_GENERIC_SUCCESS for success and PB_WRITE FAILURE for faulure.
 */
int pb_write(phonebook phonebook, char *filename);
