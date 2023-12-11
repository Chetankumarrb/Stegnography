#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    int type=check_operation_type(argv);
    if(type==e_encode)
    {
        EncodeInfo encinfo;
        if(argc>=4)
        {
            if(read_and_validate_encode_args(argv,&encinfo)==e_success)
            {
            if(do_encoding(&encinfo)==e_success)
            {
                printf("Encoding Succesfully Completed!!!\n");
            }
            }
        }
        else
        {
             printf("Please provide the more than 3 arguments for Encoding\n");
        }
    } 
    else if(type==e_decode)
    {
        DecodeInfo decInfo;
        if(argc>=3)
        {
            if(read_and_validate_decode_args(argv,&decInfo)==d_success)
            {
                printf("Read and validation completed for Decoding\n");
                if(do_decoding(&decInfo)==d_success)
                {
                    printf("Decoding Succesfully Completed!!!\n");
                    fclose(decInfo.fptr_output)
                }

            }
        }
        else
        {
             printf("Please provide the more than 2 arguments for Decoding\n");
        }
    }   
    else
    {
        printf("please provide the operation type\n");
    }    

}
    

OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))
    {
        return e_encode;
    }
    else if(!strcmp(argv[1],"-d"))
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
 