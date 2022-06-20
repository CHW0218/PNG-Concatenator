#include "catpng.h"
#include "curl.h"

int U32toU8(U8 *buffer,U32 number){
    U32 convert = htonl(number);
    memcpy(buffer, &convert, 4);

    return 0;
}
chunk_p readingChunk (char * currentBuff, chunk_p READ_CHUNK ,char chunkName[4], int *cursor){

    
    U8* CHUNK_LEN_BUFFER = calloc(1, sizeof(U32));

    memcpy(CHUNK_LEN_BUFFER, currentBuff + *cursor, sizeof(U32));
    *cursor += sizeof(U32);
    int chunk_length = *(unsigned int*)(CHUNK_LEN_BUFFER);



    write_file("recieved.png", currentBuff, 50000);

    READ_CHUNK->length = ntohl(chunk_length);

    U8* CHUNK_TYPE_BUFFER = calloc(CHUNK_TYPE_SIZE, sizeof(U8));
    memcpy(CHUNK_TYPE_BUFFER, currentBuff + *cursor, CHUNK_TYPE_SIZE);
    *cursor += CHUNK_TYPE_SIZE;
    strcpy((char *)READ_CHUNK->type ,(char *)CHUNK_TYPE_BUFFER);
    U8* dataPointer = calloc(READ_CHUNK->length, sizeof(U8));
    memcpy(dataPointer, currentBuff + *cursor, READ_CHUNK->length);
    *cursor += READ_CHUNK->length;

    READ_CHUNK->p_data = dataPointer;
    // for (int i = 0; i < READ_CHUNK->length; i++)
    // {
    //     printf("%d ",  cursor);
    // }

    int crcSize = READ_CHUNK->length+CHUNK_TYPE_SIZE;
    *cursor -= crcSize;
    U8* crcPointer = calloc(crcSize, sizeof(U8));
    memcpy(crcPointer, currentBuff + *cursor, crcSize);
    *cursor += crcSize;

    U32 computedCrc = (U32)crc(crcPointer,crcSize);

    U32 CHUNK_CRC_BUFFER;
    memcpy(&CHUNK_CRC_BUFFER, currentBuff + *cursor, sizeof(U32));
    *cursor += sizeof(U32);

    READ_CHUNK->crc = ntohl(CHUNK_CRC_BUFFER);

    // for (int i = 0; i < CHUNK_TYPE_SIZE; ++i)
    // {
    //     printf("%02X ", CHUNK->type[i]);
    // }

    if(READ_CHUNK->crc != computedCrc){
      printf("%s ",chunkName);
      printf("chunk CRC error: computed %x, expected %x \n",computedCrc,READ_CHUNK->crc);
    }

    return READ_CHUNK;


}


int catpng(int num, char *buffers[],long size)
{
    U8* DATA_BUFFER = calloc(50000000, sizeof(U8));
    U32 DATA_BUFFER_POINTER = 0;
    U32 total_height = 0;
    U32 total_width = 0;

    // printf("array: %d \n",sizeof (buffers));
    // printf("ary: %d \n",sizeof *buffers);


    for (int i = 0; i < num; ++i)
    {   
        U8* sigBuffer = calloc(PNG_SIG_SIZE, sizeof(U8));
        int cursor = 0;
        char * currentBuff = buffers[i];
        if( currentBuff == 0 )  {
            perror ("Error buffer");
            free(sigBuffer);
            return(-1);
        }else{
                
            

            memcpy(sigBuffer, currentBuff + cursor, PNG_SIG_SIZE);
            cursor += PNG_SIG_SIZE;
            if(!is_png(sigBuffer, PNG_SIG_SIZE)){
                    printf("PNG %d:Not a PNG file \n", i);
                    free(sigBuffer);
                    return 0;
            }


            data_IHDR_p IHDR_DATA = calloc(DATA_IHDR_SIZE, sizeof(U8));
            get_png_data_IHDR(IHDR_DATA, currentBuff,8, cursor);
            printf("png%d: %d x %d \n", i,IHDR_DATA->width,IHDR_DATA->height);
            total_width = IHDR_DATA->width;
            total_height += IHDR_DATA->height;
            chunk_p IHDR = malloc(CHUNK_SIZE);

            readingChunk(currentBuff, IHDR,"IHDR",&cursor);
            printf("1%d \n",  cursor);
            chunk_p IDAT = malloc(CHUNK_SIZE);
            readingChunk(currentBuff, IDAT,"IDAT",&cursor);
            printf("1%d \n",  cursor);

            chunk_p IEND = malloc(CHUNK_SIZE);
            readingChunk(currentBuff, IEND,"IEND",&cursor);
            printf("1%d \n",  cursor);
            


            U64 BUFFER_SIZE = IHDR_DATA->height*(IHDR_DATA->width*4+1);
            U8* INF_BUFFER = calloc(BUFFER_SIZE, sizeof(U8));


            // statements
            mem_inf(INF_BUFFER, &BUFFER_SIZE, IDAT->p_data, (U64)(IDAT->length));


            memcpy(&(DATA_BUFFER[DATA_BUFFER_POINTER]), INF_BUFFER, BUFFER_SIZE);

            DATA_BUFFER_POINTER += BUFFER_SIZE;


            free(INF_BUFFER);
            free(sigBuffer);
            free(IHDR);
            free(IDAT);
            free(IEND);

        }
    }

    /* File pointer to hold reference to our file */

    U64 MEM_DEF_SIZE = 1000000 ;
    U8* DEF_DATA_BUFFER = calloc(MEM_DEF_SIZE, sizeof(U8));
    U8 pngSignature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    U8* CLEAN_DATA_BUFFER = calloc(DATA_BUFFER_POINTER, sizeof(U8));

    memcpy(CLEAN_DATA_BUFFER, DATA_BUFFER, DATA_BUFFER_POINTER);




    mem_def(DEF_DATA_BUFFER, &MEM_DEF_SIZE, CLEAN_DATA_BUFFER, DATA_BUFFER_POINTER, Z_DEFAULT_COMPRESSION);



    /*
     * Open file in w (write) mode.
     * "data/file1.txt" is complete path to create file
     */
    FILE * fPtr = fopen("all.png", "w+");


    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    /* Write data to file */
    fwrite(pngSignature , 8 , sizeof(U8) , fPtr );


    // U8* sigBuffer = calloc(PNG_SIG_SIZE, sizeof(U8));
    // fread(sigBuffer,1, PNG_SIG_SIZE, fPtr);
            //     for (int i = 0; i < 8; ++i)
            // {
            //     printf("%02X ", pngSignature[i]);
            // }

        // if(!is_png(sigBuffer, PNG_SIG_SIZE)){
        //             printf("%s:Not a PNG file \n", buffers[1]);
        //             free(sigBuffer);
        //             fclose(fPtr);
        //             return 0;
        // }




    U8* PNGLength = calloc(1, sizeof(U32));
    U32toU8(PNGLength,13);
    fwrite(PNGLength , 4 , sizeof(U8) , fPtr );


    U8 PNG_IHDR_TYPE[] = {0x49, 0x48, 0x44, 0x52};
    fwrite(PNG_IHDR_TYPE , 4 , sizeof(U8) , fPtr );
    U8* totalWidth = calloc(1, sizeof(U32));
    U32toU8(totalWidth,total_width);
    fwrite(totalWidth , 4 , sizeof(U8) , fPtr );

    U8* totalHeight = calloc(1, sizeof(U32));
    U32toU8(totalHeight,total_height);
    fwrite(totalHeight , 4 , sizeof(U8) , fPtr );




    U8 PNG_IHDR_DATA[] = {0x08, 0x06, 0x0, 0x0, 0x0};
    fwrite(PNG_IHDR_DATA , 5 , sizeof(U8) , fPtr );



    int crcSize = DATA_IHDR_SIZE+CHUNK_TYPE_SIZE;
    fseek(fPtr,-crcSize,SEEK_CUR);
    U8* crcPointer = calloc(crcSize, sizeof(U8));
    fread(crcPointer, crcSize,1,fPtr);



    U32 computedCrc = (U32)crc(crcPointer,crcSize);

    U8* IHDR_CRC = calloc(1, sizeof(U32));
    U32toU8(IHDR_CRC,computedCrc);
    fwrite(IHDR_CRC , 4 , sizeof(U8) , fPtr );

    U8* IDAT_LEN = calloc(1, sizeof(U32));
    U32toU8(IDAT_LEN,MEM_DEF_SIZE);
    fwrite(IDAT_LEN , 4 , sizeof(U8) , fPtr );

    U8 PNG_IDAT_TYPE[] = {0x49, 0x44, 0x41, 0x54 };
    fwrite(PNG_IDAT_TYPE , 4 , sizeof(U8) , fPtr );

    fwrite(DEF_DATA_BUFFER , MEM_DEF_SIZE , sizeof(U8) , fPtr );

    int IDAT_crcSize = MEM_DEF_SIZE+CHUNK_TYPE_SIZE;
    fseek(fPtr,-IDAT_crcSize,SEEK_CUR);
    U8* IDAT_crcPointer = calloc(IDAT_crcSize, sizeof(U8));
    fread(IDAT_crcPointer, IDAT_crcSize,1,fPtr);

    U32 IDAT_computedCrc = (U32)crc(IDAT_crcPointer,IDAT_crcSize);

    U8* IDAT_CRC = calloc(1, sizeof(U32));
    U32toU8(IDAT_CRC,IDAT_computedCrc);
    fwrite(IDAT_CRC , 4 , sizeof(U8) , fPtr );


    U8 IEND_HEADER[] = {0,0,0,0,0x49, 0x45, 0x4E, 0x44 };
    fwrite(IEND_HEADER , 8 , sizeof(U8) , fPtr );

    int IEND_crcSize = CHUNK_TYPE_SIZE;
    fseek(fPtr,-IEND_crcSize,SEEK_CUR);
    U8* IEND_crcPointer = calloc(IEND_crcSize, sizeof(U8));
    fread(IEND_crcPointer, IEND_crcSize,1,fPtr);


    U32 IEND_computedCrc = (U32)crc(IEND_crcPointer,IEND_crcSize);

    U8* IEND_CRC = calloc(1, sizeof(U32));
    U32toU8(IEND_CRC,IEND_computedCrc);
    fwrite(IEND_CRC , 4 , sizeof(U8) , fPtr );


    /* Close file to save file data */
    fclose(fPtr);


    /* Success message */


    return 0;

}


