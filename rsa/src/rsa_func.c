#include "rsa_func.h"

int OS2IP(mpz_t MESSAGE, unsigned char *EM, size_t MESSAGE_STR_NUM) {
    unsigned long const256_ul = 256;
    mpz_t const256;
    mpz_init(const256);  // 256
    mpz_set_ui(const256, const256_ul);

    mpz_t pow_num;
    mpz_init(pow_num);
    mpz_set_ui(MESSAGE, (unsigned long)0);
    for (int i = 0; i < MESSAGE_STR_NUM; ++i) {
        mpz_pow_ui(pow_num, const256, (unsigned long)(MESSAGE_STR_NUM - 1 - i));
        mpz_mul_ui(pow_num, pow_num, (unsigned long)EM[i]);
        mpz_add(MESSAGE, MESSAGE, pow_num);
    }
    mpz_clear(const256);
    mpz_clear(pow_num);
    return 0;
}

int I2OSP(unsigned char *Message_C_ptr, mpz_t encode_small_c, size_t xLen) {
    unsigned long const256_ul = 256;
    mpz_t const256;
    mpz_init_set_ui(const256, const256_ul);  // 256
    mpz_t x;
    mpz_init(x);
    mpz_pow_ui(x, const256, (unsigned long)xLen);
    if (mpz_cmp(encode_small_c, x) >= 0) {
        printf("integer too large");
        mpz_clear(const256);
        mpz_clear(x);
        return -1;
    } else {
        mpz_t to_div_r;
        mpz_init(to_div_r);
        mpz_t to_div_q;
        mpz_init(to_div_q);
        mpz_t result_num;
        mpz_init(result_num);
        for (int i = 0; i < xLen; ++i) {
            mpz_pow_ui(to_div_r, const256, (unsigned long)(i + 1));
            mpz_pow_ui(to_div_q, const256, (unsigned long)(i));

            mpz_fdiv_r(result_num, encode_small_c, to_div_r);
            mpz_fdiv_q(result_num, result_num, to_div_q);
            unsigned long result = mpz_get_ui(result_num);
            Message_C_ptr[xLen - 1 - i] = (unsigned char)result;
        }

        Message_C_ptr[xLen] = '\0';
        mpz_clear(to_div_r);
        mpz_clear(to_div_q);
        mpz_clear(result_num);
        mpz_clear(const256);
        mpz_clear(x);
        return 0;
    }
}

int RSAEP(mpz_t encrypt_small_c, mpz_t n, mpz_t key_e, mpz_t key_N) {
    mpz_powm(encrypt_small_c, n, key_e, key_N);
    return 0;
}

int RSADP(mpz_t decrypt_small_m, mpz_t c, mpz_t key_d, mpz_t key_N) {
    if (mpz_cmp(c, key_N) >= 0) {
        printf("decryption error. -2\n");
        mpz_clear(c);
        return -1;
    }
    mpz_powm(decrypt_small_m, c, key_d, key_N);
    return 0;
}