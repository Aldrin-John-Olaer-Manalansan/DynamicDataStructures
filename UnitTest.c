/*
#include "DynamicStringArray.c"
dynamicstringarray_t strArr;
void printall(const dynamicstringarray_t* const _object) {
    printf("Array: %X %u %u Buffer: %X %u %u Rate: %f\n", _object->array, _object->elementCount, _object->maxElementCount, _object->buffer, _object->usedSize, _object->bufferSize, _object->expansionRate);
    for (size_t i = 0; i < _object->elementCount; i++) {
        printf("%u = %X = '%s'\n", i, _object->array[i], _object->array[i]);
    }
}
int main(void) {
    DynamicStringArray_InitAll(&strArr, 3, 25, 0.5);
    printall(&strArr);
    DynamicStringArray_Push(&strArr, "Hello World!");
    printall(&strArr);
    DynamicStringArray_Push(&strArr, "How Are you?");
    printall(&strArr);
    DynamicStringArray_Push(&strArr, "Im fine.");
    printall(&strArr);
    DynamicStringArray_Insert(&strArr, 1, "Thank you!");
    printall(&strArr);

    printf("%d %u %d %u\n", DynamicStringArray_Search(&strArr, "hello world!", true), DynamicStringArray_HasString(&strArr, "hello world!", true), DynamicStringArray_Search(&strArr, "hello world!", false), DynamicStringArray_HasString(&strArr, "hello world!", false));

    DynamicStringArray_Delete(&strArr, 1);
    printall(&strArr);
    DynamicStringArray_Pop(&strArr);
    printall(&strArr);
    return 0;
}
*/

#include "BinaryBuilder.c"
#include "StringBuilder.c"
stringbuilder_t stringBuilder;
int main(void) {
    printf("ASAN is running!\n");
    uint8_t *arr = malloc(10);
    arr[7] = 'H';
    printf("%X\n", arr[15]);    // This should definitely trigger ASAN
    free(arr);

    StringBuilder_InitWithMinSize(&stringBuilder, 59, 0.5);
    StringBuilder_InsertString(&stringBuilder, "Hello Word!");
    printf("%llu %llu %s\n", strlen(stringBuilder.string), stringBuilder.capacity, stringBuilder.string);
    StringBuilder_InsertFormattedString(&stringBuilder, "Float %f Int %d Hex %X", 1424232.653942, 0x12345678, 0x12345678);
    printf("%llu %llu %s\n", strlen(stringBuilder.string), stringBuilder.capacity, stringBuilder.string);
    StringBuilder_SetWriteOffset(&stringBuilder, 11);
    StringBuilder_InsertString(&stringBuilder, "How Are you?");
    printf("%llu %llu %s\n", strlen(stringBuilder.string), stringBuilder.capacity, stringBuilder.string);

    return 0;
}