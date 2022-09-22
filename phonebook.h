
#include <rmsutils.h> //utils!
#include <jsmn.h>     //json

#define INT_A 1306858826
#define INT_B 1130263739
#define INT_LE 2147483647

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


struct arr_entry{
    char *key;
    char *value;
};

struct phonebook_entry
{
    unsigned int hash;
    int datalen;
    struct arr_entry* data;
};


unsigned int hash(char *key, int size, int a, int b, int prime)
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

char *lookup(char *key, struct phonebook_entry *holder){
    int pos = hash(key, ENTRY_AMOUNT, INT_A, INT_B, INT_LE);
    if(holder[pos].data){
        for (int i = 0; holder[pos].data[i].key; i++)
        {
            if(strcmp(holder[pos].data[i].key, key) == 0){
                return holder[pos].data[i].value;
            }
        }
        return "e";
    }
}

int initpb(struct phonebook_entry *holder)
{

    static const char *JSON_STRING =
        "{\"user\": \"johndoe\", \"admin\": false,\"admin\": true,  \"uid\": 1000}";
    jsmn_parser parser;
    jsmn_init(&parser);
    int count = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), NULL, 0);
    jsmntok_t tokens[count];
    parser.pos = 0;
    int obj = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), tokens, sizeof(tokens)/sizeof(tokens)[0]);
    if (obj < 0)
    {
        printf("Failed to parse JSON: %d\n", obj);
        return 1;
    }
    /* Assume the top-level element is an object */
    if (obj < 1 || tokens[0].type != JSMN_OBJECT)
    {
        printf("Failed to parse JSON: %d\n", parser);
        printf("Object expected\n");
        return 1;
    }
    for (int i = 1; i < obj; i+= 2)
    {
        char *key = strndup(JSON_STRING + tokens[i].start, tokens[i].end - tokens[i].start);
        int pos = hash(key,ENTRY_AMOUNT, INT_A, INT_B, INT_LE);
        holder[pos].hash = pos;
        if(holder[pos].data){
            holder[pos].data = realloc(holder[pos].data, (1+holder[pos].datalen)*sizeof(struct arr_entry));
            holder[pos].data[holder[pos].datalen].key = strdup(key);
            holder[pos].data[holder[pos].datalen].value = strndup(JSON_STRING + tokens[i + 1].start, tokens[i+1].end - tokens[i+1].start);
            holder[pos].datalen++;
            continue;
        }
        int x = sizeof(holder[pos]);
        holder[pos].data = malloc(sizeof(struct arr_entry));
        holder[pos].datalen = 1;
        holder[pos].data[0].key = strdup(key);
        holder[pos].data[0].value = strndup(JSON_STRING + tokens[i + 1].start, tokens[i+1].end - tokens[i+1].start);

    }
    
}
