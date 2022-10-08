#include <phonebook.h>
/**
 * @brief a function that concatenates several string. see strcat() for reference
 *
 * @param strs an array of strings you want to concat. use format (const char *[]){} for the array
 * @return a concatenated string consisting of the strings you passed in
 * @note this returns a string that has been memory allocated, you are responsible for freeing it
 */
char *strs_cat(const char **strs)
{
    char *output = malloc(1 * sizeof(char));

    size_t curr_len = 0;

    for (int i = 0; strs[i][0] != '\0'; ++i)
    {
        curr_len += strlen(strs[i]);
        output = realloc(output, (curr_len + 1) * sizeof(char));
        if (i == 0)
        {
            strcpy(output, strs[i]);
            continue;
        }
        strcat(output, strs[i]);
    }

    return output;
}

/**
 * @brief a multiplicative hash function.
 *
 * @param key the string that should be encoded
 * @param phonebook the database your are creating a hash in
 * @return the key, encoded, and in integer form. returns NULL if the database hasnt been intialized.
 */
unsigned int pb_hash(char *key, phonebook phonebook)
{
    unsigned int sum = 0;
    int index = 0;

    while (key[index] != '\0')
    {
        sum += (int)key[index];
        index++;
    }
    if (phonebook.created == PB_GENERIC_SUCCESS)
    {
        sum = (((phonebook.a * sum) + phonebook.b) % phonebook.prime) % phonebook.size;
        return sum;
    }
    return -1;
}

/**
 * @brief A function that looks up an entry in the database by key
 *
 * @param key the string you want to look up
 * @param phonebook the database your are looking up  an entry in
 * @return the array entry looked up. if the lookup fails, NULL is returned
 */
struct arr_entry *pb_lookup(char *key, phonebook phonebook)
{
    int pos = pb_hash(key, phonebook);
    if (phonebook.holder[pos].data)
    {
        for (int i = 0; i < phonebook.holder[pos].datalen; i++)
        {
            if (strcmp(phonebook.holder[pos].data[i].key, key) == 0)
            {
                return &phonebook.holder[pos].data[i];
            }
        }
    }
    return PB_LOOKUP_FAILURE;
}

/**
 * @brief adds a entry to the database
 *
 * @param key the key by which this new entry will be acessed
 * @param value the value stored in the new entry
 * @param phonebook the database your are adding an entry to
 * @return an int status code, either PB_GENERIC_SUCCESS or PB_CREATE_FAILURE
 */
int pb_create(char *key, char *value, phonebook phonebook)
{
    int pos = pb_hash(key, phonebook);
    if (!pb_lookup(key, phonebook) && pos!=-1)
    {

        phonebook.holder[pos].hash = pos;
        if (phonebook.holder[pos].datalen > 0)
        {
            phonebook.holder[pos].data = realloc(phonebook.holder[pos].data, (1 + phonebook.holder[pos].datalen) * sizeof(arr_entry));
            phonebook.holder[pos].data[phonebook.holder[pos].datalen].key = strdup(key);
            phonebook.holder[pos].data[phonebook.holder[pos].datalen].value = strdup(value);
            phonebook.holder[pos].datalen++;
            return 0;
        }
        phonebook.holder[pos].data = malloc(sizeof(arr_entry));
        phonebook.holder[pos].datalen = 1;
        phonebook.holder[pos].data[0].key = strdup(key);
        phonebook.holder[pos].data[0].value = strdup(value);
        return PB_GENERIC_SUCCESS;
    }
    return PB_CREATE_FAILURE;
}
/**
 * @brief
 *
 * @param key the string that is the key of the entry you want to remove
 * @param phonebook the database your are removing an entry from
 * @return a statuscode, PB_GENERIC SUCCESS for success and PB_GENERIC_FAILURE for failure
 */
int pb_remove(char *key, phonebook phonebook)
{
    int pos = pb_hash(key, phonebook);
    int removemode = 0;
    if (phonebook.holder[pos].data && pos)
    {
        for (int i = 0; i < phonebook.holder[pos].datalen; i++)
        {
            if (removemode == 0)
            {

                if (strcmp(phonebook.holder[pos].data[i].key, key) == 0)
                {
                    free(phonebook.holder[pos].data[i].key);
                    free(phonebook.holder[pos].data[i].value);
                    free(&phonebook.holder[pos].data[i]);
                    removemode = 1;
                    continue;
                }
            }
            else
            {
                phonebook.holder[pos].data[i - 1] = phonebook.holder[pos].data[i];
                continue;
            }
        }
    }
    if (removemode == 1)
    {
        phonebook.holder[pos].datalen--;
        return PB_GENERIC_SUCCESS;
    }
    return PB_REMOVE_FAILURE;
}
/**
 * @brief a function that initializes the database. should be called before any other pb_ functions
 *
 * @param phonebook the (POINTER TO) the database your are initalizing
 * @param filename the name of the .pb file you are reading from. .pb files should be in JSON format.
 * @return a statuscode, PB_GENERIC_SUCCESS for success and JSON_PARSE_FAILURE or PB_CREATE FAILURE for failures
 */
int pb_init(phonebook *phonebook, char *filename)
{
    phonebook->a = 1 + (rand() % (phonebook->prime - 1 - 1));
    phonebook->b = (rand() % (phonebook->prime - 1));
    phonebook->size = 5000;
    phonebook->prime = 2147483647;
    phonebook->created = PB_GENERIC_SUCCESS;
    FILE *fp;
    fp = fopen(filename, "r");
    if(!fp){
        return 1;
    }
    if (fseek(fp, 0L, SEEK_END) != 0)
    {
        perror("seeking");
        return 1;
    };
    int sz = ftell(fp);
    char JSON_STRING[sz];
    JSON_STRING[sz] = '\0';
    rewind(fp);
    fread(JSON_STRING, 1, sz, (FILE *)fp);
    jsmn_parser parser;
    jsmn_init(&parser);
    int count = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), NULL, 0);
    jsmntok_t tokens[count];
    parser.pos = 0;
    int obj = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), tokens, sizeof(tokens) / sizeof(tokens)[0]);
    if (obj < 0)
    {
        printf("Failed to parse JSON: %d\n", obj);
        return JSON_PARSE_FAILURE;
    }
    /* Assume the top-level element is an object */
    if (obj < 1 || tokens[0].type != JSMN_OBJECT)
    {
        printf("Failed to parse JSON: %d\n", parser);
        printf("Object expected\n");
        return JSON_PARSE_FAILURE;
    }
    for (int i = 1; i < obj; i += 2)
    {
        if (tokens[i].type == JSMN_STRING && tokens[i + 1].type == JSMN_STRING)
        {
            char *key = strndup(JSON_STRING + tokens[i].start, tokens[i].end - tokens[i].start);
            char *value = strndup(JSON_STRING + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
            if (pb_create(key, value, *phonebook) == PB_CREATE_FAILURE)
            {
                return PB_CREATE_FAILURE;
            }
        }
    }

    // other stuff
    return PB_GENERIC_SUCCESS;
}

/**
 * @brief a function that writes a phonebook database to disk
 *
 * @param phonebook the database you are writing to disk
 * @param filename the name of the .pb file you are writing to. .pb files should be in JSON format.
 * @return a status code, PB_GENERIC_SUCCESS for success and PB_WRITE FAILURE for faulure.
 */
int pb_write(phonebook phonebook, char *filename)
{
    char *JSON_STRING = malloc(sizeof(char) * 2);
    JSON_STRING = strdup("{");
    for (int i = 0; i < ENTRY_AMOUNT; i++)
    {
        int currlen = 0;
        for (int j = 0; j < phonebook.holder[i].datalen; j++)
        {
            char *prepend = (strcmp(JSON_STRING, "{") == 0) ? "\"" : ",\"";
            currlen += strlen(",\"\"\"\": ");
            currlen += strlen(phonebook.holder[i].data[j].key);
            currlen += strlen(phonebook.holder[i].data[j].value);
            JSON_STRING = realloc(JSON_STRING, (currlen + 2) * sizeof(char));
            JSON_STRING = strs_cat((const char *[]){JSON_STRING, prepend, phonebook.holder[i].data[j].key, "\"", ": ", "\"", phonebook.holder[i].data[j].value, "\"", ""});
        }
    }
    JSON_STRING = strcat(JSON_STRING, "}");
    // can now write to file
    FILE *fp = fopen(filename, "w");
    if (fp)
    {
        fputs(JSON_STRING, fp);
        fclose(fp);
        free(JSON_STRING);
        return PB_GENERIC_SUCCESS;
    }
    return PB_WRITE_FAILURE;
}