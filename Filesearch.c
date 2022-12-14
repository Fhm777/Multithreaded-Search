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

int main()
{
    int n = 4;
    pthread_t t[n];
    struct data string_data[n];
    FILE *ptr;
    int a,i,j,k,flag,s,k_count,extra_char,count,char_count,find_size;
    char ch;
    char find_string[100];
    printf("Input the string to search : ");
    scanf("%[^\n]s", find_string);
    find_size = strlen(find_string);
    ptr = fopen("text.txt", "r");
    count = 0;
    flag = 1;
    fseek(ptr, 0, SEEK_END);
    char_count = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    a = (int)char_count/n; 
    extra_char = (int)char_count%n; 
    k_count = 0; 
    for(i=0; i<n; i++) 
    { 
        k=0; 
        if(i==n-1) 
        {
            s = a+extra_char;
            string_data[i].string = (char *)calloc(1,s+1); 
            fread(string_data[i].string, 1, s, ptr);
        }
        else
        {
            string_data[i].string = (char *)calloc(1,a+1); 
            fread(string_data[i].string, 1, a, ptr);
            s = strlen(string_data[i].string);
            j=0;
            for(j=0; j<find_size; j++)
            {
                if(string_data[i].string[s-1]==find_string[j])
                {
                    break;
                }
            }
            if(find_size!=(j+1))
            {
                while(flag)
                {
                    if(string_data[i].string[s+k-1]==find_string[j] && k<char_count-a)
                    {
                        k++;
                        ch = fgetc(ptr);
                        j = (j+1)%find_size;
                        if(ch == find_string[j])
                        {
                            string_data[i].string = (char *)realloc(string_data[i].string, s+k);
                            string_data[i].string[s+k-1] = ch;
                        }
                        else
                        {
                            flag = 0;
                            fseek(ptr, -1, SEEK_CUR);
                        }
                    }
                    else
                    {
                        flag = 0;
                    }
                }
            }
            flag = 1;
        }
        k_count = k + k_count;
        string_data[i].string_find = (char *)malloc(find_size);
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