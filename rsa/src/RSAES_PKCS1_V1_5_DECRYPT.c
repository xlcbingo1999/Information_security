#include "RSAES_PKCS1_V1_5_DECRYPT.h"

int RSAES_PKCS1_V1_5_DECRYPT(unsigned char *dec_message, mpz_t key_N, mpz_t key_d, unsigned char *ciphertext) {
    mpz_t key_temp_N;
    int k = 0;
    mpz_init_set(key_temp_N, key_N);
    while (mpz_cmp_ui(key_temp_N, (unsigned long)0) > 0) {
        mpz_fdiv_q_ui(key_temp_N, key_temp_N, 256);
        k++;
    }
    mpz_clear(key_temp_N);
    if (k < 11) {  // 这里最好不要用strlen去判断，默认我们会通过才对
        printf("decryption error. -1\n");
        return -1;
    } else {
        // 2. RSA decryption
        // a. OS2IP
        mpz_t c;
        mpz_init(c);
        int result_OS2IP = OS2IP(c, ciphertext, k);
        if(result_OS2IP != 0) {
            return -2;
        }
        // b. 解密 RSADP

        mpz_t decrypt_small_m;
        mpz_init(decrypt_small_m);
        int result_RSADP = RSADP(decrypt_small_m, c, key_d, key_N);
        mpz_clear(c);
        if(result_RSADP != 0) {
            return -3;
        }
        // c. I2OSP
        unsigned char EM[k + 1];
        unsigned char *EM_ptr = &EM;
        int result_I2OSP = I2OSP(EM_ptr, decrypt_small_m, k);
        if (result_I2OSP != 0) {
            mpz_clear(decrypt_small_m);
            return -4;
        } else {
            int flag = 0;
            int pos = 0;
            int EM_length = k;
            for (int i = 1; i < EM_length; ++i) {
                if (flag == 1) {
                    dec_message[i - pos - 1] = EM[i];
                }
                if (EM[i] == 0) {
                    flag = 1;
                    pos = i;
                }
            }
            dec_message[EM_length - pos - 1] = '\0';
            mpz_clear(decrypt_small_m);
            return 0;
        }
    }
}