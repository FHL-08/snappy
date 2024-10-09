#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short hash_function(const char* u, int N)
{
    unsigned long long hash = 0;
    int c;
    for (int i = 0; i < 4; i++)
    {
        c = u[i];
        hash = (hash * 131) + c;
    }
    hash ^= (hash >> 17);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);
    return (unsigned short)(hash % N);
}

unsigned int decode_bytes(unsigned char* u, int size)
{
    unsigned int result = 0;
    for (int i = (size - 1); i >= 0; i--)
    {
        result = (result << 8) | u[i];
    }
    return result;
}

void split_bytes(int size, unsigned char* array, int data, int* pointer)
{
    for (int i = 0; i < size; i++)
    {
        array[(*pointer)++] = (data >> (8 * i)) & 0xFF;
    }
    return;
}

typedef struct fragment
{
    char text[5];
    int duplicates;
    int* position;
} fragment;

fragment* create_fragment(int pos, const char* u)
{
    fragment* new_fragment = (fragment*)malloc(sizeof(fragment));
    new_fragment->duplicates = 1;
    new_fragment->position = (int*)malloc(sizeof(int));
    new_fragment->position[0] = pos;
    memcpy(new_fragment->text, u, 4);
    new_fragment->text[4] = '\0';
    return new_fragment;
}

typedef struct LinkedList
{
    fragment* bytes;
    struct LinkedList* next;
} LinkedList;

void free_LinkedList(LinkedList* list)
{
    while (list)
    {
        LinkedList* temp = list;
        list = list->next;
        free(temp->bytes->position);
        free(temp->bytes);
        free(temp);
    }
}

typedef struct HashTable
{
    LinkedList** buckets;
    int size;
} HashTable;

HashTable* create_HashTable(int N)
{
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->buckets = (LinkedList**)calloc(N, sizeof(LinkedList*));
    table->size = N;
    return table;
}

void insertToHashTable(HashTable* table, const char* text, int position)
{
    unsigned short index = hash_function(text, table->size);
    LinkedList** list = &table->buckets[index];
    while (*list)
    {
        if (memcmp((*list)->bytes->text, text, 4) == 0)
        {
            int* temp = (int*)realloc((*list)->bytes->position, ((*list)->bytes->duplicates + 1) * sizeof(int));
            if (temp == NULL)
            {
                printf("realloc failed!\n");
                free_LinkedList(*list);
                exit(0);
            }
            (*list)->bytes->position = temp;
            (*list)->bytes->position[(*list)->bytes->duplicates] = position;
            (*list)->bytes->duplicates++;
            return;
        }
        list = &(*list)->next;
    }
    *list = (LinkedList*)malloc(sizeof(LinkedList));
    (*list)->bytes = create_fragment(position, text);
    (*list)->next = NULL;
}

void free_table(HashTable** table)
{
    for (int i = 0; i < (*table)->size; i++)
    {
        if ((*table)->buckets[i] != NULL)
        {
            free_LinkedList((*table)->buckets[i]);
            (*table)->buckets[i] = NULL;
        }
    }
    free((*table)->buckets);
    (*table)->buckets = NULL;
    free(*table);
    *table = NULL;
}

typedef struct duplicate_bytes
{
    int position;
    int size;
    int offset;
} duplicate_bytes;

intmax_t getline(char** lineptr, size_t* n, FILE* stream)
{
    char* bufptr = NULL;
    char* p = bufptr;
    size_t size;
    char c;

    if (lineptr == NULL || stream == NULL || n == NULL)
    {
        return -1;
    }

    bufptr = *lineptr;
    size = 1;

    if ((c = fgetc(stream)) == EOF)
    {
        return -1;
    }

    if (bufptr == NULL)
    {
        if ((bufptr = (char*)malloc(sizeof(char))) == NULL)
        {
            return -1;
        }
    }
    p = bufptr;

    *(p++) = c;
    while ((c = fgetc(stream)) != EOF)
    {
        int offset = p - bufptr;
        size += 1;
        char* temp = (char*)realloc(bufptr, size);
        if (temp == NULL)
        {
            free(bufptr);
            bufptr = NULL;
            return -1;
        }
        bufptr = temp;
        temp = NULL;
        p = bufptr + offset;

        *(p++) = c;
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

void findMax(int arr[], int size, int* maxIndex, int* maxValue)
{
    *maxIndex = 0;
    *maxValue = arr[0];

    for (int i = 1; i < size; ++i)
    {
        if (arr[i] > *maxValue)
        {
            *maxValue = arr[i];
            *maxIndex = i;
        }
    }
}

int main()
{
    char* text =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
        "magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
        "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
        "nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit "
        "anim id est laborum. Sed ut perspiciatis, unde omnis iste natus error sit voluptatem accusantium doloremque "
        "laudantium, totam rem aperiam eaque ipsa, quae ab illo inventore veritatis et quasi architecto beatae vitae "
        "dicta sunt, explicabo. Nemo enim ipsam voluptatem, quia voluptas sit, aspernatur aut odit aut fugit, sed quia "
        "consequuntur magni dolores eos, qui ratione voluptatem sequi nesciunt, neque porro quisquam est, qui dolorem "
        "ipsum, quia dolor sit amet consectetur adipisci[ng] velit, sed quia non numquam [do] eius modi tempora "
        "inci[di]dunt, ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis "
        "nostrum[d] exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? "
        "[D]Quis autem vel eum i[r]ure reprehenderit, qui in ea voluptate velit esse, quam nihil molestiae "
        "consequatur, vel illum, qui dolorem eum fugiat, quo voluptas nulla pariatur? Lorem ipsum dolor sit amet, "
        "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
        "minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute "
        "irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
        "occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est "
        "laborum.   ";
    size_t text_size = strlen(text);

    // Locate instances of 4 or more bytes repeated multiple times in text
    HashTable* table = create_HashTable(ceil(text_size / 0.7));
    int pos1 = 0;
    duplicate_bytes* duplicates = (duplicate_bytes*)malloc(text_size * sizeof(duplicate_bytes));

    for (int i = 0; i < text_size - 3; i++)
    {
        char buffer[5];
        memcpy(buffer, text + i, 4);
        buffer[4] = '\0';

        insertToHashTable(table, buffer, i);
        unsigned short index = hash_function(buffer, table->size);
        LinkedList* head = table->buckets[index];

        while (head && memcmp(head->bytes->text, buffer, 4) != 0)
        {
            head = head->next;
        }

        if (head && head->bytes->duplicates > 1)
        {
            int new_position = head->bytes->position[head->bytes->duplicates - 1];
            int pos2 = new_position + 4;

            if (pos1 > 0 && new_position > duplicates[pos1 - 1].position &&
                new_position < (duplicates[pos1 - 1].position + duplicates[pos1 - 1].size))
            {
                continue;
            }
            duplicates[pos1].position = new_position;
            int temp_size, iter_num = head->bytes->duplicates - 2;
            int *size_arr = (int*)malloc(sizeof(int) * (iter_num + 1)), max_size, max_pos;
            for (int i = 0; i <= iter_num; i++)
            {
                temp_size = 4;
                pos2 = new_position + 4;
                while (text[head->bytes->position[iter_num - i] + temp_size] == text[pos2++] && temp_size < 65)
                {
                    temp_size++;
                }
                if (temp_size == 65)
                {
                    temp_size--;
                }
                size_arr[i] = temp_size;
            }
            if (iter_num > 0)
            {
                findMax(size_arr, iter_num + 1, &max_pos, &max_size);
                duplicates[pos1].size = max_size;
                duplicates[pos1].offset = duplicates[pos1].position - head->bytes->position[iter_num - max_pos];
            }
            else
            {
                duplicates[pos1].size = size_arr[iter_num];
                duplicates[pos1].offset = duplicates[pos1].position - head->bytes->position[iter_num];
            }
            free(size_arr);
            pos1++;
        }
    }
    free_table(&table);

    // Snappy Compression
    unsigned char* compressed_data = (unsigned char*)malloc(text_size + 1);
    int data_pointer = 0, text_pointer = 0;

    for (int i = 0; i < pos1; i++)
    {
        int length = duplicates[i].position - text_pointer, size = duplicates[i].size, offset = duplicates[i].offset;
        int num_bytes2 = (ceil(log2(offset) / 8) > 2 ? 4 : 2);
        if (length > 0)
        {
            int num_bytes1 = ceil(log2(length - 1) / 8);

            if (length > 60)
            {
                int encoding;
                switch (num_bytes1)
                {
                case 1:
                    encoding = (60 << 2);
                    break;
                case 2:
                    encoding = (61 << 2);
                    break;
                case 3:
                    encoding = (62 << 2);
                    break;
                case 4:
                    encoding = (63 << 2);
                    break;
                }
                compressed_data[data_pointer++] = encoding;
                split_bytes(num_bytes1, compressed_data, length - 1, &data_pointer);
            }
            else
            {
                compressed_data[data_pointer++] = (length - 1) << 2;
            }

            memcpy(compressed_data + data_pointer, text + text_pointer, length);
            text_pointer += length;
            data_pointer += length;
        }

        if (size <= (0x07 + 4))
        {
            compressed_data[data_pointer++] = (((size - 4) << 2) | 0x01) | ((offset & 0x700) >> 3);
            split_bytes(1, compressed_data, offset, &data_pointer);
        }
        else
        {
            switch (num_bytes2)
            {
            case 2:
                compressed_data[data_pointer++] = (((size - 1) << 2) | 0x02);
                break;
            case 4:
                compressed_data[data_pointer++] = (((size - 1) << 2) | 0x03);
                break;
            }
            split_bytes(num_bytes2, compressed_data, offset, &data_pointer);
        }
        text_pointer += size;
    }

    // Store the remaining data as literals
    if (text_pointer < text_size)
    {
        compressed_data[data_pointer++] = (text_size - text_pointer - 1) << 2;
        memcpy(compressed_data + data_pointer, text + text_pointer, text_size - text_pointer);
    }
    else
    {
        compressed_data[data_pointer] = '\0';
    }
    data_pointer += (text_size - text_pointer);
    text_pointer = text_size;

    // Calculate the size of the encoded message
    int num_bytes = ceil(log2(text_size) / 7), output_pointer = 0;
    size_t output_size = num_bytes + data_pointer;
    unsigned char* output = (unsigned char*)calloc(output_size + 1, 1);

    for (output_pointer; output_pointer < num_bytes; output_pointer++)
    {
        int flag = (num_bytes - output_pointer) == 1 ? 0 : 1;
        output[output_pointer] = (flag << 7) | ((text_size >> (7 * output_pointer)) & 0x7F);
    }
    memcpy(output + output_pointer, compressed_data, data_pointer + 1);

    // Obtain size of uncompressed data.
    int compressed_pointer = 0, decompressed_pointer = 0;
    size_t uncompressed_size = 0;
    do
    {
        uncompressed_size |= (output[compressed_pointer] & 0x7F) << (7 * compressed_pointer);
    } while (output[compressed_pointer++] >> 7);
    unsigned char* decompression = (unsigned char*)calloc(uncompressed_size + 1, 1);

    // Decompress the data using the Snappy algorithm.
    while (compressed_pointer + 1 < output_size)
    {
        unsigned int dict_length, dict_offset, compare = output[compressed_pointer] >> 2;
        switch (output[compressed_pointer] & 0x03)
        {
        case 0:
            unsigned int literal_size;
            if (compare < 60)
            {
                literal_size = compare + 1;
                compressed_pointer++;
            }
            else
            {
                literal_size = 1 + decode_bytes(output + (compressed_pointer + 1), compare - 59);
                compressed_pointer += (compare - 58);
            }
            memcpy(decompression + decompressed_pointer, output + compressed_pointer, literal_size);
            decompressed_pointer += literal_size;
            compressed_pointer += literal_size;
            break;
        case 1:
            dict_length = 4 + ((output[compressed_pointer] >> 2) & 0x07);
            dict_offset = ((((output[compressed_pointer] >> 5) & 0x7) << 8) | output[compressed_pointer + 1]);
            memcpy(decompression + decompressed_pointer, decompression + (decompressed_pointer - dict_offset),
                   dict_length);
            compressed_pointer += 2;
            decompressed_pointer += dict_length;
            break;
        case 2:
            dict_length = 1 + ((output[compressed_pointer] >> 2) & 0x3F);
            dict_offset = decode_bytes(output + (compressed_pointer + 1), 2);
            memcpy(decompression + decompressed_pointer, decompression + (decompressed_pointer - dict_offset),
                   dict_length);
            compressed_pointer += 3;
            decompressed_pointer += dict_length;
            break;
        case 3:
            dict_length = 1 + ((output[compressed_pointer] >> 2) & 0x3F);
            dict_offset = decode_bytes(output + (compressed_pointer + 1), 4);
            memcpy(decompression + decompressed_pointer, decompression + (decompressed_pointer - dict_offset),
                   dict_length);
            compressed_pointer += 5;
            decompressed_pointer += dict_length;
            break;
        }
    }
    decompression[uncompressed_size] = '\0';

    // Display Decompressed data
    printf("%s\n\n", decompression);
    printf("Compression: %s\n", !memcmp(decompression, text, sizeof(text)) ? "valid" : "invalid");

    // Free used pointer memory
    free(output);
    free(decompression);
    free(duplicates);
    free(compressed_data);
    return 0;
}
