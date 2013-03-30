// bin2hex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>


int bin2hex(FILE* in, FILE* out)
{
        char buffer[4096];
        int i, n;
        const char table[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                          '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        const char stable[16] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                           ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
        const char stable1[16] = {',', ',', ',', ',', ',', ',', ',', ',',
                           ',', ',', ',', ',', ',', ',', ',', ','};


        while( (n = fread(buffer, 1, sizeof(buffer), in)) > 0 )
        {
                for(i = 0; i < n; ++i)
                {
                        int a = (buffer[i]>>4)&0x0F;
                        int b = buffer[i]     &0x0F;
                        fputc('0', out);
                        fputc('x', out);
                        fputc(table[a], out);
                        fputc(table[b], out);
                        fputc(stable1[i&0x0F], out);
                        fputc(stable[i&0x0F], out);
                }
        }

        return 0;
}

int _tmain(int argc, char** argv)
{
        FILE* in = 0;
        FILE* out = 0;

        if(argc==3)
        {
                if ( fopen_s(&in, argv[1], "rb") !=0 )
                {
                    fprintf(stderr, "cannot open %s\n", argv[1]);
                    return 1;
                }

                if ( fopen_s(&out, argv[2], "w") !=0 )
                {
                    fprintf(stderr, "cannot open %s\n", argv[2]);
                    return 1;
                }
        }
        else if(argc==1)
        {
                in = stdin;
                out = stdout;
        }
        else if(argc==2)
        {
                if ( fopen_s(&in, argv[1], "rb") !=0)
                {
                    fprintf(stderr, "cannot open %s\n", argv[1]);
                    return 1;
                }
                out = stdout;
        }
        else
        {
                fprintf(stderr, "usage: bin2hex input output\n");
                return 1;
        }

        return bin2hex(in, out);
}




