#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"


/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encinfo)
{

    if(strstr(argv[2],".bmp") != NULL)      //check .bmp file or not(argv[2])
    {
        encinfo->src_image_fname=argv[2];
        
    }
    else
    {
        printf("Please provide the .bmp file\n");
        return e_failure;
    }

    if(strstr(argv[3],".txt") != NULL)      //check .txt file or not(argv[3])  
    {
        encinfo->secret_fname=argv[3];
    }
    else
    {
        printf("Please provide the .txt file\n");
        return e_failure;
    }
    
    if(argv[4] != NULL)                    //check argv[4]output file
    {
        if(strstr(argv[4],".bmp") != NULL)
        {
             encinfo->stego_image_fname=argv[4];
        }
        else
        {
            printf("Please provide correct type(.bmp) of output file\n");
            return e_failure;
        }
    }
    else
    {
        encinfo->stego_image_fname="output.bmp";
    }
    
return e_success;

}






Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("INFO : Files are opened Successfully\n");
    }
    else
    {
        printf("INFO : Files are not opened Successfully\n");
        return  e_failure;
    }


    if(check_capacity(encInfo)==e_success)
    {
        printf("FILE size validate\n");
    }
    else
    {
        printf("FILE size not validsate\n");
        return e_failure;
    }

 

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
       printf("Header Copied successfully\n");
    }
    else
    {
        printf("failed in copying Header\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
    {
        printf("Magic string Encoded Successfully\n");

    }
    else
    {
        printf("Unable to Encode the Magic string\n");
        return e_failure;
    }


    if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname,'.')),encInfo)==e_success)
    {
         printf("Extension size Encoded Successfully\n");
    }
    else
    {
        printf("Extension Size failed to encode\n");
        return e_failure;
    }


    if(encode_secret_file_extn(strchr(encInfo->secret_fname,'.'), encInfo)==e_success)
    {
        printf("Secret file extn Encoded\n");
    }
    else
    {
        printf("Sorry...Failed to Encode secret file extn\n");
        return e_failure;
    }
        
    if(encode_secret_file_size(get_file_size(encInfo->fptr_secret), encInfo)==e_success)
    {
        printf("File size encoded successfully\n");
    }
    else
    {
        printf("Failed to Encode File size\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("File data Encoded Successfully\n");
    }
    else
    {
        printf("Failed to Encode the File data\n");
        e_failure;
    }
    
    
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("Successfully encoded remaining img data\n");
    }
    else
    {
        printf("Failed to encode remaining img data\n");
    }

   return e_success;     
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}



Status check_capacity(EncodeInfo *encInfo)
{
    if(get_image_size_for_bmp(encInfo->fptr_src_image) >= (54+(strlen(MAGIC_STRING))*8+32+strlen(strchr(encInfo->secret_fname,'.'))*8+32+get_file_size(encInfo->fptr_secret)*8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
//size of file
uint get_file_size(FILE *fptr)
{
   fseek(fptr,0,SEEK_END);
   int size=ftell(fptr);
   rewind(fptr);
   return size;
}



//ecoding  bmp header 

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    
    if(ftell(fptr_dest_image)==54)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
    
}



//encoding magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image((char*)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


//encoding
Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
  char arr[8];
  for(int i=0;i<size;i++)
  {
    fread(arr,8,1,fptr_src_image);
    //printf("%c", data[i]);
    encode_byte_to_lsb(data[i],arr);
    fwrite(arr,8,1,fptr_stego_image);
  }
   return e_success;
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=image_buffer[i] & 0xfe | ((data>>i) & 0x01);
    }
    
    return e_success;
}
//encoding size
Status encode_size_to_lsb(unsigned int len,EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
       buffer[i] = (buffer[i] & 0xfe) | (len>>i & 0x01);
    }
    fwrite(buffer,32,1,encInfo->fptr_stego_image);

    return e_success;
}
//encoding secret file extn size
Status encode_secret_file_extn_size(unsigned int size,EncodeInfo *encInfo)
{
    if(encode_size_to_lsb(size,encInfo)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
//encoding secret file extn
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image((char *)file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
  //  printf("%ld\n", file_size);
    if(encode_size_to_lsb(file_size,encInfo)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int size=get_file_size(encInfo->fptr_secret);
   // printf("%d\n", size);
    char buffer[size];
    rewind(encInfo->fptr_secret);
        fread(buffer,size,1,encInfo->fptr_secret);
        buffer[size] = '\0';
        // printf("%s\n", buffer);

      encode_data_to_image(buffer, size, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;

}


/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src) > 0)
    {
        fputc(ch,fptr_dest);
    }
    return e_success;
}
