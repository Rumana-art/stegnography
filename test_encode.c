#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc , char *argv[])
{
   
    EncodeInfo E1;
    //validate cla
    int res = check_operation_type(argv);
      if(res == e_encode)
      {
        printf("Encoding is selected\n");
        if(read_and_validate_encode_args(argv, &E1) == e_success)
        {
          printf("Info : Read and validate encode args is success\n");
          if (open_files(&E1) == e_success )
          {
            printf("Info : Files are opened succesfully\n");
            if(do_encoding(&E1) == e_success)
            {
              printf("Info: Encoding is success ✓\n");
            }
            else

            {
              printf("Failed to encode\n");
              return 0;
            }



          }   
          else
          {
            printf("Open files is failure\n");
            return 0;
          }
        }
        else
        {
          printf("Read and validate encode args is failure\n");
          return 0;
          
        }
      }
      else if( res == e_decode)
      {
        printf("Decoding is selected\n");
        DecodeInfo D1;
        if(read_and_validate_decode_args(argv, &D1) == e_success)
        {
          printf("Info : Read and validate decode args is success ✓\n");
          if(do_decoding(&D1) == e_success)
          {
            printf("Info: Decoding is success ✓\n");
          }
          else
          {
            printf("Failed to decode\n");
            return 0;
          }
        }
        else
        {
          printf("Read and validate decode args is failure\n");
          return 0;
        }
      }
      else
      {
        printf("Invalid option\n");
        printf("For Encoding: ./a.out -e beautiful.bmp secret.txt[stegno.bmp]\n");

        printf("For coding: ./a.out -d stegno.bmp default.txt\n");
      }
      return 0;
}


OperationType check_operation_type(char *argv[])
{

    if(strcmp(argv[1],"-e") == 0)
        return e_encode;
    else if(strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}