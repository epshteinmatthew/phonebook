#include <jsmn.h>   //json
#include <stdio.h>  //inputs and outputs
#include <string.h> //string functions like strcat() and strcpy()
#include <malloc.h> //memory allocation

#define INT_A 1306858826
#define INT_B 1130263739
#define INT_LE 2147483647
//maybe generate new constants on pb_init?

#define PB_GENERIC_SUCCESS 0
#define PB_GENERIC_FAILURE 1
#define JSON_PARSE_FAILURE 2
#define PB_CREATE_FAILURE 3
#define PB_WRITE_FAILURE 4
#define PB_REMOVE_FAILURE 5
#define PB_LOOKUP_FAILURE NULL

/*
stuff to do:

entry strcut: key, values, hash

philosophy:
Have a function to initialize that loads specified file into RAM as a list of KV's
Hashing(?)
Manipulations happens only in RAM, then saves on progream exit
*/

#define PB_DIR "/home/matthew/Documents/pdftomd/phonebook" // change per user
#define ENTRY_AMOUNT 5000

struct arr_entry
{
    char *key;
    char *value;
};

struct phonebook_entry
{
    unsigned int hash;
    int datalen;
    struct arr_entry *data;
};

//utility function
char *strs_cat(const char **strs)
{
    char *output = malloc(1 * sizeof(char));

    size_t curr_len = 0;

    for (int i = 0; strs[i][0] != '\0'; ++i)
    {
        curr_len += strlen(strs[i]);
        output = realloc(output, (curr_len+1) * sizeof(char));
        if (i == 0)
        {
            strcpy(output, strs[i]);
            continue;
        }
        strcat(output, strs[i]);
    }

    return output;
}


unsigned int pb_hash(char *key, int size, int a, int b, int prime)
{
    unsigned int sum = 0;
    int index = 0;

    while (key[index] != '\0')
    {
        sum += (int)key[index];
        index++;
    }
    sum = (((a * sum) + b) % prime) % size;
    return sum;
}

struct arr_entry *pb_lookup(char *key, struct phonebook_entry *holder)
{
    int pos = pb_hash(key, ENTRY_AMOUNT, INT_A, INT_B, INT_LE);
    if (holder[pos].data)
    {
        for (int i = 0; i < holder[pos].datalen; i++)
        {
            if (strcmp(holder[pos].data[i].key, key) == 0)
            {
                return &holder[pos].data[i];
            }
        }
        //*can change to an errorcode?
        return PB_LOOKUP_FAILURE;
    }
}

int pb_create(char *key, char *value, struct phonebook_entry *holder)
{
    int pos = pb_hash(key, ENTRY_AMOUNT, INT_A, INT_B, INT_LE);
    holder[pos].hash = pos;
    if (!pb_lookup(key, holder))
    {
        if (holder[pos].datalen > 0)
        {
            holder[pos].data = realloc(holder[pos].data, (1 + holder[pos].datalen) * sizeof(struct arr_entry));
            holder[pos].data[holder[pos].datalen].key = strdup(key);
            holder[pos].data[holder[pos].datalen].value = strdup(value);
            holder[pos].datalen++;
            return 0;
        }
        holder[pos].data = malloc(sizeof(struct arr_entry));
        holder[pos].datalen = 1;
        holder[pos].data[0].key = strdup(key);
        holder[pos].data[0].value = strdup(value);
        return PB_GENERIC_SUCCESS;
    }
    return PB_CREATE_FAILURE;
}

int pb_remove(char *key, struct phonebook_entry *holder)
{
    int pos = pb_hash(key, ENTRY_AMOUNT, INT_A, INT_B, INT_LE);
    int removemode = 0;
    if (holder[pos].data)
    {
        for (int i = 0; i < holder[pos].datalen; i++)
        {
            if (removemode == 0)
            {

                if (strcmp(holder[pos].data[i].key, key) == 0)
                {
                    free(holder[pos].data[i].key);
                    free(holder[pos].data[i].value);
                    free(&holder[pos].data[i]);
                    removemode = 1;
                    continue;
                }
            }
            else
            {
                holder[pos].data[i - 1] = holder[pos].data[i];
                continue;
            }
        }
    }
    if (removemode == 1)
    {
        holder[pos].datalen--;
        return PB_GENERIC_SUCCESS;
    }
    return PB_REMOVE_FAILURE;
}

int pb_init(struct phonebook_entry *holder, char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    if(fseek(fp, 0L, SEEK_END) != 0){perror("seeking");return 1;};
    int sz = ftell(fp);
    char JSON_STRING[sz];
    JSON_STRING[sz] = '\0';
    rewind(fp);
    fread(JSON_STRING, 1,sz,  (FILE *)fp);
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
        char *key = strndup(JSON_STRING + tokens[i].start, tokens[i].end - tokens[i].start);
        char *value = strndup(JSON_STRING + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        if(pb_create(key, value, holder) == PB_CREATE_FAILURE) {return PB_CREATE_FAILURE;}
    }
    return PB_GENERIC_SUCCESS;
}

int pb_write(struct phonebook_entry *holder, char *filename)
{
    char *JSON_STRING = malloc(sizeof(char) * 2);
    JSON_STRING = strdup("{");
    for (int i = 0; i < ENTRY_AMOUNT; i++)
    {
        int currlen = 0;
            for (int j = 0; j < holder[i].datalen; j++)
            {
                char *prepend = (strcmp(JSON_STRING, "{") == 0) ? "\"" : ",\"";
                currlen += strlen(",\"\"\"\": ");
                currlen += strlen(holder[i].data[j].key);
                currlen += strlen(holder[i].data[j].value);
                JSON_STRING = realloc(JSON_STRING, (currlen + 2) * sizeof(char));
                JSON_STRING = strs_cat((const char *[]){JSON_STRING, prepend, holder[i].data[j].key, "\"", ": ", "\"", holder[i].data[j].value, "\"", ""});
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
