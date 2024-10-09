#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to represent the dynamic array
typedef struct
{
    int* data;
    size_t size;
    size_t capacity;
} Vector;

// Function to initialize the vector
void initVector(Vector* vec, size_t initialCapacity)
{
    vec->data = (int*)malloc(initialCapacity * sizeof(int));
    if (vec->data == NULL)
    {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    vec->size = 0;
    vec->capacity = initialCapacity;
}

// Function to push an element to the end of the vector
void pushBack(Vector* vec, int value)
{
    if (vec->size == vec->capacity)
    {
        // Resize the array if it's full
        vec->capacity *= 2;
        vec->data = (int*)realloc(vec->data, vec->capacity * sizeof(int));
        if (vec->data == NULL)
        {
            // Handle memory reallocation failure
            fprintf(stderr, "Memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Add the new element
    vec->data[vec->size++] = value;
}

// Function to find the position to insert the new value to maintain sortedness
size_t lowerBound(const Vector* vec, int value)
{
    size_t low = 0;
    size_t high = vec->size;

    while (low < high)
    {
        size_t mid = low + (high - low) / 2;
        if (vec->data[mid] < value)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }

    return low;
}

// Function to insert an item into the vector at the specified position
void insert(Vector* vec, int value, size_t position)
{
    if (position < 0 || position > vec->size)
    {
        // Handle invalid position
        fprintf(stderr, "Invalid position for insertion\n");
        exit(EXIT_FAILURE);
    }

    // Should happen in rare cases
    if (vec->size == vec->capacity)
    {
        // Resize the array if it's full
        vec->capacity *= 2;
        vec->data = (int*)realloc(vec->data, vec->capacity * sizeof(int));
        if (vec->data == NULL)
        {
            // Handle memory reallocation failure
            fprintf(stderr, "Memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    // Shift elements to make room for the new value
    memmove(vec->data + position + 1, vec->data + position, (vec->size - position) * sizeof(int));

    // Insert the new value at the specified position
    vec->data[position] = value;
    vec->size++;
}

// Function to deallocate memory used by the vector
void freeVector(Vector* vec)
{
    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

int main()
{
    // Example usage
    Vector myVector;
    initVector(&myVector, 5);

    // Add elements to the vector
    pushBack(&myVector, 10);
    pushBack(&myVector, 20);
    pushBack(&myVector, 30);

    // Value to be inserted
    int newValue = 25;

    // Find the position to insert the new value
    size_t insertionPoint = lowerBound(&myVector, newValue);

    // Insert the new value at the found position
    insert(&myVector, newValue, insertionPoint);

    // Display the sorted vector after insertion
    printf("Sorted Vector after insertion: ");
    for (size_t i = 0; i < myVector.size; ++i)
    {
        printf("%d ", myVector.data[i]);
    }
    printf("\n");

    // Deallocate memory used by the vector
    freeVector(&myVector);

    return 0;
}
