#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
int count=0;
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp") != NULL)
    {
        decInfo->encoded_image_fname = argv[2];
    }
    else
    {
        printf("Please pass .bmp Encoded file\n");
        return d_failure;
    }

    if(argv[3] != NULL)
    {
        decInfo->output_fname=argv[3];
    }
    else
    {
        printf("output file is not mentioned ,Default file name is added\n");
        decInfo->output_fname="output";
    }
    return d_success;
}




//Perform the decoding 
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_bmp_file(decInfo)==d_success)
    {
        printf("encoded file opened Successfully!!!\n");
    }
    else
    {
        printf("Unable to open bmp file\n");
        return d_failure;
    }

    if(decode_magic_string(MAGIC_STRING,decInfo)==d_success)
    {
        printf("Magic string Decoded successfully\n");
    }
    else
    {
        printf("unable to Decode Magic string\n");
        return d_failure;
    }

    if(decode_file_ext_size(decInfo) == d_success)
    {
        printf("Extension size decoded Successfully\n");
    }
    else
    {
        printf("Unable to decode file extension Size\n");
        return d_failure;
    }

    if(decode_file_extn(decInfo) == d_success)
    {
        printf("Extn decoded successfully\n");
        char buff[20];
        strcpy(buff, decInfo->output_fname);
        strcat(buff, decInfo->extn_secert_file);
        decInfo->fptr_output = fopen(buff, "w");
        if (decInfo->fptr_output == NULL)
        {
            perror("fopen");
            fprintf(stderr, "Error : Uanble to open file %s\n", decInfo->encoded_image_fname);
            return d_failure;
        }
    }
    else
    {
        printf("Extn decode failed \n");
        return d_failure;
    }


    if (decode_file_size(decInfo) == d_success)
    {
        printf("Decoded file size successfully\n");
    }
    else
    {
        printf("Failed to Decode file size\n");
        return d_failure;
    }


    if (decode_file_data(decInfo) == d_success)
    {
        printf("Decoded data sucessfully !!!\n");
        return d_success;
    }
    else
    {
        return d_failure;
    }

     return d_success;
}



Status open_decode_bmp_file(DecodeInfo *decInfo)
{
   decInfo->fptr_encoded_image=fopen(decInfo->encoded_image_fname,"r");
   if(decInfo->fptr_encoded_image == NULL)
   {
     perror("fopen");
    fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->encoded_image_fname);    
    return e_failure; 
   }
   return d_success;
}
  
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo)
{
    int size=strlen(magic_string);
    fseek(decInfo->fptr_encoded_image,54,SEEK_SET);
    char arr[8];
    for(int i=0;i<size;i++)
    {
        fread(arr,8,1,decInfo->fptr_encoded_image);
        for(int j=0;j<8;j++)
        {
        if(((magic_string[i] >> j) & 0x01) != (arr[j] & 0x01))
        {
            return d_failure;
        }
        }
    }
  //  printf("%s\n", magic_string);
    return d_success;
}


Status decode_file_ext_size(DecodeInfo *decInfo)
{
    if(decode_size_from_lsb(decInfo->fptr_encoded_image)==d_success)
    {
        decInfo->extn_size=count;
        return d_success;
    }
    
}


Status decode_size_from_lsb(FILE *fptr_encoded_image)
{
    char buffer[32];
    count = 0;
    fread(buffer,32,1,fptr_encoded_image);
    for(int i=0;i<=31;i++)
    {
        int lsb = buffer[i] & 0x01;
        count |= (lsb << i);    

    }

   // printf("%d\n", count);

    return d_success;
}



Status decode_file_extn(DecodeInfo *decInfo)
{
    int size=decInfo->extn_size;
    char data[size+1];
    char arr[8];
    int i;
    for( i=0;i<size;i++)
    {
    data[i]=0;
    fread(arr,8,1,decInfo->fptr_encoded_image);
    for(int j=0;j<8;j++)
    {
        char a=arr[j] & 0x01;
        data[i] |= (a<<j);
    }
    }
    data[i]='\0';
    strcpy(decInfo->extn_secert_file,data);
   // printf("%s\n", data);
   return  d_success;
    
}


Status decode_file_size(DecodeInfo *decInfo)
{
   if(decode_size_from_lsb(decInfo->fptr_encoded_image)==d_success)
    {
        decInfo->output_file_size=count;
        return d_success;
    }
    
}

Status decode_file_data(DecodeInfo *decInfo)
{
    if(decode_file_from_image(decInfo->output_file_size,decInfo->fptr_encoded_image,decInfo->fptr_output)==d_success)
    {
        return d_success;
    }
}

Status decode_file_from_image(int size,FILE *fptr_encoded_image,FILE *fptr_output)
{
    char arr[8];
    char data=0;

    rewind(fptr_output);
    for(int i=0;i<size;i++)
    {
        data =0;
        fread(arr,8,1,fptr_encoded_image);
        decode_byte_from_lsb(&data,arr); 
        fwrite(&data,1,1,fptr_output);        
    
       
    }
    
    return d_success;
   
}

Status decode_byte_from_lsb(char *data,char *image_buffer)
{
   
    for(int i=0;i<8;i++)
    {
         char c=image_buffer[i] & 0x01;
         *data |= (c << i) ;
         
    }
   //  printf("%c", *data);
   
    return d_success;
}