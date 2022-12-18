#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>

struct data
{
    char *string;
    char *string_find;
    int count;
};

void *thread_search(void *dataptr)
{
    int size_diff, string_size, find_size;
    int i=0, j=0, k=0, count=0;
    char *text,*find_string;
    struct data *string_data = (struct data*)dataptr;
    string_size = strlen(string_data->string);
    find_size = strlen(string_data->string_find);
    text = (char *)calloc(string_size+1,1);
    find_string = (char *)calloc(find_size+1,1);
    strcpy(text,string_data->string);
    strcpy(find_string,string_data->string_find);
    size_diff = string_size - find_size; 
    if(size_diff >= 0)
    {
        for(i=0; i<string_size; i++)
        {
            for(j=0; j<find_size; j++)
            {
                if(text[i+j]==find_string[j])
                {
                    k++;
                }
            }
            if(k==find_size)
            {
                count++;
            }
            k=0;
        }
    }
    string_data->count = string_data->count + count;
}

int file_size(FILE *ptr)
{ 
    int size;
    fseek(ptr, 0, SEEK_END);
    size = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    return size;
}

char* file_division(FILE *ptr , int division_size, char *find_string, int find_size, int n)
{
    int k = 0,j;
    int actual_size;
    char temp_char;
    char *divided_string = (char *)calloc(1,division_size+1);;
    fread(divided_string, 1, division_size, ptr);
    actual_size = strlen(divided_string);
    for(j=0; j<find_size; j++)
    {
        if(divided_string[actual_size-1]==find_string[j])
        {
            break;
        }
    }
    if(find_size!=(j+1))
    {
        while(!feof(ptr))
        {
            if(divided_string[actual_size+k-1]==find_string[j])
            {
                k++;
                temp_char = fgetc(ptr);
                j = (j+1)%find_size;
                if(temp_char == find_string[j])
                {
                    divided_string = (char *)realloc(divided_string, actual_size+k);
                    divided_string[actual_size+k-1] = temp_char;
                }
                else
                {
                    fseek(ptr, -1, SEEK_CUR);
                    return divided_string;
                }
            }
            else
            {
                return divided_string;
            }
        }
    }
    return divided_string;
}

int main()
{
    int extra_char_count,char_count,find_size,count=0,i,division_size;
    int n = 4;
    pthread_t t[n];
    struct data string_data[n];
    FILE *ptr;
    char find_string[100];

    printf("Input the string to search : ");
    scanf("%[^\n]s", find_string);
    find_size = strlen(find_string);

    ptr = fopen("text.txt", "r");
    char_count = file_size(ptr);
    extra_char_count = (int)char_count%n; 
    division_size = (int)char_count/n;
    for(i=0; i<n; i++)
    {
        if(i==n-1)
            division_size = division_size + extra_char_count;

        string_data[i].string = (char *)calloc(1,division_size+1);
        strcpy(string_data[i].string, file_division(ptr , division_size, find_string, find_size, n));
        string_data[i].string_find = (char *)calloc(1,find_size+1);
        strcpy(string_data[i].string_find, find_string);
        string_data[i].count = 0;
        
        pthread_create(&t[i], NULL, thread_search, (void *)&string_data[i]);
    }
    for(i=0; i<n; i++)
    {
        pthread_join(t[i], NULL);
        count = count + string_data[i].count;
    }
    printf("Frequency : %d", count);
    return 1;
}